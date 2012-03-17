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
#include		"CGraphicsContext.h"

#include		"Procedural_Statements.h"
#include		"CStackFrame.h"

#include		<UDesktop.h>
#include		<LEditText.h>

// the BkgConsole application to view the output.
#include		"UBkgConsole.h"
//#include		<PDebug.h>

NSpGameReference			UFuse::sGame;

bool						UFuse::sInitialised=false;
bool						UFuse::sInitialisedData=false;
bool						UFuse::sGameRunning=false;
bool						UFuse::sHost=false;

UInt16						UFuse::sPlayers=0;
UInt16						UFuse::sMaxPlayers=0;
UInt16						UFuse::sStartingPlayers=0;

SInt16						UFuse::sEventCounter=0;
SInt16						UFuse::sTotalEvents=0;
CFuseEvent					UFuse::sEvents[UFuse::kMaxEvents];

UInt16						UFuse::sID[UFuse::kMaxPlayers];
Str255						UFuse::sPlayerName[UFuse::kMaxPlayers];

StDialogHandler*			UFuse::sWaitingDialog=0L;

bool						UFuse::sUpdateList;
bool						UFuse::sWaiting[kMaxPlayers];
bool						UFuse::sInGame[kMaxPlayers];
bool						UFuse::sPaused[kMaxPlayers];
bool						UFuse::sDisplayingDialog;

CProgram*					UFuse::sProgram=NULL;

bool						UFuse::sRegisteredMessages=false;
bool						UFuse::sClientSentMessage=false;

TArray<CVariableNode*>		UFuse::sNodes;
TArray<CVariableNode*>		UFuse::sImportantNodes;
TArray<TTBInteger*>			UFuse::sIntegers;
TArray<TTBFloat*>			UFuse::sFloats;
TArray<CCString**>			UFuse::sStrings;
TArray<TTBInteger*>			UFuse::sImportantIntegers;
TArray<TTBFloat*>			UFuse::sImportantFloats;

UInt32						UFuse::sCurrentInteger;
UInt32						UFuse::sCurrentFloat;
UInt32						UFuse::sCurrentString;

UInt32						UFuse::sCurrentImportantInteger;
UInt32						UFuse::sCurrentImportantFloat;

SInt16						UFuse::sLastMessageTime[kMaxPlayers];

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Initialise						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Initialise the network for the type of game
void UFuse::Initialise(
	NSpGameID		inGameID,
	CProgram		&inProgram)
{
	sDisplayingDialog=false;

	sProgram=&inProgram;

	if (!sInitialised)
	{
		// Check if net sprocket is present
		ThrowIfOSStatus_(::NSpInitialize(sizeof(CFuseStringMessage),10000,100,inGameID,0));
		sInitialised=true;
		
		ThrowIfOSStatus_(::NSpInstallJoinRequestHandler((NSpJoinRequestHandlerProcPtr)UFuse::JoinRequestHandler,NULL));
	
		sRegisteredMessages=false;
	}
	
	InitialiseData(inProgram);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InitialiseGameData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::InitialiseGameData()
{
	sGameRunning=true;
	
	sEventCounter=0;
	sTotalEvents=0;
	
	sWaitingDialog=NULL;
						
	sCurrentInteger=1;
	sCurrentFloat=1;
	sCurrentString=1;
	sCurrentImportantInteger=1;
	sCurrentImportantFloat=1;
					
	int n;
	
	for (n=0; n<kMaxPlayers; n++)
		sLastMessageTime[n]=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShutDown
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::ShutDown()
{
	sInitialisedData=false;
	
	// Clear the arrays
	sNodes.RemoveAllItemsAfter(0);
	sImportantNodes.RemoveAllItemsAfter(0);
	sIntegers.RemoveAllItemsAfter(0);
	sFloats.RemoveAllItemsAfter(0);
	sStrings.RemoveAllItemsAfter(0);
	sImportantIntegers.RemoveAllItemsAfter(0);
	sImportantFloats.RemoveAllItemsAfter(0);
	
	LeaveGame();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CleanUp
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::CleanUp()
{
	sNodes.RemoveAllItemsAfter(0);
	sImportantNodes.RemoveAllItemsAfter(0);
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Host
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFuse::Host(
	UInt16				inMaxPlayers,
	Str31				inGameName,
	Str31 				inPlayerName,
	CGraphicsContext	*inContext)
{
	UPortSaver						safe;
	NSpProtocolListReference		protocolList=NULL;

	Try_
	{
		if (!sInitialised)
			UTBException::ThrowNetworkingNotInitialised();
		
		if (sGameRunning)
			UTBException::ThrowNetworkGameAlreadyRunning();
		
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
		
			// Keep attempting to host a game until a time out occurs
			OSStatus	status;
			UInt32		finishTicks=CGraphicsContext::sMsTimer.GetTime()+10000;
			
			do
			{
				status=::NSpGame_Host(&sGame,protocolList,inMaxPlayers,inGameName,password,inPlayerName,0,kNSpClientServer,0);

				if (CGraphicsContext::sMsTimer.GetTime()>finishTicks)
					ThrowIfOSStatus_(status);
			}
			while (status!=noErr);
			
			sHost=true;
			sMaxPlayers=inMaxPlayers;
			sPlayers=0;

			if (HostWaitForPlayers())
			{
				::NSpGame_EnableAdvertising(sGame,NULL,false);
				
				InitialiseGameData();
				
				// Tell all players the game is starting
				SendSimpleInternalMessage(kStartGameMessage,kNSpSendFlag_Registered,kNSpAllPlayers);
				::NSpProtocolList_Dispose(protocolList);
				
				BuildIDList();
				
				return true;
			}
			else
				::NSpGame_Dispose(sGame,kNSpGameFlag_ForceTerminateGame);
			
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
		
		if (protocolList)
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
	Str31					inPlayerName,
	Str31					inPassword,
	CCString				&inIPAddress)
{
	UPortSaver					safe;
	NSpAddressReference			address=NULL;
	OSStatus					err;
	
	Try_
	{
		if (!sInitialised)
			UTBException::ThrowNetworkingNotInitialised();
		
		if (sGameRunning)
			UTBException::ThrowNetworkGameAlreadyRunning();
			
		StDesktopDeactivator	deactivator;
		Str31					gameCreator="\pxxxx";
		*(OSType*)&gameCreator[1]=inGameType;
	
		if (JoinDialog(address,inPlayerName,inPassword,inIPAddress))
		{
			// Try to join the game
			UInt32		finishTicks=CGraphicsContext::sMsTimer.GetTime()+10000;
			
			do
			{
				err=::NSpGame_Join(&sGame,address,inPlayerName,inPassword,0L,0L,0L,0L);
				
				if (!err)
					break;
			}
			while (CGraphicsContext::sMsTimer.GetTime()<finishTicks);
			
			if (!err)
			{
				// Update the background
				if (sProgram->GetGraphicsContext())
				{
					sProgram->GetGraphicsContext()->InvalAll();
					sProgram->GetGraphicsContext()->RenderSpritesAndBlit();
				}
			
				if (!JoinWaitForPlayers())
					::NSpGame_Dispose(sGame,kNSpGameFlag_ForceTerminateGame);
				else
				{
					// Initialise the variables	
					sHost=false;
					
					InitialiseGameData();
					
					::NSpReleaseAddressReference(address);
					
					BuildIDList();
					
					return true;
				}
			}
			
			::NSpReleaseAddressReference(address);
		}
		
		// Update the background
		if (sProgram->GetGraphicsContext())
		{
			sProgram->GetGraphicsContext()->InvalAll();
			sProgram->GetGraphicsContext()->RenderSpritesAndBlit();
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
	Str31 						ioPassword,
	CCString					&inIPAddress)
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
	
	if (inIPAddress.GetLength()>0)
	{
		if (control=(LEditText*)grafPortView->FindPaneByID(kJoinDialogGameName))
		{
			control->SetText(inIPAddress,inIPAddress.GetLength());
			control->Disable();
		}
	}
	
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
//		¥ InitialiseData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::InitialiseData(
	CProgram	&ioState)
{
	if (!sInitialisedData)
	{
		SInt32		n;
		
		for (n=1; n<=sNodes.GetCount(); n++)
		{
			// Ordinary variables
			switch (sNodes[n]->mSymbol->GetType())
			{
				case CVariableSymbol::kInteger:
				{
					TTBInteger	*integer=sNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetIntegerPtr(sNodes[n]->mSymbol->GetIndex());	
					sIntegers.InsertItemsAt(1,LArray::index_Last,integer);
				}
				break;
				
				case CVariableSymbol::kFloat:
				{
					TTBFloat	*value=sNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetFloatPtr(sNodes[n]->mSymbol->GetIndex());
					sFloats.InsertItemsAt(1,LArray::index_Last,value);
				}
				break;
					
				case CVariableSymbol::kString:
				{
					CCString	**string=sNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetStringPtr(sNodes[n]->mSymbol->GetIndex());
					sStrings.InsertItemsAt(1,LArray::index_Last,string);
				}
				break;
				
				case CVariableSymbol::kIntegerArray:
				{
					CIntegerArrayStore	*intArray=sNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetIntArrayPtr(sNodes[n]->mSymbol->GetIndex());
					
					if (intArray->GetArity()==0)
						UTBException::ThrowArrayNotDimensioned();
			
					for (ArrayIndexT c=0; c<intArray->GetSize(); c++)
						sIntegers.InsertItemsAt(1,LArray::index_Last,intArray->GetData(c));
				}
				break;

				case CVariableSymbol::kFloatArray:
				{
					CFloatArrayStore	*floatArray=sNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetFloatArrayPtr(sNodes[n]->mSymbol->GetIndex());
					
					if (floatArray->GetArity()==0)
						UTBException::ThrowArrayNotDimensioned();
			
					for (ArrayIndexT c=0; c<floatArray->GetSize(); c++)
						sFloats.InsertItemsAt(1,LArray::index_Last,floatArray->GetData(c));
				}
				break;
				
				case CVariableSymbol::kStringArray:
				{
					CStrArrayStore	*stringArray=sNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetStringArrayPtr(sNodes[n]->mSymbol->GetIndex());
					
					if (stringArray->GetArity()==0)
						UTBException::ThrowArrayNotDimensioned();
			
					for (ArrayIndexT c=0; c<stringArray->GetSize(); c++)
						sStrings.InsertItemsAt(1,LArray::index_Last,stringArray->GetData(c));
				}
				break;
			}
		}
		
		// Important variables
		for (n=1; n<=sImportantNodes.GetCount(); n++)
		{
			switch (sImportantNodes[n]->mSymbol->GetType())
			{
				case CVariableSymbol::kInteger:
				{
					TTBInteger	*integer=sImportantNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetIntegerPtr(sImportantNodes[n]->mSymbol->GetIndex());	
					sImportantIntegers.InsertItemsAt(1,LArray::index_Last,integer);
				}
				break;
				
				case CVariableSymbol::kFloat:
				{
					TTBFloat	*value=sImportantNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetFloatPtr(sImportantNodes[n]->mSymbol->GetIndex());
					sImportantFloats.InsertItemsAt(1,LArray::index_Last,value);
				}
				break;
					
				case CVariableSymbol::kString:
				{
					CCString	**string=sImportantNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetStringPtr(sImportantNodes[n]->mSymbol->GetIndex());
					sStrings.InsertItemsAt(1,LArray::index_Last,string);
				}
				break;
				
				case CVariableSymbol::kIntegerArray:
				{
					CIntegerArrayStore	*intArray=sImportantNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetIntArrayPtr(sImportantNodes[n]->mSymbol->GetIndex());
					
					if (intArray->GetArity()==0)
						UTBException::ThrowArrayNotDimensioned();
			
					for (ArrayIndexT c=0; c<intArray->GetSize(); c++)
						sImportantIntegers.InsertItemsAt(1,LArray::index_Last,intArray->GetData(c));
				}
				break;

				case CVariableSymbol::kFloatArray:
				{
					CFloatArrayStore	*floatArray=sImportantNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetFloatArrayPtr(sImportantNodes[n]->mSymbol->GetIndex());
					
					if (floatArray->GetArity()==0)
						UTBException::ThrowArrayNotDimensioned();
			
					for (ArrayIndexT c=0; c<floatArray->GetSize(); c++)
						sImportantFloats.InsertItemsAt(1,LArray::index_Last,floatArray->GetData(c));
				}
				break;
				
				case CVariableSymbol::kStringArray:
				{
					CStrArrayStore	*stringArray=sImportantNodes[n]->mSymbol->GetStackFrame(ioState)->mStorage.GetStringArrayPtr(sImportantNodes[n]->mSymbol->GetIndex());
					
					if (stringArray->GetArity()==0)
						UTBException::ThrowArrayNotDimensioned();
			
					for (ArrayIndexT c=0; c<stringArray->GetSize(); c++)
						sStrings.InsertItemsAt(1,LArray::index_Last,stringArray->GetData(c));
				}
				break;
			}
		}
		
		sInitialisedData=true;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendIntegerBlockMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendIntegerBlockMessage()
{
	if ((sIntegers.GetCount()) || (sImportantIntegers.GetCount()))
	{
		CFuseIntegerBlockMessage		message;
					
		::NSpClearMessageHeader(&message.mHeader);
		message.mHeader.to=kNSpAllPlayers;
		message.mHeader.what=kFuseIntBlockMessage;
		message.mHeader.messageLen=sizeof(message);
			
		message.mStartIndex=sCurrentInteger;
		message.mImportantStartIndex=sCurrentImportantInteger;

		SInt32 	dataCount;
		
		if (sIntegers.GetCount())
		{
			for (dataCount=0; dataCount<CFuseIntegerBlockMessage::kAmount; dataCount++)
			{
				message.mValue[dataCount]=*(sIntegers[sCurrentInteger]);	
				
				sCurrentInteger++;
				if (sCurrentInteger>sIntegers.GetCount())
					sCurrentInteger=1;
			}
		}
			
		if (sImportantIntegers.GetCount())
		{
			for (dataCount=0; dataCount<CFuseIntegerBlockMessage::kImportantAmount; dataCount++)
			{		
				message.mImportantValue[dataCount]=*(sImportantIntegers[sCurrentImportantInteger]);	
				
				sCurrentImportantInteger++;
				if (sCurrentImportantInteger>sImportantIntegers.GetCount())
					sCurrentImportantInteger=1;
			}
		}
		
		if (sRegisteredMessages)
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Registered);
		else
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Normal);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendFloatBlockMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendFloatBlockMessage()
{
	if ((sFloats.GetCount()) || (sImportantFloats.GetCount()))
	{
		CFuseFloatBlockMessage		message;
				
		::NSpClearMessageHeader(&message.mHeader);
		message.mHeader.to=kNSpAllPlayers;
		message.mHeader.what=kFuseFloatBlockMessage;
		message.mHeader.messageLen=sizeof(message);
			
		message.mStartIndex=sCurrentFloat;
		message.mImportantStartIndex=sCurrentImportantFloat;

		SInt32	dataCount;
		
		if (sFloats.GetCount())
		{
			for (dataCount=0; dataCount<CFuseFloatBlockMessage::kAmount; dataCount++)
			{
				message.mValue[dataCount]=*(sFloats[sCurrentFloat]);	
			
				sCurrentFloat++;
				if (sCurrentFloat>sFloats.GetCount())
					sCurrentFloat=1;
			}
		}
			
		if (sImportantFloats.GetCount())
		{
			for (dataCount=0; dataCount<CFuseFloatBlockMessage::kImportantAmount; dataCount++)
			{	
				message.mImportantValue[dataCount]=*(sImportantFloats[sCurrentImportantFloat]);	
		
				sCurrentImportantFloat++;
				if (sCurrentImportantFloat>sImportantFloats.GetCount())
					sCurrentImportantFloat=1;
			}
		}
		
		if (sRegisteredMessages)
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Registered);
		else
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Normal);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendStringBlockMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendStringBlockMessage()
{
	if (sStrings.GetCount())
	{
		CFuseNewStringMessage		message;
				
		::NSpClearMessageHeader(&message.mHeader);
		message.mHeader.to=kNSpAllPlayers;
		message.mHeader.what=kFuseStringMessage;
		message.mHeader.messageLen=sizeof(message);
			
		message.mIndex=sCurrentString;
		strcpy(message.mValue,(*(sStrings[sCurrentString]))->mString);
			
		sCurrentString++;
		if (sCurrentString>sStrings.GetCount())
			sCurrentString=1;
		
		if (sRegisteredMessages)
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Registered);
		else
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Normal);
	}
}
	
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendMessage(
	TTBInteger 			*inData,
	CArithExpression	*inValue,
	NSpPlayerID			inTo)
{
	CFuseNewIntegerMessage		message;
	bool						found=false;
	ArrayIndexT					index=sIntegers.FetchIndexOf(inData);
	
	if (index==LArray::index_Bad)
	{
		index=sImportantIntegers.FetchIndexOf(inData);
		message.mImportant=true;
	}
	else
		message.mImportant=false;
	
	if (index>0)
		found=true;
		
	if (found)
	{
		::NSpClearMessageHeader(&message.mHeader);
		message.mHeader.to=inTo;
		message.mHeader.what=kFuseIntMessage;
		message.mHeader.messageLen=sizeof(CFuseNewIntegerMessage);
		
		message.mIndex=index;
		message.mValue=inValue->EvaluateInt(*sProgram);

		if (sRegisteredMessages)
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Registered);
		else
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Normal);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendMessage(
	TTBFloat 			*inData,
	CArithExpression	*inValue,
	NSpPlayerID			inTo)
{
	CFuseNewFloatMessage		message;
	bool						found=false;
	ArrayIndexT					index=sFloats.FetchIndexOf(inData);
	
	if (index==LArray::index_Bad)
	{
		index=sImportantFloats.FetchIndexOf(inData);
		message.mImportant=true;
	}
	else
		message.mImportant=false;
	
	if (index>0)
		found=true;
		
	if (found)
	{
		::NSpClearMessageHeader(&message.mHeader);
		message.mHeader.to=inTo;
		message.mHeader.what=kFuseFloatMessage;
		message.mHeader.messageLen=sizeof(CFuseNewFloatMessage);
		
		message.mIndex=index;
		message.mValue=inValue->EvaluateFloat(*sProgram);

		if (sRegisteredMessages)
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Registered);
		else
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Normal);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendMessage(
	CCString 		**inData,
	CStrExpression	*inValue,
	NSpPlayerID		inTo)
{
	ArrayIndexT		index=sStrings.FetchIndexOf(inData);
	
	if (index>0)
	{
		CFuseNewStringMessage		message;
			
		::NSpClearMessageHeader(&message.mHeader);
		message.mHeader.to=inTo;
		message.mHeader.what=kFuseStringMessage;
		message.mHeader.messageLen=sizeof(CFuseNewStringMessage);
		
		message.mIndex=index;
		strcpy(message.mValue,inValue->Evaluate(*sProgram).mString);

		if (sRegisteredMessages)
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Registered);
		else
			::NSpMessage_Send(sGame,&message.mHeader,kNSpSendFlag_Normal);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddNetData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::AddNetData(
	CVariableNode		*inNode,
	bool				inImportant)
{
	if (inImportant)
	{
		sImportantNodes.InsertItemsAt(1,LArray::index_Last,inNode);
		sImportantNodes[sImportantNodes.GetCount()]=inNode;
	}
	else
	{
		sNodes.InsertItemsAt(1,LArray::index_Last,inNode);
		sNodes[sNodes.GetCount()]=inNode;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetNetData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SetNetData(
	CVariableSymbol		*inSymbol,
	CGeneralExpression	*inExpression,
	CArithExpression	*inValue)
{
	if (sProgram)
	{
		if ((!sHost) && (sGameRunning))
		{
			switch (inSymbol->GetType())
			{
				case CVariableSymbol::kInteger:
				{
					TTBInteger	*integer=inSymbol->GetStackFrame(*sProgram)->mStorage.GetIntegerPtr(inSymbol->GetIndex());
					
					SendMessage(integer,inValue,kNSpHostOnly);
				}
				break;
				
				case CVariableSymbol::kFloat:
				{
					float	*value=inSymbol->GetStackFrame(*sProgram)->mStorage.GetFloatPtr(inSymbol->GetIndex());
					
					SendMessage(value,inValue,kNSpHostOnly);
				}
				break;
				
				case CVariableSymbol::kIntegerArray:
				{
					TTBInteger	*integer=static_cast<CIntegerArraySymbol*>(inSymbol)->GetIntArrayStorage(*sProgram).GetData(*sProgram,inExpression);
				
					SendMessage(integer,inValue,kNSpHostOnly);
				}
				break;
				
				case CVariableSymbol::kFloatArray:
				{
					TTBFloat	*value=static_cast<CFloatArraySymbol*>(inSymbol)->GetFloatArrayStorage(*sProgram).GetData(*sProgram,inExpression);
				
					SendMessage(value,inValue,kNSpHostOnly);
				}
				break;
			}
			
			sClientSentMessage=true;
		}
		else
		{
			switch (inSymbol->GetType())
			{
				case CVariableSymbol::kInteger:
				{
					TTBInteger	*integer=inSymbol->GetStackFrame(*sProgram)->mStorage.GetIntegerPtr(inSymbol->GetIndex());
					
					*integer=inValue->EvaluateInt(*sProgram);
				}
				break;
				
				case CVariableSymbol::kFloat:
				{
					float	*value=inSymbol->GetStackFrame(*sProgram)->mStorage.GetFloatPtr(inSymbol->GetIndex());
					
					*value=inValue->EvaluateFloat(*sProgram);
				}
				break;
				
				case CVariableSymbol::kIntegerArray:
				{
					TTBInteger	*integer=static_cast<CIntegerArraySymbol*>(inSymbol)->GetIntArrayStorage(*sProgram).GetData(*sProgram,inExpression);
				
					*integer=inValue->EvaluateInt(*sProgram);
				}
				break;
				
				case CVariableSymbol::kFloatArray:
				{
					TTBFloat	*value=static_cast<CFloatArraySymbol*>(inSymbol)->GetFloatArrayStorage(*sProgram).GetData(*sProgram,inExpression);
				
					*value=inValue->EvaluateFloat(*sProgram);
				}
				break;
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetNetData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SetNetData(
	CVariableSymbol		*inSymbol,
	CGeneralExpression	*inExpression,
	CStrExpression		*inValue)
{
	if (sProgram)
	{
		if (!sHost)
		{
			switch (inSymbol->GetType())
			{
				case CVariableSymbol::kString:
				{
					CCString	**value=inSymbol->GetStackFrame(*sProgram)->mStorage.GetStringPtr(inSymbol->GetIndex());
					
					SendMessage(value,inValue,kNSpHostOnly);
				}
				break;
				
				case CVariableSymbol::kStringArray:
				{
					CCString	**value=static_cast<CStrArraySymbol*>(inSymbol)->GetStrArrayStorage(*sProgram).GetData(*sProgram,inExpression);
				
					SendMessage(value,inValue,kNSpHostOnly);
				}
				break;
			}
			
			sClientSentMessage=true;
		}
		else
		{
			switch (inSymbol->GetType())
			{
				case CVariableSymbol::kString:
				{
					CCString	**value=inSymbol->GetStackFrame(*sProgram)->mStorage.GetStringPtr(inSymbol->GetIndex());
					
					(*value)->Assign(inValue->Evaluate(*sProgram).mString);
				}
				break;
				
				case CVariableSymbol::kStringArray:
				{
					CCString	**value=static_cast<CStrArraySymbol*>(inSymbol)->GetStrArrayStorage(*sProgram).GetData(*sProgram,inExpression);
				
					(*value)->Assign(inValue->Evaluate(*sProgram).mString);
				}
				break;
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ UpdateNetData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::UpdateNetData()
{
	if ((!sInitialised) || (!sGameRunning))
		return;
	
	bool	done=false;
	SInt32	n;
	
	while (!done)
	{
		NSpMessageHeader	*message;
		
		// Receive all messages
		while ((sGameRunning) && (message=::NSpMessage_Get(sGame)))
		{
			SInt32	id=GetPlayerID(message->from);
			
			sLastMessageTime[id]=0;
			
			switch (message->what)
			{	
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
					{
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
							
							sPlayers=0;
							sMaxPlayers=0;
						}
					}
					break;

				case kResumeMessage:
					sPaused[GetPlayerID(message->from)]=false;
					break;
					
				case kPausedMessage:
					sPaused[GetPlayerID(message->from)]=true;
					break;

				case kFuseIntBlockMessage:
				{
					SInt32		dataCount;
					SInt32		index=((CFuseIntegerBlockMessage*)message)->mStartIndex;				
					
					if ((index>=1) && (index<=sIntegers.GetCount()))
					{
						for (dataCount=0; dataCount<CFuseIntegerBlockMessage::kAmount; dataCount++)
						{						
							*(sIntegers[index])=((CFuseIntegerBlockMessage*)message)->mValue[dataCount];
							
							index++;
							if (index>sIntegers.GetCount())
								index=1;
						}
					}
					
					index=((CFuseIntegerBlockMessage*)message)->mImportantStartIndex;				
					
					if ((index>=1) && (index<=sImportantIntegers.GetCount()))
					{
						for (dataCount=0; dataCount<CFuseIntegerBlockMessage::kImportantAmount; dataCount++)
						{						
							*(sImportantIntegers[index])=((CFuseIntegerBlockMessage*)message)->mImportantValue[dataCount];
							
							index++;
							if (index>sImportantIntegers.GetCount())
								index=1;
						}
					}
				}
				break;
				
				case kFuseFloatBlockMessage:
				{
					SInt32		dataCount;
					SInt32		index=((CFuseFloatBlockMessage*)message)->mStartIndex;
					
					if ((index>=1) && (index<=sFloats.GetCount()))
					{
						for (dataCount=0; dataCount<CFuseFloatBlockMessage::kAmount; dataCount++)
						{						
							*(sFloats[index])=((CFuseFloatBlockMessage*)message)->mValue[dataCount];
							
							index++;
							if (index>sFloats.GetCount())
								index=1;
						}
					}
					
					index=((CFuseFloatBlockMessage*)message)->mImportantStartIndex;				
					
					if ((index>=1) && (index<=sImportantFloats.GetCount()))
					{
						for (dataCount=0; dataCount<CFuseFloatBlockMessage::kImportantAmount; dataCount++)
						{						
							*(sImportantFloats[index])=((CFuseFloatBlockMessage*)message)->mImportantValue[dataCount];
							
							index++;
							if (index>sImportantFloats.GetCount())
								index=1;
						}
					}
				}
				break;
				
				case kFuseIntMessage:
				{
					SInt32		index=((CFuseNewIntegerMessage*)message)->mIndex;
					
					if (((CFuseNewIntegerMessage*)message)->mImportant)
					{
						if ((index>=1) && (index<=sImportantIntegers.GetCount()))
							*(sImportantIntegers[index])=((CFuseNewIntegerMessage*)message)->mValue;
					}
					else
					{
						if ((index>=1) && (index<=sIntegers.GetCount()))
							*(sIntegers[index])=((CFuseNewIntegerMessage*)message)->mValue;
					}
				}
				break;
					
				case kFuseFloatMessage:
				{
					SInt32		index=((CFuseNewFloatMessage*)message)->mIndex;
					
					if (((CFuseNewFloatMessage*)message)->mImportant)
					{
						if ((index>=1) && (index<=sImportantFloats.GetCount()))
							*(sImportantFloats[index])=((CFuseNewFloatMessage*)message)->mValue;
					}
					else
					{
						if ((index>=1) && (index<=sFloats.GetCount()))
							*(sFloats[index])=((CFuseNewFloatMessage*)message)->mValue;
					}
				}
				break;
					
				case kFuseStringMessage:
				{
					SInt32		index=((CFuseNewStringMessage*)message)->mIndex;
					
					if ((index>=1) && (index<=sStrings.GetCount()))
						(*sStrings[index])->Assign(((CFuseNewStringMessage*)message)->mValue);
				}
				break;
			}
			
			if (sGameRunning)
				::NSpMessage_Release(sGame,message);
		}

		bool		displayDialog=false;
		
		// If any of the players are paused then display the dialog
		for (n=0; n<sStartingPlayers; n++)
		{
			if ((sInGame[n]) && (sPaused[n]))
				displayDialog=true;
		}
		
		// Handle the dialog
		if (displayDialog)
		{
			sDisplayingDialog=true;
		
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
		else
			done=true;	
	}
	
	// Determine which players we will be waiting for
	for (n=0; n<sStartingPlayers; n++)
		sPaused[n]=false;
	
	// Close the dialog if it is showing
	if (sWaitingDialog)
	{
		delete sWaitingDialog;
		sWaitingDialog=NULL;
	}
	
	if (sProgram->IsPaused())
		sProgram->Resume();
	
	sDisplayingDialog=false;
	
	if (sHost)
	{					
		SendIntegerBlockMessage();
		SendFloatBlockMessage();
		SendStringBlockMessage();
			
		// Disconnect any player that has not sent any messages for 30*15 updates
		SInt32		hostId=GetMyID();
		
		for (n=0; n<sStartingPlayers; n++)
		{
			if ((sInGame[n]) && (n!=hostId))
			{
				if (sLastMessageTime[n]>30*15)
					DisconnectPlayer(n);
				
				sLastMessageTime[n]++;
			}
		}
	}
	else
	{
		// If you haven't received any messages for 30*15 updates the you've become disconnected
		bool		found=false;
		SInt32		id=GetMyID();
		
		if (sInGame[id])
		{
			for (n=0; n<sStartingPlayers; n++)
			{
				if ((sInGame[n]) && (id!=n))
				{
					sLastMessageTime[n]++;
					
					if (sLastMessageTime[n]<30*15)
					{
						found=true;
						break;
					}
				}
			}
			
			if (!found)
			{
				DisconnectPlayer(id);
				AddEvent(CFuseEvent::kConnectionLost,0);
				sGameRunning=false;
			}
		}
		
		// If we didn't send any messages this time then send a 'still here' message
		if (!sClientSentMessage)
		{
			SendSimpleInternalMessage(kStillHereMessage,kNSpSendFlag_Normal,kNSpHostOnly);
		}
		
		sClientSentMessage=false;
	}
}

#pragma mark -

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SendSimpleInternalMessage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::SendSimpleInternalMessage(
	SInt32		inWhat,
	SInt32		inSendType,
	SInt32		inTo)
{
	if (sGameRunning)
	{
		::NSpMessage_SendTo(sGame,inTo,inWhat,NULL,0,inSendType);
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
			DisconnectPlayer(n);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DisconnectPlayer
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::DisconnectPlayer(
	SInt32		inPlayer)
{
	CDisconnectPlayerMessage		message;
				
	::NSpClearMessageHeader(message.GetHeader());
	message.GetHeader()->to=kNSpAllPlayers;
	message.GetHeader()->what=kDisconnectPlayerMessage;
	message.GetHeader()->messageLen=sizeof(CDisconnectPlayerMessage);
	message.SetID(inPlayer);

	::NSpMessage_Send(sGame,message.GetHeader(),kNSpSendFlag_Registered);

	// Update own variables
	sPlayers--;
	sInGame[inPlayer]=false;
	AddEvent(CFuseEvent::kPlayerDisconnected,inPlayer);
	sUpdateList=true;
	
	::NSpPlayer_Remove(sGame,sID[inPlayer]);
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
		SendSimpleInternalMessage(kPlayerLeftMessage,kNSpSendFlag_Registered,kNSpAllPlayers);
	
		// Delete the game field
		OSStatus	err=::NSpGame_Dispose(sGame,0L);
		if ((sHost) && (err))
			err=::NSpGame_Dispose(sGame,kNSpGameFlag_ForceTerminateGame);
		
		// Update the variables
		sGameRunning=false;
		sHost=false;
		
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
		SendSimpleInternalMessage(kPausedMessage,kNSpSendFlag_Registered,kNSpAllPlayers);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Resume
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFuse::Resume()
{
	if (sGameRunning)
	{
		SendSimpleInternalMessage(kResumeMessage,kNSpSendFlag_Registered,kNSpAllPlayers);
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
SInt32 UFuse::GetMyID()
{
	if (sGame)
		return GetPlayerID(::NSpPlayer_GetMyID(sGame));

	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetPlayerID
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 UFuse::GetPlayerID(SInt32 inNetID)
{
	for (SInt32 n=0; n<sStartingPlayers; n++)
	{
		if (sID[n]==inNetID)
			return n;
	}

	return -1;
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