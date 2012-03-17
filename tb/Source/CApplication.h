// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CApplication.h
// © Mark Tully 2000
// 4/2/00
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

#pragma once

#include			<LApplication.h>
#include			<TEventHandler.h>

#include			"COutputWindow.h"

class				LWindow;
class				CProgram;
class				CProgramInfo;

#include			<LTimerTask.h>

struct SHeldKey
{
	UInt32			lastPressTime;		// time in ms of the last time this key was held
	UInt32			scancode;			// scan code of the key that was pressed
	bool			autoRepeating;		// whether this key has began to auto repeat yet
};

// useful for passing params to tbgetevent
#define EventListWithCount(e)			e##Count,e

class CResourceContainer;
class CMacResFileCreator;
class CBundleResFileCompatCreator;
class CResource;

class CApplication : public LApplication, public LTimerTask
{
	private:
		typedef LApplication			inheritedApp;

	protected:
		static const UInt32				kMaxHeldKeysToTrack=12;	// apple docs say max tracked keys is 2 + all the modifiers so be generous as we have to track the modifiers with this too
		static const UInt32				kNumKeyMapLongs=(sizeof(KeyMap)/sizeof(UInt32));
	
		CProgram						*mCurrentProgram;
		short							mAppResFile;
		bool							mProgramPaused;
		COutputWindow					mOutputWindow;		
		bool							mShowFPS;
		bool							mAllowFullScreen;
		OSType							mReturnToApp;
		bool							mAppletMode;
		bool							mAppResumed;
		
		KeyMap							mKeyMap;
		
		////// only valid if using GetKeys() - ie USE_RNE_FOR_KEYBOARD_INPUT == 0
		UInt32							mKeyMapLastUpdateTime;		
		SHeldKey						mHeldKeys[kMaxHeldKeysToTrack];
		UInt16							mHeldKeysCount;
		//////

		void							UpdateKeyboardGetKeys();
		bool							KeyCanAutoRepeat(
											UInt32			inScanCode);
		
		// used for updating the keyboard from carbon events
		void							KeyMapPress(
											UInt32			inRaw,
											bool			inIsRepeat);
		void							KeyMapRelease(
											UInt32			inRaw);
		void							KeyMapModsChanged(
											UInt32			inMods);
		
		void /*e*/						GetAppTemplateSpec(
											FSSpec			&outSpec);
		bool							ParseHandle(
											Handle 				inCodeHandle,
											CProgramInfo		*inProgInfo,
											CResourceContainer	*inResContainer,
											bool				inOutputErrors,
											AEDesc				*outErrors);
		void /*e*/						DoAEBuild(
											const AppleEvent &inAE);
		void							DumpCompileErrorsToStream(
											Handle				inCodeHandle,
											std::ostream		&outStream);
		void							StoreCompileErrorsInAEDesc(
											AEDesc				&outDesc);
		void							ReturnToSender();
		void							HandleRuntimeException(
											const LException	&inErr);
		bool							HandleRunningProgramMouseDown(
											EventRef			inRef);


		void							MakeMenuBar();

		void							DoTask();
		
		enum
		{
			kDataType_AppResFork		= 'AppF',
			kDataType_DataBundle		= 'DatB'
		};
		CResourceContainer /*e*/		*GetResourceContainerForDataFilePath(
											Handle				inHandle);
		Handle /*e*/					MakeDataFilePathForResContainer(
											OSType				inType,
											FSSpec				*inSpec);

		void							CreateDataBuild(
											CResourceContainer		*inSourceContainer,
											CResourceContainer		*inDestContainer,
											OSType					*outCreatorCode);
		OSType							ReadCreatorCodeFromProgInfo(
											CResource				*inRes);

		void							ParseCmdLine();
	
	public:
		static CApplication				*sApplication;
		
		CMacResFileCreator				*mMacResFileCreator;
		CBundleResFileCompatCreator		*mBundleResFileCreator;

		
		// these are the events that TB receives and handles in it's main loop
		// at present, the standard loop doesn't handle mouse ups, but some commands need them
		// so they can use the events list with mouse ups included
		// nb: see EventListWithCount define
		static const EventTypeSpec		kStdEventsList[];
		static const UInt16				kStdEventsListCount;
		static const EventTypeSpec		kStdEventsListPlusAllMouse[];
		static const UInt16				kStdEventsListPlusAllMouseCount;
	
		static CApplication				*GetApplication()		{ return sApplication; }
		static double					AbsTimeStr();
		static double					AbsoluteToSeconds(
											AbsoluteTime inTime );
		static void						KeyMapXOR(
											KeyMap			outResult,
											const KeyMap	inA,
											const KeyMap	inB);
	
										CApplication();
		virtual							~CApplication();								

		bool							AllowedToGoFullScreen()		{ return mAllowFullScreen; }
		bool							AllowedToDoFades()			{ return mAllowFullScreen; }

		bool							IsFPSVisible();
		bool							IsCompiledApplet()		{ return mAppletMode; }

		short							GetAppResFile()			{ return mAppResFile; }

		virtual void /*e*/				DoAEOpen(
											const AppleEvent&	inAppleEvent,
											AEDesc				&outDesc,
											bool				inRunProgram);
		virtual void /*e*/				HandleAppleEvent(
											const AppleEvent&	inAppleEvent,
											AppleEvent&			outAEReply,
											AEDesc&				outResult,
											long				inAENumber);
		virtual bool /*e*/				OpenDocument(
											FSSpec				*inFile,
											OSType				inReturnToApp,
											bool				inReportErrors,
											AEDesc				*outErrors,
											bool				inRunProgram);

		virtual void /*e*/				StartUp();
		virtual void					ShowAboutBox();
		virtual bool					DoRaaAboutBox();
		virtual void					TakeScreenShot(
											bool				inGrabFromWindow);
		
		virtual void					FindCommandStatus(
											CommandT			inCommand,
											Boolean&			outEnabled,
											Boolean&			outUsesMark,
											UInt16&				outMark,
											Str255				outName);

		virtual Boolean	/*e*/			ObeyCommand(
											CommandT			inCommand,
											void*				ioParam);

		virtual void					UseIdleTime(
											const EventRecord&	inMacEvent);
											
		virtual Boolean					HandleKeyPress(
											const EventRecord& inKeyEvent);
		const KeyMap					&GetKeyMap()		{ return mKeyMap; }
		
		virtual void /*e*/				RestartProgram();
		virtual void /*e*/				EndProgram();
		virtual void /*e*/				PauseProgram();
		virtual void /*e*/				ResumeProgram();
		virtual void /*e*/				RunProgram();

		virtual void /*e*/				EventSuspend(
											const EventRecord& inMacEvent);
		virtual void /*e*/				EventResume(
											const EventRecord& inMacEvent);
		virtual void /*e*/				EventUpdate(
											const EventRecord& inMacEvent);
		virtual OSStatus				DoEvent(
											EventHandlerCallRef	inCallRef,
											EventRef			inEventRef);

		
		virtual void					ClearKeyboard();
};
