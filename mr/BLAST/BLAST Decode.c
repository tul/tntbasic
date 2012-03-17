/***************************************************************************************************

BLAST Decode.c
Extension of BLASTª engine
Mark Tully
8/4/96
11/5/96	:	Added the clipping decoder
27/9/96	:	Added the scaled decoding of images and scaled clipping of images. These were already
			written, see Scale 3 & 4. I however improved them, FINALLY completely finished
			phase alignment after a skip and then phase alignment after a left side clip. It all
			seems to work very nicely.
3/10/96 :	Speeded the decoding algortithms up a teeny weeny bit by using stuff like
			destPtr++=srcPtr++ instead of destPtr=srcPtr; destPtr++; srcPtr++;

This provides functions for decoding graphics from BLAST's 'IMAG' sprite format.
Note that an unimplememnted trap crash is when the decoder hits an unknown decode command
(because the image is screwed up, or it's a new command being run on an old decoder). 

SECTION 1 - Decoding IMAG Resources

***************************************************************************************************/

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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

#include	"BLAST.h"

// globals
#ifdef powerc
static Boolean				g601Chip;
#endif

// ********* SECTION 1 - Decoding IMAG Resources *********

// If we have a 601 then we can blit doubles misaligned fast due to hardware correction of the
// misaligned data.
// If we have 603 or 604 we can't.
#ifdef powerc
void Set601Presence(Boolean here)
{
	g601Chip=here;
}
#endif

// Give it a locked data handle
// the pixmap should have been passes to setupDecode to prep it
// the image MUST fit inside the pixmap or a fascist crash will result (ie no auto clipping)
void DecodeImageNoClip(BCPtr decodeRec,ImageHandle data,TintPtr tintMap,short xPos,short yPos)
{
	unsigned char 	*rowStart;	// pointer to the beginning of this row
	unsigned char 	*srcPtr;	// pointer to where we're reading from in the image data
	unsigned char 	*destPtr;	// pointer to where we're writing to in the pixmap
	unsigned long	opCode;		// the instuction code
	unsigned long	opData;		// any parameters from the code
	unsigned long	miscCount;
	Boolean			done=false;
	
	// set up a couple of variables (NB: destPtr is inited by the fist rmLineStart op)
	rowStart=decodeRec->baseAddr + (yPos-(**data).yoff)*decodeRec->rowBytes + (xPos-(**data).xoff);
	srcPtr=(unsigned char *)*data;
	srcPtr+=sizeof(ImageData);
	
	while(!done)
	{
		opCode=(*((unsigned long *)srcPtr)) >> 24;
		opData=(*((unsigned long *)srcPtr)) & 0x00ffffff;
		srcPtr+=sizeof(unsigned long);
		
		// act according to the opCode
		switch(opCode)
		{
			case rmDrawRun:
				miscCount=opData;
				// use a series of while loops to move all the data in the biggest chunks possible
				#ifdef powerc
				// raa PowerPC 601, 8 bytes in one move command
				if (g601Chip)
				{
					while (miscCount>=sizeof(double))
					{
						*((double *)destPtr)++=*((double *)srcPtr)++;
						miscCount-=sizeof(double);
					}				
				}
				#endif
				
				while(miscCount>=sizeof(unsigned long))
				{
					*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
					miscCount-=sizeof(unsigned long);				
				}
				
				while(miscCount>=sizeof(unsigned short))
				{
					*((unsigned short *)destPtr)++=*((unsigned short *)srcPtr)++;
					miscCount-=sizeof(unsigned short);				
				}
				
				while(miscCount>=sizeof(unsigned char))
				{
					*((unsigned char *)destPtr)++=*((unsigned char *)srcPtr)++;
					miscCount-=sizeof(unsigned char);				
				}
				
				// after all the data has been moved, the srcPtr needs adjusting for padding
				srcPtr+=((opData&3L)==0) ? 0 : (4-(opData&3L));
				break;
				
			case rmSkipRun:
				destPtr+=opData;
				break;
				
			case rmLineStart:
				destPtr=rowStart;
				rowStart+=decodeRec->rowBytes;
				break;
				
			case rmShadowRun:
				if (tintMap)
				{
					miscCount=opData;
				
					while(miscCount)
					{
						*((unsigned char *)destPtr)=tintMap[*((unsigned char *)destPtr)];
						destPtr+=sizeof(unsigned char);
						miscCount-=sizeof(unsigned char);				
					}
				}
				else
					destPtr+=opData;	// just skip the pixels if no TINT has been given
				break;
				
			case rmEndShape:
				done=true;
				break;
			
			default:
				Debugger();
				done=true;
				break;
		}
	}
}

void DecodeImageClip(BCPtr decodeRec,ImageHandle data,TintPtr tintMap,Rect *globalClipRect,short xPos,short yPos)
{
	unsigned char 	*rowStart;	// pointer to the beginning of this row
	unsigned char 	*srcPtr;	// pointer to where we're reading from in the image data
	unsigned char 	*destPtr;	// pointer to where we're writing to in the pixmap
	unsigned long	opCode;		// the instuction code
	unsigned long	opData;		// any parameters from the code
	unsigned long	miscCount,extraCounter;
	unsigned long	xCount=0L,yCount=0L; // the position in the shape
	Rect			clipRect;
	Boolean			done=false;
		
	// adjust the pos according to the hotspot
	yPos-=(**data).yoff;
	xPos-=(**data).xoff;
	
	// check if this sprites draw rect even falls inside the clip rect
	clipRect.left=xPos;	// left side
	clipRect.right=xPos+(**data).width;
	clipRect.top=yPos;
	clipRect.bottom=yPos+(**data).height;
	
	// if the local clipping rect is outside of the globalClipRect then don't draw it
	// rects don't touch?
	if ((clipRect.right<globalClipRect->left) || // too far left
	    (clipRect.left>globalClipRect->right) || // too far right
	    (clipRect.bottom<globalClipRect->top) || // too far up
	    (clipRect.top>globalClipRect->bottom)) // too far down
	{
		// the rects fail to touch, don't draw
		return;
	}
	
	// set up a couple of variables (NB: destPtr is inited by the first rmLineStart op)
	rowStart=decodeRec->baseAddr + yPos*decodeRec->rowBytes + xPos;
	srcPtr=(unsigned char *)*data;
	srcPtr+=sizeof(ImageData);
	
	// create a clipped rect in the coordinates of the sprite
	// the clipping rect created here is measured from the sprites hot spot
	clipRect.left = xPos < globalClipRect->left ? globalClipRect->left - xPos : 0;
	clipRect.right = (xPos+(**data).width) > globalClipRect->right ? globalClipRect->right - xPos : (**data).width;
	clipRect.top = yPos < globalClipRect->top ? globalClipRect->top - yPos : 0;
	clipRect.bottom = (yPos+(**data).height) > globalClipRect->bottom ? globalClipRect->bottom - yPos : (**data).height;
	
	while(!done)
	{
		opCode=(*((unsigned long *)srcPtr)) >> 24;
		opData=(*((unsigned long *)srcPtr)) & 0x00ffffff;
		srcPtr+=sizeof(unsigned long);
		
		// act according to the opCode
		switch(opCode)
		{
			case rmDrawRun:
				miscCount=opData;
				extraCounter=0L;
								
				// Now to adjust for clipping
				// if necessary clip to the left
				if (xCount<clipRect.left)
				{
					// It may not be necessary to draw this line at all
					if (miscCount<(clipRect.left-xCount))
					{
						destPtr+=miscCount;	// skip this line
						srcPtr+=miscCount;
						srcPtr+=((opData&3L)==0)?0:(4-(opData&3L)); // align to 4 bytes
						xCount+=miscCount;
						break;
					}
					else
					{
						// this line needs drawing but the first few pixels do not, skip to where
						// the drawing starts
						miscCount-=clipRect.left-xCount;
						destPtr+=clipRect.left-xCount;
						srcPtr+=clipRect.left-xCount;
						xCount+=clipRect.left-xCount;
					}
				}
				
				// if necessary clip to the right
				if (xCount+miscCount>clipRect.right)
				{
					// if this run does not appear at all skip it
					if (xCount>clipRect.right)
					{
						destPtr+=miscCount;
						srcPtr+=miscCount;
						srcPtr+=((opData&3L)==0)?0:(4-(opData&3L)); // align to 4 bytes
						xCount+=miscCount;
						break;
					}
					else
					{
						// if it needs drawing then draw for as many pixels as possible
						extraCounter=miscCount;
						miscCount-=(xCount+miscCount)-clipRect.right;
						extraCounter-=miscCount;
					}
				}
				
				// adjust xCount for the run
				xCount+=miscCount;
				
				// use a series of while loops to move all the data in the biggest chunks possible
				#ifdef powerc
				// raa PowerPC 601, 8 bytes in one move command
				if (g601Chip)
				{
					while (miscCount>=sizeof(double))
					{
						*((double *)destPtr)++=*((double *)srcPtr)++;
						miscCount-=sizeof(double);
					}				
				}
				#endif
				
				while(miscCount>=sizeof(unsigned long))
				{
					*((unsigned long *)destPtr)++=*((unsigned long *)srcPtr)++;
					miscCount-=sizeof(unsigned long);				
				}
				
				while(miscCount>=sizeof(unsigned short))
				{
					*((unsigned short *)destPtr)++=*((unsigned short *)srcPtr)++;
					miscCount-=sizeof(unsigned short);				
				}
				
				while(miscCount>=sizeof(unsigned char))
				{
					*((unsigned char *)destPtr)++=*((unsigned char *)srcPtr)++;
					miscCount-=sizeof(unsigned char);				
				}
				
				// adjust for the right clipping
				destPtr+=extraCounter;
				srcPtr+=extraCounter;
				xCount+=extraCounter;
				
				// after all the data has been moved, the srcPtr needs adjusting for padding
				srcPtr+=((opData&3L)==0) ? 0 : (4-(opData&3L));
				break;
				
			case rmSkipRun:
				destPtr+=opData;
				xCount+=opData;
				break;
				
			case rmLineStart:
				// if this line is above the clip rect then skip it
				if (yCount<clipRect.top)
					srcPtr+=opData;
					
				destPtr=rowStart;
				rowStart+=decodeRec->rowBytes;
				yCount++;
				xCount=0L;
				
				// if we have reached the bottom clip then exit the draw
				if (yCount>clipRect.bottom)
					done=true;
				break;
				
			case rmShadowRun:
				if (tintMap)
				{		
					// must also clip the shadows
					// note that this is a copy and paste from the draw run code with a few
					// alterations. When the draw run code wished to skip the draw run, or a 
					// few pixels it altered srcPtr. This was to move through the data. As a 
					// shadowrun has no data other than the count of how many pixels to do 
					// the shadow for, srcPtr should not be increased, therefore all code to 
					// increase srcPtr is remmed.
					
					miscCount=opData;
					extraCounter=0L;
					
					// Now to adjust for clipping
					// if necessary clip to the left
					if (xCount<clipRect.left)
					{
						// It may not be necessary to draw this line at all
						if (miscCount<(clipRect.left-xCount))
						{
							destPtr+=miscCount;	// skip this line
							//srcPtr+=miscCount;
							//srcPtr+=((opData&3L)==0)?0:(4-(opData&3L)); // align to 4 bytes
							xCount+=miscCount;
							break;
						}
						else
						{
							// this line needs drawing but the first few pixels do not, skip to where
							// the drawing starts
							miscCount-=clipRect.left-xCount;
							destPtr+=clipRect.left-xCount;
							//srcPtr+=clipRect.left-xCount;
							xCount+=clipRect.left-xCount;
						}
					}
					
					// if necessary clip to the right
					if (xCount+miscCount>clipRect.right)
					{
						// if this run does not appear at all skip it
						if (xCount>clipRect.right)
						{
							destPtr+=miscCount;
							//srcPtr+=miscCount;
							//srcPtr+=((opData&3L)==0)?0:(4-(opData&3L)); // align to 4 bytes
							xCount+=miscCount;
							break;
						}
						else
						{
							// if it needs drawing then draw for as many pixels as possible
							extraCounter=miscCount;
							miscCount-=(xCount+miscCount)-clipRect.right;
							extraCounter-=miscCount;
						}
					}
					
					// adjust xCount for the run
					xCount+=miscCount;
				
					while(miscCount)
					{
						*((unsigned char *)destPtr)=tintMap[*((unsigned char *)destPtr)];
						destPtr+=sizeof(unsigned char);
						miscCount-=sizeof(unsigned char);				
					}
					
					// adjust for the right clipping
					destPtr+=extraCounter;
					//srcPtr+=extraCounter;
					xCount+=extraCounter;
				}
				else
				{
					destPtr+=opData;	// just skip the pixels if no TINT has been given
					xCount+=opData;	
				}
				break;
				
			case rmEndShape:
				done=true;
				break;
			
			default:
				Debugger();
				done=true;
				break;
		}
	}
}

/*
	The problem with scaling images is that each scanline can start from a different position and so
	be out of phases with the scanlines above and below it. ie scaling might mean that the every
	second pixel is missed out. If the next line starts one pixel further in then missing every
	other pixel out would mean the scalines were not level, or out of phase. So I came up with some
	dogs bollocks to remidy the situation and it took a long time.
*/

// decodes an image with scaling and no clipping
void DecodeImageScaled(BCPtr decodeRec,ImageHandle data,TintPtr tintMap,short xPos,short yPos,Fixed xScale,Fixed yScale)
{
	unsigned char 	*rowStart;	// pointer to the beginning of this row
	unsigned char 	*srcPtr;	// pointer to where we're reading from in the image data
	unsigned char	*lastRow=0L;// pointer to the current row so it can be repeated if necessary
	unsigned char 	*destPtr;	// pointer to where we're writing to in the pixmap
	unsigned long	opCode;		// the instuction code
	unsigned long	opData;		// any parameters from the code
	unsigned long	miscCount;
	short			storedRow=-1;
	Boolean			done=false;
	Fixed			xCounter,yCounter=0L;
	Fixed			pixelsPassed;	// number of pixels of source passed on this scaline
	
	if (xScale<=0 || yScale<=0)
		return;
	
	// calculate the scaled hotspot
	xPos-=ScaledLength((**data).xoff,xScale);
	yPos-=ScaledLength((**data).yoff,yScale);
	
	// set up a couple of variables (NB: destPtr is inited by the fist rmLineStart op)
	rowStart=decodeRec->baseAddr + yPos*decodeRec->rowBytes + xPos;
	srcPtr=(unsigned char *)*data;
	srcPtr+=sizeof(ImageData);
	
	while(!done)
	{
		opCode=(*((unsigned long *)srcPtr)) >> 24;
		opData=(*((unsigned long *)srcPtr)) & 0x00ffffff;
		srcPtr+=sizeof(unsigned long);
		
		// act according to the opCode
		switch(opCode)
		{
			case rmDrawRun:
				// get the number of pixels we have to draw after the scale
				miscCount=opData<<kBlastFP;
				xCounter-=pixelsPassed;	
			
				while (xCounter<miscCount)
				{
					*destPtr=*(srcPtr+(xCounter>>kBlastFP));
					destPtr++;
					xCounter+=xScale;
				}
				
				// after all the data has been moved, the srcPtr needs adjusting for padding
				xCounter+=pixelsPassed;
				pixelsPassed+=miscCount;
				srcPtr+=opData;
				srcPtr+=((opData&3L)==0) ? 0 : (4-(opData&3L));
				break;
				
			case rmSkipRun:
				miscCount=opData<<kBlastFP;
				xCounter-=pixelsPassed;
				while (xCounter<miscCount)
				{
					destPtr++;
					xCounter+=xScale;
				}
				xCounter+=pixelsPassed;
				pixelsPassed+=miscCount;
				break;
				
			case rmLineStart:
				// if we're not at the desired row then attempt to skip to it
				if ((yCounter>>kBlastFP)!=storedRow)
				{
					lastRow=srcPtr;
					storedRow++;
					srcPtr+=opData;					
				}
			
				// if the requested draw row is the storedRow then draw it
				if ((yCounter>>kBlastFP)==storedRow)
				{
					srcPtr=lastRow;
					
					destPtr=rowStart;
					rowStart+=decodeRec->rowBytes;
					xCounter=0;
					pixelsPassed=0;				
					yCounter+=yScale;
				}				
				break;
				
			case rmShadowRun:
				
				miscCount=opData<<kBlastFP;
				
				xCounter-=pixelsPassed;
								
				if (tintMap)
				{
					while (xCounter<miscCount)
					{
						*destPtr=tintMap[*((unsigned char *)destPtr)];
						destPtr++;
						xCounter+=xScale;
					}
				}
				else
				{
					while (xCounter<miscCount)
					{
						destPtr++;
						xCounter+=xScale;
					}
				}
				
				xCounter+=pixelsPassed;
				pixelsPassed+=miscCount;
			
				break;
				
			case rmEndShape:
				done=true;
				break;
			
			default:
				Debugger();
				done=true;
				break;
		}
	}
}


// same as above except with clipping
void DecodeImageScaledClip(BCPtr decodeRec,ImageHandle data,TintPtr tintMap,Rect *globalClipRect,short xPos,short yPos,Fixed xScale,Fixed yScale)
{
	unsigned char 	*rowStart;	// pointer to the beginning of this row
	unsigned char 	*srcPtr;	// pointer to where we're reading from in the image data
	unsigned char	*lastRow=0L;// pointer to the current row so it can be repeated if necessary
	unsigned char 	*destPtr;	// pointer to where we're writing to in the pixmap
	unsigned char	*endSrcPtr;	// what the ptr should be after the draw run is finshed (used only for clipping with scale decode)
	unsigned long	opCode;		// the instuction code
	unsigned long	opData;		// any parameters from the code
	unsigned long	miscCount,tempLong;
	short			storedRow=-1;	// the current row number which is being stored in lastRow. Used to redraw a horizontal scanline when expanding
	Boolean			done=false;
	Fixed			xCounter,yCounter=0L;	// counters used for pixel counting.
	Fixed			pixelsPassed;	// number of pixels passed on the srcBitMap (fixed)
	short			scaledWidth,scaledHeight;
	Rect			clipRect;
	short			rowsDrawn=0;

	if (xScale<=0 || yScale<=0)
		return;

	// calculate the scaled hotspot
	xPos-=ScaledLength((**data).xoff,xScale);
	yPos-=ScaledLength((**data).yoff,yScale);
	
	// calculate the scaled dimensions
	scaledWidth=ScaledLength((**data).width,xScale);
	scaledHeight=ScaledLength((**data).height,yScale);
	
	// check if this sprites draw rect even falls inside the clip rect
	clipRect.left=xPos;	// left side
	clipRect.right=xPos+scaledWidth;
	clipRect.top=yPos;
	clipRect.bottom=yPos+scaledHeight;
	
	// if the local clipping rect is outside of the globalClipRect then don't draw it
	// rects don't touch?
	if ((clipRect.right<globalClipRect->left) || // too far left
	    (clipRect.left>globalClipRect->right) || // too far right
	    (clipRect.bottom<globalClipRect->top) || // too far up
	    (clipRect.top>globalClipRect->bottom)) // too far down
	{
		// the rects fail to touch, don't draw
		return;
	}
	
	// set up a couple of variables (NB: destPtr is inited by the first rmLineStart op)
	rowStart=decodeRec->baseAddr + yPos*decodeRec->rowBytes + xPos;
	srcPtr=(unsigned char *)*data;
	srcPtr+=sizeof(ImageData);
	
	// create a clipped rect in the coordinates of the sprite
	// the clipping rect created here is measured from the sprites hot spot
	clipRect.left = xPos < globalClipRect->left ? globalClipRect->left - xPos : 0;
	clipRect.right = (xPos+scaledWidth) > globalClipRect->right ? globalClipRect->right - xPos : scaledWidth;
	clipRect.top = yPos < globalClipRect->top ? globalClipRect->top - yPos : 0;
	clipRect.bottom = (yPos+scaledHeight) > globalClipRect->bottom ? globalClipRect->bottom - yPos : scaledHeight;
	
	// adjust for clipping at the top
	yCounter=clipRect.top*yScale;
	rowsDrawn=clipRect.top;
	rowStart+=rowsDrawn*decodeRec->rowBytes;
	
	while(!done)
	{
		opCode=(*((unsigned long *)srcPtr)) >> 24;
		opData=(*((unsigned long *)srcPtr)) & 0x00ffffff;
		srcPtr+=sizeof(unsigned long);
		
		// act according to the opCode
		switch(opCode)
		{
			case rmDrawRun:
				// get the number of pixels we have to draw after the scale
				miscCount=opData<<kBlastFP;
				endSrcPtr=srcPtr+opData;
				endSrcPtr+=((opData&3L)==0) ? 0 : (4-(opData&3L));
				
				// adjust for clipping : NB: Any changes made here should also be made to the tint
				// clipping code
				// if necessary clip to the left
				// see if the number of pixels passed on screen so far has passed the left most
				// side of the cliprect. clipRect.left is how many pixels we should skip before
				// drawing.

				// to see if it overlaps, we get the scaledLength and see if it's less than the 
				// clipRect.left.
				// This gives ScaledLength(pixelsPassed)<clipRect.left
				// now if we unscale both sides of the equation the logic is still valid and it's
				// faster. Unscaling a scale gives the orig and we end up with....
				tempLong=clipRect.left*xScale;
				if (pixelsPassed<tempLong)
				{
					// have not reached left side, must clip
					// get the total length of this line after scaling
					// was ScaledLength((miscCount+pixelsPassed)>>kBlastFP,xScale)<clipRect.left
					// see notes above
					
					// it may not be necessary to draw this line at all
					if ((miscCount+pixelsPassed)<tempLong)
					{
						// this line does not need drawing at all
						xCounter-=pixelsPassed;
						while (xCounter<miscCount)
						{
							destPtr++;
							xCounter+=xScale;
						}
						xCounter+=pixelsPassed;
						pixelsPassed+=miscCount;
						
						// skip line
						srcPtr=endSrcPtr;
						break;
					}
					else
					{
						// this line needs drawing but the first few pixels do not, skip to where
						// the drawing starts
						
						// clipRect.left contains how many on screen pixels we need to skip in
						// from the left most pixel of the image.
						// therefore clipRect.left-pixels already skipped on screen is how many
						// more pixels we need to pass.
						// pixelsPassed is how many pixels have been passed on the srcBit map
						// convert clipRect.left to the same coord sytem and sub off pixelsPassed
						// to get the number of pixels to be skipped on the srcBitMap
						// NB : Now clipRect.left * xScale is in tempLong
						// so this : tempLong=(clipRect.left*xScale)-pixelsPassed;
						// becomes :
						
						tempLong-=pixelsPassed;
											
						// skip tempLong pixels
						xCounter-=pixelsPassed;
						while (xCounter<tempLong)
						{
							destPtr++;
							xCounter+=xScale;
						}
						xCounter+=pixelsPassed;
						
						// pixels passed is ALWAYS a multiple of 1024, otherwise you get bad phase
						// if we round it up then it will record us passing more pixels than
						// we actually did. It must be rounded down, it will then yield the correct 
						// number of pixels passed.
						// make templong a multiple of 1024 by rouding down
						tempLong&=0xFFFFFC00; // 0xFFFFFFFF - 1023
							
						srcPtr+=tempLong>>kBlastFP;
						pixelsPassed+=tempLong;
						
						miscCount-=tempLong;
					}
				}
				
				// clip the right edge NB: Any changes to the clipping calcs here should also be made
				// in the tint code. Don't forget that srcPtr doesn't get adjusted in the tint
				// code, that's the only difference
				// see notes on left clipping if this looks a little mad.
				// was
				// if (ScaledLength((miscCount+pixelsPassed)>>kBlastFP,xScale)>clipRect.right)
				tempLong=clipRect.right*xScale;
				if ((miscCount+pixelsPassed)>tempLong)
				{
					// get how many pixels to skip on the srcBitMap
					tempLong=(miscCount+pixelsPassed)-tempLong;
					// if the number to skip would mean we were still in this draw then do a shorter
					// draw
					if (tempLong<miscCount)
						miscCount-=tempLong;
					else
					{
						// else skip the draw
						xCounter-=pixelsPassed;
						while (xCounter<miscCount)
							xCounter+=xScale;						
						xCounter+=pixelsPassed;
						
						pixelsPassed+=miscCount;
						srcPtr=endSrcPtr;
						break;
					}
				}

				xCounter-=pixelsPassed;
				while (xCounter<miscCount)
				{
					*destPtr=*(srcPtr+(xCounter>>kBlastFP));
					destPtr++;
					xCounter+=xScale;
				}
				xCounter+=pixelsPassed;
				pixelsPassed+=miscCount;

				// endSrcPtr is used as when we clip left, srcPtr goes up and opdata cannot simply be decreased because of the 4-byte alignment code
				srcPtr=endSrcPtr;
				break;
				
			case rmSkipRun:
				miscCount=opData<<kBlastFP;
				xCounter-=pixelsPassed;
				while (xCounter<miscCount)
				{
					destPtr++;
					xCounter+=xScale;
				}
				xCounter+=pixelsPassed;
				pixelsPassed+=miscCount;
				break;
				
			case rmLineStart:
				
				// clip the bottom (clipping of the top is done right at the beginning
				if (rowsDrawn>=clipRect.bottom)
				{
					done=true;
					break;
				}
				
				// if we're not at the desired row then attempt to skip to it
				if ((yCounter>>kBlastFP)!=storedRow)
				{
					lastRow=srcPtr;
					storedRow++;
					srcPtr+=opData;					
				}
				
				// if the requested draw row is the storedRow then draw it
				if ((yCounter>>kBlastFP)==storedRow)
				{
					srcPtr=lastRow;
					
					destPtr=rowStart;
					rowStart+=decodeRec->rowBytes;
					xCounter=0;
					pixelsPassed=0;				
					yCounter+=yScale;
					rowsDrawn++;
				}			
				break;
				
			case rmShadowRun:
				
				miscCount=opData<<kBlastFP;
				
				// clip the right edge NB: Any changes to the clipping calcs here should also be made
				// in the drawRun code. The ONLY difference is that srcPtr isn't adjusted here

				// if necessary clip to the left
				// see if the number of pixels passed on screen so far has passed the left most
				// side of the cliprect. clipRect.left is how many pixels we should skip before
				// drawing.

				// to see if it overlaps, we get the scaledLength and see if it's less than the 
				// clipRect.left.
				// This gives ScaledLength(pixelsPassed)<clipRect.left
				// now if we unscale both sides of the equation the logic is still valid and it's
				// faster. Unscaling a scale gives the orig and we end up with....
				tempLong=clipRect.left*xScale;
				if (pixelsPassed<tempLong)
				{
					// have not reached left side, must clip
					// get the total length of this line after scaling
					// was ScaledLength((miscCount+pixelsPassed)>>kBlastFP,xScale)<clipRect.left
					// see notes above
					
					// it may not be necessary to draw this line at all
					if ((miscCount+pixelsPassed)<tempLong)
					{
						// this line does not need drawing at all
						xCounter-=pixelsPassed;
						while (xCounter<miscCount)
						{
							destPtr++;
							xCounter+=xScale;
						}
						xCounter+=pixelsPassed;
						pixelsPassed+=miscCount;
						
						// skip line
						//srcPtr=endSrcPtr;
						break;
					}
					else
					{
						// this line needs drawing but the first few pixels do not, skip to where
						// the drawing starts
						
						// clipRect.left contains how many on screen pixels we need to skip in
						// from the left most pixel of the image.
						// therefore clipRect.left-pixels already skipped on screen is how many
						// more pixels we need to pass.
						// pixelsPassed is how many pixels have been passed on the srcBit map
						// convert clipRect.left to the same coord sytem and sub off pixelsPassed
						// to get the number of pixels to be skipped on the srcBitMap
						// NB : Now clipRect.left * xScale is in tempLong
						// so this : tempLong=(clipRect.left*xScale)-pixelsPassed;
						// becomes :
						
						tempLong-=pixelsPassed;
						
						// skip tempLong pixels
						xCounter-=pixelsPassed;
						while (xCounter<tempLong)
						{
							destPtr++;
							xCounter+=xScale;
						}
						xCounter+=pixelsPassed;

						//srcPtr+=tempLong>>kBlastFP;
						pixelsPassed+=tempLong;			
						miscCount-=tempLong;
					}
				}
				
				// clip the right edge NB: Any changes to the clipping calcs here should also be made
				// in the tint code. Don't forget that srcPtr doesn't get adjusted in the tint
				// code, that's the only difference
				// see notes on left clipping if this looks a little mad.
				// was
				// if (ScaledLength((miscCount+pixelsPassed)>>kBlastFP,xScale)>clipRect.right)
				tempLong=clipRect.right*xScale;
				if ((miscCount+pixelsPassed)>tempLong)
				{
					// get how many pixels to skip on the srcBitMap
					tempLong=(miscCount+pixelsPassed)-tempLong;
					// if the number to skip would mean we were still in this draw then do a shorter
					// draw
					if (tempLong<miscCount)
						miscCount-=tempLong;
					else
					{
						// else skip the draw
						xCounter-=pixelsPassed;
						while (xCounter<miscCount)
							xCounter+=xScale;						
						xCounter+=pixelsPassed;
						
						pixelsPassed+=miscCount;
						//srcPtr=endSrcPtr;
						break;
					}
				}
								
				xCounter-=pixelsPassed;
				
				if (tintMap)
				{
					while (xCounter<miscCount)
					{
						*destPtr=tintMap[*((unsigned char *)destPtr)];
						destPtr++;
						xCounter+=xScale;
					}
				}
				else
				{
					while (xCounter<miscCount)
					{
						destPtr++;
						xCounter+=xScale;
					}
				}
				
				xCounter+=pixelsPassed;
				pixelsPassed+=miscCount;
				break;
				
			case rmEndShape:
				done=true;
				break;
			
			default:
				Debugger();
				done=true;
				break;
		}
	}
}

// this proc applies the tint to the image so that from now on, when it's decoded it colours are
// tinted
void ApplyTintToImage(ImageHandle data,TintPtr theTint)
{
	unsigned char 	*srcPtr;	// pointer to where we're reading from in the image data
	unsigned long	opCode;		// the instuction code
	unsigned long	opData;		// any parameters from the code
	unsigned long	miscCount;
	Boolean			done=false;
	
	srcPtr=(unsigned char *)*data;
	srcPtr+=sizeof(ImageData);
	
	while(!done)
	{
		opCode=(*((unsigned long *)srcPtr)) >> 24;
		opData=(*((unsigned long *)srcPtr)) & 0x00ffffff;
		srcPtr+=sizeof(unsigned long);
		
		// act according to the opCode
		switch(opCode)
		{
			case rmDrawRun:
				// tint all these
				miscCount=opData;
				
				while(miscCount>=sizeof(unsigned char))
				{
					*((unsigned char *)srcPtr)=theTint[*((unsigned char *)srcPtr)];
					srcPtr++;
					miscCount-=sizeof(unsigned char);				
				}
				
				// after all the data has been moved, the srcPtr needs adjusting for padding
				srcPtr+=((opData&3L)==0) ? 0 : (4-(opData&3L));
				break;
				
			case rmSkipRun:
				break;
				
			case rmLineStart:
				break;
				
			case rmShadowRun:
				break;
				
			case rmEndShape:
				done=true;
				break;
			
			default:
				Debugger();
				done=true;
				break;
		}
	}
}










