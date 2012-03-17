// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CQDBufferedPortDrawBuffer.cpp
// © Mark Tully 2002
// 23/1/02
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
	A blast standard buffer designed to work on a window. Both the source and dest buffers are the window, the blit
	routine simply flushes the port to the screen.
*/

#include "CQDBufferedPortDrawBuffer.h"

// Flush methods
//	1	Flush entire screen
//  2	Max bounds rect of all dirty rects
//	3	Exact region
#define	FLUSH_METHOD		3

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CQDBufferedPortDrawBuffer::CQDBufferedPortDrawBuffer()
{
	mFlushRgn=::NewRgn();
	ThrowIfMemFull_(mFlushRgn);
	mTempRgn=::NewRgn();
	ThrowIfMemFull_(mTempRgn);
	
	mLastBoundsRect=boundsRect;
	::RectRgn(mFlushRgn,&mLastBoundsRect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CQDBufferedPortDrawBuffer::~CQDBufferedPortDrawBuffer()
{
	::DisposeRgn(mFlushRgn);
	::DisposeRgn(mTempRgn);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BlitAllRectsSD
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Flush changes to screen
void CQDBufferedPortDrawBuffer::BlitAllRectsSD()
{
#if FLUSH_METHOD==1
	// see if the rect has changed since last time, if it has then remake the rgn
	if (!SameRect(boundsRect,mLastBoundsRect))
	{
		mLastBoundsRect=boundsRect;
		::RectRgn(mFlushRgn,&mLastBoundsRect);
	}
	
/* Doesn't seem necessary
		if (::QDIsPortBufferDirty(sourceBuffer->rec.world))
		{
			::QDGetDirtyRegion(sourceBuffer->rec.world,mTempRgn);			
			::UnionRgn(mTempRgn,mFlushRgn,mFlushRgn);
		}*/

	::QDFlushPortBuffer(sourceBuffer->rec.world,mFlushRgn);
#elif FLUSH_METHOD==2
	// find largest enclosing rect and flush that
	if (blitList->usedRects>0)
	{
		Rect		temp;
	
		temp=blitList->rectArray[0];
	
		for (unsigned short counter=1; counter<blitList->usedRects; counter++)
		{
			temp.left=Lesser(temp.left,blitList->rectArray[counter].left);
			temp.top=Lesser(temp.top,blitList->rectArray[counter].top);
			temp.bottom=Greater(temp.bottom,blitList->rectArray[counter].bottom);
			temp.right=Greater(temp.right,blitList->rectArray[counter].right);
		}

		temp.bottom++;
		temp.right++;

		if (!SameRect(temp,mLastBoundsRect))
		{
			mLastBoundsRect=temp;
			::RectRgn(mFlushRgn,&mLastBoundsRect);
		}
		
/* Doesn't seem necessary
		if (::QDIsPortBufferDirty(sourceBuffer->rec.world))
		{
			::QDGetDirtyRegion(sourceBuffer->rec.world,mTempRgn);			
			::UnionRgn(mTempRgn,mFlushRgn,mFlushRgn);
		}*/

		::QDFlushPortBuffer(sourceBuffer->rec.world,mFlushRgn);
	}
#elif FLUSH_METHOD==3
	// build composite rgn and flush that (note, touching rects have already been merged
	if (blitList->usedRects>0)
	{
		::SetEmptyRgn(mFlushRgn);
		for (unsigned short counter=0; counter<blitList->usedRects; counter++)
		{
			Rect		&work=blitList->rectArray[counter];
			++work.right;
			++work.bottom;
			::RectRgn(mTempRgn,&blitList->rectArray[counter]);
			--work.right;
			--work.bottom;
			::UnionRgn(mTempRgn,mFlushRgn,mFlushRgn);
		}

/* Doesn't seem necessary
		if (::QDIsPortBufferDirty(sourceBuffer->rec.world))
		{
			::QDGetDirtyRegion(sourceBuffer->rec.world,mTempRgn);			
			::UnionRgn(mTempRgn,mFlushRgn,mFlushRgn);
		}*/

		::UnlockPortBits(sourceBuffer->rec.world);
		::QDFlushPortBuffer(sourceBuffer->rec.world,mFlushRgn);
		::LockPortBits(sourceBuffer->rec.world);
	}
#endif
}
