/***************************************************************************************************

BLAST Draw Buffer Base.cpp
Extension of BLAST++ª engine
© TNT Software 1997-2000
All right reserved
Mark Tully

27/8/97		:	Started it
2/1/00		:	Added accessors for bounds rect and automatic settings of bounds rect to smallest
				of source and dest. Code could break if it wasn't correctly setting the bounds
				of the ABCRecs it was passing to the BlastDrawBuffer.
11/1/00		:	Added use of canvases which is preferred over abc recs now

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

#include	"BLAST Draw Buffer Base.h"
#include	"Useful Defines.h"
#include	"CBLCanvas.h"

// ********* BlastDrawBuffer *********

BlastDrawBuffer::BlastDrawBuffer()
{
	mSourceCanvas=mDestCanvas=0;
	sourceBuffer=0L;
	destBuffer=0L;
	FSetRect(boundsRect,0,0,0,0);
	ResetBuffer();
}

void BlastDrawBuffer::SetBoundsRect(
	const Rect		&inRect)
{
	boundsRect=inRect;
}

void BlastDrawBuffer::ResetBuffer()
{
	ClearBlitList();
}

BlastDrawBuffer::~BlastDrawBuffer()
{

}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetSourceCanvas
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Use this to set the source buffer rather than SetSourceBuffer when a canvas is available
void BlastDrawBuffer::SetSourceCanvas(
	CBLCanvas						*inSource)
{
	if (inSource)
		SetSourceBuffer(&inSource->GetABCRec());

	mSourceCanvas=inSource;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetDestCanvas
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Use this to set the dest buffer rather than SetDestBuffer when a canvas is available
void BlastDrawBuffer::SetDestCanvas(
	CBLCanvas						*inDest)
{
	if (inDest)
		SetDestBuffer(&inDest->GetABCRec());

	mDestCanvas=inDest;
}
	
void BlastDrawBuffer::SetSourceBuffer(ABCPtr newSource)
{
	mSourceCanvas=0;
	sourceBuffer=newSource;
	
	if (sourceBuffer && destBuffer)
	{
		// Clip drawing to the smallest rect between source and dest
		boundsRect.right=Lesser(sourceBuffer->width,destBuffer->width);
		boundsRect.bottom=Lesser(sourceBuffer->height,destBuffer->height);
	}
	else
		FSetRect(boundsRect,0,0,0,0);
}

void BlastDrawBuffer::SetDestBuffer(ABCPtr newDest)
{
	mDestCanvas=0;
	destBuffer=newDest;

	if (sourceBuffer && destBuffer)
	{
		// Clip drawing to the smallest rect between source and dest
		boundsRect.right=Lesser(sourceBuffer->width,destBuffer->width);
		boundsRect.bottom=Lesser(sourceBuffer->height,destBuffer->height);
	}
	else
		FSetRect(boundsRect,0,0,0,0);
}

void BlastDrawBuffer::SwapSDBuffers()
{
	ABCPtr	temp;
	CBLCanvas	*temp2;
	
	SwapValues(sourceBuffer,destBuffer,temp);
	SwapValues(mSourceCanvas,mDestCanvas,temp2);
}
		
ABCPtr BlastDrawBuffer::GetSourceBuffer()
{
	return sourceBuffer;
}

ABCPtr BlastDrawBuffer::GetDestBuffer()
{
	return destBuffer;
}

void BlastDrawBuffer::GetBoundsRect(Rect &rect)
{
	rect=boundsRect;
}

void BlastDrawBuffer::BlitRectSD(Rect &rect)
{
	BlastCopy(&sourceBuffer->rec,&destBuffer->rec,&rect,&rect);
}

void BlastDrawBuffer::BlitAllSD()	
{
	BlastCopy(&sourceBuffer->rec,&destBuffer->rec,&boundsRect,&boundsRect);
}

void BlastDrawBuffer::BlitRectDS(Rect &rect)
{
	BlastCopy(&destBuffer->rec,&sourceBuffer->rec,&rect,&rect);
}

void BlastDrawBuffer::BlitAllDS()
{
	BlastCopy(&destBuffer->rec,&sourceBuffer->rec,&boundsRect,&boundsRect);
}

void BlastDrawBuffer::BlitAllRectsSD()
{
}

void BlastDrawBuffer::BlitAllRectsDS()
{
}

unsigned short BlastDrawBuffer::GetMaxBlitRects()
{
	return 0;
}

Boolean BlastDrawBuffer::SetMaxBlitRects(unsigned short numSprites,unsigned short customRects)
{
	return false;
}

unsigned short BlastDrawBuffer::GetUsedBlitRects()
{
	return 0;
}

void BlastDrawBuffer::ClearBlitList()
{

}

void BlastDrawBuffer::InvalRect(Rect &rect)
{

}

void BlastDrawBuffer::InvalRectWithClip(Rect &rect)
{

}

void BlastDrawBuffer::InvalRectWithClip(Rect &rect,Rect &clipTo)
{

}

void BlastDrawBuffer::InvalAll()
{
	InvalRect(boundsRect);
}

void BlastDrawBuffer::SetRefCon(void *refCon)
{
	this->refCon=refCon;
}

void *BlastDrawBuffer::GetRefCon()
{
	return refCon;
}
