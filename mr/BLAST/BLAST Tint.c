/***************************************************************************************************

BLAST Tint.c
Extension of BLASTª engine
Mark Tully
16/4/96
11/5/96	:	Added the fuctionality to "tint" the cluts instead of shifting them. A tint is like
			darkening a colour by 50% whereas a shift is to decrease it's components by fixed
			amounts.
6/11/96	:	Fixed a minor bug in the tinting where if a clut had a bad ct seed then a tint would
			not get done, fixed by changing the ctseed before tint

This provides functions for creating and manipulating 'TINT' resources.

SECTION 1 - Creating shifted TINT Resources
SECTION 2 - Creating tinted TINT resources
SECTION 3 - Colour mapping routines
SECTION 4 - Misc

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

#include	<fp.h>
#include	"BLAST.h"

GWorldPtr	gBL_TintWorld=0L;

// ********* SECTION 1 - Creating shifted TINT Resources *********

// This routine takes a CTab and generates a mapping array which will map one colour onto the best
// tinted colour in the palette. It makes no attempt to change the colours in the palette, they
// are mapped to the closest match.
void MakeTintMapShifted(CTabHandle theClut,TintPtr theTint,long rShift,long gShift,long bShift)
{
	char		origState=HGetState((Handle)theClut);
	RGBColor	theCol;
	short		count,index;
	Boolean		openedWorld=false;
	
	HLock((Handle)theClut);

	CTabChanged(theClut);		// important, otherwise the remapping goes : "fsd;jhgflkasrhgflkdsavbn.asdkjrhvliuabhdv.kjhopsd; jrg;osnalkgvsa;rlfjhlkhbeoirlh"

	if (!gBL_TintWorld)
	{
		OpenTintWorld(theClut);
		openedWorld=true;
	}
	
	if (gBL_TintWorld)
	{
		for(count=0; count<=(**theClut).ctSize; count++)
		{
			theCol=(**theClut).ctTable[count].rgb;
			ShiftColour(&theCol,rShift,gShift,bShift);
			index=RGB2IndexGW(theClut,&theCol);
			if (index!=-1) // if a match could not be found then put the original colour in
				theTint[count]=index;
			else
				theTint[count]=count;
		}
		
		if (openedWorld)
			CloseTintWorld();
	}

	HSetState((Handle)theClut,origState);
}

void ShiftClut(CTabHandle theClut,long rShift,long gShift,long bShift)
{
	short		count;
	
	for (count=0; count<=(**theClut).ctSize; count++)
		ShiftColour(&(**theClut).ctTable[count].rgb,rShift,gShift,bShift);

	// Tell QD the CLUT has changed
	CTabChanged(theClut);
}

void ShiftColour(RGBColor *theCol,long rShift,long gShift,long bShift)
{
	// Alter the red component
	if((theCol->red+rShift)>65535)
		theCol->red=65535;
	else if((theCol->red+rShift)<0)
		theCol->red=0;
	else
		theCol->red+=rShift;
	
	// Alter the green component	
	if((theCol->green+gShift)>65535)
		theCol->green=65535;
	else if((theCol->green+gShift)<0)
		theCol->green=0;
	else
		theCol->green+=gShift;
		
	// Alter the blue component	
	if((theCol->blue+bShift)>65535)
		theCol->blue=65535;
	else if((theCol->blue+bShift)<0)
		theCol->blue=0;
	else
		theCol->blue+=bShift;
}

// ********* SECTION 2 - Creating tinted TINT resources *********

// same as shifted one except that the colours are tinted. ie instead of minusing 5000 of each 
// colour, it would dim each one say 50%. This way all colours which are darker than grey don't 
// turn to black.
// 0% means no shift.
void MakeTintMapTinted(CTabHandle theClut,TintPtr theTint,long rPerCent,long gPerCent,long bPerCent)
{
	RGBColour			theCol;
	unsigned short		count;
	
	for(count=0; count<=(**theClut).ctSize; count++)
	{
		theCol=(**theClut).ctTable[count].rgb;
		TintColour(&theCol,rPerCent,gPerCent,bPerCent);
		theTint[count]=BlastRGB2Index(theClut,&theCol);
	}
}

// this version of tint colour scales down any colours which get to large in magnitude. Im not
// sure which out of this one and the one below is correct. I think it's the clipping one as
// this one would stop the colours being brightened any more when a component gets to max
/*
TintColour(RGBColour *theCol,signed long rPercent,signed long gPercent,signed long bPercent)
{
	double		r,g,b;
	double		smallest,largest,outBy;
	Boolean		doScale=true;
	
	// work out the 3 new colours as doubles
	r=theCol->red;
	r*=((double)rPercent)/100.00;
	r+=theCol->red;

	g=theCol->green;
	g*=((double)gPercent)/100.00;
	g+=theCol->green;
	
	b=theCol->blue;
	b*=((double)bPercent)/100.00;
	b+=theCol->blue;

	// before these were then clipped, now I scale all 3 to fit within the limits of 0-65535
	smallest=Lesser(Lesser(r,g),b);
	smallest=0;
	largest=Greater(Greater(r,g),b);
	
	// see how far they are out
	if (smallest<0 && largest>65535)
	{
		// out on both sides, see which side is furthest out
		outBy=Greater(65535-smallest,largest);
	}
	else if (largest>65535)
		outBy=largest;
	else if (smallest<0)
		outBy=65535-smallest;
	else
	{
		// none of them are out
		doScale=false;
	}

	if (doScale)
	{
		// they are too large in magnitude. The biggest magnitude is outBy (>65535)
		double scalar;
	
		// scale each component down
		scalar=65535.00/outBy;
		
		r*=scalar;
		g*=scalar;
		b*=scalar;
	}
	
	// stick the values in clipping anyway incase rounding errors put them out by 1 or 2
	theCol->red=(short)Limit(r,0,65535);
	theCol->green=(short)Limit(g,0,65535);
	theCol->blue=(short)Limit(b,0,65535);
}*/

// this version of tint colour clips the colours between 0 and 65535, so it has the habit of whiting
// out colours when you brighten them. The new version above detects when a component of the colour
// is too white and scales all components down.
// However, I think this is the better version as the one above stops the brightening when a component
// hits it's max
void TintColour(RGBColor *theCol,long rPercent,long gPercent,long bPercent)
{
	double		temp;
	
	temp=theCol->red;
	temp*=((double)rPercent)/100.00;
	temp+=theCol->red;
	theCol->red=(short)Limit(temp,0,65535);

	temp=theCol->green;
	temp*=((double)gPercent)/100.00;
	temp+=theCol->green;
	theCol->green=(short)Limit(temp,0,65535);
	
	temp=theCol->blue;
	temp*=((double)bPercent)/100.00;
	temp+=theCol->blue;
	theCol->blue=(short)Limit(temp,0,65535);
}

void TintClut(CTabHandle theClut,long rShift,long gShift,long bShift)
{
	short		count;
	
	for (count=0; count<=(**theClut).ctSize; count++)
		TintColour(&(**theClut).ctTable[count].rgb,rShift,gShift,bShift);

	// Tell QD the CLUT has changed
	CTabChanged(theClut);
}

// opacity of the overlay : 1.00 is completely opaque, 0.00 is transparent
// between 0 and 1 inclusive
void ApplyOpacityColour(RGBColour *sourceColour,RGBColour *opacityOverlay,double opacity)
{
	sourceColour->red=(short)((double)sourceColour->red*(1.00-opacity)+(double)opacityOverlay->red*opacity);
	sourceColour->green=(short)((double)sourceColour->green*(1.00-opacity)+(double)opacityOverlay->green*opacity);
	sourceColour->blue=(short)((double)sourceColour->blue*(1.00-opacity)+(double)opacityOverlay->blue*opacity);
}

void ApplyOpacityClut(CTabHandle theClut,RGBColour *opacityOverlay,double opacity)
{
	short		count;
	
	for (count=0; count<=(**theClut).ctSize; count++)
		ApplyOpacityColour(&AccessColour(theClut,count),opacityOverlay,opacity);

	// Tell QD the CLUT has changed
	CTabChanged(theClut);
}

// opacity is between 0 and 1
void MakeOpacityMap(CTabHandle theClut,TintPtr theTint,RGBColour *opacityCol,double opacity)
{
	RGBColour			theCol;
	unsigned short		count;
		
	for (count=0; count<=(**theClut).ctSize; count++)
	{
		theCol=AccessColour(theClut,count);
		ApplyOpacityColour(&theCol,opacityCol,opacity);
		theTint[count]=BlastRGB2Index(theClut,&theCol);
	}
}


// ********* SECTION 3 - Colour mapping routines *********

// Searches through the passed clut to find the closest match for a given colour
// it returns the index. This proc is pretty fast and could be used during the game loop without too much
// of a problem. It may be possible to speed it up further.
// with the colour components being in unsigned shorts, they have a range of up to 65535.
// The largest distance being the 2 colours in "colour space" would be 65535*65535*3;
// This overflows a double it seems.
// So by shifting each component down and disregarding the least significant bit you get up to 32767 which
// produces numbers which fits in an unsigned long.
unsigned char BlastRGB2Index(CTabHandle theTable,RGBColour *theCol)
{
	unsigned char		bestMatch=0;
	unsigned long		closestDist=0xFFFFFFFF,thisDist,temp;
	unsigned short		counter;

	for (counter=0; counter<=(**theTable).ctSize; counter++)
	{
		// find how close this colour is
		temp=(AccessColour(theTable,counter).red-theCol->red)>>1;
		temp*=temp;
		thisDist=temp;
		
		temp=(AccessColour(theTable,counter).green-theCol->green)>>1;
		temp*=temp;
		thisDist+=temp;
		
		temp=(AccessColour(theTable,counter).blue-theCol->blue)>>1;
		temp*=temp;
		thisDist+=temp;
		
		// see if it's closer than the current best
		if (thisDist<closestDist)
		{
			closestDist=thisDist;
			bestMatch=counter;
		}
	}
	
	return bestMatch;
}

// Same as above except that it quickly uses a gworld to allow the systems excellent routines to do
// the business instead.
short RGB2IndexGW(CTabHandle theTable,RGBColor *theCol)
{
	Boolean		openWorld=false;
	short		index=-1;
	
	if (gBL_TintWorld==0L)
	{
		openWorld=true;
		OpenTintWorld(theTable);
	}
	
	if (gBL_TintWorld)
	{
		CGrafPtr	origPort;
		GDHandle	origGD;
		
		GetGWorld(&origPort,&origGD);
		SetGWorld(gBL_TintWorld,0L);
		SetCPixel(0,0,theCol);
		index=GPixelColour(GetGWorldPixMap(gBL_TintWorld),0,0);
		SetGWorld(origPort,origGD);
	}
	else
		return -1;

	if (openWorld)
		CloseTintWorld();

	return index;
}

// ********* SECTION 4 - Misc *********

QDErr OpenTintWorld(CTabHandle theTable)
{
	Rect	rect={0,0,1,1};
	QDErr	theErr=NewGWorld(&gBL_TintWorld,8,&rect,theTable,0L,0);
	
	return theErr;
}

void CloseTintWorld()
{
	if(gBL_TintWorld)
	{
		DisposeGWorld(gBL_TintWorld);
		gBL_TintWorld=0L;
	}
}
