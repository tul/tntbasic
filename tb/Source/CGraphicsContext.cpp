// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CGraphicsContext.cpp
// © Mark Tully 2000
// 1/1/00
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

#include		"TNT_Debugging.h"
#include		"Carbon_Settings.h"
#include		<Displays.h>
#include		"CGraphicsContext.h"
#include		"Utility Objects.h"
#include		"UTBException.h"
#include		"CTBSprite.h"
#include		"Root.h"
#include		"CTMAPViewport.h"
#include		"CBLCanvas.h"
#include		"CTBSpriteGL.h"
#include		"UCursorManager.h"

// I don't think profiler likes the MP stuff that RezLib uses, it tends to crash things.
// I tried to get it working here, but I think we'd have to take the calls to the MP stuff out from RezLib
#define			DISABLE_VBL_PROC			__profile__

#if DISABLE_VBL_PROC
#undef USE_SEMAPHORE_FOR_VBL
#define USE_SEMAPHORE_FOR_VBL 0
#endif

#if USE_SEMAPHORE_FOR_VBL
MPSemaphoreID		CGraphicsContext::sVBLSemaphore=0;
#endif

CMsTimer			CGraphicsContext::sMsTimer;
CGraphicsContext	*CTBSprite::sContext=NULL;
ScreenRef			CGraphicsContext::sScreenRef=kNoScreen;
WindowGroupRef		CGraphicsContext::sWindowGroup=NULL;

static pascal OSStatus DoWindowUpdate(
	EventHandlerCallRef  nextHandler,
	EventRef             theEvent, 
	void*                userData);

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGraphicsContext::CGraphicsContext(
	SInt16		inWide,
	SInt16		inHigh,
	SInt16		inDepth,
	bool		inGoFullScreen,
	CProgram	*inProgram) :	
	mWindow(0L), mSpriteManager(this), mCanvasManager(this), mNextBlitTime(0),
	mVBLCount(0), mVBLSync(true), mFPSOn(false),
	mClockFrequency(0), mBackWindow(0),mRetraceToken(kNoRetraceProcInstalled),
	mTimeUnitsPerFrame(0),mFrameRate(0),mVBLSyncAvailable(true),
	mTakeScreenShot(false),
	mScreenShot(0),
	mSavedIconPositions(0),
	mGraphicsPaused(false),
	mProgram(inProgram),
	mAutoMoan(true),
	mMasterMoanPause(false),
	mWindowBoundsRgn(NULL),
	mGoFullScreen(inGoFullScreen)
{
	CTBSprite::SetContext(this);
		
	Try_
	{
		// Get information about the current screen setting
		mPreviousRes.rlInfoType=0;
		ThrowIfOSErr_(::GetCurrentSetting(sScreenRef,&mPreviousRes));

		if (mGoFullScreen)
		{		
			// Get the best new resolution to switch to
			UInt32		n,count,bestRes=-1;
			RLInfo		*resolutions=::GetResolutionList(sScreenRef,&count);
			SInt32		bestDiff=999999;
			UInt32		bestRate=0;		

			ECHO("Graphics context request for " << inWide << "x" << inHigh);
			
			ECHOINDENT("Available contexts are:");
			for (UInt16 pass=0; pass<2; pass++)
			{
				for (n=0; n<count; n++)
				{
					SInt32		widthDiff=resolutions[n].width-inWide;
					SInt32		heightDiff=resolutions[n].height-inHigh;
					bool		stretched;
					
					if (pass==0)	// on pass 0, we note the stretched flag, this stops us selecting
					{				// stretched reses on the first pass
						stretched=(resolutions[n].flags&rlDisplayModeIsStretched)!=0;
					}
					else
						stretched=false;
			
					// nb: assume even unsave resolutions work, last time i checked, all rezes were
					// reported as unsafe on durandal
					if ((widthDiff>-1) && (heightDiff>-1) && (!stretched) /*&& resolutions[n].isSafe*/)
					{
						if ((bestDiff>(widthDiff+heightDiff)) ||
							(bestDiff==(widthDiff+heightDiff) && bestRate<resolutions[n].refreshRate))
						{
							bestDiff=widthDiff+heightDiff;
							bestRes=resolutions[n].resolutionID;
							bestRate=resolutions[n].refreshRate;
						}
					}
					
					if (pass==0)	// report res'es on pass 1
					{
						ECHO(resolutions[n].resolutionID << ": " << resolutions[n].width << " x " << resolutions[n].height << \
						" @ " << (resolutions[n].refreshRate>>16) << "." << (resolutions[n].refreshRate&0xFF) << "Hz" << \
						(resolutions[n].isSafe ? " safe" : " unsafe") << ", " << (stretched ? "stretched" : "unstretched"));
					}
				}
				
				// pass finished - have we found a suitable res?
				if (bestRes!=-1)
				{
					break;
				}
				else
				{
					if (pass==0)
						ECHO("Can't find suitable non-strectched display, searching now for stretched display");
					bestDiff=999999;	// reset search and loop
				}
			}

			::DisposeResolutionList(sScreenRef,resolutions);
			
			if (bestRes==-1)
			{
				ECHO("Can't find suitable display!");
				UTBException::ThrowCantFindSuitableDisplay(inWide,inHigh);
			}
			
			ECHO("Choosing " << bestRes);
					
			// Switch to that resolution
			if (OSErr err=::SaveIconPositions(sScreenRef,&mSavedIconPositions))		// don't particularly care if this fails
				ECHO("Error saving desktop icons positions : " << err);
			ThrowIfOSErr_(::SetResolutionAndColorDepth(sScreenRef,bestRes,inDepth));
			
			mCurrentRes.rlInfoType=0;
			ThrowIfOSErr_(::GetCurrentSetting(sScreenRef,&mCurrentRes));
		}
		else
		{
			mCurrentRes=mPreviousRes;
		}
		
		mClockFrequency=mCurrentRes.refreshRate;

		Rect		screenBounds={0,0,mCurrentRes.height,mCurrentRes.width};
		
		// Install a VBl sync to stop hideous tearing effects
		if (mClockFrequency==0 || DISABLE_VBL_PROC)				// no vbl, must be an lcd screen...
		{
			mVBLSyncAvailable=false;			
			mRetraceToken=0;
			mVBLSync=false;			
		}
		else
		{
			mRetraceToken=::InstallVerticalRetraceProcedure(sScreenRef,VBLProc,(long)this);
			if (mRetraceToken!=kNoRetraceProcInstalled)
			{
				mVBLSyncAvailable=true;
				mVBLSync=true;
			}
			else
			{
				mVBLSyncAvailable=false;
				mVBLSync=false;
			}
		}

		// default the frame rate to 60 frames per second
		mFrameRate=60;
		
		// now init the frame rate clock, this sets up the timing vars to honour the requested
		// framerate
		ResetFrameRateClock();

		WindowPtr		backWindow=NULL;
		
		if (mGoFullScreen)
		{
			// Take control of the desktop
			ThrowIfOSErr_(::AcquireDesktop(sScreenRef,&backWindow,0L));
		}
		
		// Get a window
		Rect		windowRect;
		
		windowRect.top=0;
		windowRect.left=0;
		windowRect.bottom=inHigh;
		windowRect.right=inWide;
		
		Rect		windowBounds=windowRect;
		short		procId;
		const unsigned char	*title="\p";	
		
		CentreRectInRect(&windowBounds,&screenBounds);
		
		if (mGoFullScreen)
		{
			procId=plainDBox;
		}
		else
		{
			procId=kWindowDocumentProc;
			title=inProgram->GetProgramName();
		}
		
		mWindow=::NewCWindow(0L,&windowBounds,title,true,procId,(WindowPtr)-1,false,0L);
		if (!mWindow)
			Throw_(memFullErr);

		if (mGoFullScreen && backWindow)
		{
			// correct for qt 6.1 'my backing window is more important that your game content window' feature		
			SortOutWindowLevels(backWindow);
		}
			
		mWindowBoundsRgn=::NewRgn();
		::RectRgn(mWindowBoundsRgn,&windowBounds);

		// Fill the window with black
		UpdateInBlack(mWindow);

		if (backWindow)
		{		
			// And RezLib's blanking window
			UpdateInBlack(backWindow);
		}
		
		if (!mGoFullScreen)
		{
			InstallStdWindowHandlers(mWindow);
			UCursorManager::UpdateShieldWindow(mWindow);
		}
	}	
	Catch_(err)
	{
		KillBaseContext();
		throw;
	}
	
	#if CheckVBLTimer
		ShowFPS(true);
		mFPSTester.StartTask();
	#endif	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InstallStdWindowHandlers								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// takes the window passed and installs window mode handlers for it to handle user focus, click, drag, close etc events using
// the carbon event mgr
void CGraphicsContext::InstallStdWindowHandlers(
	WindowRef			inWindow)
{
	ThrowIfOSErr_(::InstallStandardEventHandler(::GetWindowEventTarget(inWindow)));
	
	// install an update event handler to try and stop our port being flushed when receiving events
/*	
	EventHandlerUPP  handlerUPP;
	handlerUPP = NewEventHandlerUPP(DoWindowUpdate);
	AssertThrow_(handlerUPP);
	EventTypeSpec	eventType[2];
	
	eventType[0].eventClass = kEventClassWindow;          // Set event class
	eventType[0].eventKind  = kEventWindowUpdate;          // Set event kind
	eventType[1].eventClass = kEventClassWindow;          // Set event class
	eventType[1].eventKind  = kEventWindowDrawContent;          // Set event kind
	InstallWindowEventHandler (mWindow, handlerUPP,  // Install handler
                             2, eventType,
                             NULL, NULL);
*/

}

/*
static pascal OSStatus DoWindowUpdate(
	EventHandlerCallRef  nextHandler,
	EventRef             theEvent, 
	void*                userData)
{
	ECHO("Caught window update");
	return noErr;
}*/

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SortOutWindowLevels
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// As of QuickTime 6.1, the screen blanking window has a higher window level than normal windows. This means it is
// displayed over the top of our graphics window, just what we wanted guys, thanks a lot. So we have to ensure we
// position our windows window group in front of the backing windows window group.
void CGraphicsContext::SortOutWindowLevels(
	WindowPtr		inBackWindow)
{
	// ensure that this window is in a window layer in front of the desktop blanking window returned by rezlib
	// this fixes the blank screen bug found with quicktime 6.1
	if (UEnvironment::IsRunningOSX())
	{
		if (inBackWindow)
		{
			ECHOINDENT("Reordering windows to cater for quicktime 6.1");
			
#if 1
			OSStatus			result;
			WindowGroupRef		backRef=::GetWindowGroup(inBackWindow);
			WindowGroupRef		contRef=::GetWindowGroup(mWindow);
			
			if (backRef==contRef)
				ECHO("Backing window and content window in the same group - no action to be taken");
			else
			{
				ECHO("Backing window and content window in different groups - moving backing window into same window group");

				result=::SetWindowGroup(inBackWindow,contRef);
				if (result)
				{
					ECHO("SetWindowGroup failed for TNT Basic content window, result " << result);
					Throw_(result);
				}
			}
			
			::SendBehind(inBackWindow,mWindow);
#elif 0
	
			OSStatus			result;
			
			if (!sWindowGroup)
			{
				// create a window group
				result=::CreateWindowGroup(NULL,&sWindowGroup);
				if (result)
				{
					ECHO("Error creating window group for TNT Basic content window, result " << result);
					Throw_(result);
				}
				
				WindowGroupRef		backRef=::GetWindowGroup(inBackWindow);
				SInt32				backRefLevel=1000;

				result=::GetWindowGroupLevel(backRef,&backRefLevel);
				if (result)
				{
					ECHO("GetWindowGroupLevel failed for qt backing window window, result " << result);
					Throw_(result);
				}

				result=::SetWindowGroupLevel(sWindowGroup,backRefLevel+1);
				if (result)
				{
					ECHO("SetWindowGroupLevel failed for new window group, result " << result);
					Throw_(result);
				}
			}
				
			result=::SetWindowGroup(mWindow,sWindowGroup);
			if (result)
			{
				ECHO("SetWindowGroup failed for TNT Basic content window, result " << result);
				Throw_(result);
			}
				
#elif 0	
			WindowGroupRef		backRef=::GetWindowGroup(inBackWindow);
			WindowGroupRef		myRef=::GetWindowGroup(mWindow);
			SInt32				backRefLevel,myRefLevel;
			WindowGroupRef		backParent,myParent;
			OSStatus			result;

			// windows must have same parent group to be rearranged
			backParent=::GetWindowGroupParent(backRef);
			myParent=::GetWindowGroupParent(myRef);				
			if (backParent!=myParent)
				ECHO("backing window group has different parent from TNT window group, rearrangement could fail, trying anyway");
			else
				ECHO("backing window and TNT window groups have same parent group");

			// rearrange
			result=::GetWindowGroupLevel(backRef,&backRefLevel);
			if (result)
				ECHO("GetWindowGroupLevel failed for backing window, result " << result);
			else
			{
				result=::GetWindowGroupLevel(myRef,&myRefLevel);
				if (result)
					ECHO("GetWindowGroupLevel failed for TNT window, result " << result);
				else
				{
					ECHO("backing window level " << backRefLevel << " TNT window level " << myRefLevel << ", changing to " << (backRefLevel+1));
					myRefLevel=backRefLevel+1;	// bring forward
					result=::SetWindowGroupLevel(myRef,myRefLevel);
					if (result)
						ECHO("SetWindowGroupLevel failed for TNT window, result " << result);
				}
			}
#endif

//			this doesn't work, fails with paramErr
//			OSStatus			result=::SendWindowGroupBehind(backRef,myRef);				
//			if (result)
//				ECHO("Error rearranging the window order, possible blank screen if running quicktime 6.1 or above (err code " << result << ")");

			// check
			{
				WindowGroupRef		backRef=::GetWindowGroup(inBackWindow);
				WindowGroupRef		myRef=::GetWindowGroup(mWindow);
				SInt32				myRefLevel,backRefLevel;
				if ((result=::GetWindowGroupLevel(backRef,&backRefLevel))==noErr && (result=::GetWindowGroupLevel(myRef,&myRefLevel))==noErr)
				{
					if (myRefLevel<backRefLevel)
						ECHO("TNT window has a lower level than the backing window, possible blank screen bug if running quicktime 6.1 or greater");
				}
				else
					ECHO("Can't check window group levels are correct due to error with GetWindowGroupLevel(), err code " << result);
			}
		}
		else
			ECHO("NULL backWindow returned from RezLib, can't reorder windows, possible blank screen bug if running quicktime 6.1 or above");
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ KillBaseContext
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Used by destructor/constructor to free resources from this object
void CGraphicsContext::KillBaseContext()
{
	// Remove all the viewports
	CListIndexerT<CViewport>		indexer(&mViewports);
	while (CViewport *viewport=indexer.GetNextData())
		delete viewport;
	
	if (mRetraceToken)
	{
		::DisposeVerticalRetraceProcedure(mRetraceToken);
		mRetraceToken=0;
	}
	
	// Remove the window if present
	if (mBackWindow)
	{
		::DisposeWindow(mBackWindow);
		mBackWindow=0;
	}
	if (mWindow)
	{
		UCursorManager::UpdateShieldWindow(NULL);

		::DisposeWindow(mWindow);
		mWindow=0;
	}
	if (mWindowBoundsRgn)
	{
		::DisposeRgn(mWindowBoundsRgn);
		mWindowBoundsRgn=NULL;
	}
	
	if (mGoFullScreen && sScreenRef!=kNoScreen)
	{
		::ReleaseDesktop(sScreenRef);	
		::SetResolutionAndColorDepth(sScreenRef,mPreviousRes.resolutionID,mPreviousRes.colorDepths);
		if (mSavedIconPositions)
		{
			::RestoreIconPositions(sScreenRef,mSavedIconPositions);
			mSavedIconPositions=0;
		}
	}
	
	if (mScreenShot)
	{
		::KillPicture(mScreenShot);
		mScreenShot=0;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGraphicsContext::~CGraphicsContext()
{
#if CheckVBLTimer
	ECHO("VBL firing at: "<< mFPSTester.CalcRate());
	mFPSTester.StopTask();
#endif
	
	KillBaseContext();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Initialise
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext::Initialise()
{
	OSStatus	err=noErr;
		
	// Get a screen ref for the main display
	sScreenRef=::NewScreenRef(0L,&err);
	ThrowIfOSErr_(err);
	
#if USE_SEMAPHORE_FOR_VBL
	if (UEnvironment::IsRunningOSX())
		ThrowIfOSErr_(MPCreateBinarySemaphore(&sVBLSemaphore));
#endif
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Shutdown
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext::Shutdown()
{
	if (sScreenRef!=kNoScreen)
	{
		::DisposeScreenRef(sScreenRef);
		sScreenRef=kNoScreen;
	}
	
	if (sWindowGroup)
	{
		::ReleaseWindowGroup(sWindowGroup);
		sWindowGroup=NULL;
	}

#if USE_SEMAPHORE_FOR_VBL
	if (UEnvironment::IsRunningOSX())
	{
		MPDeleteSemaphore(sVBLSemaphore);
		sVBLSemaphore=0;
	}
#endif
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShowFPS
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Shows/Hides the frames per second counter
void CGraphicsContext::ShowFPS(
	bool	inOn)
{
	mFPSOn=inOn;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StepMoan
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Steps MOAN one step
void CGraphicsContext::StepMoan()
{
	mMoanManager.StepMoanPrograms();
	
	// moan all the viewports
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *vp=indexer.GetNextData())
		vp->MoanViewport();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ WaitFrameTime
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Blocks until it's time for the frame to be blitted to screen
void CGraphicsContext::WaitFrameTime()
{
	if (!mVBLSync || !mVBLSyncAvailable)
	{
		// vbl sync off, we're using the ms timer then
		
		// wait for it...
		
		if (USE_RNE_FOR_FRAME_WAIT)
		{
			while (sMsTimer.GetTime()<mNextBlitTime)
			{
				UInt32		waitForMs=mNextBlitTime-sMsTimer.GetTime();
				EventRef	e;
				
				// block in event manager until our time - might be very crap on OS 9
				mProgram->TBReceiveNextEvent(0,0,kEventDurationMillisecond*waitForMs,false,0);
			}
		}
		else
		{
			// spin until draw time
			while (sMsTimer.GetTime()<mNextBlitTime)
				;
		}
		
		mNextBlitTime+=mTimeUnitsPerFrame;
	
		// Don't let the frame rate get more then one frame ahead
		if ((mNextBlitTime+mTimeUnitsPerFrame)<sMsTimer.GetTime())
			mNextBlitTime=sMsTimer.GetTime();
	}
	else
	{
		// vbl sync on
	
		if (mVBLCount>=mNextBlitTime)
		{
			UInt32		currTime=mVBLCount;

			// blit synchronisation missed, wait for next one
#if USE_SEMAPHORE_FOR_VBL
			if (UEnvironment::IsRunningOSX())
			{
				// wait for it to switch back
				while (mVBLCount==currTime)
					MPWaitOnSemaphore(sVBLSemaphore,kDurationMillisecond*1000);		// this will consume all outstanding semaphores before blocking on one
				// now run with it
			}
			else
			{
#endif

			// spin lock, used on os 8/9 or when USE_SEMAPHORE_FOR_VBL is 0
			while (mVBLCount==currTime)
				;

#if USE_SEMAPHORE_FOR_VBL
			}
#endif

			mNextBlitTime=mVBLCount+mTimeUnitsPerFrame;
		}
		else
		{
			// wait for it...
			while (mVBLCount<mNextBlitTime)
				;
			mNextBlitTime+=mTimeUnitsPerFrame;
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetFrameRate												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adjusts the blit timing to try to get as close to the desired fps as possible whilst still syncing with the vbl
// Passing zero is the same as maximising the framerate
void CGraphicsContext::SetFrameRate(
	UInt16			inFrameRate)
{
	if (inFrameRate)
	{
		mFrameRate=inFrameRate;
		
		Fixed			fps=inFrameRate<<16;	// convert to fixed point
		
		fps=::FixDiv(mClockFrequency,fps);
		if (fps==positiveInfinity || fps==negativeInfinity || fps<=0)
		{
			// fps was either excessively large or zero, invalid anyway
			UTBException::ThrowBadFrameRate();
		}
		else
			mTimeUnitsPerFrame=::FixRound(fps);
	}
	else
		mTimeUnitsPerFrame=0;		// max speed, aka "go crazy"
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ VBLProc													Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Used to synchronise to the VBL during
pascal void CGraphicsContext::VBLProc(
	long		inData)
{
	CGraphicsContext		*context=(CGraphicsContext*)inData;
	
	context->mVBLCount++;

#if USE_SEMAPHORE_FOR_VBL
	if (UEnvironment::IsRunningOSX())
		MPSignalSemaphore(sVBLSemaphore);
#endif
	
#if CheckVBLTimer
	context->mFPSTester.Pulse();
#endif
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetWorkCanvas
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the canvas which all drawing should be directed to.
CCanvas *CGraphicsContext::GetWorkCanvas()
{
	return mCanvasManager.GetTargettedCanvas();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetWorkCanvasIndex
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the canvas index which all drawing should be directed to.
TTBInteger CGraphicsContext::GetWorkCanvasIndex()
{
	return mCanvasManager.GetTargettedCanvasIndex();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetVBLSync
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Turns vbl syncing on and off
void CGraphicsContext::SetVBLSync(
	bool		inState)
{
	if (!mVBLSyncAvailable)
		return;

	if (inState!=mVBLSync)
	{
		mVBLSync=inState;
		
		// we also need to update the mClockFrequency and reset the blit times
		ResetFrameRateClock();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ResetFrameRateClock
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The framerate clock either works at the frequency of the monitor, or if LCD or vbl sync is
// off, at the frequency of the ms timer. If the mode is changed then the clock frequency
// needs updating, as does the next frame time and units between frames.
// Call this to reset the clock after changing the vbl sync flag
void CGraphicsContext::ResetFrameRateClock()
{
	mClockFrequency=mVBLSync ? mCurrentRes.refreshRate : (1000 << 16);
	mNextBlitTime=0;			// blit immediately
	SetFrameRate(mFrameRate);	// recalc the time between frames
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Pause														/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Pauses the dsp context
void CGraphicsContext::Pause()
{	
	mGraphicsPaused=true;

	if (mGoFullScreen)
	{
		Rect		screenBounds={0,0,mPreviousRes.height,mPreviousRes.width};
		OSErr		err;
			
		err=::ReleaseDesktop(sScreenRef);
		if (err)
			ECHO("Error releasing desktop: " << err);
		err=::SetResolutionAndColorDepth(sScreenRef,mPreviousRes.resolutionID,mPreviousRes.colorDepths);
		if (err)
			ECHO("Error restoring resolution: " << err);
		err=::RestoreIconPositions(sScreenRef,mSavedIconPositions);
		if (err)
			ECHO("Error restoring icon positions: " << err);
		mSavedIconPositions=0;

		::HideWindow(mWindow);

		// Make a black window
		mBackWindow=::NewCWindow(0L,&screenBounds,"\p",true,plainDBox,(WindowPtr)-1,false,0L);
		if (!mBackWindow)
			Throw_(memFullErr);
		
		// Fill the window with black
		UpdateInBlack(mBackWindow);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Resume													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Resumes the dsp context
void CGraphicsContext::Resume()
{
	if (mGoFullScreen)
	{
		// Remove the window
		if (mBackWindow)
		{
			::DisposeWindow(mBackWindow);
			mBackWindow=0L;
		}
		
		// Take control of the desktop
		WindowPtr	backWindow;
		::SaveIconPositions(sScreenRef,&mSavedIconPositions);
		::SetResolutionAndColorDepth(sScreenRef,mCurrentRes.resolutionID,mCurrentRes.colorDepths);
		ThrowIfOSErr_(::AcquireDesktop(sScreenRef,&backWindow,0L));

		SortOutWindowLevels(backWindow);

		::ShowWindow(mWindow);

		// Reposition our window in the centre of the screen res switching can move it around a bit
		Rect		windowBounds;
		::GetPortBounds(::GetWindowPort(mWindow),&windowBounds);
		
		RLDrawInfo	drawInfo;
		
		drawInfo.reserved=0;
		GetCurrentScreenDrawingInfo(sScreenRef,&drawInfo);
		
		CentreRectInRect(&windowBounds,&drawInfo.bounds);
		::MacMoveWindow(mWindow,windowBounds.left,windowBounds.top,false);
	}
		
	::SetPortWindowPort(mWindow);
	::SelectWindow(mWindow);

	mGraphicsPaused=false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetViewport
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// NOTE: Don't scroll the viewport directly, instead use the graphics context's ViewportOffset
// function. This function is useful for getting references to viewports to store in lists
CViewport *CGraphicsContext::GetViewport(
	TTBInteger			inCanvas)
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		if (viewport->GetCanvas()==inCanvas)
		{
			return viewport;
		}
	}
	
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetViewportXOffset										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGraphicsContext::GetViewportXOffset(
	TTBInteger		inCanvas)
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		if (viewport->GetCanvas()==inCanvas)
			return viewport->GetXOffset();
	}
	
	UTBException::ThrowViewportNotOpen();
	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetViewportYOffset										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGraphicsContext::GetViewportYOffset(
	TTBInteger		inCanvas)
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		if (viewport->GetCanvas()==inCanvas)
			return viewport->GetYOffset();
	}
	
	UTBException::ThrowViewportNotOpen();
	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetViewportWidth										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGraphicsContext::GetViewportWidth(
	TTBInteger		inCanvas)
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		if (viewport->GetCanvas()==inCanvas)
			return viewport->GetWidth();
	}
	
	UTBException::ThrowViewportNotOpen();
	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetViewportHeight										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGraphicsContext::GetViewportHeight(
	TTBInteger		inCanvas)
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		if (viewport->GetCanvas()==inCanvas)
			return viewport->GetHeight();
	}
	
	UTBException::ThrowViewportNotOpen();
	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderViewports
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext::RenderViewports()
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		viewport->Render();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ UpdateInBlack							Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Fills the passed window with black and between a begin/end update pair
void CGraphicsContext::UpdateInBlack(
	WindowPtr			inWin)
{
	UPortSaver			safe(inWin);
	RGBColour			colour={0x0000,0x0000,0x0000};
	UForeColourSaver	colourSafe(colour);
	Pattern				pattern;
	Rect				windowRect;
	
	::GetPortBounds(::GetWindowPort(inWin),&windowRect);
	
	::BeginUpdate(inWin);		
	::GetQDGlobalsBlack(&pattern);
	::FillRect(&windowRect,&pattern);
	::EndUpdate(inWin);
	
	::QDFlushPortBuffer(::GetWindowPort(inWin),0);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EventUpdate
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CGraphicsContext::EventUpdate(
		const EventRecord&	inMacEvent)
{
	WindowPtr	wp=(WindowPtr)inMacEvent.message;

	if (wp==mBackWindow)
	{
		UpdateInBlack(mBackWindow);
		
		return true;
	}
	else if (wp==mWindow)
	{
		UWindowUpdater			safe(wp);
		
		RenderSpritesAndBlit();
		
		return true;
	}
	
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EventSuspend
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext::EventSuspend(
		const EventRecord&	inMacEvent)
{
	if (mBackWindow)
		::HideWindow(mBackWindow);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EventResume
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext::EventResume(
		const EventRecord&	inMacEvent)
{		
	if (mBackWindow)
		::ShowWindow(mBackWindow);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FillQDDirtyRgn
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext::FillQDDirtyRgn()
{
	CGrafPtr		p=GetWindowPort(mWindow);

	::QDSetDirtyRegion(p,mWindowBoundsRgn);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClearQDDirtyRgn
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext::ClearQDDirtyRgn()
{
	CGrafPtr		p=GetWindowPort(mWindow);
	
	if (::QDIsPortBufferDirty(p))
	{
		RgnHandle		r=::NewRgn();
		
		::QDSetDirtyRegion(p,r);
		::DisposeRgn(r);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReactToNewMap
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// For all TMAP viewports
void CGraphicsContext::ReactToNewMap()
{
	CListIndexerT<CViewport>	indexer(&mViewports);
	CTMAPViewport				*tvp=0;
	SInt32						processed=0,max=mViewports.CountElements();
	
	while (CViewport *vp=indexer.GetNextData())
	{
		if (tvp=dynamic_cast<CTMAPViewport*>(vp))
		{
			TTBInteger		layer=tvp->GetLayerIndex();
			TTBInteger		canvasId=tvp->GetCanvas();
			Rect			rect;
			
			rect.left=tvp->GetLeft();
			rect.top=tvp->GetTop();
			rect.right=rect.left+tvp->GetWidth();
			rect.bottom=rect.top+tvp->GetHeight();
		
			RemoveViewport(canvasId,true);
			CreateViewport(*mProgram,canvasId,rect,true,layer);
		}
		
		// i'm a bit worried that removing and then creating the viewport will make the
		// list infinitely long as viewports are appended to the end of the list, so after
		// we've ran through the original viewport count we should bail.
		processed++;
		if (processed==max)
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReactToChangedTile
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// For all TMAP viewports that are viewing the changed tile, redraw them
void CGraphicsContext::ReactToChangedTile(
	TTBInteger		inX,
	TTBInteger		inY,
	TTBInteger		inZ)
{
	CListIndexerT<CViewport>	indexer(&mViewports);
	CTMAPViewport				*tvp=0;
	
	while (CViewport *vp=indexer.GetNextData())
	{
		if (tvp=dynamic_cast<CTMAPViewport*>(vp))
			tvp->ReactToChangedTile(inX,inY,inZ);
	}
}