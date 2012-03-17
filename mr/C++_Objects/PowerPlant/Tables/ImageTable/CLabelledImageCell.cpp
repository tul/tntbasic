// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CLabelledImageCell.cpp
// © Mark Tully and TNT Software 2000
// 16/8/00
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

#include	"CLabelledImageCell.h"
#include	"Utility Objects.h"
#include	"Useful Defines.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetTextPlacement
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Gets the rect for the text title based on the text traits and the string. Returns the base line which can be useful for
// actually rendering the string
void CLabelledImageCell::GetTextPlacement(
	const Rect					&inCellRect,
	Rect						&outRect,
	LString						&outString,
	Point						&outPoint)
{
	UTextTraits::SetPortTextTraits(mImageTable->GetTextTraits());
	
	// Figure default baseline.		
	FontInfo fi;
	::GetFontInfo(&fi);

	SInt16	textHeight = (SInt16) (fi.ascent + fi.descent);

	outRect=inCellRect;
	outRect.top=outRect.bottom-textHeight;

	SInt16	baseLine = (SInt16) (outRect.top + fi.ascent);
	SInt16	stringWidth;

	outString=mLabel;

	if (mLabel[0])
		stringWidth=::StringWidth(mLabel);
	else
		stringWidth=0;
		
	SInt16	maxWidth=FRectWidth(outRect)-6;

	if (maxWidth<0)
		maxWidth=0;

	// Do we need to truncate to fit in the cell?
	// This looks redundant, but it actually saves time because StringWidth is fast (native?)
	// and TruncString is slow (emulated?).

	if (stringWidth > maxWidth)
	{
		::TruncString(maxWidth,outString,truncEnd);
		stringWidth = ::StringWidth(outString);
	}
			
	// Create a text bounds that is used for hit testing and drawing.
	// Choose horizontal placement.
	SInt16		rectCentre=outRect.left+FRectWidth(outRect)/2;
	outRect.left=rectCentre-stringWidth/2-3;
	outRect.right=rectCentre+stringWidth/2+3;
	
	outPoint.h=outRect.left+3;
	outPoint.v=baseLine;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawCell
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Figures out where the cell content and label go. Draws the label and erases the area around the cell content.
// If selected will then hilight the area around the cell content or if the appropiate flag is clear, only the text
void CLabelledImageCell::DrawCell(
	const STableCell			&inCell,
	const Rect					&inRect,
	bool						inIsSelected,
	bool						inIsEnabled)
{
	LStr255						string;
	Rect						textRect;
	Point						textPoint;
	Rect						contentRect=inRect;

	GetTextPlacement(inRect,textRect,string,textPoint);

	contentRect.bottom=textRect.top;
	GetCellContentRect(contentRect);

	// Erase the rects around the content
	Rect						erasedRects[4];

	erasedRects[0]=inRect;
	erasedRects[1]=inRect;
	erasedRects[2]=inRect;
	erasedRects[3]=inRect;
	
	erasedRects[0].bottom=contentRect.top;		// top
	erasedRects[0].left=contentRect.left;
	erasedRects[0].right=contentRect.right;
	erasedRects[1].right=contentRect.left;		// left
	erasedRects[2].top=contentRect.bottom;		// bottom
	erasedRects[2].left=contentRect.left;
	erasedRects[2].right=contentRect.right;
	erasedRects[3].left=contentRect.right;		// right

	if (inIsSelected)
	{
		UForeColourSaver		safe;
		::RGBForeColour(&TColourPresets::kWhite);
		::PaintRect(&erasedRects[0]);
		::PaintRect(&erasedRects[1]);
		::PaintRect(&erasedRects[2]);
		::PaintRect(&erasedRects[3]);
	}
	else
	{
		mImageTable->ApplyForeAndBackColors();
		RGBColour				col;
		::GetBackColor(&col);
		UForeColourSaver		safe(col);		// paint in the back colour to erase rather than fill with window pattern
		::PaintRect(&erasedRects[0]);
		::PaintRect(&erasedRects[1]);
		::PaintRect(&erasedRects[2]);
		::PaintRect(&erasedRects[3]);
	}

	::MoveTo(textPoint.h,textPoint.v);
	::DrawString(string);
	
	if (inIsSelected)
	{
		unsigned char	hMode;

		hMode=::LMGetHiliteMode();
		::BitClr((Ptr)(&hMode),(long)pHiliteBit);

		// should we hilight all around the content?
		if (mFlags&kHilightBackground)
		{
			::LMSetHiliteMode(hMode); ::InvertRect(&erasedRects[0]);
			::LMSetHiliteMode(hMode); ::InvertRect(&erasedRects[1]);
			::LMSetHiliteMode(hMode); ::InvertRect(&erasedRects[2]);
			::LMSetHiliteMode(hMode); ::InvertRect(&erasedRects[3]);
		}
		else
		{
			// hilight the label with a tight rect
			::LMSetHiliteMode(hMode); ::InvertRect(&textRect);
		}
	}
	
	DrawCellContent(inCell,contentRect,inIsSelected,inIsEnabled);
}
