/***************************************************************************************************

BLAST Standard Buffers.cpp
Extension of BLAST++ª engine
© TNT Software 1997
All right reserved
Mark Tully

This file contains standard classes derived from BLAST Sprites Bases.cpp.

28/8/97		:	Started it
2/1/00		:	Added the blast copybits buffer

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

#include	"BLAST Standard Buffers.h"
#include	"Blast_Debug.h"
#include	"Utility Objects.h"

// ********* Standard Blast Draw Buffer A *********
// Handles rectangle tracking and screen update
// Uses a rectangle array and smart merging to minimise the blits

BlastStandardBufferA::~BlastStandardBufferA()
{
	if (blitList)
	{
		if (blitList->rectArray)
			delete [] blitList->rectArray;
		delete blitList;
	}
}

BlastStandardBufferA::BlastStandardBufferA()
{
	blitList=0L;
	ResetBuffer();
}

void BlastStandardBufferA::ClearBlitList()
{
	if (blitList)
		blitList->usedRects=0;
}

void BlastStandardBufferA::InvalRectWithClip(Rect &rect,Rect &clipTo)
{
	// the rect being invalled must be clipped to the clip rect
	if (NewClipRect(&rect,&clipTo)>=0)
		InvalRect(rect);
}

void BlastStandardBufferA::InvalRectWithClip(Rect &rect)
{
	// the rect being invalled must be clipped to the bounds rect
	if (NewClipRect(&rect,&boundsRect)>=0)
		InvalRect(rect);
}

// Invals the rect with smart merging to minimise blit count. Rect is not checked for clipping
void BlastStandardBufferA::InvalRect(Rect &rect)
{
	short 		count;
	Boolean		xTouch,yTouch;
	
	for (count=0; count<blitList->usedRects; count++)
	{
		xTouch=false;
		yTouch=false;
		
		// do the rect touch?
		// check horizontally
		if (rect.left < blitList->rectArray[count].left)
		{
			if (rect.right >= blitList->rectArray[count].left)
				xTouch=true;
		}
		else
		{
			if (blitList->rectArray[count].right >= rect.left)
				xTouch=true;
		}	
	
		// check vertically
		if (rect.top < blitList->rectArray[count].top)
		{
			if (rect.bottom >= blitList->rectArray[count].top)
				yTouch=true;
		}
		else
		{
			if (blitList->rectArray[count].bottom >= rect.top)
				yTouch=true;
		}
		
		if (xTouch && yTouch)
		{
			// merge the rects
			blitList->rectArray[count].left=Lesser(blitList->rectArray[count].left,rect.left);
			blitList->rectArray[count].right=Greater(blitList->rectArray[count].right,rect.right);
			blitList->rectArray[count].top=Lesser(blitList->rectArray[count].top,rect.top);
			blitList->rectArray[count].bottom=Greater(blitList->rectArray[count].bottom,rect.bottom);
			return;
		}
	}
	
	// cannot be merged, inval if space
	if (blitList->usedRects<blitList->maxRects)
		blitList->rectArray[blitList->usedRects++]=rect;	
}

unsigned short BlastStandardBufferA::GetMaxBlitRects()
{
	return blitList->maxRects;
}

unsigned short BlastStandardBufferA::GetUsedBlitRects()
{
	return blitList->usedRects;
}

void BlastStandardBufferA::BlitAllRectsSD()
{
	for (unsigned short counter=0; counter<blitList->usedRects; counter++)
		BlastCopy(&sourceBuffer->rec,&destBuffer->rec,&blitList->rectArray[counter],&blitList->rectArray[counter]);
}

void BlastStandardBufferA::BlitAllRectsDS()
{
	for (unsigned short counter=0; counter<blitList->usedRects; counter++)
		BlastCopy(&destBuffer->rec,&sourceBuffer->rec,&blitList->rectArray[counter],&blitList->rectArray[counter]);
}

Boolean BlastStandardBufferA::SetMaxBlitRects(unsigned short numSprites,unsigned short customRects)
{
	Rect		*temp;
	unsigned short	newMax=numSprites*2+customRects;
	
	// allocate the blit list if we ain't got one
	if (!blitList)
	{
		blitList=new LowLevelBlitList;
		if (!blitList)
			return false;
		// init the blit list
		blitList->rectArray=0L;
		blitList->maxRects=0;
	}
	
	blitList->usedRects=0;
	temp=new Rect[newMax];
	if (!temp)
		return false;
	else
	{
		if (blitList->rectArray)
			delete [] blitList->rectArray;
		blitList->rectArray=temp;
		blitList->maxRects=newMax;
	}
	
	return true;
}

void *BlastStandardBufferA::LLGetBlitList()
{
	return blitList;
}

void BlastStandardBufferA::LLSetBlitList(void *newBlitList)
{
	BlastAssert_(newBlitList);
	blitList=(LowLevelBlitListPtr)newBlitList;
}

void BlastStandardBufferA::LLMergeBlitList(void *newBlitList)
{
	BlastAssert_(newBlitList);

	LowLevelBlitList		*temp=(LowLevelBlitList*)newBlitList;
	
	for (short counter=0; counter<temp->usedRects; counter++)
		InvalRect(temp->rectArray[counter]);
}

// ********* CopyBits Buffer *********
// Uses the same rect tracking technique as blast standard buffer a except copies with copy bits instead
void CBlastCopyBitsBuffer::BlitAllRectsSD()
{
	BlastAssert_(blitList);

	UWorldSaver		safe(destBuffer->rec.world);

	for (unsigned short counter=0; counter<blitList->usedRects; counter++)
		BlastCopyCB(&sourceBuffer->rec,&destBuffer->rec,&blitList->rectArray[counter],&blitList->rectArray[counter]);
}

void CBlastCopyBitsBuffer::BlitAllRectsDS()
{
	UWorldSaver		safe(sourceBuffer->rec.world);

	for (unsigned short counter=0; counter<blitList->usedRects; counter++)
		BlastCopyCB(&destBuffer->rec,&sourceBuffer->rec,&blitList->rectArray[counter],&blitList->rectArray[counter]);
}

void CBlastCopyBitsBuffer::BlitRectSD(Rect &rect)
{
	UWorldSaver		safe(destBuffer->rec.world);
	BlastCopyCB(&sourceBuffer->rec,&destBuffer->rec,&rect,&rect);
}

void CBlastCopyBitsBuffer::BlitAllSD()	
{
	UWorldSaver		safe(destBuffer->rec.world);
	BlastCopyCB(&sourceBuffer->rec,&destBuffer->rec,&boundsRect,&boundsRect);
}

void CBlastCopyBitsBuffer::BlitRectDS(Rect &rect)
{
	UWorldSaver		safe(sourceBuffer->rec.world);
	BlastCopyCB(&destBuffer->rec,&sourceBuffer->rec,&rect,&rect);
}

void CBlastCopyBitsBuffer::BlitAllDS()
{
	UWorldSaver		safe(sourceBuffer->rec.world);
	BlastCopyCB(&destBuffer->rec,&sourceBuffer->rec,&boundsRect,&boundsRect);
}

// ********* StandardBuffer16Bit *********
// Uses the same rect tracking technique as blast standard buffer a except copies with BLASTCopy16 instead
// Disclaimer : Code by Mckeon from Splat
void CBlastStandardBufferA16::BlitAllRectsSD()
{
	for (unsigned short counter=0; counter<blitList->usedRects; counter++)
		BlastCopy16(&sourceBuffer->rec,&destBuffer->rec,&blitList->rectArray[counter],&blitList->rectArray[counter]);
}

void CBlastStandardBufferA16::BlitRectSD(Rect &rect)
{
	BlastCopy16(&sourceBuffer->rec,&destBuffer->rec,&rect,&rect);
}

void CBlastStandardBufferA16::BlitAllSD()
{
	BlastCopy16(&sourceBuffer->rec,&destBuffer->rec,&boundsRect,&boundsRect);
}
		
void CBlastStandardBufferA16::BlitAllRectsDS()
{
	for (unsigned short counter=0; counter<blitList->usedRects; counter++)
		BlastCopy16(&destBuffer->rec,&sourceBuffer->rec,&blitList->rectArray[counter],&blitList->rectArray[counter]);
}

void CBlastStandardBufferA16::BlitRectDS(Rect &rect)
{
	BlastCopy16(&destBuffer->rec,&sourceBuffer->rec,&rect,&rect);
}

void CBlastStandardBufferA16::BlitAllDS()
{
	BlastCopy16(&destBuffer->rec,&sourceBuffer->rec,&boundsRect,&boundsRect);
}
