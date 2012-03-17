// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLCollisionMask.cpp
// © John Treece-Birch 2000
// 26/6/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
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

#include	"CBLCollisionMask.h"

// Adopts each array contained in the array inMask.
// Doesn't adopt the inMask array itself.
CBLCollisionMask::CBLCollisionMask(UInt8* inMask[8],long inWidth,long inHeight)
{
	for (char n=0; n<8; n++)
		mMask[n]=inMask[n];
	
	mWidth=inWidth;
	mHeight=inHeight;
}

// Delete each of the arrays we adopted
CBLCollisionMask::~CBLCollisionMask()
{
	for (char n=0; n<8; n++)
		delete [] mMask[n];
}

bool CBLCollisionMask::RectCollision(Rect &inRect)
{
	UInt16		y=inRect.top;
	UInt8		*mask=mMask[0]+1+inRect.top*GetCharWidth();		// Move to the correct line in the mask
	UInt8		*nextLine=mask+GetCharWidth();					// Calculate the next line

	// Keep testing lines until we reach the bottom
	while (y<inRect.bottom)
	{
		// Test collisions on this mask line
		UInt16		x=inRect.left&0xFFF8;
		
		mask+=x/8;
		
		UInt8	bits=(*mask & (0xFF >> (inRect.left-x)));			// Mask out the first few bits
		
		while (x<(inRect.right&0xFFF8))
		{
			if (bits!=0)
				return true;
				
			x+=8;
			mask++;
		
			bits=*mask;
		}
		
		bits&=0xFF << (8-(inRect.right-x));							// Mask out the last few bits
		
		if (bits!=0)
			return true;
		
		// Move to next line
		y++;
		mask=nextLine;
		nextLine+=GetCharWidth();
	}
	
	return false;
}

bool CBLCollisionMask::SpriteCollision(CBLCollisionMask		&inMaskB,
									   Rect					&inRectA,
								 	   Rect					&inRectB)
{
	UInt16		yA=inRectA.top;
	
	UInt16		xA=inRectA.left&0xFFF8;
	UInt8		*maskA=mMask[inRectA.left-xA]+1+inRectA.top*GetCharWidth();	// Move to the correct line in the mask
	UInt8		*nextLineA=maskA+GetCharWidth();							// Calculate the next line

	UInt16		xB=inRectB.left&0xFFF8;
	UInt8		*maskB=inMaskB.mMask[inRectB.left-xB]+1+inRectB.top*inMaskB.GetCharWidth();
	UInt8		*nextLineB=maskB+inMaskB.GetCharWidth();

	// Keep testing lines until we reach the bottom
	while (yA<inRectA.bottom)
	{
		// Test collisions on this mask line
		xA=inRectA.left&0xFFF8; xB=inRectB.left&0xFFF8;
		
		maskA+=xA/8; maskB+=xB/8;
		
		UInt8	bitsA=*maskA,bitsB=*maskB;
		
		while (xA<(inRectA.right&0xFFF8))
		{
			if ((bitsA & bitsB)!=0)
				return true;
				
			xA+=8; xB+=8;
			maskA++; maskB++;
		
			bitsA=*maskA; bitsB=*maskB;
		}
		
		bitsA&=0xFF << (8-(inRectA.right-xA));
		bitsB&=0xFF << (8-(inRectA.right-xA));
		
		if ((bitsA & bitsB)!=0)
			return true;
		
		// Move to next line
		yA++;
		
		maskA=nextLineA;
		nextLineA+=GetCharWidth();
	
		maskB=nextLineB;
		nextLineB+=inMaskB.GetCharWidth();
	}
	
	return false;
}