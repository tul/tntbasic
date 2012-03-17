// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CGraphicsContext16.h
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

#include						"BLAST Layer Manager.h"
#include						"BLAST Standard Buffers.h"
#include						"CFPSSprite.h"
#include						"CBLCanvas16.h"
#include						"CGraphicsContext.h"

#define							UseCopyBitsToScreen
//#define							UseCopyBitsWorldToWorld

class CGraphicsContext16 : public CGraphicsContext
{
	private:
		void					KillContext16();		

	protected:
		BlastSpriteLayer		mLayer0;
		CTBSpriteLayer			mCanvasLayer[kMaxCanvases];
		CFPSSprite				mFPSSprite;
		BlastLayerManager		mLayerManager;
		Ptr						mCachedPortBaseAddr;
		SInt32					mRecalcScreenRecCount;

		void					LockScreen();
		void					UnlockScreen();

#ifndef UseCopyBitsWorldToWorld
		CBlastStandardBufferA16	mStore2Work;
#else
		CBlastCopyBitsBuffer	mStore2Work;
#endif
		BlastDrawBuffer			*mWork2Screen;				// this buffer is allocated dynamically depending on whether we're on x or 9

		CBLCanvas				*mStoreCanvas;
		ABCRec					mScreenRec;
		
		static const UInt16		kMaxCustomRects				= 500;
		static const UInt16		kMaxSprites					= 500;
		static const UInt16		kMaxLayers					= kMaxCanvases + 5;

	public:
		/*e*/					CGraphicsContext16(
									SInt16		inWide,
									SInt16		inHigh,
									bool		inGoFullScreen,
									CProgram	*inProgram);
		virtual					~CGraphicsContext16();
		
		BlastSpriteLayer		*GetCanvasLayer(
									TTBInteger		inCanvas);
		
		virtual void			RenderSpritesAndBlit();
		
		virtual void			StepMoan();
		
		virtual void			ShowFPS(
									bool		inOn);

		virtual void /*e*/		Pause();
		virtual void /*e*/		Resume();
									
		virtual void			InvalAll()					{ mWork2Screen->InvalAll(); }
		
		virtual void			AddSpriteToCanvas(
									CTBSprite	*inSprite,
									TTBInteger	inCanvas);
		
		virtual void			RemoveSpriteFromCanvas(
									CTBSprite	*inSprite,
									TTBInteger	inCanvas);
		
		virtual void			CreateViewport(
									CProgram		&inProgram,
									TTBInteger		inCanvasID,
									const Rect		&inRect,
									bool			inCreateTMAPViewport,
									TTBInteger		inLayer);
		
		virtual void			RemoveViewport(
									TTBInteger		inID,
									bool			inThrow);
		
		virtual void 			ViewportOffset(
									TTBInteger		inID,
									TTBInteger		inX,
									TTBInteger		inY);
									
		virtual void			ViewportOffset(
									CViewport		*inViewport,
									TTBInteger		inX,
									TTBInteger		inY);
								
		virtual bool			EventUpdate(
									const EventRecord&	inMacEvent);
		
		virtual PicHandle		TakeScreenShot(
									bool		inGrabFromWindow);
	
		EGraphicsContextType	GetType()					{ return kBlastGraphicsContext; }
};
