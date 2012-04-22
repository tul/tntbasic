// **************************************************************************************************************************
// TNT Basic
// UEditServer.cpp
// © Mark Tully 2012
// 11/4/12
// **************************************************************************************************************************

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2012, Mark Tully
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or other
*   materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
* ***** END LICENSE BLOCK ***** */

#include "UEditServer.h"
#include "libwebsockets.h"
#include "jansson.h"
#include <vector>
#include "TNT_Debugging.h"
#include "Root.h"
#include "CResourceContainerCreator.h"
#include "CResourceContainer.h"
#include "TNTBasic_Errors.h"
#include "Marks Routines.h"
#include "UTBException.h"
#include "UResources.h"
#include "Utility Objects.h"
#include "CResource.h"

struct SCmdHandler
{
	char						name[64];
	UEditServer::TCmdCallback	callback;
};

static std::vector<SCmdHandler>			sHandlers;

typedef UInt32							TProjectId;
struct SProject
{
	TProjectId							id;
	CResourceContainer					*container;
};

typedef std::vector<SProject>			TProjVector;
static TProjVector						sOpenProj;
static CFURLRef							sEditorURL(0);
static TProjectId						sRollingId=1;

#define k_jsonOKPrefix  "{ \"status\":0, \"statusstr\":\"OK\", \"response\":\""
#define k_jsonOKPrefixLen	(sizeof(k_jsonOKPrefix)-1)
#define k_jsonOKPostfix "\" }"
#define k_jsonOKPostfixLen	(sizeof(k_jsonOKPostfix)-1)
static const int k_jsonReplyPrePadding=k_jsonOKPrefixLen+LWS_SEND_BUFFER_PRE_PADDING;
static const int k_jsonReplyPostPadding=k_jsonOKPostfixLen+LWS_SEND_BUFFER_POST_PADDING;

static json_t *GetJsonParamOfType(json_t *inObject, const char *inKeyName, json_type inExpectedType);

static void do404(
	struct libwebsocket *inWsi)
{
#define k_404	"HTTP/1.0 404 Not found\x0d\x0a" \
	"Server: TNT Basic\x0d\x0a" \
	"Content-Type: text/html\x0d\x0a" \
	"Content-Length: 12\x0d\x0a" \
	"\x0d\x0a" \
	"Not found :("

	libwebsocket_write(inWsi,(unsigned char*)k_404,strlen(k_404),LWS_WRITE_HTTP);
}

static int Callback_HTTP(
	struct libwebsocket_context			*inContext,
	struct libwebsocket					*inWsi,
	enum libwebsocket_callback_reasons	inReason,
	void								*inUser,
	void								*inData,
	size_t								inLen)
{
	char client_name[128];
	char client_ip[128];

	switch (inReason)
	{
		case LWS_CALLBACK_HTTP:
			do404(inWsi);		// serve no html from TB for now
			break;

		/*
		 * callback for confirming to continue with client IP appear in
		 * protocol 0 callback since no websocket protocol has been agreed
		 * yet.  You can just ignore this if you won't filter on client IP
		 * since the default uhandled callback return is 0 meaning let the
		 * connection continue.
		 */
		case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
			libwebsockets_get_peer_addresses((int)(long)inUser, client_name,
			     sizeof(client_name), client_ip, sizeof(client_ip));
			printf("Received network connect from %s (%s)\n",
							client_name, client_ip);
			/* if we returned non-zero from here, we kill the connection */
			break;

		default:
			break;
	}

	return 0;
}

static int FindCmdHandler(
	const char							*inCommandName)
{
	int result=-1;
	for (int i=0, m=sHandlers.size(); i<m; i++)
	{
		if (strcasecmp(inCommandName,sHandlers[i].name)==0)
		{
			result=i;
			break;
		}
	}
	return result;
}

/*e*/
// registers a command callback making it accesible via the json web API
// copies the command name specified
void UEditServer::RegisterCmdHandler(
	const char							*inCommandName,
	TCmdCallback						inCallback)
{
	SCmdHandler	h;
	int			l=strlen(inCommandName);
	AssertThrowStr_(l<sizeof(h.name),"\pCmd name too long");
	memcpy(h.name,inCommandName,l+1);
	h.callback=inCallback;
	sHandlers.push_back(h);
}

/*e*/
// unregisters the cmd handler passed
void UEditServer::UnregisterCmdHandler(
	const char							*inCommandName,
	TCmdCallback						inCallback)
{
	int			idx=FindCmdHandler(inCommandName);
	AssertThrowStr_(idx!=-1,"\pCmd not registered");
	sHandlers.erase(sHandlers.begin()+idx);
}

// the inPayload ptr passed to this function should have k_jsonReplyPrePadding before it, and k_jsonReplyPostPadding after it
// inPayloadLen should not include these sizes
// the padding areas around inPayload will be written to by this function
// if the payload contains the " character, it must be escaped, ie \"
static void CmdPaddedReply(
	UEditServer::SCmdConnection			*inConnection,
	char								*inPayload,
	size_t								inPayloadLen)
{
	memcpy(inPayload-k_jsonOKPrefixLen,k_jsonOKPrefix,k_jsonOKPrefixLen);
	memcpy(inPayload+inPayloadLen,k_jsonOKPostfix,k_jsonOKPostfixLen);

	int	res=libwebsocket_write(inConnection->wsi,(unsigned char*)inPayload-k_jsonOKPrefixLen,k_jsonOKPrefixLen+inPayloadLen+k_jsonOKPostfixLen,LWS_WRITE_TEXT);
	if (res!=0)
	{
		ECHO("CmdError : error writing to websocket " << res << "\n");
	}
}

void CmdError(
	UEditServer::SCmdConnection			*inConnection,
	int									inErrCode,
	const char							*inStr)
{
	static const int	kJsonReplyLen=1024;
	char				reply[LWS_SEND_BUFFER_PRE_PADDING+kJsonReplyLen+LWS_SEND_BUFFER_POST_PADDING];
	char				*jsonData=reply+LWS_SEND_BUFFER_PRE_PADDING;

	snprintf(jsonData,kJsonReplyLen,
		"{ status : %d , statusstr : \"%s\" }",
		inErrCode,
		inStr);

	ECHO("CmdError : " << jsonData << "\n");

	int	res=libwebsocket_write(inConnection->wsi,(unsigned char*)jsonData,strlen(jsonData),LWS_WRITE_TEXT);
	if (res!=0)
	{
		ECHO("CmdError : error writing to websocket " << res << "\n");
	}
}

void CmdException(
	UEditServer::SCmdConnection			*inConnection,
	const LException					*inErr)
{
	unsigned char		errStr[257];

	ErrStr_(*inErr,errStr);
	errStr[errStr[0]+1]=0;
	CmdError(inConnection,ErrCode_(*inErr),(char*)errStr+1);
}

static int Callback_Cmd(
	struct libwebsocket_context			*inContext,
	struct libwebsocket					*inWsi,
	enum libwebsocket_callback_reasons	inReason,
	void								*inUser,
	void								*inData,
	size_t								inLen)
{
	UEditServer::SCmdConnection	*connection=(UEditServer::SCmdConnection*)inUser;
	switch (inReason)
	{
		case LWS_CALLBACK_ESTABLISHED:
			// init connection object here
			memset(connection,0,sizeof(*connection));
			connection->wsi=inWsi;
			break;

		case LWS_CALLBACK_CLOSED:
			// shutdown connection object here
			break;

		case LWS_CALLBACK_RECEIVE:
			// process cmd here
			Try_
			{
				json_error_t	error;
				json_t	*cmd=json_loadb((const char*)inData,inLen,0,&error);
				if (!cmd)
				{
					ECHO("Failed to parse json \n" <<
						"Text: " << error.text << "\n" << 
						"Source: " << error.source << "\n" << 
						"Line: " << error.line << "\n" << 
						"Column: " << error.column << "\n" << 
						"Positin: " << error.position << "\n");
					UTBException::ThrowEditorJsonParseFail();
				}
				Try_
				{
					const char	*cmds=json_string_value(GetJsonParamOfType(cmd,"command",JSON_STRING));
					int			h=FindCmdHandler(cmds);
					if (h==-1)
					{
						UTBException::ThrowEditorUnknownCmd(cmds);
					}
					sHandlers[h].callback(cmd,connection);
				}
				Catch_(err)
				{
					json_decref(cmd);
					throw;
				}
				json_decref(cmd);
			}
			Catch_(err)
			{
				CmdException(connection,&err);
			}
			break;
	}

	return 0;
}

static libwebsocket_context *sContext=NULL;
static const int kEditServerPort=7676;

static struct libwebsocket_protocols sProtocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",		/* name */
		Callback_HTTP,		/* callback */
		0					/* per_session_data_size */
	},
	{
		"tb-cmd-protocol",		/* name */
		Callback_Cmd,			/* callback */
		sizeof(UEditServer::SCmdConnection)	/* per_session_data_size */
	},
	{
		NULL, NULL, 0		/* End of list */
	}
};

struct libwebsocket_extension sExtensions[]=
{
	{ NULL, NULL, 0 }
};

static size_t CalcBase64Len(size_t inOrigData)
{
	size_t	len=(inOrigData+2)/3;		// div 3 with roundup
	return len*4;						// each 3 byte set produces 4 bytes of Base64 output
}

// This function was adapted from https://github.com/johnmccutchan/pal and has had minor alterations to fit into TB
// the original licence is in the function below
// returns 0 for success and -1 for error
// output_length should contain the length of the output buffer on entry and will be filled out with the used
// bytes on exit
static int Base64Encode(const unsigned char* data, size_t input_length, char* target, size_t* output_length)
{
/*
  Copyright (c) 2012 John McCutchan <john@johnmccutchan.com>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.

  3. This notice may not be removed or altered from any source
  distribution.
*/
#define palAssert		0&sizeof

	static const char Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static const char Pad64 = '=';
	size_t datalength = 0;
	unsigned char input[3];
	unsigned char output[4];
	size_t i;
	size_t srclength = input_length;
	const unsigned char* src = data;

	while (2 < srclength) {
		input[0] = *src++;
		input[1] = *src++;
		input[2] = *src++;
		srclength -= 3;

		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
		output[3] = input[2] & 0x3f;
		palAssert(output[0] < 64);
		palAssert(output[1] < 64);
		palAssert(output[2] < 64);
		palAssert(output[3] < 64);

		if (datalength + 4 > *output_length)
			return (-1);

		target[datalength++] = Base64[output[0]];
		target[datalength++] = Base64[output[1]];
		target[datalength++] = Base64[output[2]];
		target[datalength++] = Base64[output[3]];
	}

	if (0 != srclength) {
		input[0] = input[1] = input[2] = '\0';
		for (i = 0; i < srclength; i++)
			input[i] = *src++;

		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
		palAssert(output[0] < 64);
		palAssert(output[1] < 64);
		palAssert(output[2] < 64);

		if (datalength + 4 > *output_length)
			return (-1);
		target[datalength++] = Base64[output[0]];
		target[datalength++] = Base64[output[1]];
		if (srclength == 1)
			target[datalength++] = Pad64;
		else
			target[datalength++] = Base64[output[2]];
		target[datalength++] = Pad64;
	}

	*output_length = datalength;
	return 0;
}

/*e*/
// create a new handle containing the data from inHandleToCopy and convert it to base64
// resultant handle will have inPrePadLen bytes padding before the base64 data begins and inPostPadLen bytes afterwards
static Handle NewBase64HandleWithPadding(
	Handle			inHandleToCopy,
	size_t			inPrePadLen,
	size_t			inPostPadLen)
{
	size_t			len=GetHandleSize(inHandleToCopy);
	size_t			blen=CalcBase64Len(len);
	size_t			newLen=blen;
	Handle			out=::NewHandle(blen+inPrePadLen+inPostPadLen);
	ThrowIfMemFull_(out);
	int				res=Base64Encode((const unsigned char*)*inHandleToCopy,len,*out+inPrePadLen,&newLen);
	if (res!=0 || newLen!=blen)
	{
		::DisposeHandle(out);
		AssertThrow_(kTNTErr_InternalError);
	}
	return out;
}

/*e*/
// get json parameter, throw if it's missing
static json_t *GetJsonParam(
	json_t				*inObject,
	const char			*inKeyName)
{
	json_t				*res=json_object_get(inObject,inKeyName);
	if (!res)
	{
		UTBException::ThrowEditorMissingParameter(inKeyName);
	}
	return res;
}

static const char *JsonTypeToString(
	json_type			inType)
{
	switch (inType)
	{
		case JSON_OBJECT: return "JSON_OBJECT";
		case JSON_ARRAY: return "JSON_ARRAY";
		case JSON_STRING: return "JSON_STRING";
		case JSON_INTEGER: return "JSON_INTEGER";
		case JSON_REAL: return "JSON_REAL";
		case JSON_TRUE: return "JSON_TRUE";
		case JSON_FALSE: return "JSON_FALSE";
		case JSON_NULL: return "JSON_NULL";
	}
	return "JSON_UNKNOWN_TYPE!";
}

/*e*/
static json_t *GetJsonParamOfType(
	json_t				*inObject,
	const char			*inKeyName,
	json_type			inExpectedType)
{
	json_t				*value=GetJsonParam(inObject,inKeyName);
	if (json_typeof(value)!=inExpectedType)
	{
		UTBException::ThrowEditorParamWrongType(inKeyName,JsonTypeToString(inExpectedType),JsonTypeToString(json_typeof(value)));
	}
	return value;
}

/*e*/
static TProjectId GetProjectId(
	json_t				*inObject)
{
	json_int_t	id=json_integer_value(GetJsonParamOfType(inObject,"projectid",JSON_INTEGER));
	return TProjectId(id);
}

/*e*/
static CResourceContainer *GetProject(
	json_t				*inObject)
{
	TProjectId			id=GetProjectId(inObject);
	CResourceContainer	*container=NULL;
	for (TProjVector::iterator iter=sOpenProj.begin(), eiter=sOpenProj.end(); iter!=eiter; ++iter)
	{
		if (iter->id==id)
		{
			container=iter->container;
			break;
		}
	}
	if (!container)
	{
		UTBException::ThrowEditorInvalidProjectId(id);
	}
	return container;
}

/*e*/
static ResType GetResType(
	json_t				*inObject,
	const char			*inKeyName)
{
	json_t				*jstr=GetJsonParamOfType(inObject,inKeyName,JSON_STRING);
	const char			*str=json_string_value(jstr);
	if (!str || strlen(str)!=4)
	{
		UTBException::ThrowResTypeWrongLength();
	}
	ResType	t;
	memcpy(&t,str,4);
#if TARGET_RT_LITTLE_ENDIAN
	t=CFSwapInt32BigToHost(t);
#endif
	return t;
}

/*e*/
static TResId GetResId(
	json_t				*inObject,
	const char			*inKeyName)
{
	json_t				*v=GetJsonParamOfType(inObject,inKeyName,JSON_INTEGER);
	return TResId(json_integer_value(v));
}

/*e*/
static void CmdGetResource(
	json_t						*inCmd,
	UEditServer::SCmdConnection	*inClient)
{
	ResType				type=GetResType(inCmd,"type");
	SInt32				id=GetResId(inCmd,"id");
	CResourceContainer	*container=GetProject(inCmd);
	StTNTResource		res(container,type,id);
	StHandleReleaser	rel(NewBase64HandleWithPadding(res->GetReadOnlyDataHandle(),k_jsonReplyPrePadding,k_jsonReplyPostPadding));
	UHandleLocker		lock(rel.GetHandle());

	CmdPaddedReply(inClient,*rel+k_jsonReplyPrePadding,::GetHandleSize(rel.GetHandle())-k_jsonReplyPrePadding-k_jsonReplyPostPadding);
}

/*e*/
void UEditServer::Initialise()
{
	AssertThrow_(!sContext);
	sContext=libwebsocket_create_context(kEditServerPort,NULL,sProtocols,sExtensions,NULL,NULL,-1,-1,0);
	ThrowIfMemFull_(sContext);

	// first see if there is a folder called 'editor' in the app directory, if so, use that as our editor
	FSRef		ref;
	CFURLRef	url;
	CFURLRef	baseUrl;
	GetAppFSRef(&ref);
	url=CFURLCreateFromFSRef(kCFAllocatorDefault,&ref);
	baseUrl=CFURLCreateCopyDeletingLastPathComponent(kCFAllocatorDefault,url);
	CFRelease(url);
	url=CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault,baseUrl,CFSTR("editor/index.html"),false);

	CFRelease(baseUrl);
	Boolean		ok=CFURLGetFSRef(url,&ref);

	if (ok)
	{
		ECHO("Found editor in app's directory\n");
		sEditorURL=url;
	}
	else
	{
		ECHO("Using editor in app's resources\n");
		CFRelease(url);

		CFBundleRef		app=CFBundleGetMainBundle();
		sEditorURL=CFBundleCopyResourceURL(app,CFSTR("index"),CFSTR("html"),CFSTR("editor"));
	}
	
	AssertThrow_(sEditorURL);

	RegisterCmdHandler("getresource",CmdGetResource);
}

void UEditServer::Shutdown()
{
	for (TProjVector::iterator iter=sOpenProj.begin(), eiter=sOpenProj.end(); iter!=eiter; ++iter)
	{
		SProject	&proj=*iter;
		Try_
		{
			proj.container->Flush();
		}
		Catch_(err)
		{
			ECHO("Error flushing CResourceContainer\n");
		}
		delete proj.container;
	}
	sOpenProj.clear();

	if (sContext)
	{
		libwebsocket_context_destroy(sContext);
		sContext=NULL;
	}
	sHandlers.clear();
	if (sEditorURL)
	{
		CFRelease(sEditorURL);
		sEditorURL=0;
	}
}

void UEditServer::Tick()
{
	if (sContext)
	{
		libwebsocket_service(sContext,0);
	}
}

static void OpenEditor(
	SProject		*inProj)
{
	CFMutableStringRef		mutableStr=CFStringCreateMutableCopy(kCFAllocatorDefault,512,CFURLGetString(sEditorURL));
	CFStringAppendFormat(mutableStr,NULL,CFSTR("?projectid=%d"),inProj->id);
	CFURLRef				withParams=CFURLCreateWithString(kCFAllocatorDefault,mutableStr,NULL);
	CFRelease(mutableStr);
	OSStatus				err=LSOpenCFURLRef(withParams,NULL);
	ECHO("Opening editor for " << withParams << " : ");
	CFRelease(withParams);
	if (err)
	{
		ECHO("failed " << err << "\n");
	}
	else
	{
		ECHO("OK\n");
	}
}

/*e*/
void UEditServer::OpenForEdit(
	FSSpec			*inSpec)
{
	CResourceContainerCreator *rcc=CResourceContainerCreator::GetCreatorForFile(inSpec);
	if (!rcc)
	{
		Throw_(kTBErr_UnrecognisedResFileFormat);
	}
	CResourceContainer		*rc=rcc->OpenFile(inSpec,false);
	SProject	proj={sRollingId++,rc};
	sOpenProj.push_back(proj);
	OpenEditor(&sOpenProj.back());
}
