// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CProgram.h
// © Mark Tully 1999
// 22/12/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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

#pragma once

#include						"CLinkedListT.h"
#include						"CInputManager.h"
#include						"CArrayStackT.h"
#include						"CIdentifierScope.h"
#include						"UKaboom.h"
#include						"UTNTMapManager.h"
#include						"CVariableSymbol.h"
#include						"CProgramInfo.h"
#include						"CKeyQueue.h"
#include						"CStringEditBuffer.h"
#include						"ResourceTypes.h"

class							CVariableNode;
class							CStackFrame;
class							CStatement;
class							CLabelTable;
class							CStrSymbolTable;
class							CIntegerSymbolTable;
class							CDataStatement;
class							CGraphicsContext;
class							CSpriteManager;
class							CIntegerArraySymbolTable;
class							CStrArraySymbolTable;
class							CFloatSymbolTable;
class							CFloatArraySymbolTable;
class							CProcedure;
class							CLinkableObject;
class							CProcNode;
class							CError;
class							CResourceContainer;

class CProgram
{
	public:
		enum EProgramState
		{
			kAwaitingLinking,		// The program is awaiting linking
			kReady,					// The program is ready to go
			kRunning,				// The program is running
			kPaused,				// The program was running but was paused and must be resumed before it will continue
		};				

	protected:
		Str255						mProgramName;
		CProcNode					*mProcList;			// list of procedures extracted by preprocessor
		CStatement					*mCode;
		CStatement					*mPC;
		CGeneralExpression			*mDC;
		CGraphicsContext			*mGraphicsContext;
		CVariableNode				*mGlobals;			// A list of identifiers in the root frame which are placed in all procedure's scopes
		CLinkedListT<CStackFrame>	mStackFrames;		// Linked list is used so frames are auto deleted and is doesn't require run time handle resizing as arrays do
		CDeletingLinkedListT<CError>	mErrors;
		bool						mMiscErrorFlag;		// set when yyerror is called. Allows us to find errors which aren't handled by error rules.
		CIdentifierScope			mRootScope;
		EProgramState				mProgramState;
		Handle						mSourceH;			// Text source code for this program - is allowed to be nil
		CProgramInfo				*mProgramInfo;
		CVariableSymbol::ESymbolType	mCurrentDecl;	// The type of the current declaration mode
		bool						mIsInDimStatement,mWantLabels;
		static CProgram				*sParsingProgram;
		CProcedure					*mParserInProcedure;
		bool						mReportVarNotDecl;
		bool						mBreakDisabled;
		bool						mAllowAutoRepeatingKeys;		// whether keys held down on the keyboard generate mutliple key press events
		bool						mForceEventProcess;				// forces the event receiving and processing bit of the interpreter to run immediately instead of waiting for the scheduled time
		CKeyQueue					mKeyQueue;			// queue of key events for this program
		CStringEditBuffer			mEditBuffer;		// string editing buffer for this program
		CResourceContainer			*mResContainer;		// resources for this program (if any)
		
		SInt32						mMusicVolume;		// This is the music volume for the program, it is applied to a music when it is loaded
		bool						mMusicLooping;		// Should musics loaded by this program loop?
		
		struct											// This contains any variables which are needed to pause/resume the program
		{
			
		} mPauseState;									

		// Scope table used during parsing only
		CArrayStackT<CIdentifierScope*>	mParsingScope;
		CArrayStackT<CVariableSymbol::ESymbolType>	mPausedDeclModes;
		
		TArray<CProcedure*>			mProcDefs;
		TArray<CLinkableObject*>	mLinkableObjects;
				
	public:
		
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//		¥ Constructor/Destructor
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
								CProgram(
									Handle		inSourceH,
									CProcNode	*inProcs);
		virtual					~CProgram();

		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//		¥ Execution Stack Frame Management
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		// Used by statements during execution
		CStackFrame /*e*/		*RootStackFrame();
		CStackFrame	/*e*/		*CurrentStackFrame();
		CStackFrame	/*e*/		*PopStackFrame();
		void					PushStackFrame(
									CStackFrame		*inFrame);

		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//		¥ Parsing Scope Management
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		// Use these accessors to get the current symbol table during parsing. They currently don't follow symbol tables
		// during execution meaning that any lookup of a variable name during execution will fail.
		// You can force fetching of the root tables by passing true to these.
		CVariableSymbolTable /*e*/		*GetVarTable(
											bool		inRootTable=false);
		CLabelTable	 /*e*/				*GetLabelTable();
		CIdentifierScope				*GetDataStatementScope();
		CIdentifierScope				*PopParsingScope();
		void /*e*/						PushParsingScope(
											CIdentifierScope *inItem);
		void							PauseParsingScope();
		void							ResumeParsingScope(
											CIdentifierScope *inItem);
		void							CheckParsingScope(
											STextRange			&inRange,
											ArrayIndexT			inShouldBe);

		void /*e*/						AddGlobals(
											CVariableNode		*inGlobals,
											const STextRange	&inRange,
											bool				inAddToNetwork,
											bool				inImportant);
		CVariableSymbol::ESymbolType	GetDeclarationMode()		{ return mCurrentDecl; }
		void							SetDeclarationMode(
											CVariableSymbol::ESymbolType	inType) { mCurrentDecl=inType; }
		void /*e*/						PauseDeclarationMode()		{ mPausedDeclModes.PushItem(mCurrentDecl); mCurrentDecl=CVariableSymbol::kUndefined; }
		void							ResumeDeclarationMode()		{ try { if (mPausedDeclModes.GetCount()) mPausedDeclModes.PopItem(mCurrentDecl); } catch(...) {} }
		void							ClearPausedDeclModes()		{ mPausedDeclModes.RemoveAllItemsAfter(0); }
		// These two are used to let the lexer know whether we're in a dim statement allowing for better error reporting
		// if dimming a non array.
		bool							IsInDimStatement()			{ return mIsInDimStatement; }
		void							SetInDimStatement(
											bool	inDim)			{ mIsInDimStatement=inDim; }
		// These two are used to let the lexer know whether we should interpret identifiers as label references rather than
		// variables
		bool							WantLabels()				{ return mWantLabels; }
		void							SetWantLabels(
											bool	inLab)			{ mWantLabels=inLab; }
		bool							ReportVarNotDecl()			{ return mReportVarNotDecl; }
		void							SetReportVarNotDecl(
											bool	inReport)		{ mReportVarNotDecl=inReport; }

											
		void							LogError(
											const LException &inDesc);
		CLinkedListT<CError>			&GetErrors()				{ return mErrors; }
		bool							HasMiscError()				{ return mMiscErrorFlag; }
		void							SetMiscErrorFlag()			{ mMiscErrorFlag=true; }

		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//		¥ Linking
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		void /*e*/				Link();
		void /*e*/				AddLinkableObject(
									CLinkableObject	*inLink)					{ mLinkableObjects.AddItem(inLink); }
		void					RemoveLinkableObject(
									CLinkableObject *inLink)					{ mLinkableObjects.Remove(inLink); }
		void					AddProcDef(
									CProcedure		*inProc);
		CProcedure				*ResolveProc(
									const char		*inProcName);
		CProcedure				*GetLastProcedure();
		bool					IsProcedure(
									const char		*inProcName);
		void					SetParserInProcedure(
									CProcedure		*inProc)					{ mParserInProcedure=inProc; }
		CProcedure				*GetCurrentlyParsingProc()						{ return mParserInProcedure; }
		bool					IsParserInProcedure()							{ return mParserInProcedure!=0; }
																	
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//		¥ Accessors
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		StringPtr				GetProgramName()								{ return mProgramName; }
		CStatement				*GetFirstStatement()							{ return mCode; }
		CGraphicsContext		*GetGraphicsContext()							{ return mGraphicsContext; }
		void					SetGraphicsContext(
									CGraphicsContext	*inContext)				{ mGraphicsContext=inContext; }
		CGraphicsContext	/*e*/*CheckGraphicsMode();
		CSpriteManager			&GetSpriteManager();
		void					SetCode(
									CStatement		*inCode)					{ mPC=mCode=inCode; }
		void					SetDC(
									CGeneralExpression	*inDC)						{ mDC=inDC; }
		CGeneralExpression		*GetDC()										{ return mDC; }	
		void					SetPC(
									CStatement		*inStatement)				{ mPC=inStatement; }
		CStatement				*GetPC()										{ return mPC; }
		Handle					GetSourceCode()									{ return mSourceH; }	// Can be nil!
		OSType					GetCreatorCode();
		void					SetProgramInfo(CProgramInfo *inInfo)			{ Assert_(!mProgramInfo); mProgramInfo=inInfo; }
		void					SetProgramResContainer(CResourceContainer *inC)	{ Assert_(!mResContainer); mResContainer=inC; }
		CResourceContainer		*GetResContainer()								{ AssertThrow_(mResContainer); return mResContainer; }
		TResId /*e*/			GetResIdForName(
									ResType		inType,
									CCString	&inString);
		
		SInt32					GetMusicVolume()								{ return mMusicVolume; }
		bool					IsMusicLooping()								{ return mMusicLooping; }
		void					StoreMusicVolume(
									SInt32			inVol)						{ mMusicVolume=inVol; }
		void					StoreMusicLooping(
									bool			inLoop)						{ mMusicLooping=inLoop; }
									
		bool					InHardwareMode();

		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//		¥ Parsing
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		static CProgram			*GetParsingProgram();
		static CProgram /*e*/	*ParseAndLink(
									Handle			inCode,
									bool			&outContainsErrors,
									const StringPtr	inProgramName);
	
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//		¥ Execution
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		void /*e*/				BeginExecution();
		bool /*e*/				Step();
		void /*e*/				Pause();
		void /*e*/				Resume();
		bool					IsPaused()								{ return mProgramState==kPaused; };
		void /*e*/				EndExecution();

		bool					HasValidInput()							{ return CInputManager::ValidInput(); }
		void					ConfigureControls()						{ CInputManager::Edit(); }
		CKeyQueue				*GetKeyQueue()							{ return &mKeyQueue; }
		CStringEditBuffer		*GetStringEditBuffer()					{ return &mEditBuffer; }
		bool					KeysCanRepeat()							{ return mAllowAutoRepeatingKeys; }
		void					SetKeyRepeat(
									bool		inAllow)				{ mAllowAutoRepeatingKeys=inAllow; }

		void					DisableBreak()							{ mBreakDisabled=true; }
		bool					CanBreak()								{ return !mBreakDisabled; }
		
		// used to force immediate event processing from the interpreter. set this flag when you know that there's an
		// event in the queue that you need processing immediately.
		bool					PopForceEventProcessFlag()				{ bool was=mForceEventProcess; mForceEventProcess=false; return was; }
		void					SetForceEventProcess()					{ mForceEventProcess=true; }
		
		bool					EventUpdate(
									const EventRecord& inMacEvent);
		void					EventSuspend(
									const EventRecord& inMacEvent);
		void					EventResume(
									const EventRecord& inMacEvent);
		void					HandleKeyPressByScancode(
									UInt32				inScancode,
									UInt32				inMacKeyModifiers,
									bool				inIsRepeat);
		void					HandleKeyPressByRawcode(
									UInt32				inTBRawcode,
									bool				inIsRepeatingKey);
									
		OSStatus				TBReceiveNextEvent(
								  UInt32                 inNumTypes,
								  const EventTypeSpec *  inList,
								  EventTimeout           inTimeout,
								  Boolean                inPullEvent,
								  EventRef *             outEvent);
								  
		bool					MouseEventIsOverGameWindow(
									EventRef		inEvent);
		bool					ShouldPauseProgramWhenFocusLost();
		bool					ShouldProcessMenuShortCuts();
};
