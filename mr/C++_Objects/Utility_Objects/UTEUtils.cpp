// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UTEUtils.cpp
// ©ÊMark Tully and TNT Software 1999
// 16/9/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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
	Static utility functions for working with TE. Other clever functions are available in teutilities.c in DragText demo
	which came with the Drag and Drop SDK.
*/

#include "TNT_Debugging.h"
#include "UTEUtils.h"
#include "Useful Defines.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNumLines										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TE has this "feature" where if there's no text it sez there's no lines. I beleive that there is one line which is empty.
// If you text has a return at the end then TE also doesn't add on the extra line for that, it counts the return as being
// on the previous line. This routines returns the number of lines in the te as read from the TE nlines field, but adds one
// on if the te is empty or ends in a return.
SInt16 UTEUtils::GetNumLines(
	TEHandle		inTE)
{
	SInt16			numLines=(**inTE).nLines;

	if (!numLines)
		numLines++;
	
	// If last char is a CR
	if ((**inTE).teLength && ((*(**inTE).hText)[(**inTE).teLength-1]==kCR))
		numLines++;

	return numLines;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SimTEClick										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// A reimplementation of TEClick which allows for better processing than te's infamous click loop. A loop which calls this
// will be able to control things like automatic scrolling with scroll bar updating a lot easier than a click loop ever
// could. It could be argued that replacing a tool box function isn't a good idea - but in some cases it's so difficult to
// try to get the click loop working right things like SimTEClick seem a good idea.
// ioFirstPass is a bool which you pass in as true. It's used so SimTEClick can tell when it's on the first iteration of the
// loop.
// inMouse is the mouse in local coords.
// inExtend is whether to extend the selection - it's whether the shift key is down
// ioClickRecord is the click record for the te, pass the same click record for the same te each time. You don't need to do
// anything with the click record.
// inTEHandle is the te handle to do the click in.
// returns the offset clicked on. It is sometimes useful in autoscrolling - especially when combined with GetLineForOffset.
//
// Behaviour:
// ----------
// Same as CodeWarrior.
// inExtend = false
//		Single click - move point, if drag, drag from that point
//		Double click - select word, if drag, drag from that word, selecting words at a time
//		Treble click - select line, if drag, drag from that line, selecting lines at a time
// inExtend = true
//		Single click - extend the selection towards the point if the point is outside of the selection, if drag, drag with
//					   move the end of the selection which was moved by the initial click
//		Double click - ignore inExtend = true
//		Treble click - 			"
SInt16 UTEUtils::SimTEClick(
	Point				inMouse,
	STEClickRecord		&ioClickRecord,
	bool				inExtend,
	bool				&ioFirstPass,
	TEHandle			inTEHandle)
{
	// Do first pass processing. This involves figuring out the click type and also trying to detect if the anchor offset
	// has become invalidated.
	if (ioFirstPass)
	{
 		// if the selection has changed from the last time we were called then the anchor point is now invalid.
 		// Of course you could design a case where the selection changes in such a way that this test wouldn't pick it up and
 		// then you'd get non-standard behaviour when extending the selection. It would only happen rarely if at all though.
		if ((ioClickRecord.mLastAnchorOffset!=(**inTEHandle).selStart) && (ioClickRecord.mLastAnchorOffset!=(**inTEHandle).selEnd))
			ioClickRecord.mLastAnchorOffset=STEClickRecord::kNullAnchorOffset;

		// alter the type of the click stored in the click record
		UInt32			clickTime=TickCount();
		
		if ((clickTime-ioClickRecord.mLastClickTime)<GetDblTime())
		{
			switch (ioClickRecord.mLastClickType)
			{
				case STEClickRecord::kSingleClick: ioClickRecord.mLastClickType=STEClickRecord::kDoubleClick; break;
				case STEClickRecord::kDoubleClick: ioClickRecord.mLastClickType=STEClickRecord::kTrebleClick; break;
				case STEClickRecord::kTrebleClick: break;
			}
		}
		else
			ioClickRecord.mLastClickType=STEClickRecord::kSingleClick;
		
		ioClickRecord.mLastClickTime=clickTime;

//		if (ioClickRecord.mLastClickType!=STEClickRecord::kSingleClick)
//			inExtend=false;					// can't extend from a double or treble click
	
		ioFirstPass=false;
	}
	else
		inExtend=true;


	SInt16				currentOffset=::TEGetOffset(inMouse,inTEHandle);
	SInt16				selStart=(**inTEHandle).selStart;
	SInt16				selEnd=(**inTEHandle).selEnd;

	if (inExtend)
	{
		// There are three possible cases, either the mouse is before the selection, in the selection, or after the selection.
		if (currentOffset<=selStart)
		{
			// Extend using the sel end as the anchor point
			ioClickRecord.mLastAnchorOffset=selEnd;
		}
		else if (currentOffset>=selEnd)
		{
			// Extend using the sel start as the anchor point
			ioClickRecord.mLastAnchorOffset=selStart;
		}
		else
		{
			// Extend using which ever out of the selStart and selEnd was the last anchor point. (ie from the last selection
			// drag).
			if (ioClickRecord.mLastAnchorOffset==STEClickRecord::kNullAnchorOffset)
			{
				// If there is a null anchor offset it means that the user hasn't selected it themselves. It was probably
				// made by a SelectAll call or a ::TESetSelect call directly. In cases such as this we set the anchor point
				// to the selection start.
				ioClickRecord.mLastAnchorOffset=selStart;
			}
		}
	}
	else
		ioClickRecord.mLastAnchorOffset=currentOffset;
	
	// The new selection will be between the anchor and the current mouse position. If that's the same as current then we
	// do nothing, if it's different we change the selection.
	SInt16				newSelStart,newSelEnd;
	
	if (currentOffset<ioClickRecord.mLastAnchorOffset)
	{
		newSelStart=currentOffset;
		newSelEnd=ioClickRecord.mLastAnchorOffset;
	}
	else
	{
		newSelStart=ioClickRecord.mLastAnchorOffset;
		newSelEnd=currentOffset;
	}
	
	// Now "round the selection out" to the nearest multiple of a word or line depending on the click type
	switch (ioClickRecord.mLastClickType)
	{
		case STEClickRecord::kSingleClick:			// no rounding for a single click
			break;
			
		case STEClickRecord::kDoubleClick:			// select word
			{
				SInt16				dontCare;
				
				GetWordBoundary(newSelStart,newSelStart,dontCare);
				GetWordBoundary(newSelEnd,dontCare,newSelEnd);
			}
			break;
			
		case STEClickRecord::kTrebleClick:			// select line
			{
				SInt16				dontCare;
				
				GetLineBoundary(newSelStart,newSelStart,dontCare);
				GetLineBoundary(newSelEnd,dontCare,newSelEnd);
			}
			break;
	}
	
	if (newSelStart!=selStart || newSelEnd!=selEnd)
		::TESetSelect(newSelStart,newSelEnd,inTEHandle);
	
	return currentOffset;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetLineBoundary									Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Given an offset this returns the offsets of the beginning and end of the line the offset is contained in.
void UTEUtils::GetLineBoundary(
	SInt16						inOffset,
	SInt16						&outLineStart,
	SInt16						&outLineEnd)
{
	SignalString_("UTEUtils::GetLineBoundary() unimplemented");
	outLineStart=outLineEnd=inOffset;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetWordBoundary									Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Given an offset this returns the offset of the beginning and end of the word the offset is contained in
void UTEUtils::GetWordBoundary(
	SInt16						inOffset,
	SInt16						&outWordStart,
	SInt16						&outWordEnd)
{
	SignalString_("UTEUtils::GetWordBoundary() unimplemented");
	outWordStart=outWordEnd=inOffset;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetLineForOffset									Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Given an offset and a TextEdit handle, returns the line number of the line that contains the offset.
SInt16 UTEUtils::GetLineForOffset(
	SInt16		inOffset,
	TEHandle	inTE)
{
	SInt16		line = 0;

	if (inOffset > (**inTE).teLength)
		return((**inTE).nLines);

	while ((**inTE).lineStarts[line] < inOffset)
		line++;
	
	return line;
}
