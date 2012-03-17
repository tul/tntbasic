// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CWrappingViewport.cpp
// © Mark Tully 2002
// 6/2/02
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

/*
	An extension of the viewport idea that allows a viewport to represent a larger scrollable area than the true canvas area
	by wrapping around when scrolling off the edge. If you scroll off the right edge by 10 pixels, you get a band of 10 pixels
	from the left edge instead.
	This is useful for performing functions such as tile scrolling.

	A wrapped viewport has a real area and an affective area (which is usually bigger). The affective area is created by
	wrapping the coordinates that are off the real area back onto the other side.
	This is useful for tile scrolling because it means that if continuously scrolling you just
	keep drawing 1 row of tiles on the edge and moving your rect, as if you were tile scrolling with an infinite canvas.
	
	I decided not to add wrapping viewports to TNT Basic, except for use for tile scrolling. The reason being that it upsets
	the user's perception of sprites because they're aware the canvas they opened is only so large and could be confused by
	sprites being wrapped (as they would be).
	With tile scrolling this is less of a problem because the user thinks of the canvas as being as large as the map and so
	sprites wrapping goes unnoticed.
	
	If it could all be explained with less difficulty than the usefulness it would grant then we could add generic use of
	wrapped viewports, but I think that tmap only is sufficient; being both easier to explain and to visualise.
*/

#include "CWrappingViewport.h"
#include "CGraphicsContext16.h"
#include "CGraphicsContextGL.h"
#include "CGLCanvas.h"
#include "CTBSpriteGL.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The effective width/height are simulated width of the canvas being scrolled. The max offset is effectiveWidth-width of viewport
CWrappingViewport::CWrappingViewport(
	CProgram		&inProgram,
	TTBInteger		inCanvasId,
	const Rect		&inDestRect,
	TTBInteger		inEffectiveWidth,
	TTBInteger		inEffectiveHeight) :
	mEffectiveWidth(inEffectiveWidth),
	mEffectiveHeight(inEffectiveHeight),
	CViewport(inProgram,inCanvasId,inDestRect)
{
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Render
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Renders the viewport. This can involve up to four individual renders depending on how the viewport is offset over the edges
// of the canvas
void CWrappingViewport::Render()
{
	CGraphicsContext	*graphics=mProgram->CheckGraphicsMode();
	
	if (graphics->GetType()==CGraphicsContext::kBlastGraphicsContext)
		RenderBlastViewport();
	else
		RenderOpenGLViewport();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderBlastViewport
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CWrappingViewport::RenderBlastViewport()
{
	CGraphicsContext	*graphics=mProgram->CheckGraphicsMode();
	CCanvas				*fromCanvas=graphics->mCanvasManager.GetCanvas(mCanvas);
	
	if ((mUpdateAll) || (((CBLCanvas*)fromCanvas)->GetDrawBuffer()->GetUsedBlitRects()))
	{
		CCanvas				*toCanvas=graphics->mCanvasManager.GetCanvas(0);
		Rect				copyRect=GetViewportRect();
		TTBInteger			srcCanvasWidth=fromCanvas->GetWidth(),srcCanvasHeight=fromCanvas->GetHeight();
		
		// if scrolled right off the edge of the canvas, wrap the source rect back in bounds
		if (copyRect.left>=srcCanvasWidth)
		{
			copyRect.left%=srcCanvasWidth;
			copyRect.right=copyRect.left+GetWidth();
		}
		if (copyRect.top>=srcCanvasHeight)
		{
			copyRect.top%=srcCanvasHeight;
			copyRect.bottom=copyRect.top+GetHeight();
		}
		
		// Left side split copy, when the viewport has scrolled of the right edge
		if (copyRect.right>srcCanvasWidth)
		{
			if (copyRect.bottom>srcCanvasHeight)	// scrolled off the bottom too
			{
				// we're in a state of 4 quarters now
				// '''''''''''''''''''''''
				// '  (1)  |    (2)      '
				// '       |             '
				// 'ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ'
				// '  (3)  |(0,0)   (4)  '
				// '       |             '
				// '''''''''''''''''''''''
				// copy on screen (2) and (4)
				TTBInteger		widthOfOnScreenRightHalf=copyRect.right%srcCanvasWidth; // no pixels it's overlapping on the right border
				TTBInteger		widthOfOnScreenLeftHalf=FRectWidth(copyRect)-widthOfOnScreenRightHalf;
				TTBInteger		heightOfOnScreenBottomHalf=copyRect.bottom%srcCanvasHeight; // no pixels it's overlapping on the bottom border
				TTBInteger		heightOfOnScreenTopHalf=FRectHeight(copyRect)-heightOfOnScreenBottomHalf;				
				Rect			tempCopyRect;

				tempCopyRect.left=0;
				tempCopyRect.right=widthOfOnScreenRightHalf;

				tempCopyRect.top=0;
				tempCopyRect.bottom=heightOfOnScreenBottomHalf;
				toCanvas->Copy(fromCanvas,tempCopyRect,mBounds.left+widthOfOnScreenLeftHalf,mBounds.top+heightOfOnScreenTopHalf);

				tempCopyRect.bottom=srcCanvasHeight;
				tempCopyRect.top=srcCanvasHeight-heightOfOnScreenTopHalf;
				toCanvas->Copy(fromCanvas,tempCopyRect,mBounds.left+widthOfOnScreenLeftHalf,mBounds.top);
				
				// copy on screen (1) and (3)
				tempCopyRect.right=srcCanvasWidth;
				tempCopyRect.left=srcCanvasWidth-widthOfOnScreenLeftHalf;

				tempCopyRect.top=0;
				tempCopyRect.bottom=heightOfOnScreenBottomHalf;
				toCanvas->Copy(fromCanvas,tempCopyRect,mBounds.left,mBounds.top+heightOfOnScreenTopHalf);
				
				tempCopyRect.bottom=srcCanvasHeight;
				tempCopyRect.top=srcCanvasHeight-heightOfOnScreenTopHalf;
				toCanvas->Copy(fromCanvas,tempCopyRect,mBounds.left,mBounds.top);				
			}
			else
			{
				// two half split
				// '''''''''''''''''''''''
				// '       |(0,0)        '
				// '       |             '
				// '       |             '
				// '       |             '
				// '       |             '
				// '''''''''''''''''''''''
				// on screen left rect is sourced from the right side of the source canvas
				// on screen right rect is sourced from the left side of the source canvas

				// copy on screen right rect
				TTBInteger		widthOfOnScreenRightHalf=copyRect.right%srcCanvasWidth; // no pixels it's overlapping on the right border
				TTBInteger		widthOfOnScreenLeftHalf=FRectWidth(copyRect)-widthOfOnScreenRightHalf;
				Rect			tempCopyRect=copyRect;
				
				tempCopyRect.left=0;
				tempCopyRect.right=widthOfOnScreenRightHalf;
				toCanvas->Copy(fromCanvas,tempCopyRect,mBounds.left+widthOfOnScreenLeftHalf,mBounds.top);
				
				// copy on screen left rect
				tempCopyRect.right=srcCanvasWidth;
				tempCopyRect.left=srcCanvasWidth-widthOfOnScreenLeftHalf;
				toCanvas->Copy(fromCanvas,tempCopyRect,mBounds.left,mBounds.top);
			}
		}
		else if (copyRect.bottom>srcCanvasHeight)
		{
			// two half vertical split
			// '''''''''''''''''''''''
			// '                     '
			// '                     '
			// 'ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ'
			// '(0,0)                '
			// '                     '
			// '''''''''''''''''''''''

			// on screen top rect is sourced from the bottom side of the source canvas
			// on screen bottom rect is sourced from the top side of the source canvas

			// copy on screen bottom rect
			TTBInteger		heightOfOnScreenBottomHalf=copyRect.bottom%srcCanvasHeight; // no pixels it's overlapping on the bottom border
			TTBInteger		heightOfOnScreenTopHalf=FRectHeight(copyRect)-heightOfOnScreenBottomHalf;
			Rect			tempCopyRect=copyRect;
			
			tempCopyRect.top=0;
			tempCopyRect.bottom=heightOfOnScreenBottomHalf;
			toCanvas->Copy(fromCanvas,tempCopyRect,mBounds.left,mBounds.top+heightOfOnScreenTopHalf);
			
			// copy on screen top rect
			tempCopyRect.bottom=srcCanvasHeight;
			tempCopyRect.top=srcCanvasHeight-heightOfOnScreenTopHalf;
			toCanvas->Copy(fromCanvas,tempCopyRect,mBounds.left,mBounds.top);
		}
		else
		{
			// completely on canvas - cool
			toCanvas->Copy(fromCanvas,copyRect,mBounds.left,mBounds.top);
		}
		
		((CBLCanvas*)fromCanvas)->GetDrawBuffer()->ClearBlitList();
		mUpdateAll=false;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RenderOpenGLViewport
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CWrappingViewport::RenderOpenGLViewport()
{
	CGraphicsContextGL	*graphics=(CGraphicsContextGL*)mProgram->CheckGraphicsMode();
	CGLCanvas			*fromCanvas=(CGLCanvas*)graphics->mCanvasManager.GetCanvas(mCanvas);
		
	float				screenWidth=graphics->GetWidth()/2.0f;
	float				screenHeight=graphics->GetHeight()/2.0f;
	float				across,down;

	Rect				copyRect=GetViewportRect();
	TTBInteger			srcCanvasWidth=fromCanvas->GetWidth(),srcCanvasHeight=fromCanvas->GetHeight();
	
	// if scrolled right off the edge of the canvas, wrap the source rect back in bounds
	if (copyRect.left>=srcCanvasWidth)
	{
		copyRect.left%=srcCanvasWidth;
		copyRect.right=copyRect.left+GetWidth();
	}
	
	if (copyRect.top>=srcCanvasHeight)
	{
		copyRect.top%=srcCanvasHeight;
		copyRect.bottom=copyRect.top+GetHeight();
	}
	
	glPixelStorei(GL_UNPACK_ROW_LENGTH,fromCanvas->GetWidth());
	
	// Left side split copy, when the viewport has scrolled of the right edge
	if (copyRect.right>srcCanvasWidth)
	{
		if (copyRect.bottom>srcCanvasHeight)	// scrolled off the bottom too
		{
			// we're in a state of 4 quarters now
			// '''''''''''''''''''''''
			// '  (1)  |    (2)      '
			// '       |             '
			// 'ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ'
			// '  (3)  |(0,0)   (4)  '
			// '       |             '
			// '''''''''''''''''''''''
			TTBInteger		widthOfOnScreenRightHalf=copyRect.right%srcCanvasWidth; // no pixels it's overlapping on the right border
			TTBInteger		widthOfOnScreenLeftHalf=FRectWidth(copyRect)-widthOfOnScreenRightHalf;
			TTBInteger		heightOfOnScreenBottomHalf=copyRect.bottom%srcCanvasHeight; // no pixels it's overlapping on the bottom border
			TTBInteger		heightOfOnScreenTopHalf=FRectHeight(copyRect)-heightOfOnScreenBottomHalf;		
			
			// Copy section 1
			across=(mBounds.left/screenWidth)-1.0f;
			down=-(((mBounds.bottom-heightOfOnScreenBottomHalf)/screenHeight)-1.0f);
			
			glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,fromCanvas->GetWidth()-widthOfOnScreenLeftHalf);
			
			glRasterPos2f(across,down);
			glDrawPixels(widthOfOnScreenLeftHalf,heightOfOnScreenTopHalf,GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
			
			// Copy section 2
			across=((mBounds.left+widthOfOnScreenLeftHalf)/screenWidth)-1.0f;
			
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
			
			glRasterPos2f(across,down);
			glDrawPixels(widthOfOnScreenRightHalf,heightOfOnScreenTopHalf,GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
			
			// Copy section 3
			across=(mBounds.left/screenWidth)-1.0f;
			down=-((mBounds.bottom/screenHeight)-1.0f);
			
			glPixelStorei(GL_UNPACK_SKIP_ROWS,fromCanvas->GetHeight()-heightOfOnScreenBottomHalf);
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,fromCanvas->GetWidth()-widthOfOnScreenLeftHalf);
			
			glRasterPos2f(across,down);
			glDrawPixels(widthOfOnScreenLeftHalf,heightOfOnScreenBottomHalf,GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
			
			// Copy section 4
			across=((mBounds.left+widthOfOnScreenLeftHalf)/screenWidth)-1.0f;
			
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
			
			glRasterPos2f(across,down);
			glDrawPixels(widthOfOnScreenRightHalf,heightOfOnScreenBottomHalf,GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
		}
		else
		{
			// two half split
			// '''''''''''''''''''''''
			// '       |(0,0)        '
			// '       |             '
			// '       |             '
			// '       |             '
			// '       |             '
			// '''''''''''''''''''''''
			// on screen left rect is sourced from the right side of the source canvas
			// on screen right rect is sourced from the left side of the source canvas
			TTBInteger		widthOfOnScreenRightHalf=copyRect.right%srcCanvasWidth; // no pixels it's overlapping on the right border
			TTBInteger		widthOfOnScreenLeftHalf=FRectWidth(copyRect)-widthOfOnScreenRightHalf;
			
			// copy on screen right rect
			across=((mBounds.left+widthOfOnScreenLeftHalf)/screenWidth)-1.0f;
			down=-((mBounds.bottom/screenHeight)-1.0f);
			
			glPixelStorei(GL_UNPACK_SKIP_ROWS,fromCanvas->GetHeight()-GetHeight()-(GetYOffset()%srcCanvasHeight));
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
			
			glRasterPos2f(across,down);
			glDrawPixels(widthOfOnScreenRightHalf,GetHeight(),GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
			
			// copy on screen left rect
			across=(mBounds.left/screenWidth)-1.0f;
			
			glPixelStorei(GL_UNPACK_SKIP_ROWS,fromCanvas->GetHeight()-GetHeight()-(GetYOffset()%srcCanvasHeight));
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,fromCanvas->GetWidth()-widthOfOnScreenLeftHalf);
			
			glRasterPos2f(across,down);
			glDrawPixels(widthOfOnScreenLeftHalf,GetHeight(),GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
		}
	}
	else if (copyRect.bottom>srcCanvasHeight)
	{
		// two half vertical split
		// '''''''''''''''''''''''
		// '                     '
		// '                     '
		// 'ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ'
		// '(0,0)                '
		// '                     '
		// '''''''''''''''''''''''

		// on screen top rect is sourced from the bottom side of the source canvas
		// on screen bottom rect is sourced from the top side of the source canvas
		TTBInteger		heightOfOnScreenBottomHalf=copyRect.bottom%srcCanvasHeight; // no pixels it's overlapping on the bottom border
		TTBInteger		heightOfOnScreenTopHalf=FRectHeight(copyRect)-heightOfOnScreenBottomHalf;
		
		// copy on screen bottom rect
		across=(mBounds.left/screenWidth)-1.0f;
		down=-((mBounds.bottom/screenHeight)-1.0f);
			
		glPixelStorei(GL_UNPACK_SKIP_ROWS,fromCanvas->GetHeight()-heightOfOnScreenBottomHalf);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS,GetXOffset()%srcCanvasWidth);
			
		glRasterPos2f(across,down);
		glDrawPixels(GetWidth(),heightOfOnScreenBottomHalf,GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
		
		// copy on screen top rect
		down=-(((mBounds.bottom-heightOfOnScreenBottomHalf)/screenHeight)-1.0f);

		glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS,GetXOffset()%srcCanvasWidth);
			
		glRasterPos2f(across,down);
		glDrawPixels(GetWidth(),heightOfOnScreenTopHalf,GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
	}
	else
	{
		// completely on canvas - cool	
		across=(mBounds.left/screenWidth)-1.0f;
		down=-((mBounds.bottom/screenHeight)-1.0f);

		glPixelStorei(GL_UNPACK_SKIP_ROWS,fromCanvas->GetHeight()-GetHeight()-(GetYOffset()%srcCanvasHeight));
		glPixelStorei(GL_UNPACK_SKIP_PIXELS,GetXOffset()%srcCanvasWidth);
			
		glRasterPos2f(across,down);
		glDrawPixels(GetWidth(),GetHeight(),GL_RGB,GL_UNSIGNED_BYTE,fromCanvas->GetPixels());
	}
	
	glRasterPos2f(0,0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
	
	RenderSprites();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ VerifyNewOffsets												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Check that the current offsets are valid for the source canvas
// Don't store or change any viewport vars, just check the x,y and throw if it's bad
void CWrappingViewport::VerifyNewOffsets(
	TTBInteger		inNewX,
	TTBInteger		inNewY)
{
	// Check the viewport is still inside the canvas	
	if ((inNewY<0) || (inNewX<0) ||
		((inNewY+GetHeight())>mEffectiveHeight) || ((inNewX+GetWidth())>mEffectiveWidth))
	{
		UTBException::ThrowViewportOutOfCanvas();
	}
}