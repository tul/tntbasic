/***************************************************************************************************

BLAST Windows.c
Extension of BLASTª engine
© TNT Software 1997
All right reserved
Mark Tully
12/3/96
11/5/96	:	New routine, FastGetPICTCLUT which makes use of the QuickDraw bottleneck bypasses
			instead of using GetPictInfo

This provides functions for manipulating the windows of BLASTª.
NB: Procedures preceeded by BL_ are internal and should not be called from your game.

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

#include	"BLAST.h"
#include	<Palettes.h>
#include	<PICTUtils.h>


// ********* SECTION 1 - Misc *********

// stops the cursor from appearing inside the play window
void HideCursorInPlayWindow()
{
	Point		theOffset={0,0};
	PixMapHandle	pm=GetGWorldPixMap((CWindowPtr)gPlayWindow);

	// find out where our window is in global coords
	LocalToGlobal(&theOffset);
	theOffset.h=(**pm).bounds.left;
	theOffset.v=(**pm).bounds.top;
	
	ShieldCursor(&gPlayWindow->portRect,theOffset);
}

// Opens a window noted to be a non-BLAST type. Use this to open up your play window and then
// set it using SetPlayWindow(windowptr) or gPlayWindow=windowPtr.
// The rect is the dimensions, not the position. Also note that the window is automatucally
// centred on the play device.
// The window is made the current port.
// The update routine is called once on the window and is passed the windowptr.
// Just use DisposeWindow to close a non-BLAST window.
// This routine changes the GDevice to the one the window is going to open on, this is because of
// strangeness when going direct to screen on a second monitor
// If you will be going direct to screen into this window, pass goingDTS as true. You should
// then call EnableDTS is you are using the BLAST Sprite engine.
// If you don't say goingDTS and then go dts, your code will break on multi-monitor systems.
// If you say goingDTS and then at any point use quick draw to draw to your window, QD will
// in short, miss the window and draw elsewhere.
// goingDTS=true means you will only use DTS techniques
// goingDTS=false means you will only use quick draw
WindowPtr NewNonBlastWindow(short width,short height,UpdateProc updateRoutine,Boolean goingDTS) 
{
	WindowPtr	temp;
	Rect		winRect={0,0,0,0};
	GDHandle	savedGD;

	savedGD=GetGDevice();
	if (goingDTS)
		SetGDevice(gPlayDevice);
	
	winRect.bottom=height;
	winRect.right=width;
	
	CenterRectInRect(&winRect,&(**gPlayDevice).gdRect);
	
	temp=NewCWindow(0L,&winRect,"\p",true,plainDBox,(WindowPtr)-1,false,wNonBlastType);
	if (!temp)
		return 0L;
	
	SetPort(temp); // all drawing operations go to this window now	
	BackPat(&qd.black); // using this instead of BackColor is better as it doesn't hose the palette manager's ideal view of the world. I could go on more about this but I'm already way off the screen, so far infact that I bet nobody every reads this far ever again. Well while I'm right out here I may as well say that setting the BackColor to black stops the setting/getting of palettes working proper and also makes CopyBits mess up.
	EraseRect(&temp->portRect);
	
	if (updateRoutine)
		updateRoutine(temp);
	
	SetGDevice(savedGD);
	
	return temp;
}

// Based on TOTMGPG Page 104
// Sets the windows palette to the CLUT specified. The colours are an exact match and are in the
// same order that they are in the CLUT. (ie index them the same way).
void SetWindPaletteFromClut(WindowPtr theWindow,CTabHandle theClut)
{
	PaletteHandle	thePalette=0L;
		
	// Create a palette from the 'clut'
	thePalette=GetPalette(theWindow);
	if (!thePalette) // B and W or using default palette. Create a new palette for it
		thePalette=NewPalette((**theClut).ctSize+1,theClut,pmTolerant+pmExplicit,0x0000); // give it a palette
	else
		CTab2Palette(theClut,thePalette,pmTolerant+pmExplicit,0L); // redo the colours in existing palette

	NSetPalette(theWindow,thePalette,pmAllUpdates);
	ActivatePalette(theWindow);
}

// this proc can be used to dispose of a non blast window. It checks if the window is the play
// window and if so sets gPlayWindow to 0L. It also frees up the palette memory of a window, which
// leaks out if you call set wind palette from clut and then don't explicity dispose of the
// palette.
void DisposeNonBlastWindow(WindowPtr theWindow)
{
	if (theWindow)
	{
		PaletteHandle	thePalette=GetPalette(theWindow);
		
		if (thePalette)
			DisposePalette(thePalette);
		DisposeWindow(theWindow);
		if (theWindow==gPlayWindow)
			gPlayWindow=0L;
	}
}

// this proc is used to turn dts on/off in a window. It basically has to open the window and close
// it again so be wary of failiure
WindowPtr ChangeWindowMode(WindowPtr wasWin,UpdateProc updateRoutine,Boolean dts)
{
	Rect		wasRect=wasWin->portRect;
	
	DisposeWindow(wasWin);
	
	return NewNonBlastWindow(FRectWidth(wasRect),FRectHeight(wasRect),updateRoutine,dts);
}





