// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CGraphicsContextGL.cpp
// © John Treece-Birch 2002
// 22/2/02
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

#define			GL_EXT_packed_pixels		1

/*
	OGL Sprites TODO
	
	¥ Use a layer manager
	¥ Use sprites' render function
	
	Doing these should merge more source code between the 2 graphics contexts
*/

#include		"Carbon_Settings.h"
#include		"TNT_Debugging.h"
#include		<Displays.h>
#include		"CGraphicsContextGL.h"
#include		"Utility Objects.h"
#include		"UTBException.h"
#include		"CGLCanvas.h"
#include		"CTBSpriteGL.h"
#include		"Root.h"
#include		"CTMAPViewport.h"
#include		"CApplication.h"
#include		"CImageBank.h"
//#include		<AGL/glext.h>

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGraphicsContextGL::CGraphicsContextGL(
	SInt16		inWide,
	SInt16		inHigh,
	bool		inGoFullScreen,
	CProgram	*inProgram) :
	CGraphicsContext(inWide,inHigh,32,inGoFullScreen,inProgram),
	mOpenGLContext(NULL),
	mWidth(inWide),
	mHeight(inHigh)
{
	for (SInt16 n=0; n<11; n++)
		mNumberImage[n]=NULL;
	
	Try_
	{
		mOpenGLContext=CreateOpenGLContext();
		if (!mOpenGLContext)
			Throw_(memFullErr);
			
		AttachOpenGLContext(mOpenGLContext,mWindow);
		
		CreateFPSFont();
		
		Rect		windowRect;
		
		::GetPortBounds(::GetWindowPort(mWindow),&windowRect);

		::SetPortWindowPort(mWindow);		// set port to the window else copybits won't work
		::LocalToGlobalRect(&windowRect);

		// Open a work canvas
		mWorkCanvas=new CGLCanvas(windowRect.right-windowRect.left,windowRect.bottom-windowRect.top);
		ThrowIfMemFull_(mWorkCanvas);

		mCanvasManager.SetMainCanvas(mWorkCanvas);
		
		mBackgroundRect.top=0;
		mBackgroundRect.left=0;
		mBackgroundRect.bottom=inHigh;
		mBackgroundRect.right=inWide;
	}
	
	Catch_(err)
	{
		KillContextGL();
		throw;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ KillContextGL
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Used by destructor/constructor to free resources from this object
void CGraphicsContextGL::KillContextGL()
{
	DestroyFPSFont();
	
	DestroyOpenGLContext(mOpenGLContext);
	
	// KillBaseContext(); // should get called by destructor
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGraphicsContextGL::~CGraphicsContextGL()
{
#if TIME_RENDER_LOOP
	double		total=0;
	double		temp;
	ECHO("Render times in Ms\n------------------");
	temp=AbsoluteToSeconds(gViewportTime)/gTimeSlots; total+=temp;
	ECHO("Avg time for viewport render: " << 1000.0*temp);
	temp=AbsoluteToSeconds(gStoreTime)/gTimeSlots; total+=temp;
	ECHO("Avg time for store: " << 1000.0*temp);
	temp=AbsoluteToSeconds(gRenderTime)/gTimeSlots; total+=temp;
	ECHO("Avg time for render: " << 1000.0*temp);
	temp=AbsoluteToSeconds(gFlushTime)/gTimeSlots;
	total+=temp;
	ECHO("Avg time for flush: " << 1000.0*temp);
	temp=AbsoluteToSeconds(gRestoreTime)/gTimeSlots;
	total+=temp;
	ECHO("Avg time for restore: " << 1000.0*temp);
	ECHO("Avg total time: " << 1000.0*total);
#endif
	
	KillContextGL();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShowFPS
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Shows/Hides the frames per second counter
void CGraphicsContextGL::ShowFPS(
	bool	inOn)
{
	CGraphicsContext::ShowFPS(inOn);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderSpritesAndBlit
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Performs one render sprites and blit cycle.
// If the member variable mTakeScreenShot is set then the window is not blitted to but a screen shot is saved into
// mScreenShot as a picture handle.
void CGraphicsContextGL::RenderSpritesAndBlit()
{
	if (mGraphicsPaused)
		return;
		
	if (mAutoMoan && !mMasterMoanPause)
		StepMoan();
		
	WaitFrameTime();					// hum..dee...humm...... and go!
	
	// Render the background
	float		screenWidth=mWidth/2.0f;
	float		screenHeight=mHeight/2.0f;
	float		across=(mBackgroundRect.left/screenWidth)-1.0f;
	float		down=-((mBackgroundRect.bottom/screenHeight)-1.0f);
	
	glPixelStorei(GL_UNPACK_ROW_LENGTH,mWidth);
	glPixelStorei(GL_UNPACK_SKIP_ROWS,mHeight-mBackgroundRect.bottom);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS,mBackgroundRect.left);
	
	glRasterPos2f(across,down);
	glDrawPixels(FRectWidth(mBackgroundRect),FRectHeight(mBackgroundRect),GL_RGB,GL_UNSIGNED_BYTE,mWorkCanvas->GetPixels());
	glRasterPos2f(0,0);
	
	glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
	
	RenderViewports();
	RenderSprites();
		
	if (!mTakeScreenShot)
	{
		RenderFPS();
	
		aglSwapBuffers(mOpenGLContext);
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
			Rect				rect={0,0,mHeight,mWidth};
			GWorldPtr			world;
				
			::NewGWorld(&world,32,&rect,NULL,NULL,kNativeEndianPixMap);
			
			if (world)
			{
				CGrafPtr			oldPort;
					
				::GetPort(&oldPort);
				::SetPort(world);

				GLubyte				*pixels=new GLubyte[mWidth*mHeight*3];
				
				// Copy the contents to the gworld
				glReadPixels(0,0,mWidth,mHeight,GL_RGB,GL_UNSIGNED_BYTE,pixels);
				
				for (SInt16 y=0; y<mHeight; y++)
				{
					for (SInt16 x=0; x<mWidth; x++)
					{
						RGBColour	colour;
						
						colour.red=pixels[(y*mWidth+x)*3+0]*256;
						colour.green=pixels[(y*mWidth+x)*3+1]*256;
						colour.blue=pixels[(y*mWidth+x)*3+2]*256;
						
						::SetCPixel(x,(mHeight-1)-y,&colour);
					}
				}

				delete [] pixels;
						
				if (mScreenShot=::OpenPicture(&rect))
				{
					// Copy the gworld to the picture
					UForeColourSaver	safe1(TColourPresets::kBlack);
					UBackColourSaver	safe2(TColourPresets::kWhite);
					PixMapHandle		source=::GetGWorldPixMap(world);
					
					::LockPixels(source);
					::CopyBits((BitMap*)*source,(BitMap*)*source,&rect,&rect,srcCopy,NULL);
					::UnlockPixels(source);
					
					::ClosePicture();

					ECHO("Screenshot ok");
				}
				
				::SetPort(oldPort);
				::DisposeGWorld(world);
			}
			else
				ECHO("Screenshot failed");
		}
		Catch_(err)
		{
			ECHO("Screenshot error: " << ErrCode_(err));
		}
	}
	
	if (!mFPSCounter.IsRunning())
		mFPSCounter.StartTask();
		
	mFPSCounter.Pulse();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StepMoan
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::StepMoan()
{
	CListIndexerT<CTBSpriteGL>	indexer(&mSprites);
		
	while (CTBSpriteGL *sprite=indexer.GetNextData())
		sprite->MoanSprite();
		
	CGraphicsContext::StepMoan();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderSprites
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::RenderSprites()
{
	// Initialise sprite drawing
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	CListIndexerT<CTBSpriteGL>	indexer(&mSprites);

	Rect	bounds={0,0,mHeight,mWidth};
	
	while (CTBSpriteGL *sprite=indexer.GetNextData())
		sprite->Render(mWidth,mHeight,bounds,0,0);
	
	// Shutdown sprite drawing
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderViewports
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::RenderViewports()
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		viewport->Render();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ UpdateSpritePriority
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::UpdateSpritePriority(
	CTBSprite	*inSprite)
{
	if (inSprite->GetCanvas()==0)
	{
		CListIndexerT<CTBSpriteGL>		indexer(&mSprites);
	
		while (CTBSpriteGL *current=indexer.GetNextData())
		{
			if ((current!=inSprite) && (current->GetPriority()>=inSprite->GetPriority()))
			{
				mSprites.MoveElement(((CTBSpriteGL*)inSprite)->GetGLLink(),current->GetGLLink(),false);
				return;
			}
		}
		
		mSprites.UnlinkElement(((CTBSpriteGL*)inSprite)->GetGLLink());
		mSprites.AppendElement(((CTBSpriteGL*)inSprite)->GetGLLink());
	}
	else
	{
		// Update the sprite in the viewport
		CListIndexerT<CViewport>		indexer(&mViewports);
	
		while (CViewport *viewport=indexer.GetNextData())
		{
			if (viewport->GetCanvas()==inSprite->GetCanvas())
			{
				viewport->UpdateSpritePriority(inSprite);
				return;
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Pause														/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Pauses the dsp context
void CGraphicsContextGL::Pause()
{
	CGraphicsContext::Pause();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Resume													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Resumes the dsp context
void CGraphicsContextGL::Resume()
{
	CGraphicsContext::Resume();
	
	RenderSpritesAndBlit();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddSpriteToCanvas
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::AddSpriteToCanvas(
	CTBSprite	*inSprite,
	TTBInteger	inCanvas)
{
	if (inCanvas==0)
		mSprites.AppendElement(((CTBSpriteGL*)inSprite)->GetGLLink());
	else
	{
		CListIndexerT<CViewport>		indexer(&mViewports);
	
		while (CViewport *viewport=indexer.GetNextData())
		{
			if (viewport->GetCanvas()==inCanvas)
			{
				viewport->AddSprite(inSprite);
				break;
			}
		}
	}
	
	UpdateSpritePriority(inSprite);
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RemoveSpriteFromCanvas
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::RemoveSpriteFromCanvas(
	CTBSprite	*inSprite,
	TTBInteger	inCanvas)
{
	if (inCanvas==0)
		mSprites.UnlinkElement(((CTBSpriteGL*)inSprite)->GetGLLink());
	else
	{
		CListIndexerT<CViewport>		indexer(&mViewports);
	
		while (CViewport *viewport=indexer.GetNextData())
		{
			if (viewport->GetCanvas()==inCanvas)
			{
				viewport->RemoveSprite(inSprite);
				break;
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CreateViewport									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::CreateViewport(
	CProgram		&inProgram,
	TTBInteger		inCanvasID,
	const Rect		&inRect,
	bool			inCreateTMAPViewport,
	TTBInteger		inLayer)
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
		viewport=CTMAPViewport::CreateNewTMAPViewport(inProgram,inCanvasID,inLayer,inRect); //new CWrappingViewport(inProgram,inCanvasID,inRect,10000,10000);
	else
		viewport=new CViewport(inProgram,inCanvasID,inRect);
		
	mViewports.AppendElement(viewport);
	
	// Check the viewport is inside the canvas
	canvasRect=mCanvasManager.GetCanvas(inCanvasID)->GetBoundsRect();
	Rect	viewportRect=viewport->GetViewportRect();

	if ((viewportRect.top<canvasRect.top) || (viewportRect.left<canvasRect.left) ||
		(viewportRect.bottom>canvasRect.bottom) || (viewportRect.right>canvasRect.right))
	{
		UTBException::ThrowViewportOutOfCanvas();
	}
	
	ViewportOffset(inCanvasID,0,0);
	
	CalculateBackgroundRect();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RemoveViewport
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::RemoveViewport(
	TTBInteger		inCanvas,
	bool			inThrow)
{
	CListIndexerT<CViewport>		indexer(&mViewports);
	
	while (CViewport *viewport=indexer.GetNextData())
	{
		if (viewport->GetCanvas()==inCanvas)
		{
			delete viewport;
			
			CalculateBackgroundRect();
			
			return;
		}
	}
	
	if (inThrow)
		UTBException::ThrowViewportNotOpen();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ViewportOffset										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::ViewportOffset(
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
			return;
		}
	}
	
	UTBException::ThrowViewportNotOpen();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ViewportOffset										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// sets the viewport offset to the specified x,y
void CGraphicsContextGL::ViewportOffset(
	CViewport		*inViewport,
	TTBInteger		inX,
	TTBInteger		inY)
{
	inViewport->ViewportOffset(inX,inY);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CreateOpenGLContext
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
AGLContext CGraphicsContextGL::CreateOpenGLContext()
{
	GLint				attrib[5]={AGL_RGBA,AGL_DOUBLEBUFFER,AGL_DEPTH_SIZE,8,AGL_NONE};
	AGLPixelFormat		format;
	AGLContext			context;

	// Choose pixel format
	format=aglChoosePixelFormat(NULL,0,attrib);
	if(format==NULL)
		return NULL;
	
	// Create an AGL context
	context=aglCreateContext(format,NULL);	

	// Destroy pixel format
	aglDestroyPixelFormat(format);			
	
	return context;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DestroyOpenGLContext
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::DestroyOpenGLContext(
	AGLContext inContext)
{
	if (inContext)
	{
		aglSetCurrentContext(NULL);
		aglSetDrawable(inContext,NULL);
		aglDestroyContext(inContext);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AttachOpenGLContext
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
GLboolean CGraphicsContextGL::AttachOpenGLContext(
	AGLContext		inContext,
	CWindowPtr		inWindow)
{
	// Attach the context to the window
	OSErr	err=aglSetDrawable(inContext,::GetWindowPort(inWindow));
	if (!err)
		return GL_FALSE;

	// Make context current
	err=aglSetCurrentContext(inContext);
	if (!err)
		return GL_FALSE;

	// Initialise drawing states
	glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_FLAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
//	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE,1);
	
	// Initialise the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	return GL_TRUE;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ TakeScreenShot
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
PicHandle CGraphicsContextGL::TakeScreenShot(
	bool		inGrabFromWindow)
{
	mTakeScreenShot=true;
	RenderSpritesAndBlit();
		
	PicHandle		p=mScreenShot;
	mScreenShot=0;
		
	return p;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CreateFPSFont
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::CreateFPSFont()
{
	UResFileSaver				safeResFile(CApplication::GetApplication()->GetAppResFile());
	PicHandle					maskPic=GetPicture(kPICT_FPSFont);
	
	ThrowIfMemFull_(maskPic);
	
	Rect						picFrame=(**maskPic).picFrame;
	BigEndianRectToNative(picFrame);
	CGLCanvas					*pictureCanvas=new CGLCanvas(FRectWidth(picFrame),FRectHeight(picFrame));
	RGBColour					red={0xFFFF,0x0000,0x0000};
	CImageBank::SImageFlags		flags;
	
	pictureCanvas->DrawPict(maskPic,picFrame);
	
	ReleaseResource((Handle)maskPic);
	
	Rect	rect={0,0,19,13};
	
	for (SInt16 n=0; n<11; n++)
	{
		mNumberImage[n]=CGLImageBank::EncodeImage(pictureCanvas,NULL,rect,rect,red,0,0,flags);
		
		rect.left+=14;
		rect.right+=14;
	}
	
	delete pictureCanvas;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DestroyFPSFont
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::DestroyFPSFont()
{
	for (SInt16 n=0; n<11; n++)
	{
		delete mNumberImage[n];
		mNumberImage[n]=NULL;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderFPS
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::RenderFPS()
{
	if (mFPSOn)
	{
		// Calculate the string
		Str32	fpsString;
		UInt32	fps=mFPSCounter.CalcRate()*100;

		if (fps>999999)
			fps=999999;

		// Initialise sprite drawing
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
		float	screenWidth=mWidth/2.0f;
		float	screenHeight=mHeight/2.0f;	
		
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

		bool		drawing=false;
		SInt16		n;
		
		for (n=0; n<7; n++)
		{
			SInt16		image;
			
			switch (n)
			{
				case 0: image=fps/100000; fps-=image*100000; break;
				case 1: image=fps/10000;  fps-=image*10000;  break;
				case 2: image=fps/1000;   fps-=image*1000;   break;
				case 3: image=fps/100;    fps-=image*100;    break;
				case 4: image=10; break;
				case 5: image=fps/10;     fps-=image*10;     break;
				case 6: image=fps/1;      fps-=image*1;      break;
			}
			
			if ((image>0) || (n>=3))
				drawing=true;
				
			if (drawing)
			{
				float	top=mHeight-22,left=mWidth-7*14+n*13,bottom=top+19,right=left+13;

				top=-(top/screenHeight-1.0f);
				left=left/screenWidth-1.0f;
				bottom=-(bottom/screenHeight-1.0f);
				right=right/screenWidth-1.0f;

				glBindTexture(GL_TEXTURE_2D,mNumberImage[image]->GetTextureNumber());
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f,mNumberImage[image]->GetHeightPercent()); glVertex3f(left,bottom,0.0f);
					
					glTexCoord2f(0.0f,0.0f); glVertex3f(left,top,0.0f);
					
					glTexCoord2f(mNumberImage[image]->GetWidthPercent(),0.0f); glVertex3f(right,top,0.0f);
					
					glTexCoord2f(mNumberImage[image]->GetWidthPercent(),mNumberImage[image]->GetHeightPercent()); glVertex3f(right,bottom,0.0f);
				glEnd();
			}
		}
		
		// Shutdown sprite drawing
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CalculateBackgroundRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::CalculateBackgroundRect()
{
	mBackgroundRect.top=0;
	mBackgroundRect.left=0;
	mBackgroundRect.bottom=mHeight;
	mBackgroundRect.right=mWidth;
	
	FindBackgroundTop();
	FindBackgroundLeft();
	FindBackgroundBottom();
	FindBackgroundRight();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FindBackgroundTop
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::FindBackgroundTop()
{
	for (SInt32 y=mBackgroundRect.top; y<mBackgroundRect.bottom; y++)
	{
		for (SInt32 x=mBackgroundRect.left; x<mBackgroundRect.right; x++)
		{
			CListIndexerT<CViewport>		indexer(&mViewports);
			bool							found=false;
	
			while (CViewport *viewport=indexer.GetNextData())
			{
				if ((y>=viewport->mBounds.top) && (y<viewport->mBounds.bottom) &&
					(x>=viewport->mBounds.left) && (x<viewport->mBounds.right))
				{
					found=true;
					break;
				}
			}
			
			if (!found)
				return;
		}
		
		mBackgroundRect.top=y;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FindBackgroundLeft
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::FindBackgroundLeft()
{
	for (SInt32 x=mBackgroundRect.left; x<mBackgroundRect.right; x++)
	{
		for (SInt32 y=mBackgroundRect.top; y<mBackgroundRect.bottom; y++)
		{
			CListIndexerT<CViewport>		indexer(&mViewports);
			bool							found=false;
	
			while (CViewport *viewport=indexer.GetNextData())
			{
				if ((y>=viewport->mBounds.top) && (y<viewport->mBounds.bottom) &&
					(x>=viewport->mBounds.left) && (x<viewport->mBounds.right))
				{
					found=true;
					break;
				}
			}
			
			if (!found)
				return;
		}
		
		mBackgroundRect.left=x;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FindBackgroundBottom
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::FindBackgroundBottom()
{
	for (SInt32 y=mBackgroundRect.bottom-1; y>=mBackgroundRect.top; y--)
	{
		for (SInt32 x=mBackgroundRect.left; x<mBackgroundRect.right; x++)
		{
			CListIndexerT<CViewport>		indexer(&mViewports);
			bool							found=false;
	
			while (CViewport *viewport=indexer.GetNextData())
			{
				if ((y>=viewport->mBounds.top) && (y<viewport->mBounds.bottom) &&
					(x>=viewport->mBounds.left) && (x<viewport->mBounds.right))
				{
					found=true;
					break;
				}
			}
			
			if (!found)
				return;
		}
		
		mBackgroundRect.bottom=y;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FindBackgroundRight
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphicsContextGL::FindBackgroundRight()
{
	for (SInt32 x=mBackgroundRect.right-1; x>=mBackgroundRect.left; x--)
	{
		for (SInt32 y=mBackgroundRect.top; y<mBackgroundRect.bottom; y++)
		{
			CListIndexerT<CViewport>		indexer(&mViewports);
			bool							found=false;
	
			while (CViewport *viewport=indexer.GetNextData())
			{
				if ((y>=viewport->mBounds.top) && (y<viewport->mBounds.bottom) &&
					(x>=viewport->mBounds.left) && (x<viewport->mBounds.right))
				{
					found=true;
					break;
				}
			}
			
			if (!found)
				return;
		}
		
		mBackgroundRect.right=x;
	}
}