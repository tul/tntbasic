// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Picture Utilities.cpp
// Mark Tully
// 21/4/98
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

#include		"Picture Utilities.h"
#include		"MR Debug.h"

// -------------------------------------------------------------------------------------------------------------------------
// ---- QuickDraw override prototypes
// -------------------------------------------------------------------------------------------------------------------------

pascal void GrabDepthProc(const BitMap *srcMap,const Rect *srcRect,const Rect *dstRect,short tMode,RgnHandle maskRgn);
pascal void GrabClutProc(const BitMap *srcMap,const Rect *srcRect,const Rect *dstRect,short tMode, RgnHandle maskRgn);

// -------------------------------------------------------------------------------------------------------------------------
// ---- Util classes used by this file
// -------------------------------------------------------------------------------------------------------------------------

typedef class TPictUtilGlobals
{
	private:

	public:
		static CTabHandle			lastClut;
		static CWindowPtr			tempGrafPort;
		static unsigned short		lastPictureDepth;
		
		static void /*e*/			OpenTempPort();
		static void					CloseTempPort();
		
} TPictUtilGlobals;

CTabHandle							TPictUtilGlobals::lastClut=0L;
unsigned short						TPictUtilGlobals::lastPictureDepth=0;
CWindowPtr							TPictUtilGlobals::tempGrafPort=0L;

// ---- Static /*e*/
// Opens a temp cgrafport and sets the port
void TPictUtilGlobals::OpenTempPort()
{
	if (!TPictUtilGlobals::tempGrafPort)
	{
		Rect		smallRect={-10,-10,-9,-9};
		tempGrafPort=(CWindowPtr)NewCWindow(0L,&smallRect,"\p",false,plainDBox,(WindowPtr)-1L,false,0L);
		if (!tempGrafPort)
			Throw_(memFullErr);
	}
	
	SetPort((GrafPtr)TPictUtilGlobals::tempGrafPort);
}

// ---- Static
void TPictUtilGlobals::CloseTempPort()
{
	if (tempGrafPort)
		DisposeWindow((WindowPtr)tempGrafPort);
	tempGrafPort=0L;
}

typedef class USetTempPort
{
	private:
		Boolean						openedPort;
		GrafPtr						wasPort;
		
	public:
		/*e*/						USetTempPort();
									~USetTempPort();
} USetTempPort;

/*e*/
USetTempPort::USetTempPort()
{
	GetPort(&wasPort);
	if (!TPictUtilGlobals::tempGrafPort)
		openedPort=true;
	else
		openedPort=false;
	TPictUtilGlobals::OpenTempPort();		// Open port and set it
}

USetTempPort::~USetTempPort()
{
	if (openedPort)
		TPictUtilGlobals::CloseTempPort();
	SetPort(wasPort);
}

// -------------------------------------------------------------------------------------------------------------------------
// ---- Public code
// -------------------------------------------------------------------------------------------------------------------------

/*e*/
// This routine gets the ctab for an indexed colour picture
void GetPictureClut(PicHandle picture,CTabHandle &ctab)
{
	ctab=0L;
	
	USetTempPort		temp;
	CQDProcs			procs;
	CQDProcs			*oldProcs;
	
	// Install custom procs
	SetStdCProcs(&procs);									// Get up standard pointers
	procs.bitsProc=NewQDBitsUPP(GrabClutProc);
	ThrowIfMemFull_(procs.bitsProc);

	#if TARGET_API_MAC_CARBON==1
		oldProcs=GetPortGrafProcs(GetWindowPort(TPictUtilGlobals::tempGrafPort));		// save old bottlenecks
		SetPortGrafProcs(GetWindowPort(TPictUtilGlobals::tempGrafPort),&procs);
	#else
		oldProcs=TPictUtilGlobals::tempGrafPort->grafProcs;		// save old bottlenecks
		TPictUtilGlobals::tempGrafPort->grafProcs=&procs;		// install our new bottlenecks
	#endif
	TPictUtilGlobals::lastClut=0L;
	
	#if TARGET_API_MAC_CARBON==1
		Rect		portRect;
		::GetPortBounds(GetWindowPort(TPictUtilGlobals::tempGrafPort),&portRect);
		::DrawPicture(picture,&portRect);
	#else
		::DrawPicture(picture,&TPictUtilGlobals::tempGrafPort->portRect);
	#endif

	#if TARGET_API_MAC_CARBON==1
		SetPortGrafProcs(GetWindowPort(TPictUtilGlobals::tempGrafPort),oldProcs);
	#else
		TPictUtilGlobals::tempGrafPort->grafProcs=oldProcs;	// restore old bottlenecks
	#endif

	DisposeQDBitsUPP(procs.bitsProc);
	
	if (!TPictUtilGlobals::lastClut)
		Throw_(paramErr);									// That pic must have been a little bit whurr
	
	ctab=TPictUtilGlobals::lastClut;
}

/*e*/
// Gets the depth in bit planes of a picture
unsigned char GetPictureDepth(PicHandle picture)
{
	USetTempPort		temp;
	CQDProcs			procs;
	CQDProcs			*oldProcs;
	
	// Install custom procs
	SetStdCProcs(&procs);									// Get up standard pointers
	procs.bitsProc=NewQDBitsUPP(GrabDepthProc);
	ThrowIfMemFull_(procs.bitsProc);

	#if TARGET_API_MAC_CARBON==1
		oldProcs=GetPortGrafProcs(GetWindowPort(TPictUtilGlobals::tempGrafPort));		// save old bottlenecks
		SetPortGrafProcs(GetWindowPort(TPictUtilGlobals::tempGrafPort),&procs);
	#else
		oldProcs=TPictUtilGlobals::tempGrafPort->grafProcs;		// save old bottlenecks
		TPictUtilGlobals::tempGrafPort->grafProcs=&procs;		// install our new bottlenecks
	#endif
	TPictUtilGlobals::lastClut=0L;
	
	#if TARGET_API_MAC_CARBON==1
		Rect		portRect;
		::GetPortBounds(GetWindowPort(TPictUtilGlobals::tempGrafPort),&portRect);
		::DrawPicture(picture,&portRect);
	#else
		::DrawPicture(picture,&TPictUtilGlobals::tempGrafPort->portRect);
	#endif

	#if TARGET_API_MAC_CARBON==1
		SetPortGrafProcs(GetWindowPort(TPictUtilGlobals::tempGrafPort),oldProcs);
	#else
		TPictUtilGlobals::tempGrafPort->grafProcs=oldProcs;	// restore old bottlenecks
	#endif

	DisposeQDBitsUPP(procs.bitsProc);
	
	if (!TPictUtilGlobals::lastPictureDepth)
		Throw_(paramErr);		// That pic must have been a little bit whurr
	
	return TPictUtilGlobals::lastPictureDepth;
}

// -------------------------------------------------------------------------------------------------------------------------
// ---- QD Overrides
// -------------------------------------------------------------------------------------------------------------------------

pascal void GrabDepthProc(const BitMap *srcMap,const Rect *srcRect,const Rect *dstRect,short tMode,RgnHandle maskRgn)
{
	PixMapPtr	thePixMap=(PixMapPtr)srcMap;	// cast as a PixMapPtr

	if (thePixMap->rowBytes & 0x8000)			// verify that this is a color source
		TPictUtilGlobals::lastPictureDepth=thePixMap->pixelSize;
	else
		TPictUtilGlobals::lastPictureDepth=1;
}

pascal void GrabClutProc(const BitMap *srcMap,const Rect *srcRect,const Rect *dstRect,short tMode, RgnHandle maskRgn)
{
	PixMapPtr	thePixMap=(PixMapPtr)srcMap;	// cast as a PixMapPtr

	if (thePixMap->rowBytes & 0x8000)			// verify that this is a color source
	{ 
		TPictUtilGlobals::lastClut=thePixMap->pmTable;		// get the color table handle
		HandToHand((Handle*)&TPictUtilGlobals::lastClut);	// make a copy
	}
}















