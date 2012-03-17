// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UFuse.cpp
// © John Treece-Birch 2000
// 18/3/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
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

#include		"UFuse.h"
#include		"TNT_Debugging.h"
#include		"Utility Objects.h"
#include		"UCursorManager.h"
#include		"Marks Routines.h"
#include		"CGraphicsContext16.h"

#include		<UDesktop.h>
#include		<LEditText.h>

// the BkgConsole application to view the output.
#include		"UBkgConsole.h"
#include		<PDebug.h>

NSpGameReference			UFuse::sGame;

bool						UFuse::sInitialised=false;
bool						UFuse::sGameRunning=false;
bool						UFuse::sHost=false;
bool						UFuse::sSynced=false;

UInt16						UFuse::sPlayers=0;
UInt16						UFuse::sMaxPlayers=0;
UInt16						UFuse::sStartingPlayers=0;

UInt16						UFuse::sSyncMessages=0;

SInt16						UFuse::sEventCounter=0;
SInt16						UFuse::sTotalEvents=0;
CFuseEvent					UFuse::sEvents[UFuse::kMaxEvents];

UInt16						UFuse::sID[UFuse::kMaxPlayers];
Str255						UFuse::sPlayerName[UFuse::kMaxPlayers];

StDialogHandler*			UFuse::sWaitingDialog=0L;

SInt32						UFuse::sTime;
SInt16						UFuse::sCurrentMessage;
CFuseGeneralMessage			UFuse::sMessage[UFuse::kMaxMessages];

bool						UFuse::sUpdateList;
bool						UFuse::sWaiting[kMaxPlayers];
bool						UFuse::sInGame[kMaxPlayers];
bool						UFuse::sPaused[kMaxPlayers];

CProgram*					UFuse::sProgram=NULL;

bool						UFuse::sRegisteredMessages=false;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Initialise						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Initialise the network for the type of game
void UFuse::Initialise(
	NSpGameID		inGameID,
	CProgram		&inProgram)
{
	sTime=0L;
	sCurrentMessage=0;

	sProgram=&inProgram;

	if (!sInitialised)
	{
		// Check if net sprocket is present
		ThrowIfOSStatus_(::NSpInitialize(sizeof(CFuseIntMessage),10000,100,inGameID,0));
		sInitialised=true;
		
		ThrowIfOSStatus_(::NSpInstallJoinRequestHandler((NSpJoinRequestHandlerProcPtr)UFuse::JoinRequestHandler,NULL));
	
		sRegisteredMessages=false;
	}
	
	ClearAllMessages();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShutDown
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::ShutDown()
{
	LeaveGame();
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Host
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::Host(
	UInt16				inMaxPlayers,
	Str31				inGameName,
	Str31 				inPlayerName,
	CGraphicsContext16	*inContext)
{
	UPortSaver						safe;
	NSpProtocolListReference		protocolList;

	Try_
	{
		if (sInitialised)
		{
			StDesktopDeactivator	deactivator;
			ThrowIfOSStatus_(::NSpProtocolList_New(NULL,&protocolList));
			Str31					password="\p";
			
			if (HostDialog(protocolList,inGameName,inPlayerName,password))
			{
				// Update the background
				if (inContext)
				{
					inContext->InvalAll();
					inContext->RenderSpritesAndBlit();
				}
			
				ThrowIfOSStatus_(::NSpGame_Host(&sGame,protocolList,inMaxPlayers,inGameName,password,inPlayerName,0,kNSpClientServer,0));

				sHost=true;
				sMaxPlayers=inMaxPlayers;
				sPlayers=0;
	
				if (HostWaitForPlayers())
				{
					::NSpGame_EnableAdvertising(sGame,0L,false);
					sGameRunning=true;
					
					sTime=0;
					sSyncMessages=0;
					sCurrentMessage=0;
					ClearAllMessages();
					sSynced=false;

					sEventCounter=0;
					sTotalEvents=0;
					
					sWaitingDialog=0L;
					
					// Tell all players the game is starting
					SendSimpleInternalMessage(kStartGameMessage,kNSpSendFlag_Registered);
					::NSpProtocolList_Dispose(protocolList);
					
					BuildIDList();
					
					return true;
				}
				else
					::NSpGame_Dispose(sGame,kNSpGameFlag_ForceTerminateGame);
			}
			
			// Update the background
			if (inContext)
			{
				inContext->InvalAll();
				inContext->RenderSpritesAndBlit();
			}
			
			::NSpProtocolList_Dispose(protocolList);
		}
		
		return false;
	}
	
	Catch_(err)
	{
		if (sGameRunning)
		{
			sGameRunning=false;
			::NSpGame_Dispose(sGame,kNSpGameFlag_ForceTerminateGame);
		}
		
		::NSpProtocolList_Dispose(protocolList);
		throw err;
	}
	
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HostDialog
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::HostDialog(
	NSpProtocolListReference		&protocolList,
	Str31							ioGameName,
	Str31 							ioPlayerName,
	Str31 							ioPassword)
{
	StDialogHandler		dialog(132,LCommander::GetTopCommander());
	bool				done=false,ok=false;
	LWindow				*grafPortView=dialog.GetDialog();
	
	LEditText			*control;
	LControl			*popUp;
	
	// Set initial values
	if (control=(LEditText*)grafPortView->FindPaneByID(kHostDialogPlayerName))
		control->SetText(&ioPlayerName[1],ioPlayerName[0]);
	if (control=(LEditText*)grafPortView->FindPaneByID(kHostDialogPassword))
		control->SetText(&ioPassword[1],ioPassword[0]);
	
	while (!done)
	{
		switch (dialog.DoDialog())
		{
			case msg_Cancel:
				done=true;
				ok=false;
				break;
		
			case msg_OK:
				done=true;
				ok=true;
				break;
		}
	}
	
	if (ok)
	{
		long	length;
		
		if (control=(LEditText*)grafPortView->FindPaneByID(kHostDialogPlayerName))
		{
			control->GetText((char*)&ioPlayerName[1],31,&length);
			ioPlayerName[0]=length;
		}
		
		if (control=(LEditText*)grafPortView->FindPaneByID(kHostDialogPassword))
		{
			control->GetText((char*)&ioPassword[1],31,&length);
			ioPassword[0]=length;
		}
		
		// Create the protocol
		NSpProtocolReference	ipProtocol=::NSpProtocol_CreateIP(kTCPPort,0,0);
		ThrowIfOSStatus_(::NSpProtocolList_Append(protocolList,ipProtocol));
	}
	
	return ok;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HostWaitForPlayers
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::HostWaitForPlayers()
{
	StDialogHandler		dialog(128,LCommander::GetTopCommander());
	bool				done=false,ok=false;
	TFuseError			result;
	
	LWindow				*grafPortView=dialog.GetDialog();
	
	while (!done)
	{
		switch (dialog.DoDialog())
		{
			case msg_Nothing:
				result=HandleMessages(grafPortView,done,ok);
				
				switch (result)
				{
					case kNoErr:
						break;
				}
				break;
				
			case msg_KickedOut:
				KickOutPlayer(grafPortView);
				break;
		
			case msg_Cancel:
				done=true;
				ok=false;
				break;
		
			case msg_OK:
				done=true;
				ok=true;
				break;	
		}
	}
	
	return ok;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Join						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::Join(
	NSpGameID				inGameType,
	const Str31				inString,
	Str31					inPlayerName,
	Str31					inPassword,
	CGraphicsContext16		*inContext)
{
	UPortSaver					safe;
	NSpAddressReference			address=0L;
	OSStatus					err;
	
	Try_
	{
		if (sInitialised)
		{
			StDesktopDeactivator	deactivator;
			Str31					gameCreator="\pxxxx";
			*(OSType*)&gameCreator[1]=inGameType;
		
			if (JoinDialog(address,inPlayerName,inPassword))
			{
				err=::NSpGame_Join(&sGame,address,inPlayerName,inPassword,0L,0L,0L,0L);
				if (!err)
				{
					// Update the background
					if (inContext)
					{
						inContext->InvalAll();
						inContext->RenderSpritesAndBlit();
					}
				
					if (!JoinWaitForPlayers())
						::NSpGame_Dispose(sGame,kNSpGameFlag_ForceTerminateGame);
					else
					{
						// Initialise the variables	
						sGameRunning=true;
						sHost=false;
						
						sTime=0;
						sSyncMessages=0;
						sCurrentMessage=0;
						ClearAllMessages();
						sSynced=false;
						
						sEventCounter=0;
						sTotalEvents=0;
						
						sWaitingDialog=0L;
						
						::NSpReleaseAddressReference(address);
						
						BuildIDList();
						
						//ThrowIfOSStatus_(::NSpInstallAsyncMessageHandler((NSpMessageHandlerProcPtr)UFuse::HandleMessage,NULL));
						return true;
					}
				}
				
				::NSpReleaseAddressReference(address);
			}
			
			// Update the background
			if (inContext)
			{
				inContext->InvalAll();
				inContext->RenderSpritesAndBlit();
			}
		}
		
		return false;
	}
	
	Catch_(err)
	{
		if (sGameRunning)
			::NSpGame_Dispose(sGame,kNSpGameFlag_ForceTerminateGame);
	
		if (address)
			::NSpReleaseAddressReference(address);
			
		throw(err);
	}
	
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ JoinDialog
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::JoinDialog(
	NSpAddressReference			&outAddressReference,
	Str31 						ioPlayerName,
	Str31 						ioPassword)
{
	StDialogHandler		dialog(133,LCommander::GetTopCommander());
	bool				done=false,ok=false;
	LWindow				*grafPortView=dialog.GetDialog();
	
	LEditText			*control;
	LControl			*popUp;
	LStaticText			*staticText;
	SInt32				protocol=0;
	
	// Set initial values
	if (control=(LEditText*)grafPortView->FindPaneByID(kJoinDialogPlayerName))
		control->SetText(&ioPlayerName[1],ioPlayerName[0]);
	if (control=(LEditText*)grafPortView->FindPaneByID(kJoinDialogPassword))
		control->SetText(&ioPassword[1],ioPassword[0]);
	
	while (!done)
	{
		switch (dialog.DoDialog())
		{	
			case msg_Cancel:
				done=true;
				ok=false;
				break;
		
			case msg_OK:
				done=true;
				ok=true;
				break;
		}
	}
	
	if (ok)
	{
		char	gameName[256],netType[5]="TNTB";
		long	length;
		
		if (control=(LEditText*)grafPortView->FindPaneByID(kJoinDialogPlayerName))
		{
			control->GetText((char*)&ioPlayerName[1],31,&length);
			ioPlayerName[0]=length;
		}
		
		if (control=(LEditText*)grafPortView->FindPaneByID(kJoinDialogGameName))
		{
			control->GetText((char*)&gameName[0],31,&length);
			gameName[length]=0;
		}
		
		if (control=(LEditText*)grafPortView->FindPaneByID(kJoinDialogPassword))
		{
			control->GetText((char*)&ioPassword[1],31,&length);
			ioPassword[0]=length;
		}
		
		// Create the address reference
		char            tcpPortStr[32];	
		std::sprintf(tcpPortStr,"%i",kTCPPort);
				
		outAddressReference=::NSpCreateIPAddressReference(gameName,tcpPortStr);
	}
	
	return ok;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ JoinWaitForPlayers
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::JoinWaitForPlayers()
{
	StDialogHandler		dialog(129,LCommander::GetTopCommander());
	bool				done=false,ok=false;
	TFuseError		result;
	
	LWindow				*grafPortView=dialog.GetDialog();
	
	while (!done)
	{
		switch (dialog.DoDialog())
		{
			case msg_Nothing:
				result=HandleMessages(grafPortView,done,ok);
				
				switch (result)
				{
					case kNoErr:
						break;
					
					case kGameAlreadyRunning:
						DisplayMessage("\pThis game is already running.");
						break;
					
					case kHostQuitGame:
						DisplayMessage("\pThe host terminated the game.");
						break;
						
					case kPlayerKickedOut:
						DisplayMessage("\pYou have been kicked out of the game.");
						break;
				}
				break;
		
			case msg_Cancel:
				done=true;
				ok=false;
				break;
		}
	}
	
	return ok;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HandleMessages
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TFuseError UFuse::HandleMessages(LWindow *inGrafPort,bool &outDone,bool &outok)
{
	TFuseError	error=kNoErr;

	while (NSpMessageHeader	*message=::NSpMessage_Get(sGame))
	{
		switch (message->what)
		{			
			case kNSpPlayerJoined:
			case kNSpPlayerLeft:
				RebuildPlayersList(inGrafPort);
				break;

			case kNSpJoinDenied:
				outDone=true;
				outok=false;
				return kGameAlreadyRunning;
				break;

			case kNSpGameTerminated:
			case kNSpHostChanged:
				outDone=true;
				outok=false;
				return kHostQuitGame;
				break;

			case kKickedOutMessage:
				if (KickedOut((CKickOutMessage*)message))
				{
					outDone=true;
					outok=false;
					error=kPlayerKickedOut;
				}
				break;
			
			case kStartGameMessage:
				outDone=true;
				outok=true;
				break;
		}
		
		::NSpMessage_Release(sGame,message);
	}
	
	return error;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RebuildPlayersList
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::RebuildPlayersList(LWindow *inGrafPort)
{
	OSErr						err;
	NSpPlayerEnumerationPtr		players;
	
	// Grab all the players in the game
	if (err=::NSpPlayer_GetEnumeration(sGame,&players))
		return;
	
	// List all the players
	sPlayers=players->count;
	
	LTextColumn	*list=(LTextColumn*)inGrafPort->FindPaneByID('pltb');
	if (list)
	{
		list->RemoveAllRows(true);	
	
		// Add all the players to the list
		for (SInt16 count=0; count<sPlayers; count++)
		{		
			CopyPStr(players->playerInfo[count]->name,sPlayerName[count]);
			list->InsertRows(1,65535,&sPlayerName[count][1],sPlayerName[count][0],true);
		}
	}

	if (sHost)
	{
		LStaticText	*text=(LStaticText*)inGrafPort->FindPaneByID('sttx');
		if (text)
		{
			if (sPlayers==sMaxPlayers)
				text->SetText("Game is full",12);
			else
				text->SetText("Waiting for players...",22);
		}
	}

	// Release player info
	::NSpPlayer_ReleaseEnumeration(sGame,players);
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendFuseInteger					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendFuseInteger(SInt32 inType,SInt32 inContent)
{
	if (sGameRunning)
	{
		CFuseIntMessage	message;
		
		::NSpClearMessageHeader(message.GetHeader());
		message.GetHeader()->to=kNSpAllPlayers;
		message.GetHeader()->what=kFuseIntMessage;
		message.GetHeader()->messageLen=sizeof(CFuseIntMessage);
		
		message.SetType(inType);
		message.SetInteger(inContent);
		message.SetAge(sTime);

		if (sRegisteredMessages)
			::NSpMessage_Send(sGame,message.GetHeader(),kNSpSendFlag_Registered);
		else
			::NSpMessage_Send(sGame,message.GetHeader(),kNSpSendFlag_Normal);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendFuseFloat					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendFuseFloat(SInt32 inType,float inContent)
{
	if (sGameRunning)
	{
		CFuseFloatMessage	message;
		
		::NSpClearMessageHeader(message.GetHeader());
		message.GetHeader()->to=kNSpAllPlayers;
		message.GetHeader()->what=kFuseFloatMessage;
		message.GetHeader()->messageLen=sizeof(CFuseFloatMessage);
		
		message.SetType(inType);
		message.SetFloat(inContent);
		message.SetAge(sTime);

		if (sRegisteredMessages)
			::NSpMessage_Send(sGame,message.GetHeader(),kNSpSendFlag_Registered);
		else
			::NSpMessage_Send(sGame,message.GetHeader(),kNSpSendFlag_Normal);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendFuseString					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendFuseString(SInt32 inType,CCString &inContent)
{
	if (sGameRunning)
	{
		CFuseStringMessage		message;
		
		::NSpClearMessageHeader(message.GetHeader());
		message.GetHeader()->to=kNSpAllPlayers;
		message.GetHeader()->what=kFuseStringMessage;
		message.GetHeader()->messageLen=sizeof(CFuseStringMessage);
				
		message.SetType(inType);		
		message.SetString(inContent);
		message.SetAge(sTime);

		if (sRegisteredMessages)
			::NSpMessage_Send(sGame,message.GetHeader(),kNSpSendFlag_Registered);
		else
			::NSpMessage_Send(sGame,message.GetHeader(),kNSpSendFlag_Normal);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendSimpleInternalMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendSimpleInternalMessage(
	SInt32		inWhat,
	SInt32		inSendType)
{
	if (sGameRunning)
	{
		::NSpMessage_SendTo(sGame,kNSpAllPlayers,inWhat,NULL,0,inSendType);
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MoreMessages
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::MoreMessages()
{
	if (sGameRunning)
	{
		if (!sSynced)
		{
			GetAllMessages();
			
			sCurrentMessage=-1;
		}
	
		// Look for the next valid message
		for (UInt16 n=sCurrentMessage+1; n<kMaxMessages; n++)
		{
			if (sMessage[n].GetAge()==sTime)
			{
				sCurrentMessage=n;
				return true;
			}
		}
	}
	
	sTime++;
	sSynced=false;
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetMessageType
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 UFuse::GetMessageType()
{
	if (sGameRunning)
	{
		return sMessage[sCurrentMessage].GetType();
	}

	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetMessageSender
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 UFuse::GetMessageSender()
{
	if (sGameRunning)
	{
		return GetPlayerID(sMessage[sCurrentMessage].GetHeader()->from);
	}

	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetMessageDataType
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 UFuse::GetMessageDataType()
{
	if (sGameRunning)
	{
		return sMessage[sCurrentMessage].GetDataType();
	}

	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetMessageInteger
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 UFuse::GetMessageInteger()
{
	if (sGameRunning)
	{
		return sMessage[sCurrentMessage].GetInteger();
	}

	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetMessageFloat
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
float UFuse::GetMessageFloat()
{
	if (sGameRunning)
	{
		return sMessage[sCurrentMessage].GetFloat();
	}

	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetMessageString
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
UInt8* UFuse::GetMessageString()
{
	if (sGameRunning)
	{
		return sMessage[sCurrentMessage].GetString();
	}

	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetAllMessages
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::GetAllMessages()
{
	UPortSaver			safe;
	UInt32				startTicks=::TickCount();

	if (sRegisteredMessages)
		SendSimpleInternalMessage(kSyncMessage,kNSpSendFlag_Registered);
	else
		SendSimpleInternalMessage(kSyncMessage,kNSpSendFlag_Normal);

	// Wait until everyone has sent messages
	while (sPlayers>sSyncMessages+1)
	{
		// Handle any messages
		if (NSpMessageHeader *message=::NSpMessage_Get(sGame))
		{
			switch (message->what)
			{
				case kNSpError:
					break;
					
				case kNSpJoinRequest:
					break;
					
				case kNSpJoinApproved:
					break;
					
				case kNSpJoinDenied:
					break;
					
				case kNSpPlayerJoined:
					break;
					
				case kNSpPlayerLeft:
					break;
				
				case kNSpHostChanged:
					if (((NSpHostChangedMessage*)message)->newHost==GetMyID())
						sHost=true;
					break;
				
				case kNSpGameTerminated:
					sGameRunning=false;
					AddEvent(CFuseEvent::kGameTerminated,0);
					break;
			
				case kPlayerLeftMessage:
					sPlayers--;
					sInGame[GetPlayerID(message->from)]=false;
					AddEvent(CFuseEvent::kPlayerLeft,GetPlayerID(message->from));
					sUpdateList=true;
					break;
					
				case kDisconnectPlayerMessage:
					UInt16	id=((CDisconnectPlayerMessage*)message)->GetID();
					if (id!=GetMyID())
					{	
						// Remove the player from the game
						sPlayers--;
						sInGame[id]=false;
						AddEvent(CFuseEvent::kPlayerDisconnected,id);
						sUpdateList=true;
					}
					else
					{	// Remove self from the game
						
						// Delete the game field
						OSStatus	err=::NSpGame_Dispose(sGame,0L);
						if ((sHost) && (err))
							err=::NSpGame_Dispose(sGame,kNSpGameFlag_ForceTerminateGame);
						
						// Update the variables
						sGameRunning=false;
						sHost=false;
						sSynced=false;
						
						sSyncMessages=0;
						
						sPlayers=0;
						sMaxPlayers=0;
					}
					break;
			
				case kSyncMessage:
					sSyncMessages++;
					sWaiting[GetPlayerID(message->from)]=false;
					sUpdateList=true;
					break;

				case kPausedMessage:
					sPaused[GetPlayerID(message->from)]=true;
					break;

				case kFuseIntMessage:
					StoreIntMessage(message);
					break;
					
				case kFuseFloatMessage:
					StoreFloatMessage(message);
					break;
					
				case kFuseStringMessage:
					StoreStringMessage(message);
					break;
					
				default:
					SysBeep(0);
					break;
			}
			
			::NSpMessage_Release(sGame,message);
		}

		// If we have been waiting a while then bring up a dialog asking to disconnect them
		if ((startTicks+kTimeOutTicks)<TickCount())
		{
			if (!sProgram->IsPaused())
				sProgram->Pause();
		
			if (!sWaitingDialog)
			{
				sWaitingDialog=new StDialogHandler(131,LCommander::GetTopCommander());
				sUpdateList=true;
			}
			
			switch (sWaitingDialog->DoDialog())
			{
				case msg_Nothing:
					UpdateNonSyncedList(sWaitingDialog->GetDialog());
					break;
			
				case msg_OK:
					DisconnectNonSyncedPlayers();
					break;
			}
		}
	}

	// Close the dialog if it is showing
	if (sWaitingDialog)
	{
		delete sWaitingDialog;
		sWaitingDialog=0L;
	}
	
	if (sProgram->IsPaused())
		sProgram->Resume();
	
	// Determine which players we will be waiting for
	for (UInt16 n=0; n<sStartingPlayers; n++)
	{
		sWaiting[n]=sInGame[n];
		sPaused[n]=false;
	}
	
	// Don't wait for self
	sWaiting[GetMyID()]=false;
				
	sSyncMessages=0;
	sSynced=true;
}

void UFuse::ClearAllMessages()
{
	for (UInt16 n=0; n<kMaxMessages; n++)
	{
		sMessage[n].SetAge(-1);
	}
}

void UFuse::StoreIntMessage(NSpMessageHeader *message)
{
	for (UInt16 n=0; n<kMaxMessages; n++)
	{
		if (sMessage[n].GetAge()<sTime)
		{
			sMessage[n].SetHeader(((CFuseIntMessage*)message)->GetHeader());		
			sMessage[n].SetDataType(kIntegerDataType);
			
			sMessage[n].SetType(((CFuseIntMessage*)message)->GetType());
			sMessage[n].SetInteger(((CFuseIntMessage*)message)->GetInteger());
			sMessage[n].SetAge(((CFuseIntMessage*)message)->GetAge());

			n=kMaxMessages;
		}
	}
}

void UFuse::StoreFloatMessage(NSpMessageHeader *message)
{
	for (UInt16 n=0; n<kMaxMessages; n++)
	{
		if (sMessage[n].GetAge()<sTime)
		{
			sMessage[n].SetHeader(((CFuseFloatMessage*)message)->GetHeader());
			sMessage[n].SetDataType(kFloatDataType);
			
			sMessage[n].SetType(((CFuseFloatMessage*)message)->GetType());
			sMessage[n].SetFloat(((CFuseFloatMessage*)message)->GetFloat());
			sMessage[n].SetAge(((CFuseFloatMessage*)message)->GetAge());

			n=kMaxMessages;
		}
	}
}

void UFuse::StoreStringMessage(NSpMessageHeader *message)
{
	for (UInt16 n=0; n<kMaxMessages; n++)
	{
		if (sMessage[n].GetAge()<sTime)
		{
			sMessage[n].SetHeader(((CFuseStringMessage*)message)->GetHeader());		
			sMessage[n].SetDataType(kStringDataType);
			
			sMessage[n].SetType(((CFuseStringMessage*)message)->GetType());
			sMessage[n].SetString(((CFuseStringMessage*)message)->GetString());
			sMessage[n].SetAge(((CFuseStringMessage*)message)->GetAge());

			n=kMaxMessages;
		}
	}
}

#pragma mark -

void UFuse::UpdateNonSyncedList(LWindow *inGrafPort)
{
	if (sUpdateList)
	{
		OSErr						err;
		NSpPlayerEnumerationPtr		players;
		
		// Grab all the players in the game
		if (err=::NSpPlayer_GetEnumeration(sGame,&players))
			return;
		
		// List all the players
		sPlayers=players->count;
		
		if (LTextColumn	*list=(LTextColumn*)inGrafPort->FindPaneByID('pltb'))
		{
			list->RemoveAllRows(true);	
		
			// Add all the players to the list
			for (SInt16 count=0; count<sPlayers; count++)
			{
				if (sWaiting[GetPlayerID(players->playerInfo[count]->id)])
				{
					Str255		string;
				
					CopyPStr(players->playerInfo[count]->name,string);
				
					if (sPaused[GetPlayerID(players->playerInfo[count]->id)])
						ConcatPStr(string,"\p (Paused)");
					else
						ConcatPStr(string,"\p (No Current Connection)");
				
					list->InsertRows(1,65535,&string[1],string[0],true);
				}
			}
		}

		// Release player info
		::NSpPlayer_ReleaseEnumeration(sGame,players);
		
		sUpdateList=false;
	}
}

void UFuse::DisconnectNonSyncedPlayers()
{
	for (UInt16 n=0; n<sStartingPlayers; n++)
	{
		if (sWaiting[n])
		{
			CDisconnectPlayerMessage		message;
				
			::NSpClearMessageHeader(message.GetHeader());
			message.GetHeader()->to=kNSpAllPlayers;
			message.GetHeader()->what=kDisconnectPlayerMessage;
			message.GetHeader()->messageLen=sizeof(CDisconnectPlayerMessage);
			message.SetID(n);

			::NSpMessage_Send(sGame,message.GetHeader(),kNSpSendFlag_Registered);

			// Update own variables
			sPlayers--;
			sInGame[n]=false;
			AddEvent(CFuseEvent::kPlayerDisconnected,n);
			sUpdateList=true;
		}
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MoreNetEvents
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::MoreNetEvents()
{
	if (sEventCounter<sTotalEvents)
	{
		sEventCounter++;		
		return true;
	}
	
	sEventCounter=0;
	sTotalEvents=0;
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNetEventType
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 UFuse::GetNetEventType()
{
	if (sEventCounter>0)
		return sEvents[sEventCounter-1].type;
		
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNetEventContent
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 UFuse::GetNetEventContent()
{
	if (sEventCounter>0)
		return sEvents[sEventCounter-1].content;
		
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddEvent
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::AddEvent(SInt32 inType,SInt32 inContent)
{
	if (sTotalEvents<kMaxEvents)
	{
		sEvents[sTotalEvents].type=inType;
		sEvents[sTotalEvents].content=inContent;
		
		sTotalEvents++;
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LeaveGame
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::LeaveGame()
{
	if (sGameRunning)
	{
		// Send the leaving message
		SendSimpleInternalMessage(kPlayerLeftMessage,kNSpSendFlag_Registered);
	
		// Delete the game field
		OSStatus	err=::NSpGame_Dispose(sGame,0L);
		if ((sHost) && (err))
			err=::NSpGame_Dispose(sGame,kNSpGameFlag_ForceTerminateGame);
		
		// Update the variables
		sGameRunning=false;
		sHost=false;
		sSynced=false;
		
		sSyncMessages=0;
		
		sPlayers=0;
		sMaxPlayers=0;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Pause
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::Pause()
{
	if (sGameRunning)
	{
		SendSimpleInternalMessage(kPausedMessage,kNSpSendFlag_Registered);
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ KickOutPlayer
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::KickOutPlayer(LWindow *inGrafPort)
{
	NSpPlayerEnumerationPtr		players;
	OSErr						err;
	STableCell					theCell;
	
	// Grab all the players in the game
	if (err=::NSpPlayer_GetEnumeration(sGame,&players))
		return;

	LTextColumn	*list=(LTextColumn*)inGrafPort->FindPaneByID('pltb');
	if (list)
	{
		for (SInt16 count=0; count<sPlayers; count++)
		{		
			list->IndexToCell(count,theCell);
			
			if (!list->CellIsSelected(theCell))
			{
				SendKickOutMessage(players->playerInfo[count]->id);
			}
		}
	}
	
	::NSpPlayer_ReleaseEnumeration(sGame,players);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ KickedOut
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::KickedOut(CKickOutMessage *inMessage)
{
	if (!sHost)
	{
		if (inMessage->GetID()==GetMyID())
			return true;
	}
	
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendKickOutMessage				/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendKickOutMessage(NSpPlayerID inNumber)
{
	CKickOutMessage		message;
	
	::NSpClearMessageHeader(message.GetHeader());
	message.GetHeader()->to=kNSpAllPlayers;
	message.GetHeader()->what=kKickedOutMessage;
	message.GetHeader()->messageLen=sizeof(CKickOutMessage);

	message.SetID(inNumber);

	OSStatus		status=::NSpMessage_Send(sGame,message.GetHeader(),kNSpSendFlag_Registered);
	ThrowIfOSErr_(status);
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetMyID
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
UInt16 UFuse::GetMyID()
{
	if (sGame)
		return GetPlayerID(::NSpPlayer_GetMyID(sGame));

	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetPlayerID
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
UInt16 UFuse::GetPlayerID(UInt16 inNetID)
{
	for (UInt16 n=0; n<sStartingPlayers; n++)
	{
		if (sID[n]==inNetID)
			return n;
	}

	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetPlayerName
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
UInt8* UFuse::GetPlayerName(SInt32 inID)
{
	return sPlayerName[inID];
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BuildIDList
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::BuildIDList()
{
	NSpPlayerEnumerationPtr		players;

	sStartingPlayers=sPlayers;
	
	// Grab all the players in the game
	if (OSErr err=::NSpPlayer_GetEnumeration(sGame,&players))
		return;
	
	// Add all the players to the list
	for (UInt16 count=0; count<sStartingPlayers; count++)
	{
		sID[count]=players->playerInfo[count]->id;
		sInGame[count]=true;
		sPaused[count]=false;
	}
	
	// Determine which players we will be waiting for
	for (UInt16 n=0; n<sStartingPlayers; n++)
		sWaiting[n]=sInGame[n];
		
	// Don't wait for self
	sWaiting[GetMyID()]=false;
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DisplayMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::DisplayMessage(Str255 inString)
{
	StDialogHandler		dialog(130,LCommander::GetTopCommander());
	bool				done=false;
	
	LWindow				*grafPortView=dialog.GetDialog();
	
	LStaticText	*text=(LStaticText*)grafPortView->FindPaneByID('sttx');
	if (text)
		text->SetText((char*)&inString[1],inString[0]);
	
	while (!done)
	{
		switch (dialog.DoDialog())
		{
			case msg_OK:
				done=true;
				break;	
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HandleJoinRequest
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
pascal Boolean UFuse::JoinRequestHandler(NSpGameReference				inGame,
										   NSpJoinRequestMessage		*inMessage,
										   void*						inContext,
										   Str255						outReason)
{
	if (sGameRunning)
	{
		CopyPStr("\pThe game is already running.",outReason);
		return false;
	}
	
	return true;
}