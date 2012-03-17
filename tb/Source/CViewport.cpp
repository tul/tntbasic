// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CViewport.cpp
// © John Treece-Birch 2000
// 20/7/00
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
#include		"CGraphicsContext.h"
#include		"CGraphicsContextGL.h"
#include		"Utility Objects.h"
#include		"UTBException.h"
#include		"CViewport.h"
#include		"CBLCanvas16.h"
#include		"CGLCanvas.h"
#include		"CTBSpriteGL.h"

void CViewport::Render()
{
	CGraphicsContext	*graphics=mProgram->CheckGraphicsMode();

	if (graphics->GetType()==CGraphicsContext::kBlastGraphicsContext)
	{
		CCanvas				*fromCanvas=graphics->mCanvasManager.GetCanvas(mCanvas);
		
		if ((mUpdateAll) || (((CBLCanvas*)fromCanvas)->GetDrawBuffer()->GetUsedBlitRects()))
		{
			CCanvas			*toCanvas=graphics->mCanvasManager.GetCanvas(0);

			toCanvas->Copy(fromCanvas,GetViewportRect(),mBounds.left,mBounds.top);
			
			((CBLCanvas*)fromCanvas)->GetDrawBuffer()->ClearBlitList();
			mUpdateAll=false;
		}
	}
	else
	{
		CGraphicsContextGL	*glGraphics=(CGraphicsContextGL*)graphics;
		CGLCanvas			*fromCanvas=(CGLCanvas*)graphics->mCanvasManager.GetCanvas(mCanvas);
		
		float				screenWidth=glGraphics->GetWidth()/2.0f;
		float				screenHeight=glGraphics->GetHeight()/2.0f;
	
		float				across=(mBounds.left/screenWidth)-1.0f;
		float				down=-((mBounds.bottom/screenHeight)-1.0f);
		
		glPixelStorei(GL_UNPACK_ROW_LENGTH,fromCanvas->GetWidth());
		glPixelStorei(GL_UNPACK_SKIP_ROWS,fromCanvas->GetHeight()-GetHeight()-GetYOffset());
		glPixelStorei(GL_UNPACK_SKIP_PIXELS,GetXOffset());
		
		glRasterPos2f(across,down);
		glDrawPixels(GetWidth(),GetHeight(),GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
		glRasterPos2f(0,0);
		
		glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
		glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
	
		RenderSprites();
	}
}

void CViewport::RenderSprites()
{
	CGraphicsContextGL	*graphics=(CGraphicsContextGL*)mProgram->CheckGraphicsMode();
	
	// Render Sprites
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	CListIndexerT<CTBSpriteGL>	indexer(&mSprites);
	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	while (CTBSpriteGL *sprite=indexer.GetNextData())
		sprite->Render(graphics->GetWidth(),graphics->GetHeight(),mBounds,mXOffset,mYOffset);
	
	// Shutdown sprite drawing
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ VerifyNewOffsets												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Check that the current offsets are valid for the source canvas
// Don't store or change any viewport vars, just check the x,y and throw if it's bad
void CViewport::VerifyNewOffsets(
	TTBInteger		inNewX,
	TTBInteger		inNewY)
{
	// Check the viewport is still inside the canvas
	Rect	canvasRect=mProgram->CheckGraphicsMode()->mCanvasManager.GetCanvas(mCanvas)->GetBoundsRect();
	
	if ((inNewY<canvasRect.top) || (inNewX<canvasRect.left) ||
		((inNewY+GetHeight())>canvasRect.bottom) || ((inNewX+GetWidth())>canvasRect.right))
	{
		UTBException::ThrowViewportOutOfCanvas();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddSprite
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CViewport::AddSprite(
	CTBSprite	*inSprite)
{
	mSprites.LinkElement(((CTBSpriteGL*)inSprite)->GetGLLink());
	((CTBSpriteGL*)inSprite)->SetViewport(this);
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RemoveSprite
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CViewport::RemoveSprite(
	CTBSprite	*inSprite)
{
	mSprites.UnlinkElement(((CTBSpriteGL*)inSprite)->GetGLLink());
	((CTBSpriteGL*)inSprite)->SetViewport(NULL);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ UpdateSpritePriority
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CViewport::UpdateSpritePriority(
	CTBSprite	*inSprite)
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

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MoanViewport
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Scrolls a viewport using the local moan channel
void CViewport::MoanViewport()
{
	TBLImageVector		x,y,i;
	float				xScale,yScale;
	float				rot;
	
	mLocalMoanChannel.StepMoan(x,y,i,xScale,yScale,rot);
	
	if (x || y)
	{
		CGraphicsContext	*gc=mProgram->CheckGraphicsMode();
		gc->ViewportOffset(this,mXOffset+x,mYOffset+y);
	}
	
	// Moan all the sprites
	CListIndexerT<CTBSpriteGL>	indexer(&mSprites);
	
	while (CTBSpriteGL *sprite=indexer.GetNextData())
		sprite->MoanSprite();
}