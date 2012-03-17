// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Zooms.c
// Mark Tully
// ??/??/96
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

#include	"Marks Routines.h"
#include	"FixMath.h"

#define		ZOOMSTEPS	16
#define		ONE 		65536L

Fixed	fract;
Boolean	gZoomOn=true;

// Now we use the drag manager for zoom effects
#ifndef USE_DRAG_MANAGER_FOR_ZOOMS
	#define	USE_DRAG_MANAGER_FOR_ZOOMS	1
#endif

void SetZoom(Boolean z)
{
	gZoomOn=z;
}

// Give it two global rects
// rects can be made global by the routine LToG()

void ZoomRect (Rect *smallrect, Rect *bigrect, Boolean zoomup) 
{
	#if USE_DRAG_MANAGER_FOR_ZOOMS
		if (gZoomOn)
		{
			if (zoomup)
				ZoomRects(smallrect,bigrect,10,kZoomNoAcceleration);
			else
				ZoomRects(bigrect,smallrect,10,kZoomNoAcceleration);
		}
	#else
		Fixed		factor;
		Rect		rect1, rect2, rect3, rect4;
		GrafPtr		savePort, deskPort;
		int			i;
		long        tm;

		if (gZoomOn)
		{
			GetPort (&savePort);
			OpenPort (deskPort = (GrafPtr) NewPtr (sizeof (GrafPort)));
			InitPort (deskPort);
			SetPort (deskPort);
			PenPat (&qd.gray);		//¥ Original, comment for black zoom.
		//	PenPat (&qd.black);		//¥ Uncomment for black zoom.
			PenMode (notPatXor);	//¥ Original, comment for black zoom.
		//	PenMode (patXor);		//¥ Uncomment for black zoom.
			if (zoomup) 
			{
				rect1 = *smallrect;
				factor = FixRatio (6, 5);
				fract = FixRatio (541, 10000);
			}
			else
			{
				rect1 = *bigrect;
				factor = FixRatio (5, 6);
				fract = ONE;
			}
			rect2 = rect1;
			rect3 = rect1;
			FrameRect (&rect1);
			for (i = 1; i <= ZOOMSTEPS; i++) 
			{
				rect4.left = Blend (smallrect->left, bigrect->left);
				rect4.right = Blend (smallrect->right, bigrect->right);
				rect4.top = Blend (smallrect->top, bigrect->top);
				rect4.bottom = Blend (smallrect->bottom, bigrect->bottom);
				FrameRect (&rect4);
				FrameRect (&rect1);
				rect1 = rect2;
				rect2 = rect3;
				rect3 = rect4;
				fract = FixMul (fract, factor);
				tm = TickCount (); // These two lines are a crude waitvbl
				while (tm == TickCount ());
			}
			FrameRect (&rect1);
			FrameRect (&rect2);
			FrameRect (&rect3);
			ClosePort (deskPort);
			DisposePtr ((Ptr) deskPort);
			PenNormal ();
			SetPort (savePort);
		}
	#endif
}

void LToG (Rect *r) 
{
	Point	p1, p2;

	p1 = topLeft (*r);
	p2 = botRight (*r);
	LocalToGlobal (&p1);
	LocalToGlobal (&p2);
	Pt2Rect (p1, p2, r);
}

int Blend (int i1, int i2) 
{
	Fixed	smallFix, bigFix, tempFix;

	smallFix = ONE * i1;
	bigFix = ONE * i2;
	tempFix = FixMul (fract, bigFix) +FixMul (ONE-fract, smallFix);
	return (FixRound (tempFix));
}
