/***************************************************************************************************

BLAST Image Sprites.cpp
Extension of BLAST++ª engine
© TNT Software 1997
All right reserved
Mark Tully

29/8/97		:	Started it

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

#include	"BLAST Image Sprites.h"

ImageSprite8::ImageSprite8()
{
	image=0L;
}

void ImageSprite8::RenderSprite(long xOff,long yOff,ABCPtr rec)
{
	if (image)
	{
		TintHandle		tint=GetTint();
		DecodeImageNoClip(&rec->rec,image,tint?**tint:0L,x+xOff,y+yOff);
	}
}

void ImageSprite8::RenderSpriteClipped(long xOff,long yOff,ABCPtr rec,Rect &clipRect)
{
	if (image)
	{
		TintHandle		tint=GetTint();;
		DecodeImageClip(&rec->rec,image,tint?**tint:0L,&clipRect,x+xOff,y+yOff);
	}
}

// work out the sprites rect
// if too big, clip then set the clip flag
void ImageSprite8::RecalcSprite(long xOff,long yOff,Rect &clipRect,Boolean masterRecalc)
{
	if (needsRecalc || masterRecalc)
	{
		needsRecalc=false;
		if (image)
		{
			spriteRect.left=x-(**image).xoff+xOff;
			spriteRect.top=y-(**image).yoff+yOff;
			spriteRect.right=spriteRect.left+(**image).width;
			spriteRect.bottom=spriteRect.top+(**image).height;
			
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

void ImageSprite8::SetImage(ImageHandle image)
{
	this->image=image;
	needsRecalc=true;
}

void ImageSprite8::SetImage(short bank,short image)
{
	this->image=AccessImage(bank,image);
	needsRecalc=true;
}

ImageHandle ImageSprite8::GetImage()
{
	return image;
}



