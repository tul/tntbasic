// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CFuse.h
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

#include		"OpenPlay.h"

#include		<LDialogBox.h>
#include		<UModalDialogs.h>
#include		<LTextColumn.h>
#include		<LStaticText.h>

#include		"CProgram.h"

typedef			UInt32				TFuseError;

class CVariableNode;
class CVariableSymbol;

class CFuseIntegerBlockMessage
{
	public:
		static const short			kAmount=10;
		static const short			kImportantAmount=25;
	
		NSpMessageHeader			mHeader;
		UInt16						mStartIndex;
		SInt32						mValue[kAmount];
		
		UInt16						mImportantStartIndex;
		SInt32						mImportantValue[kImportantAmount];
};

class CFuseFloatBlockMessage
{
	public:
		static const short			kAmount=10;
		static const short			kImportantAmount=25;
	
		NSpMessageHeader			mHeader;
		UInt16						mStartIndex;
		float						mValue[kAmount];
		
		UInt16						mImportantStartIndex;
		float						mImportantValue[kImportantAmount];
};

class CFuseNewIntegerMessage
{
	public:
		NSpMessageHeader			mHeader;
		UInt16						mIndex;
		SInt32						mValue;
		bool						mImportant;
};

class CFuseNewFloatMessage
{
	public:
		NSpMessageHeader			mHeader;
		UInt16						mIndex;
		float						mValue;
		bool						mImportant;
};

class CFuseNewStringMessage
{
	public:
		NSpMessageHeader			mHeader;
		SInt32						mIndex;
		char						mValue[255];
};

class CFuseIntMessage
{
	private:	
		NSpMessageHeader			header;
		
		SInt32						type;
		SInt32						age;
		SInt32						integer;
	
	public:
		NSpMessageHeader*			GetHeader()						{ return &header; };

		void						SetType(SInt32 inType)			{ type=inType; };
		void						SetAge(SInt32 inTime)			{ age=inTime; };
	
		SInt32						GetType()						{ return type; };
		SInt32						GetAge()						{ return age; };

		// Handle content
		void						SetInteger(SInt32 inContent)	{ integer=inContent; };
		SInt32						GetInteger()					{ return integer; };
};

class CFuseFloatMessage
{
	private:	
		NSpMessageHeader			header;
		
		SInt32						type;
		SInt32						age;
		float						mFloat;
	
	public:
		NSpMessageHeader*			GetHeader()						{ return &header; };

		void						SetType(SInt32 inType)			{ type=inType; };
		void						SetAge(SInt32 inTime)			{ age=inTime; };
	
		SInt32						GetType()						{ return type; };
		SInt32						GetAge()						{ return age; };

		// Handle content
		void						SetFloat(float inContent)		{ mFloat=inContent; };
		float						GetFloat()						{ return mFloat; };
};

class CFuseStringMessage
{
	private:	
		NSpMessageHeader			header;
		
		SInt32						type;
		SInt32						age;
		UInt8						string[255];
	
	public:
		NSpMessageHeader*			GetHeader()						{ return &header; };

		void						SetType(SInt32 inType)			{ type=inType; };
		void						SetAge(SInt32 inTime)			{ age=inTime; };
	
		SInt32						GetType()						{ return type; };
		SInt32						GetAge()						{ return age; };

		// Handle content
		void						SetString(CCString &inContent)	{ CopyCAsPStr((UInt8*)inContent.mString,string); };
		UInt8*						GetString()						{ return string; };
};


class CFuseGeneralMessage
{
	private:	
		NSpMessageHeader			header;
		
		UInt8						dataType;
		SInt32						type;
		SInt32						age;
	
		SInt32						integer;
		float						mFloat;
		UInt8						string[255];
	
	public:
		NSpMessageHeader*			GetHeader()								{ return &header; };
		void						SetHeader(NSpMessageHeader* inHeader)	{ header=*inHeader; }

		void						SetDataType(UInt8 inDataType)	{ dataType=inDataType; }
		void						SetType(SInt32 inType)			{ type=inType; };
		void						SetAge(SInt32 inTime)			{ age=inTime; };
	
		UInt8						GetDataType()					{ return dataType;}
		SInt32						GetType()						{ return type; };
		SInt32						GetAge()						{ return age; };

		// Handle content
		void						SetInteger(SInt32 inContent)	{ integer=inContent; };
		SInt32						GetInteger()					{ return integer; };
		
		void						SetFloat(float inContent)		{ mFloat=inContent; };
		float						GetFloat()						{ return mFloat; };
		
		void						SetString(UInt8* inString)		{ CopyPStr(inString,string); };
		UInt8*						GetString()						{ return string; };
};

class CKickOutMessage
{
	private:
		NSpMessageHeader			header;
		NSpPlayerID					id;
	
	public:
		NSpMessageHeader*			GetHeader()							{return &header;};
		
		NSpPlayerID					GetID()								{return id;};
		void						SetID(NSpPlayerID inNumber)			{id=inNumber;};
};

class CDisconnectPlayerMessage
{
	private:
		NSpMessageHeader			header;
		NSpPlayerID					id;
		
	public:
		NSpMessageHeader*			GetHeader()							{return &header;};
		
		NSpPlayerID					GetID()								{return id;};
		void						SetID(NSpPlayerID inNumber)			{id=inNumber;};
};

class CFuseEvent
{
	public:
		SInt32						content,type;
		
		enum
		{
			kGameTerminated=0,
			kPlayerLeft,
			kPlayerDisconnected,
			kConnectionLost,
		};
};

class UFuse
{
	public:
		static const SInt16			kMaxEvents=100;
		static const SInt16			kMaxPlayers=50;
		
		enum
		{
			kIntegerDataType=0,
			kFloatDataType,
			kStringDataType,
		};

	private:
		static const UInt32			kTimeOutTicks=150;
	
		static const UInt32			msg_KickedOut='kick';
	
		enum
		{
			kNoErr=0,
			kHostQuitGame,
			kPlayerKickedOut,
			kGameAlreadyRunning,
		};
	
		// Host Dialog Constants
		static const UInt32			kHostDialogPlayerName='PlNa';
		static const UInt32			kHostDialogPassword='Pass';
		
		// Join Dialog Constants
		static const UInt32			kJoinDialogPlayerName='PlNa';
		static const UInt32			kJoinDialogGameName='GaNa';
		static const UInt32			kJoinDialogPassword='Pass';
		
		// Protocol Constants
		static const UInt16			kTCPPort=54321;
		
		// Gathering messages
		static const UInt32			kStartGameMessage='GO!!';
		static const UInt32			kKickedOutMessage='KICK';
		
		// Internal messages
		static const UInt32			kPausedMessage='PAUS';
		static const UInt32			kResumeMessage='RSME';
		static const UInt32			kPlayerLeftMessage='LEFT';
		static const UInt32			kDisconnectPlayerMessage='DISC';
		static const UInt32			kStillHereMessage='STIL';
		
		// External messages
		static const UInt32			kFuseIntMessage='FUSI';
		static const UInt32			kFuseFloatMessage='FUSF';
		static const UInt32			kFuseStringMessage='FUSS';
		static const UInt32			kFuseIntBlockMessage='FIBL';
		static const UInt32			kFuseFloatBlockMessage='FFBL';
	
		// Variables
		static NSpGameReference		sGame;
		static bool					sInitialised,sInitialisedData,sGameRunning,sHost;
		static UInt16				sPlayers,sMaxPlayers,sStartingPlayers;
	
		static bool					sInGame[kMaxPlayers];
		static SInt16				sLastMessageTime[kMaxPlayers];
	
		static SInt16				sEventCounter,sTotalEvents;
		static CFuseEvent			sEvents[kMaxEvents];
		
		static TArray<CVariableNode*>	sNodes;
		static TArray<CVariableNode*>	sImportantNodes;
		static TArray<TTBInteger*>		sIntegers;
		static TArray<TTBFloat*>		sFloats;
		static TArray<CCString**>		sStrings;
		static TArray<TTBInteger*>		sImportantIntegers;
		static TArray<TTBFloat*>		sImportantFloats;
		
		static CProgram*			sProgram;
		static StDialogHandler		*sWaitingDialog;
		
		static UInt32				sCurrentInteger,sCurrentFloat,sCurrentString;
		static UInt32				sCurrentImportantInteger,sCurrentImportantFloat;
		
		static UInt16				sID[kMaxPlayers];
		static Str255				sPlayerName[kMaxPlayers];
		
		static bool					sUpdateList;
		static bool					sWaiting[kMaxPlayers];
		static bool					sPaused[kMaxPlayers];
		static bool					sDisplayingDialog;
	
		static bool					sRegisteredMessages;
		static bool					sClientSentMessage;
	
		// Functions
		static bool					HostDialog(
										NSpProtocolListReference		&protocolList,
										Str31							inGameName,
										Str31 							inPlayerName,
										Str31 							inPassword);
		static bool					JoinDialog(
										NSpAddressReference			&outAddressReference,
										Str31 						ioPlayerName,
										Str31 						ioPassword,
										CCString					&inIPAddress);
		
		static bool					HostWaitForPlayers();
		static bool					JoinWaitForPlayers();
	
		static bool					HandleNonSparkMessages();
	
		static TFuseError			HandleMessages(
										LWindow							*inGrafPort,
										bool							&outDone,
										bool							&outok);
										
		static void					RebuildPlayersList(
										LWindow							*inGrafPort);

		static void					KickOutPlayer(
										LWindow							*inGrafPort);
		
		static void					DisplayMessage(
										Str255							inString);
		
		static void					SendSimpleInternalMessage(
										SInt32							inWhat,
										SInt32							inSendType,
										SInt32							inTo);
										
		static void					SendKickOutMessage(
										NSpPlayerID						inNumber);
	
		static bool					KickedOut(
										CKickOutMessage					*inMessage);
		
		static SInt32				GetPlayerID(SInt32 inNetID);
		static void					BuildIDList();
		
		static void					AddEvent(SInt32 inType,SInt32 inContent);
	
		static void					UpdateNonSyncedList(LWindow *inGrafPort);
		static void					DisconnectNonSyncedPlayers();
		
		static pascal Boolean		JoinRequestHandler(NSpGameReference				inGame,
													   NSpJoinRequestMessage		*inMessage,
													   void*						inContext,
													   Str255						outReason);
		
		static void					SendIntegerBlockMessage();
		static void					SendFloatBlockMessage();
		static void					SendStringBlockMessage();
		
		static void					SendMessage(
										TTBInteger			*inData,
										CArithExpression	*inValue,
										NSpPlayerID			inTo);
										
		static void					SendMessage(
										TTBFloat 			*inData,
										CArithExpression	*inValue,
										NSpPlayerID			inTo);
										
		static void					SendMessage(
										CCString 			**inData,
										CStrExpression		*inValue,
										NSpPlayerID			inTo);
										
		static void					DisconnectPlayer(
										SInt32		inPlayer);
										
		static void					InitialiseGameData();
										
	public:
		static void					Initialise(
										NSpGameID						inGameID,
										CProgram						&inProgram);
										
		static void 				ShutDown();
		static void					CleanUp();

		static bool					Host(
										UInt16							inMaxPlayers,
										Str31							inGameName,
										Str31							inPlayerName,
										CGraphicsContext				*inContext);
										
		static bool					Join(
										NSpGameID						inGameID,
										Str31 							inPlayerName,
										Str31							inPassword,
										CCString						&inIPAddress);

		static void					InitialiseData(
										CProgram			&ioState);
		static void					AddNetData(
										CVariableNode		*inInteger,
										bool				inImportant);
		static void					SetNetData(
										CVariableSymbol		*inSymbol,
										CGeneralExpression	*inExpression,
										CArithExpression	*inValue);
		static void					SetNetData(
										CVariableSymbol		*inSymbol,
										CGeneralExpression	*inExpression,
										CStrExpression		*inValue);
		static void					UpdateNetData();

		static UInt16				CountPlayers()						{ return sPlayers; }
		static SInt32				GetMyID();
		static UInt8*				GetPlayerName(
										SInt32							inID);
		
		static bool					MoreNetEvents();
		static SInt32				GetNetEventType();
		static SInt32				GetNetEventContent();
		
		static void					LeaveGame();
		
		static void					Pause();
		static void					Resume();
		
		static void					HandleMessage();
		
		static void					SetRegisteredMessages(bool inValue)		{ sRegisteredMessages=inValue; }
};