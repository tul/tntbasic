//	PixmapSprite16.h
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

#pragma once

#include "Blast++.h"

typedef class PixmapSprite16 : public BlastSprite
{
	protected:
	
		ABCPtr				spriteRec;
		Rect				sourceRect;
		short				alpha;
		short				xoff;
		short				yoff;
		short				width;
		short				height;
		unsigned short		maskColour;
		
	public:
	
	//	***Constructor/Destructor***
							PixmapSprite16();
							~PixmapSprite16();

	//	***Mask colour***
		unsigned short		GetMaskColour()				{ return maskColour; }
		unsigned short		SetMaskColour(
								unsigned short	inCol)	{ maskColour=inCol; }
						
	//	***Setup Functions***
		virtual void		SetSpriteRec(ABCPtr theRec);
		virtual void		SetSprite(Rect &theRect);
					
	//	***Virtual Function Overides***
		virtual void		RenderSprite(long xOff,long yOff,ABCPtr rec);
		virtual void		RenderSpriteClipped(long xOff,long yOff,ABCPtr rec,Rect &clipRect);
		virtual void		RecalcSprite(long xOff,long yOff,Rect &clipRect,Boolean masterRecalc);
		
} PixmapSprite16, *PixmapSprite16Ptr, **PixmapSprite16Handle;