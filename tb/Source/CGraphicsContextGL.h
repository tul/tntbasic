// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CGraphicsContextGL.h
// © John Treece-Birch 2002
// 20/2/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

#if __MWERKS__
#include						<agl.h>
#else
#include						<AGL/agl.h>
#endif
#include						"CGraphicsContext.h"

class CGLCanvas;
class CTBSpriteGL;
class CGLSpriteImage;

class CGraphicsContextGL : public CGraphicsContext
{
	private:
		void						KillContextGL();		

	protected:
		CGLCanvas					*mWorkCanvas;
		CLinkedListT<CTBSpriteGL>	mSprites;
		SInt16						mWidth,mHeight;
		AGLContext					mOpenGLContext;
		CRateTimer					mFPSCounter;
		Rect						mBackgroundRect;

		CGLSpriteImage				*mNumberImage[11];
		
		void						RenderSprites();

		void						CreateFPSFont();
		void						DestroyFPSFont();
		void						RenderFPS();

		AGLContext					CreateOpenGLContext();
		void						DestroyOpenGLContext(
										AGLContext inContext);
		GLboolean					AttachOpenGLContext(
										AGLContext		inContext,
										CWindowPtr		inWindow);
										
		void						FindBackgroundTop();
		void						FindBackgroundLeft();
		void						FindBackgroundBottom();
		void						FindBackgroundRight();
		void						CalculateBackgroundRect();
									
	public:
		/*e*/						CGraphicsContextGL(
										SInt16		inWide,
										SInt16		inHigh,
										bool		inGoFullScreen,
										CProgram	*inProgram);
		virtual						~CGraphicsContextGL();
		
		virtual void				RenderSpritesAndBlit();

		virtual void				ShowFPS(
										bool	inOn);
									
		void /*e*/					Pause();
		void /*e*/					Resume();
									
		virtual void				InvalAll()					{ }
		
		void						UpdateSpritePriority(
										CTBSprite	*inSprite);
	
		void						AddSpriteToCanvas(
										CTBSprite	*inSprite,
										TTBInteger	inCanvas);
		
		void						RemoveSpriteFromCanvas(
										CTBSprite	*inSprite,
										TTBInteger	inCanvas);
		
		virtual void				CreateViewport(
										CProgram		&inProgram,
										TTBInteger		inCanvasID,
										const Rect		&inRect,
										bool			inCreateTMAPViewport,
										TTBInteger		inLayer);
		
		virtual void				RemoveViewport(
										TTBInteger		inID,
										bool			inThrow);
		
		virtual void 				ViewportOffset(
										TTBInteger		inID,
										TTBInteger		inX,
										TTBInteger		inY);
										
		virtual void				ViewportOffset(
										CViewport		*inViewport,
										TTBInteger		inX,
										TTBInteger		inY);

		virtual PicHandle			TakeScreenShot(
										bool		inGrabFromWindow);
											
		SInt16						GetWidth()							{ return mWidth; }
		SInt16						GetHeight()							{ return mHeight; }
		
		void						RenderViewports();
		virtual void				StepMoan();
									
		static ScreenRef			GetScreenRef()						{ return sScreenRef; }
		
		AGLContext					GetOpenGLContext()					{ return mOpenGLContext; }
		
		EGraphicsContextType		GetType()							{ return kOpenGLGraphicsContext; }
};