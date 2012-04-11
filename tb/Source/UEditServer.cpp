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
}

void UEditServer::Tick()
{
	if (sContext)
	{
		libwebsocket_service(sContext,0);
	}
}
