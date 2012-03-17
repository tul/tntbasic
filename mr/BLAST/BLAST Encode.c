/***************************************************************************************************

BLAST Encode.c
Extension of BLASTª engine
Mark Tully
7/4/96
23/7/96		:	Altered the encoder so that when shrinking to the bounding rect of a sprite it
				did so a LOT faster. Did this by using a macro instead of GPixelColour.

This provides functions for encoding graphics into BLAST's 'IMAG' sprite format. They are only
necessary for encoding of the data and so this file doesn't need including in you game.

SECTION 1 - Encoding PICT Resources

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

// ********* SECTION 1 - Encoding PICT Resources *********

// A handy routine which will return the colour of a pixel in a pixmap
// It's not effiecient enough to be called a lot as it works out some values every time even
// though they don't change.
short GPixelColour(PixMapHandle thePixMap,short x,short y)
{
	Ptr		pixBase;
	short	rowBytes,colour;

	LockPixels(thePixMap);
	
	pixBase=GetPixBaseAddr(thePixMap);
	rowBytes=(**thePixMap).rowBytes & 0x3fff;
	
	colour=(unsigned char)*(pixBase + y * rowBytes + x);
	
	UnlockPixels(thePixMap);
	
	return colour;
}

short FindGWUnusedCol(GWorldPtr theWorld)
{
	Ptr				pixBase;
	short			rowBytes,colour,colCount[255],xCount,yCount;
	short			width,height;
	PixMapHandle	thePixMap=GetGWorldPixMap(theWorld);

	LockPixels(thePixMap);
	
	pixBase=GetPixBaseAddr(thePixMap);
	rowBytes=(**thePixMap).rowBytes & 0x3fff;
	width=(theWorld->portRect.right)-(theWorld->portRect.left);
	height=(theWorld->portRect.bottom)-(theWorld->portRect.top);
		
	// clear all the counts
	for(xCount=0; xCount<=255; xCount++)
		colCount[xCount]=0;
	
	for (yCount=0; yCount<height; yCount++)
	{
		for(xCount=0; xCount<width; xCount++)
		{
			colour=(unsigned char)*(pixBase + yCount * rowBytes + xCount);
			colCount[colour]++;
		}
	}
	
	UnlockPixels(thePixMap);

	// scan through for a 0 use col
	for(xCount=0; xCount<=255; xCount++)
	{
		if (colCount[xCount]==0)
			return xCount;
	}

	return -1; // not found
}


#define		BL_PixCol(x,y)		(*(baseAddr + rowBytes*(y) + (x)))

// The mother of all Encoders, the TNT Hieroglpyh Encode routine
// clearCol and shadowCol are the indexs of the colours to use for shadows and transparencies.
// A value of -1 means none.

Boolean EckerEncodeTheRect(EncodePtr theEncode)
{
	unsigned char 		*srcPtr;		// position in the pixmap
	unsigned char		*destPtr;		// position in the shape data
	unsigned char		*baseAddr;		// start of pixMap
	unsigned char		*rowStart;		// start of current scanline
	unsigned char		*lineStartPtr;	// address of the line start token for this line
	unsigned char		*runTokenPtr;	// where is the token for the current run
	
	short				runCounter;		// the number of pixels in the current run
	short				rowBytes;		// how many bytes in a scanline
	short				width,height;
	short				xCount,yCount;
	short				runMode;		// the mode that the encoder is in
	Boolean				runSwitch=false;	// a flag saying whether the encoder should switch modes
	Rect				shrunkRect=*theEncode->theRect;
	short				clearCol=theEncode->clearCol;		// the transparent colour
	short				shadowCol=theEncode->shadowCol;		// the shadow colour
	
	// get pointer to source
	baseAddr=theEncode->theRec->baseAddr;
	rowBytes=theEncode->theRec->rowBytes;
	
	if (theEncode->shrinkRect)
	{
		// shrink the rectangle to the smallest size possible
		// move top down
		short		counter,col;
		Boolean		exit=false;
		
		while (shrunkRect.top<shrunkRect.bottom && !exit)
		{
			for (counter=shrunkRect.left; counter<shrunkRect.right; counter++)
			{
				col=BL_PixCol(counter,shrunkRect.top);
				if (col!=clearCol) // hit the data, stop shrinking
				{
					exit=true;
					break;
				}
			}
			if (!exit) // shrink it
				shrunkRect.top++;
		}
		
		// move bottom up
		exit=false;
		while (shrunkRect.bottom>shrunkRect.top && !exit)
		{
			for (counter=shrunkRect.left; counter<shrunkRect.right; counter++)
			{
				col=BL_PixCol(counter,shrunkRect.bottom-1);
				if (col!=clearCol) // hit the data, stop shrinking
				{
					exit=true;
					break;
				}
			}
			if (!exit) // shrink it
				shrunkRect.bottom--;
		}
		
		// move left right
		exit=false;
		while (shrunkRect.left<shrunkRect.right && !exit)
		{
			for (counter=shrunkRect.top; counter<shrunkRect.bottom; counter++)
			{
				col=BL_PixCol(shrunkRect.left,counter);
				if (col!=clearCol) // hit the data, stop shrinking
				{
					exit=true;
					break;
				}
			}
			if (!exit) // shrink it
				shrunkRect.left++;
		}
		
		// more right left
		exit=false;
		while (shrunkRect.right>shrunkRect.left && !exit)
		{
			for (counter=shrunkRect.top; counter<shrunkRect.bottom; counter++)
			{
				col=BL_PixCol(shrunkRect.right-1,counter);
				if (col!=clearCol) // hit the data, stop shrinking
				{
					exit=true;
					break;
				}
			}
			if (!exit) // shrink it
				shrunkRect.right--;
		}
	}
	
	width=shrunkRect.right-shrunkRect.left;
	height=shrunkRect.bottom-shrunkRect.top;
	
	if (theEncode->theImage==0L)
	{
		theEncode->theImage=(ImageHandle)NewHandle(WorstCaseEncode(&shrunkRect));
		if (!theEncode->theImage)
			return false; // not enough RAM
	}
	
	// get pointer to dest
	destPtr=(unsigned char *)*theEncode->theImage; // pointer to destination buffer
	destPtr+=sizeof(ImageData); // skip past the header
	
	// Fill out the header
	// alter the x and y off sets so that the changes in shrinking the rect 
	// have no effect on any position strategies of the user
	(**theEncode->theImage).xoff=theEncode->theRect->left-shrunkRect.left;
	(**theEncode->theImage).yoff=theEncode->theRect->top-shrunkRect.top;
	(**theEncode->theImage).width=width;
	(**theEncode->theImage).height=height;
	
	// work out where we start from
	rowStart=baseAddr + rowBytes * shrunkRect.top + shrunkRect.left;
	
	// scan the shape
	for (yCount=0; yCount<height; yCount++)
	{
		// for each row do...
		lineStartPtr=destPtr;			// make a note of the place to place the line start token
		destPtr+=sizeof(unsigned long); // increase to point at next free slot (where the next token goes)
		
		// at the beginning of each row we are not in any run
		runMode=rmNoRun;
		runCounter=0;
		
		// move to the start of the row
		srcPtr=rowStart;
		
		// for each pixel of the row
		for (xCount=0; xCount<width; xCount++)
		{
			do		// this do while should only ever loop around once per pixel and only if a runswitch occurs
			{
				runSwitch=false;		// don't switch modes (therefore don't loop around)
				
				switch (runMode)
				{
					case rmSkipRun:		// we are currently skipping pixels, if this one is clear cont
						if (*srcPtr==clearCol)
							runCounter++;
						else
							runSwitch=true;	// if not it's time to switch run mode
						break;
					
					case rmDrawRun:		// we are currently drawing pixels, if this is not a shadow or clear then store it
						if ((*srcPtr!=clearCol) && (*srcPtr!=shadowCol))
						{
							*destPtr=*srcPtr;	// store the data
							destPtr++;
							runCounter++;
						}
						else
							runSwitch=true;
						break;
						
					case rmShadowRun:	// in a shadow run, if this is a shadow pixel continue
						if (*srcPtr==shadowCol)
							runCounter++;
						else
							runSwitch=true;
						break;
					
					case rmNoRun:		// if we're not in a run then switch into one
						runSwitch=true;
						break;
				}
				
				// at this stage, the pixel will have been processed unless the runMode was wrong for
				// the pixel. If it was wrong, the runSwitch flag will have been set. If it is set
				// then we switch modes here and loop back and do the above switch statement again.
				if (runSwitch)
				{
					// Time to switch runModes. First thing is to finish the current run if we were
					// actually in one.
					if (runMode!=rmNoRun)
					{
						*((unsigned long *)runTokenPtr)=(runMode<<24)+runCounter; // store the opCode
						if (runMode==rmDrawRun) // if we've been writing pixels then make sure it's all aligned
						{
							*((unsigned long *)destPtr)=0L; // clear the next 4 bytes
							destPtr += ((runCounter & 3L) == 0) ? 0 : (4-(runCounter & 3L)); // pad the data from the last run out so that it finishes on a 4 byte boundry
						}
					}
					
					// Reset for the next run
					runCounter=0; // reset for the next run
					runTokenPtr=destPtr;
					destPtr+=sizeof(unsigned long); // remember where the runtoken is to go
					
					if (*srcPtr==clearCol)		// switch the mode
						runMode=rmSkipRun;
					else if (*srcPtr==shadowCol)
						runMode=rmShadowRun;
					else
						runMode=rmDrawRun;
				}
				
			} while (runSwitch); // loop around again if there was a runMode switch
		
			srcPtr++; // next pixel across
		}

		// That is that row finished
		// We must finish any run that we are in as the run cannot continue across rows
		// it does not need to be finished if it is a skip pixel run as it is not necessary to
		// save them over rows (NB: Should never get to here no rmNoRun)
		if (runMode!=rmSkipRun)
		{
			*((unsigned long *)runTokenPtr)=(runMode<<24)+runCounter; // store the opCode
			if (runMode==rmDrawRun) // if data was being written then align it
			{
				*((unsigned long *)destPtr)=0L; // clear the next 4 bytes
				destPtr += ((runCounter & 3L) == 0) ? 0 : (4-(runCounter & 3L)); // pad the data from the last run out so that it finishes on a 4 byte boundry
			}
		}
		else
		{
			// as we don't need to write rmSkipRun tokens, we can move dest pointer back from
			// pointing at the slot after where the skip opcode was going to go to where the
			// skip op code was going to go (which is still empty).
			destPtr-=sizeof(unsigned long);
		}
			
		// Create line start token and increase row start
		*((unsigned long *)lineStartPtr)=(rmLineStart<<24)+(destPtr-(lineStartPtr+4)); // the data in the linestart op is how long this line is
		rowStart+=rowBytes; // start next row
	}
	
	// Finshed the shape
	// Create the end shape token
	*((unsigned long *)destPtr)= rmEndShape << 24;
	destPtr+=sizeof(unsigned long);

	if (theEncode->shrinkHandle)
	{
		// Resize the handle from the worst case size to the real size
		SetHandleSize((Handle)theEncode->theImage,destPtr-(unsigned char *)(*theEncode->theImage));
	}
	
	return true;
}

// left here for compatability
ImageHandle EncodeTheRect(PixMapHandle thePixMap,Rect *theRect,short clearCol,short shadowCol)
{
	EncodeRec		theEncode=emptyEncodeRec;
	BCRec			theRec;
	
	LockPixels(thePixMap);
	theEncode.theRec=&theRec;

	theRec.world=0L;
	theRec.pixMap=thePixMap;
	theRec.baseAddr=(unsigned char *)GetPixBaseAddr(thePixMap);
	theRec.rowBytes=(**thePixMap).rowBytes&0x3FFF;

	theEncode.theImage=0L;
	theEncode.theRect=theRect;
	theEncode.clearCol=clearCol;
	theEncode.shadowCol=shadowCol;
	theEncode.shrinkHandle=true;
	theEncode.shrinkRect=true;

	EckerEncodeTheRect(&theEncode);
	
	UnlockPixels(thePixMap);
	
	return theEncode.theImage;
}

Size WorstCaseEncode(Rect *theRect)
{
	short			width=theRect->right-theRect->left;
	short			height=theRect->bottom-theRect->top;
	
	// At the worst encode, the maximum size of the image data would be
	// 8 bytes per pixel (4 byte opCode and 4 byte data) + 4 bytes per row (end line opCode)
	// + 4 bytes per shape (end shape opCode) + header of ImageData
	// 8  * shapewidth * shapeHeight + 4 * shapeHeight + 4 + sizeof(ImageData)
	// sizeof(ImageData) is the size of the IMAG header

	return 8*width*height+4*height+4+sizeof(ImageData);
}


