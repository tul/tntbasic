// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Rect routines
// Mark Tully
// ??/11/95
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

#include	"Marks Routines.h"

// A lot of these routines have been deleted and made into defines instead which are faster.
// They are in  "Useful Defines.h" and are called in almost the same way.

// Adjusts rectA to be central in rectB
void CentreRectInRect(
	Rect		*rectA,
	const Rect	*rectB)
{
	short	widthA, heightA;
	
	widthA = rectA->right - rectA->left;
	heightA = rectA->bottom - rectA->top;
	
	rectA->left = rectB->left + ((rectB->right - rectB->left) - widthA) / 2;
	rectA->right = rectA->left + widthA;
	
	rectA->top = rectB->top + ((rectB->bottom - rectB->top) - heightA) / 2;
	rectA->bottom = rectA->top + heightA;
}

// This routine returns true if a rectangle has got the ability to display a colour pict, it's
// used to decide whether the colour pict should be drawn. The rect is in global coords.
Boolean ColourRect(Rect theRect)
{
	GDHandle		gd;
	
		// has the ability, get the pixel depth of the maximum depth device insecting with the
		// rect
		gd=GetMaxDevice(&theRect);
		if ((**(**gd).gdPMap).pixelSize > 1)
			return true;	// depth is more that 1
	
	return false;
}

// must set the ports before you call these

void LocalToGlobalRect(Rect *theRect)
{
	LocalToGlobal((Point *)&theRect->top);
	LocalToGlobal((Point *)&theRect->bottom);
}

void GlobalToLocalRect(Rect *theRect)
{
	GlobalToLocal((Point *)&theRect->top);
	GlobalToLocal((Point *)&theRect->bottom);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ScaleRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// rectA is shrunk (or optional expanded) to be as large as possible inside rectB without being
// distorted. Good for scaling images
// 22/9/00: Rewrote, fixing a bug probably due to trucation in a divide
// Returns the scale used > 1 means scale up, 1 stay same and < 1 scale down
float ScaleRect(
	Rect		*ioRectA,
	const Rect	*inRectB,
	Boolean		inGoBigger)
{
	float		xScale=((float)FRectWidth(*ioRectA))/((float)FRectWidth(*inRectB));
	float		yScale=((float)FRectHeight(*ioRectA))/((float)FRectHeight(*inRectB));
	float		scaler=1.0;

	// if one of the scale values is > 1 then it means the rect is too big and must be scaled down
	if (xScale>1 || yScale>1)
	{
		// scale down
		// pick the scale furthest away from 1, which represents the biggest dimension, and divide by it to scale the rect
		if (xScale>yScale)
			scaler=xScale;
		else
			scaler=yScale;
		
		ioRectA->right=ioRectA->left+((float)FRectWidth(*ioRectA))/scaler;
		ioRectA->bottom=ioRectA->top+((float)FRectHeight(*ioRectA))/scaler;
	}
	else if (inGoBigger && xScale<1 && yScale<1)
	{
		// scale up?
		// both xScale and yScale are <=1 which means the rect can grow. We pick the scale closest to 1 (which will therefore
		// cause the least scaling) and use that as our scale value

		if (xScale<yScale)
			scaler=yScale;
		else
			scaler=xScale;
			
		ioRectA->right=ioRectA->left+((float)FRectWidth(*ioRectA))/scaler;
		ioRectA->bottom=ioRectA->top+((float)FRectHeight(*ioRectA))/scaler;
	}

	CenterRectInRect(ioRectA,inRectB);
	NewClipRect(ioRectA,inRectB);
/*
	short	widthA,heightA,biggestA,rectDimB;
	float	scaleFactor;
	Boolean	wid=false;	
	
	// get rect width
	widthA=rectA->right-rectA->left;
	heightA=rectA->bottom-rectA->top;
	
	if (widthA>heightA)
	{
		biggestA=widthA;
		wid=true; // Can't be bothered with full names you see...
	}
	else
		biggestA=heightA; // Don't need to se the dim bit as it default's to height
		
	// Biggest now contains the larger dimension, either width or height and dim sez
	// which one
	// Now get the same dimension from rectB
	if (wid) // find the width of the rect
		rectDimB=rectB->right-rectB->left;
	else // find height of the rect
		rectDimB=rectB->bottom-rectB->top;
	
	// So now we know what the biggest dimension of the picture has to fit into
	// Find an appropiate scale factor
	scaleFactor=(float)biggestA/(float)rectDimB;
	if ((!goBigger) && (scaleFactor<1)) // If we didn't do this the rects would be blown up if too small
		scaleFactor=1;
	
	// Now scale the picture down using that factor
	widthA/= scaleFactor;
	heightA/= scaleFactor;

	// Verify that the new dimensions do fit inside the rect (which they may not due to
	// rounding errors)
	if (widthA>(rectB->right-rectB->left))
		widthA=rectB->right-rectB->left;
		
	if (heightA>(rectB->bottom-rectB->top))
		heightA=rectB->bottom-rectB->top;
	
	// Now the rect will fit in the other rect without distortion, center it in it
	// This finds the proper left edge inside the rect and then the right edge
	FastSetRect(*rectA,0,0,heightA,widthA);
	CenterRectInRect(rectA,rectB);*/
	
	return 1.0/scaler;
}

// clips rect to lie inside clipRect.
// If it's clipped off totally it returns -1
// If no change is made it returns 0
// If it's clipped a bit it returns 1
// Note all these are defined in marks routines.h under rects.c
signed char NewClipRect(Rect *rect,const Rect *clipRect)
{
	// check for out of bounds
	if (rect->right<=clipRect->left || rect->left>=clipRect->right || rect->top>=clipRect->bottom || rect->bottom<=clipRect->top)
	{
		rect->bottom=rect->top;
		return -1;
	}
	else
	{
		signed char		clipped=0;
	
		if (rect->top<clipRect->top)
		{
			rect->top=clipRect->top;
			clipped=1;
		}
		if (rect->bottom>clipRect->bottom)
		{
			rect->bottom=clipRect->bottom;
			clipped=1;
		}
		if (rect->left<clipRect->left)
		{
			rect->left=clipRect->left;
			clipped=1;
		}
		if (rect->right>clipRect->right)
		{
			rect->right=clipRect->right;
			clipped=1;
		}
	
		/*rect->top=Greater(rect->top,clipRect->top);
		rect->left=Greater(rect->left,clipRect->left);
		rect->bottom=Lesser(rect->bottom,clipRect->bottom);
		rect->right=Lesser(rect->right,clipRect->right);*/
		return clipped;
	}
}		
