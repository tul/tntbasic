// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Blast Pixel Double EPX.t
// Mark Tully
// 29/7/98
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1998, Mark Tully and John Treece-Birch
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

// This file allows a pixel doubler to be created with does smoothing and no clipping for any depth.
// To use, go to your source file and do:
//		#define		__EXPANDER_NAME					**YOUR FUNCTION NAME**
//		#define		__PIXEL_C_DATATYPE				**PIXEL TYPE OF PIXMAP**
//		#define		__EXPANDED_PIXEL_C_DATATYPE		**DOUBLE OF THE PIXEL TYPE OF PIXMAP**
//		#include	"Blast Pixel Double EPX.t"
//		#undef		__Expander_Name
//		#undef		__PIXEL_C_DATATYPE
//		#undef		__EXPANDED_PIXEL_C_DATATYPE

//		Other options include the following defines
//		#define		__EXPAND_WITH_16BIT_ANTIALIAS	Does a 50% blend of smoothed colours
//		#define		__WRAP_PIXEL_SOURCES_AROUND		Allows border smoothing by accessing the surrounding pixels on the borders
//													by wrapping around the rect

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BlastPixelDoubleNoClipEPX
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Does a pixel doubling with no clipping and applies a simple smooth algorithm as used in LucasArts games. The algorithm is
// taken from Gurus and is known as EPX (Eric's Pixel eXapansion). This probably won't be fast enough until it's in PPC
// asm. This should be portable to other depths quite easily.
// Things worth noting are that you can't smooth the border of inDoubleRect as the smoothing process involves accessing
// data around the rect during smoothing. As smoothing the border would access random memory I don't do it.
// P is the pixel to be smoothed.
// Pixel A is the pixel above the source pixel
// Pixel B is the pixel to the right of the source pixel
// Pixel C is the pixel to the left of the source pixel
// Pixel D is the pixel below the source pixel
/*
		+----+----+----+
		|    |    |    |
		|    | A  |    |
		+----+----+----+
		|    |    |    |
		| C  | P  | B  |
		+----+----+----+
		|    |    |    |
		|    | D  |    |
		+----+----+----+
*/


#define			ExpandPixel(inPixel)				((((__EXPANDED_PIXEL_C_DATATYPE)inPixel) << sizeof(__PIXEL_C_DATATYPE)*8) | ((__EXPANDED_PIXEL_C_DATATYPE)inPixel))

// Enumerations for the smooth mode
#ifndef			__BLAST_EPX_EXPANDER_ENUMS__
#define			__BLAST_EPX_EXPANDER_ENUMS__

enum
{
	// Bit values of the smooth operation
	kAEqualsB=1,
	kAEqualsC=2,
	kAEqualsD=4,
	kDEqualsC=8,
	kDEqualsB=16,
	kCEqualsB=32,
	
	// Modes
	kSmoothTopLeft=kAEqualsC,
	kSmoothTopRight=kAEqualsB,
	kSmoothBottomLeft=kDEqualsC,
	kSmoothBottomRight=kDEqualsB,
	kSmoothTopLeftAndBottomRight=kSmoothTopLeft+kSmoothBottomRight,
	kSmoothTopRightAndBottomLeft=kSmoothTopRight+kSmoothBottomLeft,
};
#endif

void __EXPANDER_NAME(BCPtr inSourceRec,BCPtr outDestRec,Rect *inDoubleRect,short inDestX,short inDestY)
{
	unsigned char					*destRow1,*destRow2,*startDestRow;
	unsigned char					*startSourceRow;
	unsigned char					*pixelPtr,*pixelAPtr,*pixelBPtr,*pixelCPtr,*pixelDPtr;
	unsigned short					origWidth,origHeight,width,height,xCounter;
	unsigned long					sourceRB,destRB,doubleDestRB;
	__EXPANDED_PIXEL_C_DATATYPE		expandedPixel;
	__PIXEL_C_DATATYPE				pixelA,pixelB,pixelC,pixelD,sourcePixel;
	unsigned char					smoothMode;
	
	height=origHeight=FRectHeight(*inDoubleRect);
	origWidth=FRectWidth(*inDoubleRect);
	
	sourceRB=inSourceRec->rowBytes;
	destRB=outDestRec->rowBytes;
	doubleDestRB=destRB*2;
	
	// Calc the dest ptrs
	startDestRow=outDestRec->baseAddr+inDestY*destRB+inDestX*sizeof(__PIXEL_C_DATATYPE);
	
	// Calc the source ptr
	startSourceRow=inSourceRec->baseAddr+inDoubleRect->top*sourceRB+inDoubleRect->left*sizeof(__PIXEL_C_DATATYPE);

	while (height--)
	{
		// Setup the ptrs for this scan line
		pixelPtr=startSourceRow;
		startSourceRow+=sourceRB;
		
		// Find the pixel above
		if (height==origHeight-1)							// If this is the top scanline
		{
			#ifdef __WRAP_PIXEL_SOURCES_AROUND
				pixelAPtr=pixelPtr+sourceRB*height;			// bottom scanline
			#else
				pixelAPtr=pixelPtr;
			#endif
		}
		else
			pixelAPtr=pixelPtr-sourceRB;
		
		// Find the pixel below
		if (height)											// If this is not the bottom scanline
			pixelDPtr=pixelPtr+sourceRB;
		else
		{
			#ifdef __WRAP_PIXEL_SOURCES_AROUND				// Top scanline
				pixelDPtr=inSourceRec->baseAddr+inDoubleRect->top*sourceRB+inDoubleRect->left*sizeof(__PIXEL_C_DATATYPE);
			#else
				pixelDPtr=pixelPtr;
			#endif
		}
		
		// Find the pixel to the left
		#ifdef __WRAP_PIXEL_SOURCES_AROUND					// On left side, where do we get the left pixel from?
			pixelCPtr=pixelPtr+(origWidth-1);
		#else
			pixelCPtr=pixelPtr;
		#endif

		// Find the pixel to the right
		pixelBPtr=pixelPtr+sizeof(__PIXEL_C_DATATYPE);

		destRow1=startDestRow;
		destRow2=destRow1+destRB;
		startDestRow+=doubleDestRB;
		
		width=origWidth;
				
		while (width--)
		{			
			// Work out which corners can be smoothed
			pixelA=*(__PIXEL_C_DATATYPE*)pixelAPtr;
			pixelB=*(__PIXEL_C_DATATYPE*)pixelBPtr;
			pixelC=*(__PIXEL_C_DATATYPE*)pixelCPtr;
			pixelD=*(__PIXEL_C_DATATYPE*)pixelDPtr;
			
			// Do we smooth?
			smoothMode=0;
			if (pixelA==pixelC)
				smoothMode|=kAEqualsC;
			if (pixelA==pixelB)
				smoothMode|=kAEqualsB;
			if (pixelC==pixelD)
				smoothMode|=kDEqualsC;
			if (pixelB==pixelD)
				smoothMode|=kDEqualsB;
			if (pixelA==pixelD)
				smoothMode|=kAEqualsD;
			if (pixelB==pixelC)
				smoothMode|=kCEqualsB;
			
			// Expand the pixel
			sourcePixel=*(__PIXEL_C_DATATYPE *)pixelPtr;
			expandedPixel=ExpandPixel(sourcePixel);

			#ifdef __EXPAND_WITH_16BIT_ANTIALIAS
				// Apply a 50% blend on the pixel which will be put down as the smoothed one
				if (pixelA!=sourcePixel)
					pixelA=BlastBlend1650(pixelA,sourcePixel);
				if (pixelD!=sourcePixel)
					pixelD=BlastBlend1650(pixelD,sourcePixel);
			#endif

			switch (smoothMode)
			{
				// Smooth...
				case kSmoothTopLeftAndBottomRight:
					*((__PIXEL_C_DATATYPE *)destRow1)++=pixelA;
					*((__PIXEL_C_DATATYPE *)destRow1)++=sourcePixel;
					*((__PIXEL_C_DATATYPE *)destRow2)++=sourcePixel;				
					*((__PIXEL_C_DATATYPE *)destRow2)++=pixelD;
					break;
					
				case kSmoothTopRightAndBottomLeft:
					*((__PIXEL_C_DATATYPE *)destRow1)++=sourcePixel;
					*((__PIXEL_C_DATATYPE *)destRow1)++=pixelA;
					*((__PIXEL_C_DATATYPE *)destRow2)++=pixelD;
					*((__PIXEL_C_DATATYPE *)destRow2)++=sourcePixel;				
					break;
				
				case kSmoothTopLeft:
					*((__PIXEL_C_DATATYPE *)destRow1)++=pixelA;
					*((__PIXEL_C_DATATYPE *)destRow1)++=sourcePixel;
					*((__EXPANDED_PIXEL_C_DATATYPE *)destRow2)++=expandedPixel;
					break;

				case kSmoothTopRight:
					*((__PIXEL_C_DATATYPE *)destRow1)++=sourcePixel;
					*((__PIXEL_C_DATATYPE *)destRow1)++=pixelA;
					*((__EXPANDED_PIXEL_C_DATATYPE *)destRow2)++=expandedPixel;
					break;

				case kSmoothBottomLeft:
					*((__EXPANDED_PIXEL_C_DATATYPE *)destRow1)++=expandedPixel;					
					*((__PIXEL_C_DATATYPE *)destRow2)++=pixelD;
					*((__PIXEL_C_DATATYPE *)destRow2)++=sourcePixel;
					break;

				case kSmoothBottomRight:
					*((__EXPANDED_PIXEL_C_DATATYPE *)destRow1)++=expandedPixel;					
					*((__PIXEL_C_DATATYPE *)destRow2)++=sourcePixel;				
					*((__PIXEL_C_DATATYPE *)destRow2)++=pixelD;
					break;

				default:
					// Straight expansion
					*((__EXPANDED_PIXEL_C_DATATYPE *)destRow1)++=expandedPixel;
					*((__EXPANDED_PIXEL_C_DATATYPE *)destRow2)++=expandedPixel;
					break;
			}
			
			// Move onto next pixel
			pixelPtr+=sizeof(__PIXEL_C_DATATYPE);
			pixelAPtr+=sizeof(__PIXEL_C_DATATYPE);
			pixelDPtr+=sizeof(__PIXEL_C_DATATYPE);
			
			// Move the right edge on, if it's got to the edge of the rect then either wrap around or stop
			if (width==1)	// Width is about to become zero
			{
				#ifdef __WRAP_PIXEL_SOURCES_AROUND
					pixelBPtr=startSourceRow-sourceRB;
				#endif
			}
			else
				pixelBPtr+=sizeof(__PIXEL_C_DATATYPE);
			
			// Move the left edge on. Left edge ptr is always 1 to the left of the central pixel unless on the first
			// pixel in which case it could either be the central pixel or could be sourcing from the other side of the
			// rect
			pixelCPtr=pixelPtr-sizeof(__PIXEL_C_DATATYPE);
		}
	}
}

#undef ExpandPixel