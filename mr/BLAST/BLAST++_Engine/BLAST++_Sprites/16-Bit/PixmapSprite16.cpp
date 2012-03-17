//	PixmapSprite16.cpp
//	By Richard Mckeon
//	29/10/97

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1997, Mark Tully and John Treece-Birch
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

#include "PixmapSprite16.h"
#include "BlastCopy16.h"
#include "16-Bit Transparency.h"

PixmapSprite16::PixmapSprite16()
{
	alpha=255;
	xoff=0;
	yoff=0;
	maskColour=0;
}

PixmapSprite16::~PixmapSprite16()
{

}

void PixmapSprite16::SetSpriteRec(ABCPtr theRec)
{
	spriteRec=theRec;
}

void PixmapSprite16::SetSprite(Rect &theRect)
{
	FSetRect(sourceRect,theRect.top,theRect.left,theRect.bottom,theRect.right);
	FSetRect(spriteRect,theRect.top,theRect.left,theRect.bottom,theRect.right);
	width=sourceRect.right-sourceRect.left;
	height=sourceRect.bottom-sourceRect.top;
}

void PixmapSprite16::RenderSprite(long xOff,long yOff,ABCPtr rec)
{
	if(alpha==255)
		BlastCopy16Mask(&spriteRec->rec,&rec->rec,&sourceRect,&spriteRect,maskColour);
	else if(alpha>=0)
		TransCopyMask(&spriteRec->rec,&rec->rec,&sourceRect,&spriteRect,alpha,maskColour);
}

void PixmapSprite16::RenderSpriteClipped(long xOff,long yOff,ABCPtr rec,Rect &clipRect)
{
	Rect	clippedSourceRect;
	
	FSetRect(clippedSourceRect,sourceRect.top,sourceRect.left,sourceRect.bottom,sourceRect.right);
	
	FOffset(clippedSourceRect,x,y);
	NewClipRect(&clippedSourceRect,&clipRect);
	FOffset(clippedSourceRect,-x,-y);
	
	if(alpha==255)
		BlastCopy16Mask(&spriteRec->rec,&rec->rec,&clippedSourceRect,&spriteRect,maskColour);
	else if(alpha>=0)
		TransCopyMask(&spriteRec->rec,&rec->rec,&clippedSourceRect,&spriteRect,alpha,maskColour);
}

void PixmapSprite16::RecalcSprite(long xOff,long yOff,Rect &clipRect,Boolean masterRecalc)
{
	if (needsRecalc || masterRecalc)
	{
		needsRecalc=false;
		if (spriteRec)
		{
			spriteRect.left=x-xoff+xOff;
			spriteRect.top=y-yoff+yOff;
			spriteRect.right=spriteRect.left+width;
			spriteRect.bottom=spriteRect.top+height;
			
			// does it need clipping?
			needsClip=NewClipRect(&spriteRect,&clipRect);
		}
		else
		{
			FSetRect(spriteRect,0,0,0,0);
			needsClip=csClippedOff;
		}
	}
}