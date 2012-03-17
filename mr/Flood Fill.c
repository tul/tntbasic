// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Flood fill.c
// Mark Tully
// 27/6/96
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

// These routines can be used to generate a flood fill effect which can be used in paint packages
// and things. Give your app about 1.2 Meg of stack (remebering that the stack comes out of the
// apps heap and so you have to increase the minimnum and preferred memory settings).

#include			<QDOffScreen.h>
#include			"Marks Routines.h"

// There are a lot of globals for these routines. This is mainly because I had to avaoid passing
// them on the stack because of overflow. It turned out that the amount of space allocated on
// the stack was a constant 64 bytes with CodeWarrior anyway which means that there was no
// stack space saving.
// However, it will be faster for a tight recursive routine like this if it's variables are
// not having to be constantly moved around and copied into their local equivalents and so they
// have been left as globals.

PixMapHandle		gSourcePixMap;		// The handle to the source pixmap
unsigned char		*gSourceBase;		// base address of the source pixmap
short				gSourceRB;			// number of bytes used per scanline
unsigned char		gMatchIndex;		// index of the colour which we are flooding
unsigned char		gFloodIndex;		// the index of the colour which we are flooding with
Rect				gBoundsRect;		// boundry of the pixmap which is being operated on
Point				gCoords;			// coords of the point being operated on. As a global
										// to reduce stack requirements.

#define				kMaxDepth			16384	// Number of recursions to execute before 
												// stack overflow. Will be OK if 1 meg + stack
#define				kMaxToStash			35000	// the size of the array used to stash coords
												// which couldn't be finished because the stack
												// filled up.

Point				*gStashedCoords=0L;				// array to store the coords of any points
													// which couldn't be finished as the stack
													// was full.
													// It is based on a worst case and so is a
													// little wasteful on RAM but dynamically
													// allocating it could mean that it isn't
													// available or would lead to fragmenting
													// the heap, either way it would bring the
													// app in question to it's knees.

short				gStashedCount=0;	// a count of how many points have been stashed into the
										// gStashedCoords array
										
short				gStackDepth=0;		// the number of recursions of the FillPixel routine.
										// Used to watch for stack overflow.

// Call this at the VERY beginning of your app. It allocates memory which is ABSOLUTELY
// vital to the filling process. If it returns false then do not do any fills.
Boolean InitFill()
{
	gStashedCoords=(Point*)NewPtr(kMaxToStash*sizeof(Point));
	return gStashedCoords!=0L;
}

// releases the memory allocated by the above
KillFill()
{
	DisposePtr((Ptr)gStashedCoords);
	gStashedCoords=0L;
}

// Call this before DoFloodFill. It is used to set up for a FloodFill.
// Source world is a 256 color GWorld on which to flood fill
SetUpFill(GWorldPtr sourceWorld,unsigned char fillCol)
{
	// find out where the pixmap is
	gSourcePixMap=GetGWorldPixMap(sourceWorld);
	
	// lock it
	LockPixels(gSourcePixMap);
	
	// find it's base address
	gSourceBase=(unsigned char *)GetPixBaseAddr(gSourcePixMap);
	
	// Remember how many bytes there are in a row for calculating offsets into the pixmaps
	gSourceRB=(**gSourcePixMap).rowBytes&0x3FFF;
	
	// the size of the pixmap
	gBoundsRect=(**gSourcePixMap).bounds;
	ZeroRectCorner(gBoundsRect); // making sure it's measured from (0,0)
	
	// and finally the flood colour's index
	gFloodIndex=fillCol;
}

// Call this after the above with the coords to start the flood fill.
void DoFloodFill(Point coords)
{
	// Init the fill
	gCoords=coords;
	gStashedCount=0;
	gStackDepth=0;

	// make note of the colour we're filling
	gMatchIndex=PixColour();
	
	// if they're equal then there's nothing to do
	if (gMatchIndex!=gFloodIndex)
	{
		// recursively branch out from the current coords to fill all connected, same coloured
		// pixels.
		FillPixel();
		
		// if there were too many pixels to do in one go, they were stashed for filling now.
		// recursively fill each of them.
		while(gStashedCount)
		{
			gCoords=gStashedCoords[--gStashedCount];
			FillPixel();
		}
	}
}

// This is the recursive routine
// It fills a pixel and the four adjacent pixels if necessary
static void FillPixel(void)
{
	// keep a count of how many recursions have been done so that the stack doesn't overflow
	gStackDepth++;
	
	// if we've exceeded the max depth which is defined to suit the allocated stack size
	if (gStackDepth>=kMaxDepth)
	{
		// stash the coordinate for a later recursion
		// watch for the stash overflow, this should NEVER happen (except in development...)
		if (gStashedCount==kMaxToStash)
		{
			SysBeep(0);
			Debugger();
		}
		else	
			gStashedCoords[gStashedCount++]=gCoords; // stash the coords
		
		// reduce the recursion count before returning
		gStackDepth--;
		return;
	}
	
	// firstly check that the point given lies inside the given pixmap
	if (gCoords.v<gBoundsRect.bottom && gCoords.h<gBoundsRect.right && gCoords.h>-1 && gCoords.v>-1)
	{
		// if so check if the color is correct.
		if (PixColour()==gMatchIndex)
		{
			// the color indexes match, fill it and the four adjacent pixels

			// fill this pixel
			MarkPixel();
			// and the four adjancent one (recursivly)
			gCoords.v++;
			FillPixel();	 // below
			gCoords.v-=2;
			FillPixel(); 	// above
			gCoords.v++;
			gCoords.h++;
			FillPixel(); 	// right
			gCoords.h-=2;
			FillPixel(); 	// left
			gCoords.h++;	// back to the center pixel
		}
	}
	gStackDepth--;
}

// returns the colour index of a pixel on gSourcePixMap
static unsigned char PixColour(void)
{
	return (unsigned char)*(gSourceBase + gCoords.v * gSourceRB + gCoords.h);
}

// marks a pixel on the pixmap as filled by setting it to gFillColour
static void MarkPixel(void)
{
	unsigned char 	*thisPixel=gSourceBase;
	
	// move down the pixmap
	thisPixel+=gSourceRB*gCoords.v;
	
	// for 1 bit pixmaps use
	/*
	// and accross to the right byte (by masking out the lower 3 bits of the coord, thus making
	// coords.h a multiple of 8)
	thisPixel+=(gCoords.h&0xFFF8)/8;
	
	// now set the bit
	(*thisPixel)|=1<<(7-(gCoords.h%8));
	*/
	
	// for 8 bit pixmaps use
	thisPixel+=gCoords.h;
	*thisPixel=gFloodIndex;
}

// call after DoFloodFill returns to clean up
ShutDownFill()
{
	UnlockPixels(gSourcePixMap);
}