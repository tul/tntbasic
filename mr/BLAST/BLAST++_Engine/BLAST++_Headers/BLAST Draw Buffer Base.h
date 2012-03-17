/***************************************************************************************************

BLAST Draw Buffer Base.h
Header file for the BLAST++ª engine
© TNT Software 1997-2000
All right reserved
Mark Tully
27/8/97

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

#pragma once

#include		"BLAST.h"
#include		"Linked Lists Lib.h"

class			CBLCanvas;

// ********* Blast draw buffer *********
// This object provides takes a pair of bitmaps and tracks the rectangles which need coping
// between them to keep the screen updated.
// Deriving from this allows you to implement custom copy routines, rectangle filtering and
// new ways of tracking and merging the invaled rects.

typedef class BlastDrawBuffer
{
	private:
		void					*refCon;
	
	protected:
		ABCPtr					sourceBuffer;
		ABCPtr					destBuffer;
		CBLCanvas				*mSourceCanvas;
		CBLCanvas				*mDestCanvas;
		Rect					boundsRect;

		// ********* RefCon access *********
		void						SetRefCon(void *refCon);
		void						*GetRefCon();

	public:
		// ********* Constructor/Destructor *********
								BlastDrawBuffer();
		virtual					~BlastDrawBuffer();
		virtual void			ResetBuffer();
	
		// ********* Buffer management *********
		void					SetSourceBuffer(ABCPtr newSource);
		void					SetDestBuffer(ABCPtr newDest);
		void					SwapSDBuffers();
		void					SetSourceCanvas(
									CBLCanvas				*inSource);
		void					SetDestCanvas(
									CBLCanvas				*inDest);

		CBLCanvas				*GetSourceCanvas()		{ return mSourceCanvas; } // CAN BE NIL!!!!!		
		CBLCanvas				*GetDestCanvas()		{ return mDestCanvas; } // CAN BE NIL!!!!!		
		ABCPtr					GetSourceBuffer();
		ABCPtr					GetDestBuffer();

		// ********* Blit routines *********
		virtual void			BlitAllRectsSD();
		virtual void			BlitRectSD(Rect &rect);
		virtual void			BlitAllSD();
		
		virtual void			BlitAllRectsDS();
		virtual void			BlitRectDS(Rect &rect);
		virtual void 			BlitAllDS();
		
		// ********* Rect tracking routines *********
		virtual void			ClearBlitList();
		virtual void			InvalRect(Rect &rect);
		virtual void			InvalRectWithClip(Rect &rect);
		virtual void			InvalRectWithClip(Rect &rect,Rect &clipTo);
		virtual void			InvalAll();

		// ********* Low level blit list operators *********
		virtual void			*LLGetBlitList() = 0;
		virtual void			LLSetBlitList(void *blitList) = 0;
		virtual void			LLMergeBlitList(void *blitList) = 0;
		
		// ********* Rect utilities *********
		virtual unsigned short	GetMaxBlitRects();
		virtual Boolean			SetMaxBlitRects(unsigned short numSprites,unsigned short customRects);
		virtual unsigned short	GetUsedBlitRects();
		virtual void			GetBoundsRect(Rect &rect);
		virtual void			SetBoundsRect(
									const Rect		&inRect);

} BlastDrawBuffer, *BlastDrawBufferPtr, **BlastDrawBufferHandle;

