// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// HideMenuBar.c
// Mark Tully
// Ages ago
// 8/9/96 : Fumigated
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

/*
	Are you having problems with menu bar hiding?
	Is it something like, you hide the menu bar, but you can't draw over it?
	Or you show the menu bar, but you draw over it instead of clipping?
	The window's vis rgn which sez where you can draw is hosed. After calling show/hide menu you
	should call CalcVis(theWindow). It's a toolbox call.

	This shouldn't happen anymore as the visrgns are rebuilt
*/

#include	"BLAST.h"

short		gOldMBarHeight;
RgnHandle	gOldGrayRgn=0L;
Boolean		gHidden=false;

// Stop a conflict with Uni Headers 3.2
#ifndef __MENUS__
void HideMenuBar()
{
	PowerHideMenuBar(true);
}
#endif

void PowerHideMenuBar(Boolean callPaintOne)
{
	if (!gHidden)
	{
		RgnHandle		mainScreenRgn;				// the region of the menu bar
	
		// record the menu bar height
		gOldMBarHeight=GetMBarHeight();

		// set the menu bar to no height
		LMSetMBarHeight(0);
		
		// save the original gray regions (so we can restore it later)
		gOldGrayRgn=NewRgn();
		CopyRgn(GetGrayRgn(),gOldGrayRgn);
	
		// make sure that the entire main screen is ours to play with (no menu bars or corners)
		mainScreenRgn=NewRgn();
		RectRgn(mainScreenRgn,&(**GetMainDevice()).gdRect);
		UnionRgn(mainScreenRgn,GetGrayRgn(),GetGrayRgn());
		DisposeRgn(mainScreenRgn);
	
		RecalcVisRgns();
	
		// draw the desktop
		if (callPaintOne)
			PaintOne(0L,GetGrayRgn());
		gHidden=true;
	}
}

// Stop a conflict with Uni Headers 3.2
#ifndef __MENUS__

// The only way to leave the little rounded corners black is to erase the window which is filling
// them with black before closing it.
void ShowMenuBar()
{
	if (gHidden)
	{
		// set the menu bar to its normal height
		LMSetMBarHeight(gOldMBarHeight);
		
		// restore the original gray region
		CopyRgn(gOldGrayRgn,GetGrayRgn());
		DisposeRgn(gOldGrayRgn);
		gOldGrayRgn=0L;
		
		// draw the menu bar (but not the corners)
		DrawMenuBar();
		gHidden=false;
		RecalcVisRgns();
	}
}

#endif
