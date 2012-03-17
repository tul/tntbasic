// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CGraphicsContext.h
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

#pragma once

#include						<DrawSprocket.h>
#include						"CSpriteManager.h"
#include						"UTimingUtils.h"
#include						"CCanvasManager.h"
#include						"CViewport.h"
#include						"RezLib.h"
#include						"CTBSpriteLayer.h"
#include						"CMoanManager.h"

class CProgram;

#define CheckVBLTimer			0

class CGraphicsContext
{
	private:
		void					KillBaseContext();		

	public:
		static const short		kMaxCanvases=20;
		
	protected:
		bool					mFPSOn;
		bool					mGraphicsPaused;

		CProgram				*mProgram;

		CLinkedListT<CViewport>	mViewports;

		volatile UInt32			mVBLCount;					// # of vbl ticks so far, essemtially a clock that runs at the Hz of the screen
		bool					mVBLSync;					// true/false controls whether to wait for VBL before blitting
		bool					mVBLSyncAvailable;			// vbl syncing is not available on lcd screens
		
		UInt32					mNextBlitTime;				// these two are in vbl ticks if vbl syncing is on, or
		UInt16					mTimeUnitsPerFrame;			// in ms ticks vbl syncing is off
		Fixed					mClockFrequency;			// frequency in Hz of the above vars, 16:16 fixed point notation
															// is 1000Hz for ms timer, or the screen rate for vbl timer

		UInt16					mFrameRate;					// the programs desired frame rate (from 'set framerate' command)
		
		static ScreenRef		sScreenRef;
		static WindowGroupRef	sWindowGroup;				// window group of the tnt basic content window

		WindowPtr				mWindow,mBackWindow;		// back window is opened when the context is paused, it ensures the desktop remains covered. It remains open until resumption, it is hidden if tb is suspened
		RgnHandle				mWindowBoundsRgn;			// entire window bounds rgn
		RetraceToken			mRetraceToken;
		RLInfo					mPreviousRes,mCurrentRes;
		Handle					mSavedIconPositions;
		bool					mGoFullScreen;
		
		bool					mTakeScreenShot;
		PicHandle				mScreenShot;
		
		CRateTimer				mFPSTester;

		bool					mAutoMoan;					// if true, moan is done every draw frame, else every time 'step moan' is called
		bool					mMasterMoanPause;
		
#if USE_SEMAPHORE_FOR_VBL
		static MPSemaphoreID	sVBLSemaphore;				// used to block the main task until the vbl fires
#endif
		
		static pascal void		VBLProc(
									long		inData);

		void					ResetFrameRateClock();
		void					SortOutWindowLevels(
									WindowPtr	inBackWindow);
		void					InstallStdWindowHandlers(
									WindowRef	inWindow);

	public:
		enum EGraphicsContextType
		{
			kBlastGraphicsContext=0,
			kOpenGLGraphicsContext,
		};

		static CMsTimer			sMsTimer;
		
		CSpriteManager			mSpriteManager;
		CCanvasManager			mCanvasManager;
		CMoanManager			mMoanManager;

								CGraphicsContext(
									SInt16		inWide,
									SInt16		inHigh,
									SInt16		inDepth,
									bool		inGoFullScreen,
									CProgram	*inProgram);
		virtual					~CGraphicsContext();
		
		static void				Initialise();
		static void				Shutdown();
		
		bool					UsingWindowMode()					{ return !mGoFullScreen; }
		
		CCanvas					*GetWorkCanvas();
		TTBInteger				GetWorkCanvasIndex();
		CCanvas					*GetCanvas(TTBInteger inCanvas)		{ return mCanvasManager.GetCanvas(inCanvas); }
		
		virtual void			RenderSpritesAndBlit()=0;
		void					WaitFrameTime();

		virtual void			ShowFPS(
									bool		inOn);

		bool					GetVBLSync()				{ return mVBLSync; }
		void					SetVBLSync(
									bool		inState);

		virtual void /*e*/		Pause();
		virtual void /*e*/		Resume();
		
		virtual void			StepMoan();
		bool					IsMoanAuto()				{ return mAutoMoan; }
		void					SetAutoMoan(
									bool		inAuto)		{ mAutoMoan=inAuto; }
		
		void					SetMasterMoanPause(
									bool		inState)		{ mMasterMoanPause=inState; }
		bool					GetMasterMoanPause()			{ return mMasterMoanPause; }
		
		void /*e*/				SetFrameRate(
									UInt16		inFPS);
									
		virtual void			InvalAll()=0;
		
		virtual PicHandle		TakeScreenShot(
									bool		inGrabFromWindow)=0;
		
		virtual void			AddSpriteToCanvas(
									CTBSprite	*inSprite,
									TTBInteger	inCanvas)=0;
		
		virtual void			RemoveSpriteFromCanvas(
									CTBSprite	*inSprite,
									TTBInteger	inCanvas)=0;
		
		virtual void			CreateViewport(
									CProgram		&inProgram,
									TTBInteger		inCanvasID,
									const Rect		&inRect,
									bool			inCreateTMAPViewport,
									TTBInteger		inLayer)=0;
		
		virtual void			RemoveViewport(
									TTBInteger		inID,
									bool			inThrow)=0;
		
				CViewport		*GetViewport(
									TTBInteger		inCanvas);
	
		virtual void 			ViewportOffset(
									TTBInteger		inID,
									TTBInteger		inX,
									TTBInteger		inY)=0;
									
		virtual void			ViewportOffset(
									CViewport		*inViewport,
									TTBInteger		inX,
									TTBInteger		inY)=0;
		
		TTBInteger				GetViewportXOffset(
									TTBInteger		inCanvas);

		TTBInteger				GetViewportYOffset(
									TTBInteger		inCanvas);
									
		TTBInteger				GetViewportWidth(
									TTBInteger		inCanvas);
									
		TTBInteger				GetViewportHeight(
									TTBInteger		inCanvas);
		
		void					RenderViewports();

		static void				UpdateInBlack(
									WindowPtr			inWin);
								
		virtual bool			EventUpdate(
									const EventRecord&	inMacEvent);
									
		void					EventSuspend(
									const EventRecord&	inMacEvent);
									
		void					EventResume(
									const EventRecord&	inMacEvent);
		
		static ScreenRef		GetScreenRef()						{ return sScreenRef; }
		
		void					ClearQDDirtyRgn();
		void					FillQDDirtyRgn();
		
		void					ReactToChangedTile(
									TTBInteger		inX,
									TTBInteger		inY,
									TTBInteger		inZ);

		void							ReactToNewMap();
		
		virtual EGraphicsContextType	GetType()=0;
		
		inline WindowRef		GetGameWindow()		{ return mWindow; }
};