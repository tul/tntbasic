// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CGLCanvas.h
// © John Treece-Birch 2002
// 22/1/02
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

#pragma once

#include		"BLAST.h"
#include		"BLAST_Types.h"
#if __MWERKS__
#include		<agl.h>
#else
#include		<AGL/agl.h>
#endif
#include		"CCString.h"
#include		"CCanvas.h"

class CGLCanvas : public CCanvas
{
	private:
		GLubyte					*mPixels;
		
		float					mBackRed,mBackGreen,mBackBlue;
		float					mForeRed,mForeGreen,mForeBlue;
		
		inline bool				InRect(
									const Rect		&inRect,
									SInt16			inX,
									SInt16			inY)
								{
									if ((inX>=inRect.left) && (inX<=inRect.right) && (inY>inRect.top) && (inY<inRect.bottom))
										return true;
										
									return false;
								}
								
		inline void				PaintPixel(
									SInt16		inX,
									SInt16		inY);
									
		void					DrawLineGL(
									SInt16			sourceX,
									SInt16			sourceY,
									SInt16			destX,
									SInt16 			destY);
		
		void					DrawLineGLClip(
									SInt16			sourceX,
									SInt16			sourceY,
									SInt16			destX,
									SInt16 			destY,
									const Rect		&inRect);

	public:
		/*e*/					CGLCanvas(
									SInt16					inWidth,
									SInt16					inHeight);
		virtual					~CGLCanvas();

		void					GetPixelRGB(
									SInt16			inX,
									SInt16			inY,
									RGBColour		&outColour);
		void					SetPixelRGB(
									SInt16			inX,
									SInt16			inY,
									const RGBColour	&inColour);
		TBLColour24				GetPixel24(
									SInt16			inX,
									SInt16			inY);
		void					SetPixel24(
									SInt16			inX,
									SInt16			inY,
									TBLColour24		inCol);
									
		void					DrawLine(
									SInt16			inX1,
									SInt16			inY1,
									SInt16			inX2,
									SInt16			inY2);
		
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

		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//	¥ Inval
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		virtual void			InvalCanvasRect(
									const Rect		&inRect)			{}
		virtual void			InvalCanvas()							{}
		virtual bool			GetInvalCanvasAsDraw()					{ return false; }
		virtual void			SetInvalCanvasAsDraw(
									bool			inState)			{}
									
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//	¥ Text
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		virtual void			DrawText(
									SInt16			inX,
									SInt16			inY,
									const CCString	&inStr);
		virtual void			CalcTextRect(
									SInt16			inX,
									SInt16			inY,
									const CCString	&inStr,
									Rect			&outRect);
		
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//	¥ Pen accessors
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		virtual void			GetForeColourRGB(
									RGBColour		&outColour)
								{
									outColour.red=mForeRed*256;
									outColour.green=mForeGreen*256;
									outColour.blue=mForeBlue*256;
								}
								
		virtual void			SetForeColourRGB(
									const RGBColour	&inColour)
								{
									mForeRed=(float)inColour.red/256;
									mForeGreen=(float)inColour.green/256;
									mForeBlue=(float)inColour.blue/256;
								}
								
		virtual void			GetBackColourRGB(
									RGBColour		&outColour)
								{
									outColour.red=mBackRed*256;
									outColour.green=mBackGreen*256;
									outColour.blue=mBackBlue*256;
								}
								
		virtual void			SetBackColourRGB(
									const RGBColour	&inColour)
								{
									mBackRed=(float)inColour.red/256;
									mBackGreen=(float)inColour.green/256;
									mBackBlue=(float)inColour.blue/256;
								}
									
		virtual void			DrawPict(
									PicHandle		inHandle,
									const Rect		&inRect);
		
		GLubyte*				GetPixels()									{ return mPixels; }
		
		virtual void			Copy(
									CCanvas			*inSourceCanvas,
									const Rect		&inCopyRect,
									SInt16			inDestX,
									SInt16			inDestY);
									
		virtual void			PaintAll();
		virtual void			EraseAll();
		
		void					ClipRect(
									Rect		&ioRect);
};