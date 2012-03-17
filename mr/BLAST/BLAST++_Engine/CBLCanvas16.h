// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLCanvas16.h
// © Mark Tully 2000
// 5/1/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
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

#include		"CBLCanvas.h"
class			CBLMask16;

class CBLCanvas16 : public CBLCanvas
{
	private:
		typedef CBLCanvas		inherited;

		inline bool				InRect(
									const Rect		&inRect,
									SInt16			inX,
									SInt16			inY)
								{
									if ((inX>=inRect.left) && (inX<=inRect.right) && (inY>inRect.top) && (inY<inRect.bottom))
										return true;
										
									return false;
								}

		inline void				FillRun(
									long				inY,
									long				x,
									long				endX,
									const RGBColour		&sourceRGB);

	public:
		/*e*/					CBLCanvas16(
									WindowPtr		inWindow,			// window's base addrs are calculated differently to gworlds so we need 2 constructors
									bool			inAdopt=true);
		/*e*/					CBLCanvas16(
									GWorldPtr		inWorld,
									bool			inAdopt=true);
		/*e*/					CBLCanvas16(
									SInt16			inWidth,
									SInt16			inHeight,
									GDHandle		inDevice=0L);

		virtual void /*e*/		SetGWorld(
									GWorldPtr		inWorld,
									bool			inAdopt);

		void					GetPixelRGB(
									SInt16			inX,
									SInt16			inY,
									RGBColour		&outColour);
		void					SetPixelRGB(
									SInt16			inX,
									SInt16			inY,
									const RGBColour	&inColour);
		TBLColour16				GetPixel16(
									SInt16			inX,
									SInt16			inY);
		void					SetPixel16(
									SInt16			inX,
									SInt16			inY,
									TBLColour16		inCol);
									
		void					DrawLine(
									SInt16			inX1,
									SInt16			inY1,
									SInt16			inX2,
									SInt16			inY2);									
		
		void					DrawLine16(
									SInt16			sourceX,
									SInt16			sourceY,
									SInt16			destX,
									SInt16 			destY);
		
		void					DrawLine16Clip(
									SInt16			sourceX,
									SInt16			sourceY,
									SInt16			destX,
									SInt16 			destY,
									const Rect		&inRect);
		
		void					FillRect(
									const Rect		&inRect);									
		void					FrameRect(
									const Rect		&inRect);
		
		void					FillPoly(
									SInt32			*inArray,
									SInt32			inNumCoOrdPairs);
		void					FramePoly(
									SInt32			*inArray,
									SInt32			inNumCoOrdPairs);
									
		void					FillOval(
									const Rect		&inRect);									
		void					FrameOval(
									const Rect		&inRect);

		void					MaskCopy(
									CBLCanvas16		&inSource,
									const Rect		&inSourceRect,
									CBLMask16		*inMask,
									SInt16			inDestX,
									SInt16			inDestY);

};
