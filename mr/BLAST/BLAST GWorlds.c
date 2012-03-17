/***************************************************************************************************

BLAST GWorlds.c
Extension of BLASTª engine
Mark Tully
9/5/96

This provides procedures for manipulating GWorlds in general

SECTION 1 - Misc

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

#include	<PICTUtils.h>
#include	<string>
#include	"BLAST.h"

// ********* SECTION 1 - Misc *********

// this proc will draw the picture into the world and change the worlds palette to suit
OSErr DrawPicIntoWorld(GWorldPtr drawWorld,PicHandle thePic,short x,short y)
{
	GDHandle	saveDev;
	CGrafPtr	saveWorld;
	Rect		drawRect=(**thePic).picFrame;
	OSErr		err;

	BigEndianRectToNative(drawRect);
	
	GetGWorld(&saveWorld,&saveDev);
	SetGWorld(drawWorld,0L);
	ZeroRectCorner(drawRect);
	FOffset(drawRect,x,y);
	err=SetGWPaletteFromPICT(drawWorld,thePic);
	DrawPicture(thePic,&drawRect);
	SetGWorld(saveWorld,saveDev);

	return err;
}

// Give it a gworld pointer and a PICT and the PICTs palette will be installed into the gworld.
// At the moment, the entire GWorld has to be rerendered after this call
// Poking the gworld CTable isn't good as I think it will leak memory
OSErr SetGWPaletteFromPICT(GWorldPtr theWorld,PicHandle thePic)
{
	#if TARGET_API_MAC_CARBON
	CTabHandle		theTab=FastGetPICTCLUT(thePic,theWorld);
	
	if (theTab)
	{
		ChangeGWorldClut(theWorld,theTab);
		DisposeHandle((Handle)theTab);
	}
	else
		return memFullErr;
	#else
		return -1;	// misc error
	#endif
	
	return noErr;
}

// Opens up a gworld
// custom cluts can be put into the gworld by setting the customClut flag
// if theCLUT is a handle to a CLUT that is used for the gworld, otherwise the clut id is used
OSErr NewNonBlastGWorld(GWorldPtr *theWorld,Rect *theRect,short depth,CTabHandle *theClut,short clutID,Boolean customClut)
{
	QDErr		err;
	CTabHandle	theTab=0L;
	Boolean		useTab=false;
	
	*theWorld=0L;
	
	// Load the clut if necessary
	if (customClut)
	{
		if (theClut==0L)
		{
			theTab=GetCTable(clutID);
			useTab=true;
		}
		else
		{
			if (*theClut==0L)
				*theClut=GetCTable(clutID);
			theTab=*theClut;
		}
	}
	
	if (customClut && (theTab==0L)) // if it's supposed to be a custom clut and it's not loaded
		return resNotFound;
		
	err=NewGWorld(theWorld,depth,theRect,theTab,0L,kNativeEndianPixMap);
	if (err)
		*theWorld=0L;
	
	if (useTab)
		DisposeCTable(theTab);

	if (*theWorld)
		EraseGWorld(*theWorld);
	
	if (!*theWorld && err==noErr)
		err=memFullErr;	// insert you favourite error here...
	
	return err;
}

// changes the clut of a gworld
void ChangeGWorldClut(GWorldPtr theWorld,CTabHandle theClut)
{
	PixMapHandle		portPixMap=GetGWorldPixMap(theWorld);
	short				numToDo,count;
	
/*	HandToHand((Handle*)&theClut);

	DisposeCTable((**portPixMap).pmTable);
	(**portPixMap).pmTable=theClut;*/
	
	numToDo=Lesser((**theClut).ctSize,(**(**portPixMap).pmTable).ctSize);
	
	for (count=0; count<=numToDo; count++)
		(**(**portPixMap).pmTable).ctTable[count].rgb=(**theClut).ctTable[count].rgb;
	CTabChanged((**portPixMap).pmTable);
}

// returns true if a 1 bit pixmap has it's pixel set at the coords given
Boolean PixelMarked1Bit(BCPtr theRec,Point thePoint)
{
	unsigned char	*thisPixel=theRec->baseAddr;
	
	// move down the pixmap
	thisPixel+=theRec->rowBytes*thePoint.v;
	
	// and accross to the right byte (by masking out the lower 3 bits of the coord, thus making
	// thePoint.h a multiple of 8) then diving by 8 (asr 3)
	thisPixel+=(thePoint.h&0xFFF8)>>3;
	
	// now get the bit
	return (*thisPixel)&(1<<(7-(thePoint.h&7)));	//(*thisPixel)|=1<<(7-(gCoords.h%8));
}

void EraseGWorld(GWorldPtr theWorld)
{
	GDHandle	oldDev;
	CGrafPtr	oldPort;
	
	GetGWorld(&oldPort,&oldDev);
	SetGWorld(theWorld,0L);
	BackColor(blackColor);
	
	#if TARGET_API_MAC_CARBON
	Rect	rect;
	GetPortBounds(theWorld,&rect);
	EraseRect(&rect);
	#else
	EraseRect(&theWorld->portRect);
	#endif
	BackColor(whiteColor);
	SetGWorld(oldPort,oldDev);
}

void BlastEraseRect(GWorldPtr theWorld,Rect *eraseMe,Byte colIndex)
{
	PixMapHandle		thePixMap=GetGWorldPixMap(theWorld);
	unsigned char		*pointer,*baseAddr=(unsigned char *)GetPixBaseAddr(thePixMap);
	short				rowBytes=(**thePixMap).rowBytes&0x3FFF;
	short				xCount,yCount;
	long				colIndexLong=colIndex<<24 | colIndex<<16 | colIndex<<8 | colIndex;
	short				colIndexShort=colIndex<<8 | colIndex;
	
	for (yCount=eraseMe->top; yCount<eraseMe->bottom; yCount++)
	{
		xCount=eraseMe->right-eraseMe->left+1;
		
		pointer=baseAddr+yCount*rowBytes+eraseMe->left;
		
		while (xCount>sizeof(long))
		{
			*((long *)pointer)=colIndexLong;
			xCount-=sizeof(long);
			pointer+=sizeof(long);
		}
		
		while (xCount>sizeof(short))
		{
			*((short *)pointer)=colIndexShort;
			xCount-=sizeof(short);
			pointer+=sizeof(short);
		}
		
		while (xCount>sizeof(Byte))
		{
			*((Byte *)pointer)=colIndex;
			xCount-=sizeof(Byte);
			pointer+=sizeof(Byte);
		}
	}
}

// Opens a new gworld with the pic you give it
OSErr NewGWorldWithPic(GWorldPtr *theWorld,short picID,short depth)
{
	PicHandle	thePicture=0L;
	OSErr		err;
	
	*theWorld=0L;
	
	thePicture=GetPicture(picID);
	if (!thePicture)
		return resNotFound; // don't know what the real error is, but it wouldn't load (ResError() doesn't work for GetResource() and friends
	
	err=NewGWorldWithPicHandle(theWorld,thePicture,depth);
		
	ReleaseResource((Handle)thePicture);
	
	return err;
}

OSErr NewGWorldWithPicHandle(GWorldPtr *theWorld,PicHandle thePicture,short depth)
{
	OSErr		err=noErr;
	CTabHandle	picCTab=0L;
	
	*theWorld=0L;
	
	if (!depth) // depth 0, use pictures depth. The only safe way to get the pictures depth is
	{			// to call GetPictInfo() which takes far too long. If you know the depth then pass it
		PictInfo		piccyInfo;
		
		std::memset(&piccyInfo,0,sizeof(piccyInfo));
		
		if (!GetPictInfo(thePicture,&piccyInfo,0,0,systemMethod,0))
		{
			depth=piccyInfo.depth;
			if (!depth)
				depth=32;		// probably a compressed source - just set it to 32 bit
		}
		else
			depth=32;
	}
	
	if (depth<=8 && depth>1)
	{
		if (picCTab=FastGetPICTCLUT(thePicture,0L))
			CTabChanged(picCTab);
	}
	
	if (picCTab || depth==1 || depth>8)	// clut not available for them
	{
		Rect			picRect=(**thePicture).picFrame;

		BigEndianRectToNative(picRect);

		ZeroRectCorner(picRect);
			
		err=(OSErr)NewGWorld(theWorld,depth,&picRect,picCTab,0L,kNativeEndianPixMap);
		if (!err)
		{
			GDHandle	origDev;
			CGrafPtr	origPort;
		
			LockPixels(GetGWorldPixMap(*theWorld));
			
			GetGWorld(&origPort,&origDev);
			SetGWorld(*theWorld,0L);
			
			#if TARGET_API_MAC_CARBON
			Pattern		pattern;
			GetQDGlobalsBlack(&pattern);
			BackPat(&pattern);
			
			Rect	rect;
			GetPortBounds(*theWorld,&rect);
			
			EraseRect(&rect);
			#else
			BackPat(&qd.black);
			EraseRect(&(*theWorld)->portRect);
			#endif
			
			

			DrawPicture(thePicture,&picRect);

			SetGWorld(origPort,origDev);
			
			UnlockPixels(GetGWorldPixMap(*theWorld));
		}
			
		if (picCTab)
			DisposeHandle((Handle)picCTab);
	}
	else
	{
		if (picCTab)
			DisposeHandle((Handle)picCTab);
		return -1;
	}

	return err;
}
