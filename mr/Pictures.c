// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Pictures.c
// Mark Tully
// 9/7/95
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1995, Mark Tully and John Treece-Birch
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

#include	<PictUtils.h>
#include	<Palettes.h>
#include	"Marks Routines.h"

// Globals
CTabHandle		gTheColours=0L;

// This routine will take a rect and a pichandle and will adjust the rect so the
// picture will fit in it without being distorted. It will also centre the pict in
// the rect. It's basically the forerunner to scale rect in rects.c
void ScalePic(PicHandle thePic,Rect *theRect,Boolean goBigger)
{	
	short	width,height,biggest,rectDim;
	float	scaleFactor;
	Rect	frame=(**thePic).picFrame;
	Boolean	wid=false;	
	
	BigEndianRectToNative(frame);
	
	// get picture width
	width=frame.right-frame.left;
	height=frame.bottom-frame.top;
	
	if (width>height)
	{
		biggest=width;
		wid=true; // Can't be bothered with full names you see...
	}
	else
		biggest=height; // Don't need to se the dim bit as it default's to height
		
	// Biggest now contains the larger dimension, either width or height and dim sez
	// which one
	// Now get the same dimension from the rect
	if (wid) // find the width of the rect
		rectDim=(*theRect).right - (*theRect).left;
	else // find height of the rect
		rectDim=(*theRect).bottom - (*theRect).top;
	
	// So now we know what the biggest dimension of the picture has to fit into
	// Find an appropiate scale factor
	scaleFactor=(float)biggest/(float)rectDim;
	if ((!goBigger) && (scaleFactor<1)) // If we didn't do this the pics would be blown up if too small
		scaleFactor=1;
	
	// Now scale the picture down using that factor
	width/= scaleFactor;
	height/= scaleFactor;

	// Verify that the new dimensions do fit inside the rect (which they may not due to
	// rounding errors)
	if ( width > ((*theRect).right - (*theRect).left))
		width=(*theRect).right - (*theRect).left;
		
	if ( height > ((*theRect).bottom - (*theRect).top))
		height=(*theRect).bottom - (*theRect).top;
	
	// Now the picture will fit in the rect without distortion, center it in it
	// This finds the proper left edge inside the rect and then the right edge
	(*theRect).left+=((*theRect).right - (*theRect).left)/2 - width/2; 
	(*theRect).right=(*theRect).left+width;

	// Finds top edge & bottom edge
	(*theRect).top+=((*theRect).bottom - (*theRect).top)/2 - height/2; 
	(*theRect).bottom=(*theRect).top+height;
}

Boolean DrawScaledPic(short pic, Rect *theRect)
{
	PicHandle	thePic;
		
	// Firstly, see if we can get the pic, if not then it's not there so exit
	thePic=GetPicture(pic);
	if (!thePic) 
		return false;
	
	if (!theRect)
	{
		*theRect=(**thePic).picFrame;
		BigEndianRectToNative(*theRect);
	}
	
	EraseRect(theRect);
	
	ScalePic(thePic,theRect,false);
	DrawPicture(thePic,theRect);
	ReleaseResource((Handle)thePic); // Can get rid of that picture now it's on the screen
	return true;
}

PicHandle PixMapToPict(PixMapHandle inMap,Rect *sourceRect,Rect *destRect)
{
	PicHandle		recordPic=0L;
			
	recordPic=OpenPicture(destRect);
	if (!recordPic)
		return 0L;
	CopyBits((BitMap *)*inMap,(BitMap *)*inMap,sourceRect,destRect,srcCopy,0L);
	ClosePicture();
	
	return recordPic;
}

OSErr GetPICTCLUT(PicHandle thePic,CTabHandle *theTab)
{
	PictInfo		piccyInfo;
	OSErr			err;
	
	*theTab=0L;
	err=GetPictInfo(thePic,&piccyInfo,returnColorTable,256,systemMethod,0);
	if (err)
		return err;
	*theTab=piccyInfo.theColorTable;
	return noErr;
}

CTabHandle FastGetPICTCLUT(PicHandle thePic,CGrafPtr thePort)
{
	CQDProcs	newProcs, *oldProcs;
	QDBitsUPP	bitsUPP=0L;
	GrafPtr		savePort;
	Rect		destRect={0,0,1,1}; // doesn't matter as it's not being drawn anyway
	WindowPtr	tempWindow=0L;

	// Init
	gTheColours=0L;
	// Open up a quick grafport if we didn't get one
	if (!thePort)
	{
		Rect	rect={0,0,1,1};
		
		tempWindow=NewCWindow(0L,&rect,"\p",false,plainDBox,0L,false,0L);
		if (!tempWindow)
			return 0L;
		thePort=::GetWindowPort(tempWindow);
	}
	
	// Switch ports
	GetPort(&savePort);
	#if TARGET_API_MAC_CARBON==1	
		SetPort(thePort);		// compiles in carbon <- 	// 	SetPort((GrafPort*)thePort);
	#else
		SetPort((GrafPort*)thePort);
	#endif
	
	SetStdCProcs(&newProcs);					// set up standard pointers
	bitsUPP=NewQDBitsUPP(BL_CustomStdBitsProc);
	newProcs.bitsProc = bitsUPP;				// modify them slightly

	#if TARGET_API_MAC_CARBON==1
		oldProcs=::GetPortGrafProcs(thePort);
		::SetPortGrafProcs(thePort, &newProcs);
	#else
		oldProcs = thePort->grafProcs;				// save old bottlenecks
		thePort->grafProcs = &newProcs;				// install our new bottlenecks
	#endif
	
	DrawPicture(thePic, &destRect);				// this does all the magic

	#if TARGET_API_MAC_CARBON==1
		::SetPortGrafProcs(thePort,oldProcs);
	#else	
		thePort->grafProcs = oldProcs;				// restore old bottlenecks
	#endif
	SetPort(savePort);							// and the port
	::DisposeQDBitsUPP(bitsUPP);
	
	if (tempWindow)
		::DisposeWindow(tempWindow);
	
	return gTheColours;							// return the global result
}

pascal void BL_CustomStdBitsProc(const BitMap *srcMap,const Rect *srcRect,const Rect *dstRect,
																short tMode, RgnHandle maskRgn)
{
	PixMapPtr	thePixMap=(PixMapPtr)srcMap;	// cast as a PixMapPtr

	if (thePixMap->rowBytes & 0x8000)			// verify that this is a color source
	{ 
		gTheColours = thePixMap->pmTable;		// get the color table handle
		if (gTheColours)
			HandToHand((Handle *)&gTheColours); 	// make a copy
	}
}


short SavePict(PicHandle thePic,short resID,StringPtr name,Boolean overWrite)
{
	Handle		temp;
	short		err;
	
	temp=Get1Resource('PICT',resID);
	if (temp)
	{
		if (overWrite)
		{
			RemoveResource(temp);
			err=ResError();
			if (err)
				return err;
			DisposeHandle(temp);
		}
		else
			return addResFailed;
	}
	
	AddResource((Handle)thePic,'PICT',resID,name); // after giving a handle to Addresource, does it
													// become the property of the resource manager?
													// if so, do not dispose of it (which I what I
													// am presuming). It is isn't then it should
													// be disposed of
	err=ResError();
	if (err)
		return err;
		
	WriteResource((Handle)thePic);
	return ResError();
}