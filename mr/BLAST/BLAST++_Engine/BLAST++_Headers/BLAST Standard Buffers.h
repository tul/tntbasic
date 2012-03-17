/***************************************************************************************************

BLAST Standard Buffers.h
Header file for the BLAST++ª engine
© TNT Software 1997
All right reserved
Mark Tully
28/8/97

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

#include		"BLAST Draw Buffer Base.h"

// 8-Bit BlastCopy merged rects list buffer
class BlastStandardBufferA : public BlastDrawBuffer
{
	protected:
		typedef struct LowLevelBlitList
		{
			Rect			*rectArray;
			unsigned short	usedRects,maxRects;
		} LowLevelBlitList, *LowLevelBlitListPtr;

		LowLevelBlitListPtr		blitList;

	public:
		// ********* Constructor/Destructor *********
								BlastStandardBufferA();
		virtual					~BlastStandardBufferA();
		
		// ********* Blit routines *********
		virtual void			BlitAllRectsSD();
		virtual void			BlitAllRectsDS();
		
		// ********* Rect tracking routines *********
		virtual void			ClearBlitList();
		virtual void			InvalRect(Rect &rect);
		virtual void			InvalRectWithClip(Rect &rect);
		virtual void			InvalRectWithClip(Rect &rect,Rect &clipTo);
		
		// ********* Rect utilities *********
		virtual unsigned short	GetMaxBlitRects();
		virtual Boolean			SetMaxBlitRects(unsigned short numSprites,unsigned short customRects);
		virtual unsigned short	GetUsedBlitRects();

		// ********* Low level blit accesors *********
		virtual void			*LLGetBlitList();
		virtual void			LLSetBlitList(void *blitList);
		virtual void			LLMergeBlitList(void *blitList);

};

// CopyBit based merged rects list buffer
class CBlastCopyBitsBuffer : public BlastStandardBufferA
{
	public:
		virtual void			BlitAllRectsSD();
		virtual void			BlitRectSD(Rect &rect);
		virtual void			BlitAllSD();
		
		virtual void			BlitAllRectsDS();
		virtual void			BlitRectDS(Rect &rect);
		virtual void 			BlitAllDS();
};

// 16-bit version
class CBlastStandardBufferA16 : public BlastStandardBufferA
{
	public:
		virtual void			BlitAllRectsSD();
		virtual void			BlitRectSD(Rect &rect);
		virtual void			BlitAllSD();
		
		virtual void			BlitAllRectsDS();
		virtual void			BlitRectDS(Rect &rect);
		virtual void 			BlitAllDS();
};