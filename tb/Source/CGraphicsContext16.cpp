// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CGraphicsContext16.cpp
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

/*
	Encapsulates managing the graphics for TNT Basic.
	It manages a work world, a store world an a window. The window and monitor resolution are managed using draw sprocket.
	It uses copybits to blit to the screen and BlastCopy16 between the worlds.
	
	It also uses VBL syncing while blitting and regulates the frame rate of the blits. It manages a full sprite context
	via a sprite manager and BLAST++ and supports showing the FPS.
	
	It's not incredibly modular due to the way it kindof does everything but it's easy to use and you can't really go far
	wrong with it.
*/

/*
	Window PixMaps on OS X

	the problem with blitting currently is that the base addr and the clipping rects are wrong
	this due to the pixmap bound being bigger than the port bounds for windows on os x.
	i think this is cos they include the title bar, borders etc in the buffer.
	so basically, the code to make the bcrec from a window needs to take this into account when
	trying to figure out point (0,0) on the pixmap. Actually, I think 0,0 is calculated by just
	adding the negative coords still, but then maybe you shouldn't do it for the gdevice...
	confused now...
	Anyway, the bigger rect from the window port rect results in a larger clipping rgn (and
	presumably drawable area) for windows than backbuffers and the different in rects causes a
	crash when blastcopy copies without being clipped right.
	
	Flickering
	
	In some cases, it is possible that sprites will flicker in TNT Basic when drawing direct
	to window. This happens if a quickdraw command draws to the window and invals some part
	of the canvas. The changes are automatically flushed the next time RNE is called, and then
	when the sprites are rendered and flushed you get the flicker.
	This can be stopped by clearing the dirty rgn before calling RNE, but you'll have to ensure
	you keep track of the dirty rgns yourself then, as tnt basic does.

	Or, in short : Before calling RNE call graphicscontext->ClearQDDirtyRgn() or your sprites
	will flicker.
*/

#include		"TNT_Debugging.h"
#include		"Carbon_Settings.h"
#include		<Displays.h>
#include		"CGraphicsContext16.h"
#include		"CQDBufferedPortDrawBuffer.h"
#include		"Utility Objects.h"
#include		"UTBException.h"
#include		"CBLCanvas16.h"
#include		"CTBSprite16.h"
#include		"CTBSpriteGL.h"
#include		"Root.h"
#include		"CTMAPViewport.h"

// I don't think profiler likes the MP stuff that RezLib uses, it tends to crash things.
// I tried to get it working here, but I think we'd have to take the calls to the MP stuff out from RezLib
#define			DISABLE_VBL_PROC			__profile__
#define			TIME_RENDER_LOOP			0

#if DISABLE_VBL_PROC
#undef USE_SEMAPHORE_FOR_VBL
#define USE_SEMAPHORE_FOR_VBL 0
#endif

#if TIME_RENDER_LOOP
UInt32						gTimeSlots;
AbsoluteTime				gViewportTime;
AbsoluteTime				gStoreTime;
AbsoluteTime				gRenderTime;
AbsoluteTime				gFlushTime;
AbsoluteTime				gRestoreTime;
#endif

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Allocates gworlds blast ++ classes.
CGraphicsContext16::CGraphicsContext16(
	SInt16		inWide,
	SInt16		inHigh,
	bool		inGoFullScreen,
	CProgram	*inProgram) :
	CGraphicsContext(inWide,inHigh,16,inGoFullScreen,inProgram),
	mStoreCanvas(0),
	mWork2Screen(0),
	mCachedPortBaseAddr(NULL),
	mRecalcScreenRecCount(0)
{
	InitABCRec(&mScreenRec);
		
	Try_
	{
		Rect		windowRect={0,0,inHigh,inWide};
		
		// Make an abcrec from the window, if the context is paused and then resumed this abcrec must be rebuilt as dsp
		// reallocates the pixmap leaving the abcrec's ptrs hanging
		// NB: mScreenRec is not used under OS X
		MakeBCRecFromWindow(mWindow,&mScreenRec.rec,::GetMainDevice());
		if (!BCToABCRec(&mScreenRec))
			Throw_(memFullErr);

		Rect			worldRect;
		
		::GetPortBounds(::GetWindowPort(mWindow),&worldRect);

		::SetPortWindowPort(mWindow);		// set port to the window else copybits won't work
		::LocalToGlobalRect(&worldRect);

		GWorldPtr			storeWorld=0;
		CBLCanvas16			*workCanvas=0;

		// Open a work canvas
		if (DRAW_DIRECT_TO_WINDOW)
		{
			// we need to lock the port bits so that all drawing is directed to the window's
			// backbuffer
			::LockPortBits(GetWindowPort(mWindow));
			workCanvas=new CBLCanvas16(mWindow,false);
			::UnlockPortBits(GetWindowPort(mWindow));
		}
		else
		{
			GWorldPtr			workWorld=0;
			// For optimal performance, we should created the gworld linked to the device we want to blit to
			// from what i can tell, at least on Mac OS X 10.4.2, this makes the gworld the same depth as the target device
			// if we're not doing full screen then the depth of the main screen isn't controlled by us and so we can't rely on it
			// being 16. For tb to work, its sprite engine needs a 16 bit backing store, so we need to detach it from the device when
			// in window mode, else attach it to the device for full screen.
			GWorldFlags			flags=kNativeEndianPixMap;
			if (mGoFullScreen)
			{
				flags|=noNewDevice;
				ThrowIfOSErr_(::NewGWorld(&workWorld,16,&windowRect,0L,::GetMainDevice(),flags));
			}
			else
			{
				ThrowIfOSErr_(::NewGWorld(&workWorld,16,&windowRect,0L,NULL,flags));
			}
			workCanvas=new CBLCanvas16(workWorld);
		}
		
		ThrowIfMemFull_(workCanvas);
		mCanvasManager.SetMainCanvas(workCanvas);
		
		// create the buffer that copies to screen. These are different on X and 9, the X
		// version uses QDFlushPortBuffer, the 9 version uses either copy bits or blast copy
		if (DRAW_DIRECT_TO_WINDOW)
			mWork2Screen=new CQDBufferedPortDrawBuffer;
		else
		{
#ifndef UseCopyBitsToScreen
			mWork2Screen=new CBlastStandardBufferA16;
#else
			mWork2Screen=new CBlastCopyBitsBuffer;
#endif
		}
		ThrowIfMemFull_(mWork2Screen);

		// Open a store canvas
		ThrowIfOSErr_(::NewGWorld(&storeWorld,16,&windowRect,0L,::GetMainDevice(),kNativeEndianPixMap|noNewDevice));
		mStoreCanvas=CBLCanvas::NewCanvas(storeWorld,true);
		ThrowIfMemFull_(mStoreCanvas);

		// Set up BLAST++
		// Initialize the buffers
		mStore2Work.SetSourceCanvas(mStoreCanvas);
		mStore2Work.SetDestCanvas(workCanvas);
		mWork2Screen->SetSourceCanvas(workCanvas);
		
		if (DRAW_DIRECT_TO_WINDOW)
			mWork2Screen->SetDestCanvas(workCanvas);			// when drawing direct to window, work and screen are the same
		else
			mWork2Screen->SetDestBuffer(&mScreenRec);
		
		workCanvas->SetDrawBuffer(mWork2Screen);
				
		if (!mStore2Work.SetMaxBlitRects(kMaxSprites,kMaxCustomRects))
			Throw_(memFullErr);
		if (!mWork2Screen->SetMaxBlitRects(kMaxSprites,kMaxCustomRects))
			Throw_(memFullErr);
	
		// Initialise the layer manager
		mLayerManager.SetDrawBuffer(mWork2Screen);
		mLayerManager.SetBackupBuffer(&mStore2Work);
		mLayerManager.SetMaxLayers(kMaxLayers);
		
		for (short n=kMaxCanvases-1; n>=0; n--)
		{
			if (mLayerManager.AddLayer(&mCanvasLayer[n])==-1)
				Throw_(memFullErr);
		}
		
		if (mLayerManager.AddLayer(&mLayer0)==-1)
			Throw_(memFullErr);
		
		// Initialise the layers
		Rect	theRect={0,0,0,0};
		for (short n=0; n<kMaxCanvases; n++)
			mCanvasLayer[n].SetLayerClipRect(theRect);
		
		mFPSSprite.SetX(inWide-50);
		mFPSSprite.SetY(inHigh-20);
		
		if (DRAW_DIRECT_TO_WINDOW)
			LockScreen();
		workCanvas->EraseAll();
		if (DRAW_DIRECT_TO_WINDOW)
			UnlockScreen();			
		mStoreCanvas->EraseAll();
	}	
	Catch_(err)
	{
		ECHO("Error caught in CGraphicsContext16 constructor, err code " << ErrCode_(err));
		KillContext16();
		throw;
	}
	
	#if CheckVBLTimer
		ShowFPS(true);
		mFPSTester.StartTask();
	#endif
	
	#if TIME_RENDER_LOOP
	gTimeSlots=0;
	gViewportTime.hi=gStoreTime.hi=gRenderTime.hi=gFlushTime.hi=gRestoreTime.hi=0;
	gViewportTime.lo=gStoreTime.lo=gRenderTime.lo=gFlushTime.lo=gRestoreTime.lo=0;
	#endif
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ KillContext16
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Used by destructor/constructor to free resources from this object
void CGraphicsContext16::KillContext16()
{
	delete mStoreCanvas;
	mStoreCanvas=0;
	delete mWork2Screen;
	mWork2Screen=0;
		
	::DestroyABCRec(&mScreenRec);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGraphicsContext16::~CGraphicsContext16()
{
#if CheckVBLTimer
	ECHO("VBL firing at: "<< mFPSTester.CalcRate());
	mFPSTester.StopTask();
#endif

#if TIME_RENDER_LOOP
	double		total=0;
	double		temp;
	ECHO("Render times in Ms\n------------------\n");
	temp=CApplication::AbsoluteToSeconds(gViewportTime)/gTimeSlots; total+=temp;
	ECHO("Avg time for viewport render: " << 1000.0*temp);
	temp=CApplication::AbsoluteToSeconds(gStoreTime)/gTimeSlots; total+=temp;
	ECHO("Avg time for store: " << 1000.0*temp);
	temp=CApplication::AbsoluteToSeconds(gRenderTime)/gTimeSlots; total+=temp;
	ECHO("Avg time for render: " << 1000.0*temp);
	temp=CApplication::AbsoluteToSeconds(gFlushTime)/gTimeSlots;
	total+=temp;
	ECHO("Avg time for flush: " << 1000.0*temp);
	temp=CApplication::AbsoluteToSeconds(gRestoreTime)/gTimeSlots;
	total+=temp;
	ECHO("Avg time for restore: " << 1000.0*temp);
	ECHO("Avg total time: " << 1000.0*total);
#endif

	if (DRAW_DIRECT_TO_WINDOW)
		ECHO("Screen rec was rebuilt " << mRecalcScreenRecCount << " times");
	
	KillContext16();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShowFPS
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Shows/Hides the frames per second counter
void CGraphicsContext16::ShowFPS(
	bool	inOn)
{
	if (inOn!=mFPSOn)
	{
		CGraphicsContext::ShowFPS(inOn);
		
		if (inOn)
			mLayer0.AddSpriteToLayer(&mFPSSprite);
		else
			mLayer0.RemoveSpriteFromLayer(&mFPSSprite);
	}
}

#if TIME_RENDER_LOOP
	#define TIME_VARS					AbsoluteTime start,tempTime;
	#define TIME_BEGIN_VIEWPORT			start=UpTime();
	#define TIME_BEGIN_STASHBACKDROP	tempTime=UpTime(); gViewportTime=AddAbsoluteToAbsolute(gViewportTime,SubAbsoluteFromAbsolute(tempTime,start)); start=tempTime;
	#define TIME_BEGIN_RENDER			tempTime=UpTime(); gStoreTime=AddAbsoluteToAbsolute(gStoreTime,SubAbsoluteFromAbsolute(tempTime,start)); start=tempTime;
	#define TIME_BEGIN_FLUSH			tempTime=UpTime(); gRenderTime=AddAbsoluteToAbsolute(gRenderTime,SubAbsoluteFromAbsolute(tempTime,start)); start=tempTime;
	#define TIME_BEGIN_RESTORE			tempTime=UpTime(); gFlushTime=AddAbsoluteToAbsolute(gFlushTime,SubAbsoluteFromAbsolute(tempTime,start)); start=tempTime;
	#define TIME_END					tempTime=UpTime(); gRestoreTime=AddAbsoluteToAbsolute(gRestoreTime,SubAbsoluteFromAbsolute(tempTime,start)); start=tempTime; gTimeSlots++;
#else
	#define TIME_VARS
	#define TIME_BEGIN_VIEWPORT
	#define TIME_BEGIN_STASHBACKDROP
	#define TIME_BEGIN_RENDER
	#define TIME_BEGIN_FLUSH
	#define TIME_BEGIN_RESTORE
	#define TIME_END
#endif

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LockScreen
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Locks the window port pixels and calculates the ABCRec for the window. If the base address hasn't changed for the
// window then it keeps the previous ABCRec to speed things up.
void CGraphicsContext16::LockScreen()
{
	CGrafPtr		port=GetWindowPort(mWindow);

	::LockPortBits(port);
	
	PixMapHandle	h=::GetPortPixMap(port);
	Ptr				p=::GetPixBaseAddr(h);
	ABCRec			&rec=mWork2Screen->GetSourceCanvas()->GetABCRec();

	if (p!=mCachedPortBaseAddr)
	{
		mCachedPortBaseAddr=p;
		mRecalcScreenRecCount++;
	
		DestroyABCRec(&rec);
		MakeBCRecFromWindow(mWindow,&rec.rec,GetMainDevice());
		if (!BCToABCRec(&rec))
			Throw_(memFullErr);	
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ UnlockScreen
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext16::UnlockScreen()
{
	::UnlockPortBits(GetWindowPort(mWindow));	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderSpritesAndBlit
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Performs one render sprites and blit cycle.
// If the member variable mTakeScreenShot is set then the window is not blitted to but a screen shot is saved into
// mScreenShot as a picture handle.
void CGraphicsContext16::RenderSpritesAndBlit()
{
	TIME_VARS
	
	if (mGraphicsPaused)			// if the context is paused, then the graphics rec of the window's backbuffer is unlocked and drawing to it will crash us
		return;

	if (mAutoMoan && !mMasterMoanPause)
		StepMoan();

	GrafPtr		windowPort=::GetWindowPort(mWindow);	

	mLayerManager.CalcAllLayers();			// calculate the areas to be blitted

	TIME_BEGIN_VIEWPORT

	if (DRAW_DIRECT_TO_WINDOW)
		LockScreen();

	RenderViewports();						// render the viewports

	TIME_BEGIN_STASHBACKDROP
	
	mLayerManager.StashBackdrop();			// back up areas that sprites will trash

	TIME_BEGIN_RENDER
	
	mLayerManager.RenderAllLayers();		// render the sprites
	
	TIME_BEGIN_FLUSH

	if (!mTakeScreenShot)
	{
		WaitFrameTime();					// hum..dee...humm...... and go!

#if 0	// BLIT FULL BACK BUFFER?
		mWork2Screen->BlitAllSD();
		::QDFlushPortBuffer(windowPort,0);

#else
		mWork2Screen->BlitAllRectsSD();			// blit to screen
		
		if (!DRAW_DIRECT_TO_WINDOW)
			::QDFlushPortBuffer(windowPort,0);
#endif
	}
	else
	{
		// record the work buffer as a pic handle
		mTakeScreenShot=false;
		
		// destroy existing screenshot
		if (mScreenShot)
		{
			::KillPicture(mScreenShot);
			mScreenShot=0;
		}
		
		ECHO("Taking screen shot from work buffer");
		
		Try_
		{
			UWorldSaver			safe(mWork2Screen->GetSourceBuffer()->rec.world);
			UForeColourSaver	safe1(TColourPresets::kBlack);
			UBackColourSaver	safe2(TColourPresets::kWhite);
			
			PicHandle			recordPic=0L;
			Rect				r=(**mWork2Screen->GetSourceBuffer()->rec.pixMap).bounds;
					
			mScreenShot=OpenPicture(&r);
			if (mScreenShot)
			{				
				::CopyBits(	(BitMap*)*mWork2Screen->GetSourceBuffer()->rec.pixMap,
							(BitMap*)*mWork2Screen->GetSourceBuffer()->rec.pixMap
							,&r,&r,srcCopy,0L);
				ClosePicture();
				ECHO("Screenshot ok");
			}
			else
				ECHO("Screenshot failed");
			
		}
		Catch_(err)
		{
			ECHO("Screenshot error: " << ErrCode_(err));
		}
	}

	TIME_BEGIN_RESTORE

	mWork2Screen->ClearBlitList();			// clear the blit list
		
	mLayerManager.RestoreBackdrop();		// restore the backdrop

	if (DRAW_DIRECT_TO_WINDOW)
		UnlockScreen();

	TIME_END
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StepMoan
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Steps MOAN one step
void CGraphicsContext16::StepMoan()
{
	mLayerManager.MoanAllLayers();			// moan all the sprites (local channels)
	
	CGraphicsContext::StepMoan();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetCanvasLayer
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
BlastSpriteLayer *CGraphicsContext16::GetCanvasLayer(
	TTBInteger		inCanvasIndex)
{
	GetCanvas(inCanvasIndex);

	if (inCanvasIndex==0)
		return &mLayer0;
	else
		return &mCanvasLayer[inCanvasIndex-1];
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Pause														/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Pauses the dsp context
void CGraphicsContext16::Pause()
{
	// take a screen shot from the window contents and store it incase they choose take
	// screen shot while paused
	if (mScreenShot)
	{
		::KillPicture(mScreenShot);
		mScreenShot=NULL;
	}
	mScreenShot=TakeScreenShot(true);

	// if we're going direct to the window, hiding and unhiding the window will
	// trash our backbuffer and lose its contents. To rectify this we store the contents
	// in our store world before hand
	if (DRAW_DIRECT_TO_WINDOW)
	{
		LockScreen();
		mStore2Work.BlitAllDS();
		UnlockScreen();
	}
	
	CGraphicsContext::Pause();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Resume													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Resumes the dsp context
void CGraphicsContext16::Resume()
{
	CGraphicsContext::Resume();
	
	if (mScreenShot)
	{
		::KillPicture(mScreenShot);
		mScreenShot=NULL;
	}
	
	// An important note
	// When draw sprocket recieves a suspend event it seems to deallocate the front window's pixmap handle, it reallocates
	// it again when the context is resumed. This means our ABCRec get's fecked over and needs to be reallocated also
	// NB: Screen rec is not used under os x
	::DestroyABCRec(&mScreenRec);
	MakeBCRecFromWindow(mWindow,&mScreenRec.rec,GetMainDevice());
	if (!BCToABCRec(&mScreenRec))
		Throw_(memFullErr);

	if (DRAW_DIRECT_TO_WINDOW)
	{
		LockScreen();		// lock and recalc our port bits

		// if we're going direct to the window, hiding and unhiding the window will have
		// trashed our backbuffer and lost its contents. To rectify this we stored the contents
		// in our store world before hand - phew

		mStore2Work.BlitAllSD();

		UnlockScreen();
	}
	
	// refresh our window
	mWork2Screen->InvalAll();
	RenderSpritesAndBlit();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddSpriteToCanvas
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext16::AddSpriteToCanvas(
	CTBSprite	*inSprite,
	TTBInteger	inCanvas)
{
	if (inCanvas==0)
		mLayer0.AddSpriteToLayer((CTBSprite16*)inSprite,0);
	else
		mCanvasLayer[inCanvas-1].AddSpriteToLayer((CTBSprite16*)inSprite,0);
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RemoveSpriteFromCanvas
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext16::RemoveSpriteFromCanvas(
	CTBSprite	*inSprite,
	TTBInteger	inCanvas)
{
	if (inCanvas==0)
		mLayer0.RemoveSpriteFromLayer((CTBSprite16*)inSprite);
	else
		mCanvasLayer[inCanvas-1].RemoveSpriteFromLayer((CTBSprite16*)inSprite);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CreateViewport									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext16::CreateViewport(
	CProgram		&inProgram,
	TTBInteger		inCanvasID,
	const Rect		&inRect,
	bool			inCreateTMAPViewport,
	TTBInteger		inLayerIndex)		// layer index is ingored if not creating a tmap
{
	// Check the viewport is entirely on screen
	Rect	canvasRect=mCanvasManager.GetCanvas(0)->GetBoundsRect();

	if ((inRect.top<canvasRect.top) || (inRect.left<canvasRect.left) ||
		(inRect.bottom>canvasRect.bottom) || (inRect.right>canvasRect.right))
	{
		UTBException::ThrowViewportOffScreen();
	}

	RemoveViewport(inCanvasID,false);

	CViewport	*viewport=0;
	
	if (inCreateTMAPViewport)
		viewport=CTMAPViewport::CreateNewTMAPViewport(inProgram,inCanvasID,inLayerIndex,inRect); //new CWrappingViewport(inProgram,inCanvasID,inRect,10000,10000);
	else
		viewport=new CViewport(inProgram,inCanvasID,inRect);

	mViewports.AppendElement(viewport);
	
	mCanvasLayer[inCanvasID-1].SetLayerClipRect(inRect);
	
	// Check the viewport is inside the canvas
	canvasRect=mCanvasManager.GetCanvas(inCanvasID)->GetBoundsRect();
	Rect	viewportRect=viewport->GetViewportRect();

	if ((viewportRect.top<canvasRect.top) || (viewportRect.left<canvasRect.left) ||
		(viewportRect.bottom>canvasRect.bottom) || (viewportRect.right>canvasRect.right))
	{
		UTBException::ThrowViewportLargerThanCanvas();
	}
	
	ViewportOffset(inCanvasID,0,0);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RemoveViewport
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext16::RemoveViewport(
	TTBInteger		inCanvas,
	bool			inThrow)
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		if (viewport->GetCanvas()==inCanvas)
		{
			delete viewport;
		
			Rect	theRect={0,0,0,0};
			mCanvasLayer[inCanvas-1].SetLayerClipRect(theRect);
			
			return;
		}
	}
	
	if (inThrow)
		UTBException::ThrowViewportNotOpen();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ViewportOffset										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContext16::ViewportOffset(
	TTBInteger		inCanvas,
	TTBInteger		inX,
	TTBInteger		inY)
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		if (viewport->GetCanvas()==inCanvas)
		{
			viewport->ViewportOffset(inX,inY);
			mCanvasLayer[inCanvas-1].SetXOrigin(inX-viewport->GetLeft());
			mCanvasLayer[inCanvas-1].SetYOrigin(inY-viewport->GetTop());
						
			return;
		}
	}
	
	UTBException::ThrowViewportNotOpen();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ViewportOffset										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// sets the viewport offset to the specified x,y
void CGraphicsContext16::ViewportOffset(
	CViewport		*inViewport,
	TTBInteger		inX,
	TTBInteger		inY)
{
	inViewport->ViewportOffset(inX,inY);
	mCanvasLayer[inViewport->GetCanvas()-1].SetXOrigin(inX-inViewport->GetLeft());
	mCanvasLayer[inViewport->GetCanvas()-1].SetYOrigin(inY-inViewport->GetTop());
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EventUpdate
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CGraphicsContext16::EventUpdate(
		const EventRecord&	inMacEvent)
{
	WindowPtr	wp=(WindowPtr)inMacEvent.message;

	if (wp==mWindow)
	{
		UWindowUpdater			safe(wp);
		
		mWork2Screen->InvalAll();
	}
	
	return CGraphicsContext::EventUpdate(inMacEvent);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ TakeScreenShot
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Takes a screenshot of the current frame. Returns 0L if there was an error, else returns a pic handle which the caller
// adopts.
// There are two method for screen capture, one is to capture it from the window, the other from the backbuffer.
// The backbuffer method seems neater to me, but it has the problem that what's in the backbuffer is what's there now,
// and what's on the screen is what was there last time it was drawn. This has the side effect that those games that
// use drawing commands, like fireworks, come out black, as their backbuffer is empty 90% of the time and is only drawn
// in when it's time to blit.
// If the screenshot is taken from the menu then we have to use the backbuffer. If from ingame we can use the window.
// UPDATE: When pausing, the window contents are saved into mScreenShot so that there's a valid
// screen shot available even when paused.
PicHandle CGraphicsContext16::TakeScreenShot(
	bool		inGrabFromWindow)
{
	// do we have a saved screen shot of the last time the window was updated?
	if (!inGrabFromWindow && mScreenShot)
	{
		ECHO("Taking screen shot from cached screen shot image");
		PicHandle	temp=mScreenShot;
		if (::HandToHand((Handle*)&temp))
			return NULL;
		return temp;
	}

	// if drawing direct to the window, then it doesn't seem possible to record a picture directly from the window, so we'll
	// have to use a gworld instead.
	if (inGrabFromWindow && DRAW_DIRECT_TO_WINDOW)
	{
		ECHO("Attempting to grab from window buffer");
		
		GrafPtr			windowPort=::GetWindowPort(mWindow);
		Rect			winRect;
		OSErr			err;
		PicHandle		recordPic=NULL;
			
		
		::GetPortBounds(windowPort,&winRect);
		
		{
			UWorldSaver			safe(mStoreCanvas->GetGWorld());		// the store gworld will do us nicely
			UForeColourSaver	safe1(TColourPresets::kBlack);
			UBackColourSaver	safe2(TColourPresets::kWhite);
			Rect				storeRect=mStoreCanvas->GetBoundsRect();
			
			PixMapHandle	h=::GetGWorldPixMap(windowPort);
			
			::CopyBits((BitMap*)*h,GetPortBitMapForCopyBits(mStoreCanvas->GetGWorld()),&winRect,&storeRect,srcCopy,0);
			
			recordPic=::OpenPicture(&storeRect);
			
			if (!recordPic)
			{
				ECHO("Couldn't open picture...trying another approach...");
			}
			else
			{
				CopyBits(GetPortBitMapForCopyBits(mStoreCanvas->GetGWorld()),GetPortBitMapForCopyBits(mStoreCanvas->GetGWorld()),&storeRect,&storeRect,srcCopy,0L);
				ClosePicture();
			}
		}
		
		if (recordPic)
		{
			ECHO("Screenshot OK");
			return recordPic;
		}
	}

	if (!inGrabFromWindow)
	{
		mTakeScreenShot=true;
		RenderSpritesAndBlit();
		
		PicHandle		p=mScreenShot;
		mScreenShot=0;
		
		return p;
	}
	else
	{
		ECHO("Taking screenshot from window");
		GrafPtr				windowPort=::GetWindowPort(mWindow);
		UPortSaver			safe0(mWindow);
		UForeColourSaver	safe1(TColourPresets::kBlack);
		UBackColourSaver	safe2(TColourPresets::kWhite);

// only need to lock if drawing to surface without using quickdraw
//		::LockPortBits(windowPort);

		PixMapHandle	h=::GetGWorldPixMap(windowPort);
		
		Rect			rect;
		PicHandle		p=0;
		
		::GetPortBounds(windowPort,&rect);

		p=OpenPicture(&rect);
		if (p)
		{				
			::CopyBits((BitMap*)*h,(BitMap*)*h,&rect,&rect,srcCopy,0);
			ClosePicture();
			ECHO("Screenshot OK");
		}
		else
			ECHO("Screenshot failed");
			
//		::UnlockPortBits(windowPort);
		
		return p;
	}
}
