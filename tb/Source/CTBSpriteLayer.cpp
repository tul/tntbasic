// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CTBSpriteLayer.cpp
// © Mark Tully 2002
// 7/2/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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
	Special note
	------------
	This class is not needed because TNT Basic un-wraps the viewports before rendering the sprites onto it, therefore no
	special processing of sprites is needed.
	If you render to canvases OTHER than canvas 0 in the future, then this class may well be needed to cater for wrapped
	canvases managed by wrapped viewports.
*/

/*
	A sprite layer that works with wrapped viewports.
	
	A wrapped viewport has a real area and an affective area (which is usually bigger). The affective area is created by
	wrapping the coordinates that are off the real area back onto the other side, so copies from off the right edge simply
	come back on the left edge. This is useful for tile scrolling because it means that if continuously scrolling you just
	keep drawing 1 row of tiles on the edge and moving your rect, as if you were tile scrolling with an infinite canvas.
	
	The problem comes with sprites, because in reality the real area being used is wrapped in the wrong order (eg the right
	side of the screen is in the left side of the wrapped canvas). Sprites need to be aware of this so they can be rendered
	in the correct location. If a sprite is straddling the edge of the real area (which could well be in the middle of the
	screen when the view is composited) then it will need to be drawn in two halfs in two places. For example the left half
	on the right edge of the real area and the right half on the left edge of the real area - it will be rejoined when the
	viewport is blitted. The worst case comes when the sprite has to be split in quarters because it is straddling seams in
	both the x and y axis.
	
	To cater for this, a special implementation of a blast sprite layer has been created that renders/calculates each sprite
	up to four times depending on it's relative position in the real canvas.
	
	Caveat
	------
	At the moment, only one viewport is permitted per canvas. This means that split screen scrolling to the same canvas is
	not possible, hence the same sprite cannot be rendered on screen in two locations (as would happen if both views had the
	same offset). If we ever changed tb to allow multiple viewports to the same canvas then we'd have to rethink the sprite
	strategy a bit because the same sprite layer would have to be rendered twice with different offsets and clipping rects.

	The same goes for wrapped sprite layers except it would be even more confusing because the same sprite could be 8 places
	at once (worst case).
*/

#include "CTBSpriteLayer.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CalcAllSprites
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This looks if there is a wrapped viewport attached to the layer, if there is it calcs the sprites that are over the edges
// splitting them as needs be.
/*
void CTBSpriteLayer::CalcAllSprites(
	BlastDrawBufferPtr	inInvalBuffer)
{
	if (mLayerHidden)
		return;
	
	// do we have a wrapped viewport?
	if (!mWrappedViewport)
	{
		inheritedLayer::CalcAllSprites(inInvalBuffer);
		return;
	}
	
	// This structure describes a section of the SOURCE canvas (that is the wrapped canvas) and then the x,y offsets to
	// where this rectangle should appear on the DESTINATION canvas so that the seams of the wrapped canvas are aligned.
	struct SCanvasSection
	{
		Rect			clipRect;				// rectangle on the canvas
		TBLImageVector	xorigin,yorigin;		// location of origin (zero point) relative to top left of clip rect. + is down and right
	};
	
	// retrieve canvas sections from wrapped viewport
	
	// render sprites onto canvas
	
	// when viewport is blitted and wrapped the sprites will be made whole
}
*/