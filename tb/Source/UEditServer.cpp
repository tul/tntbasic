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

struct SCmdHandler
{
	char						name[64];
	UEditServer::TCmdCallback	callback;
};

static std::vector<SCmdHandler>			s_handlers;

static int FindCmdHandler(
	const char							*inCommandName)
{
	int result=-1;
	for (int i=0, m=s_handlers.size(); i<m; i++)
	{
		if (strcasecmp(inCommandName,s_handlers[i].name)==0)
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
	s_handlers.push_back(h);
}

/*e*/
// unregisters the cmd handler passed
void UEditServer::UnregisterCmdHandler(
	const char							*inCommandName,
	TCmdCallback						inCallback)
{
	int			idx=FindCmdHandler(inCommandName);
	AssertThrowStr_(idx!=-1,"\pCmd not registered");
	s_handlers.erase(s_handlers.begin()+idx);
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
	LException							*inErr)
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
			if (connection->curCmd)
			{
				json_decref(connection->curCmd);
			}
			break;

		case LWS_CALLBACK_RECEIVE:
			// process cmd here
			ECHO("Received cmd : ");
			UBkgConsole::gLogFile->write((const char*)inData,inLen);
			ECHO("\n");
			CmdError(connection,-1,"TOOD :)");
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

/*e*/
void UEditServer::Initialise()
{
	AssertThrow_(!sContext);
	sContext=libwebsocket_create_context(kEditServerPort,NULL,sProtocols,sExtensions,NULL,NULL,-1,-1,0);
	ThrowIfMemFull_(sContext);
}

void UEditServer::Shutdown()
{
	if (sContext)
	{
		libwebsocket_context_destroy(sContext);
		sContext=NULL;
	}
	s_handlers.clear();
}

void UEditServer::Tick()
{
	if (sContext)
	{
		libwebsocket_service(sContext,0);
	}
}

/*e*/
void UEditServer::OpenForEdit(
	FSSpec			*inSpec)
{
	//CResourceContainerCreator::GetCreatorForFile(inSpec);
}
