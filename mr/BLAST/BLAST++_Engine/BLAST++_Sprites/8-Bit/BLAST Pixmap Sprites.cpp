/***************************************************************************************************

BLAST Pixmap Sprites.cpp
Extension of BLAST++ª engine
© TNT Software 1997
All right reserved
Mark Tully

4/9/97		:	Started it

***************************************************************************************************/

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
	
#include		"BLAST Pixmap Sprites.h"

PixmapSprite8::PixmapSprite8()
{
	image=0L;
}
	
void PixmapSprite8::RenderSprite(long xOff,long yOff,ABCPtr rec)
{
	if (image)
	{
		// DecodePixmap(DestRec,SourceImage,position)
		// DecodePixmapFlip(sourceRec,destRec,rect,x,y,unsigned char flipState)
	}
}

void PixmapSprite8::RenderSpriteClipped(long xOff,long yOff,ABCPtr rec,Rect &clipRect)
{
	if (image)
	{
	
	}
}

void PixmapSprite8::RecalcSprite(long xOff,long yOff,Rect &clipRect,Boolean masterRecalc)
{
	if (needsRecalc || masterRecalc)
	{
		needsRecalc=false;
		
		if (image)
		{
			spriteRect.top=y+xOff-image->ySpot;
			spriteRect.bottom=spriteRect.top+FRectHeight(image->sourceRect);
			spriteRect.left=x+xoff-image->xSpot;
			spriteRect.right=spriteRect.left+FRectWidth(image->sourceRect);
			needsClip=BetterClipRect(&spriteRect,&clipRect);
		}
		else
		{
			FSetRect(spriteRect,0,0,0,0);
			needsClip=csClippedOff;
		}
	}
}

void PixmapSprite8::SetImage(PixmapImagePtr image)
{
	this->image=image;
}

PixmapImagePtr PixmapSprite8::GetImage()
{
	return image;
}

// this is the general blit pixmap routine which despatches call to the other routines
void BlitPixmap(DestRec,SourceImage,position)
{
	// check the flags
	if (image->flags&(psFlagFlipAroundX|psFlagFlipAroundY))
	{
		// blit with a flip
		BlitPixmapFlipped();
	}
	else
	{
		// blit without the flip
	}
	
}

void BlitPixmapFlipped()
{
	unsigned char		*destStartRow,*sourceStartRow;
	unsigned char		*sourcePtr,*destPtr;
	short				width=FRectWidth(image->sourceRect);
	Boolean				yFlip=(image->flags&psFlagFlipAroundX!=0);
	
	// locate the correct row
	if (yFlip)
		startSRow=image->sourceRec->rowTables[image->sourceRect->bottom]; // start from the bottom
	else
		startSRow=image->sourceRec->rowTables[image->sourceRect->top]; // start as normal at the top
	
	// indent in to the data
	if (xFlip)
		startSRow+=image->sourceRect->right-1;
	else
		startSRow+=image->sourceRect->left;
	
	// set up the dest ptr and co
	destStartRow=destRec->rowTable[y]+x;
	sourcePtr=startSRow;
	destPtr=destSRow;
	
	if (xFlip)
	{
		// xFlip is true, yFlip maybe true
		if (yFlip)
		{
			// totally flipped man			
			for (yCounter=0; yCounter<height; yCounter++)
			{
				for (xCounter=0; xCounter<width; xCounter++)
				{
					*(dest++)=*(source--);
				}
				sourceStartRow-=sourceRB;
				sourcePtr=sourceStartRow;
				destStartRow+=destRB;
				destPtr=destStartRow;
			}
		}
		else
		{
			// x flip only
			for (yCounter=0; yCounter<height; yCounter++)
			{
				for (xCounter=0; xCounter<width; xCounter++)
				{
					*(dest++)=*(source--);
				}
				sourceStartRow+=sourceRB;
				sourcePtr=sourceStartRow;
				destStartRow+=destRB;
				destPtr=destStartRow;
			}
		}
	}
	else
	{
		// xFlip is false, yFlip must be true otherwise this routine wouldn't be called
	
	}
}



