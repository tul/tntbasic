// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CHexEditorPane.cpp
// © Mark Tully and TNT Software 1999
// 18/9/99
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
	The hex editor pane is a pane which edits data provided for it by a parent hex view. This pane can edit as either hex
	or ascii. Some of the code in this class is messy. 
	
	Code duplication in BeTarget and DontBeTarget
	Caret drawing and invalling code in BeTarget and DontBeTarget uses a region instead of a much quicker calculation
	No undo for typing
*/

#include		"TNT_Debugging.h"
#include		"CHexEditorPane.h"
#include		"Marks Routines.h"
#include		"Utility Objects.h"
#include		"CHexView.h"
#include		"Useful Defines.h"
#include		<UKeyFilters.h>
#include		<PP_KeyCodes.h>

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CHexEditorPane::CHexEditorPane(
	LCommander	*inSuperCommander,
	SPaneInfo	&inPaneInfo,
	bool		inDisplayAsHex) :
	inheritedPane(inPaneInfo),
	LCommander(inSuperCommander)
{
	mLastCaretToggleTime=0;
	mDataStore=0L;
	mDisplayAsHex=inDisplayAsHex;
	mEditing=false;
	mSelStart=0;
	mSelEnd=0;
	mCaretPhase=true;
	mOfficialTargetStatus=false;
	mWaitingSecondNibble=false;
	
	::GetFNum("\pmonaco",&mFontId);
	
	StartIdling();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetRangeRegion									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Constructs a region in local coords which contains all the chars between inStart and inEnd. Returns nil for no region.
RgnHandle CHexEditorPane::GetRangeRegion(
	CDataStore::TDataOffset		inSelStart,
	CDataStore::TDataOffset		inSelEnd)
{
	if (inSelStart==inSelEnd)
		return 0L;

	StRegion					region;
	CDataStore::TDataOffset		rowBytes=CalcMaxBytesPerRow();
	TRowIndex					startRow,endRow;
	CDataStore::TDataOffset		startBytesFromLeft,endBytesFromLeft;
	Rect						frameRect;
	Rect						sameRowRect={0,0,0,0};			// special case for when a the selection starts and ends on the same row
	
	if (!rowBytes)
		return 0L;
	
	CalcLocalFrameRect(frameRect);
		
	startRow=inSelStart/rowBytes;
	endRow=inSelEnd/rowBytes;
		
	startBytesFromLeft=inSelStart%rowBytes;
	endBytesFromLeft=inSelEnd%rowBytes;

	// if we're at the end of a row then endRow will mistakenly be set to the next row, correct for this
	if (inSelEnd && endBytesFromLeft==0)
	{
		endRow--;
		endBytesFromLeft=rowBytes;
	}

	
	Rect		rowRect;

	rowRect=frameRect;
	// Indent the sides to take into account the padding used
	rowRect.left+=kHilightIndent;
	rowRect.right-=kHilightIndent;
	rowRect.top=startRow*kRowHeight;
	rowRect.bottom=rowRect.top+kRowHeight;
	
	// Get the selection region for the start row
	{
		Rect		partialRect=rowRect;

		partialRect.left=OffsetToXCoord(startBytesFromLeft);
		
		if (startBytesFromLeft==0)			// if zero, move the hilight rect left a little to make sure it goes 1 pixel
			partialRect.left+=(-kLeftIndent+kHilightIndent);
	
		// If this is the top row we move the top down a little to allow for the hilight frame
		if (startRow==0)
			partialRect.top+=kHilightIndent;
	
		if (startRow!=endRow)
			region+=partialRect;
		else
			sameRowRect=partialRect;
	}
	
	// Add the rows between the start row and end row
	FOffset(rowRect,0,kRowHeight);

	if (endRow>(startRow+1))
	{	
		rowRect.bottom+=(endRow-startRow-2)*kRowHeight;

		// If this is the bottom row we take row hilight pixels from the rect
		if (!endBytesFromLeft)
			rowRect.bottom-=kHilightIndent;

		region+=rowRect;
		rowRect.top=rowRect.bottom-kRowHeight;
		FOffset(rowRect,0,kRowHeight);
	}

	// Get the selection region for the end row
	if (endBytesFromLeft)
	{
		Rect		partialRect=rowRect;
		
		partialRect.right=OffsetToXCoord(endBytesFromLeft);

		if (endBytesFromLeft==rowBytes)		// if it's to go right up to the edge
			partialRect.right+=(kRightIndent-kHilightIndent);
		
		if (startRow!=endRow)
		{
			// If this is the bottom row in the view then bring the rect's bottom line up to make sure we don't draw into the
			// hilight
			if (partialRect.bottom==frameRect.bottom)
				partialRect.bottom-=kHilightIndent;
			region+=partialRect;
		}
		else
		{
			// If this is the bottom row in the view then bring the rect's bottom line up to make sure we don't draw into the
			// hilight
			if (sameRowRect.bottom==frameRect.bottom)
				sameRowRect.bottom-=kHilightIndent;
			sameRowRect.right=partialRect.right;
			region+=sameRowRect;
		}
	}
		
	return region.Release();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AdjustSelection
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called by parent hex view to change the selection range when the user deletes the selection or replaces it with other
// data. The new range should not be drawn, but the caret is erased if present as it has a tendancy to be left behind when
// refresh data range is called later to redraw the changed data.
void CHexEditorPane::AdjustSelection(
	CDataStore::TDataOffset		inSelStart,
	CDataStore::TDataOffset		inSelEnd)
{
	DrawCaret(kCaretOff);			// Erase caret first other wise it can get left behind
	mSelStart=inSelStart; mSelEnd=inSelEnd;
	mWaitingSecondNibble=false;		// moving the caret interrupts inline editing
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetSelection
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Changes the selection, refreshing the changed parts
void CHexEditorPane::SetSelection(
	CDataStore::TDataOffset		inSelStart,
	CDataStore::TDataOffset		inSelEnd)
{
	Try_
	{
		if (!GetSuperView())		// need a super view to do drawing clipped to a certain region
			Throw_(-1);
	
		if (mEditing)
		{
			FocusDraw();
		
			// Get was selection
			StRegion				wasSelection(GetRangeRegion(mSelStart,mSelEnd),false);

			// Get new selection
			StRegion				newSelection(GetRangeRegion(inSelStart,inSelEnd),false);
			
			if (!mOfficialTargetStatus)
			{
				// outline the regions
				if (!wasSelection.IsEmpty())
					OutlineRgn(wasSelection);
				if (!newSelection.IsEmpty())
					OutlineRgn(newSelection);
			}
			
			DrawCaret(kCaretOff); // erase caret if on

			mSelStart=inSelStart;
			mSelEnd=inSelEnd;
			
			Point				portOrigin={0,0};
			
			LocalToPortPoint(portOrigin);		// used to convert between local and port coord for the regions
					
			// Calculate difference between regions
			if ((!wasSelection.IsEmpty()) && (!newSelection.IsEmpty()))
			{
				StRegion				region;

				::XorRgn(wasSelection,newSelection,region);
				
				// Do a local to port coord conversion on rgn
				::OffsetRgn(region,portOrigin.h,portOrigin.v);

				mSuperView->Draw(region);
			}
			else if (!wasSelection.IsEmpty())
			{
				// Do a local to port coord conversion on rgn
				::OffsetRgn(wasSelection,portOrigin.h,portOrigin.v);

				mSuperView->Draw(wasSelection);
			}
			else if (!newSelection.IsEmpty())
			{
				// Do a local to port coord conversion on rgn
				::OffsetRgn(newSelection,portOrigin.h,portOrigin.v);

				mSuperView->Draw(newSelection);
			}
			
			// Draw the caret
			mCaretPhase=true;
			mLastCaretToggleTime=TickCount();	// ensures it stays on for a cycle
			DrawCaret(mOfficialTargetStatus ? kCaretOn : kCaretDimmed);		// does nothing if mSelStart!=mSelEnd
		}
		else
		{
			mSelStart=inSelStart;
			mSelEnd=inSelEnd;
		}
	}
	Catch_(err)
	{
		mSelStart=inSelStart;
		mSelEnd=inSelEnd;
		Draw(0L);
	}
	
	mWaitingSecondNibble=false;			// moving the caret interrupts inline editing
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetDataStore
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called by the parent view to set the data store for this element
void CHexEditorPane::SetDataStore(
	CDataStore	*inDataStore)
{
	mDataStore=inDataStore;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetIdealHeight
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the ideal height in pixels that this pane would have to be to display all the data at the current width
SInt32 CHexEditorPane::GetIdealHeight()
{
	TRowIndex	numRows=1;

	if (mDataStore)
	{
		Size						numBytes=mDataStore->GetLength();
		CDataStore::TDataOffset		bytesPerRow=CalcMaxBytesPerRow();
		
		if (bytesPerRow && numBytes)
		{
			numRows=numBytes/bytesPerRow;
			if (numBytes%bytesPerRow)
				numRows++;
		}
	}
	
	return numRows*kRowHeight;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReplaceInvisibles
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Replace the unprintable characters in the memory indicated for the ASCII printout
void CHexEditorPane::ReplaceInvisibles(
	unsigned char					*ioCharsBuffer,
	CDataStore::TDataOffset			inCount)
{
	while (inCount--)
	{
		if (!IsCharDefinedInASCIITable(*ioCharsBuffer))
			*ioCharsBuffer=kMissingCharSymbol;
		ioCharsBuffer++;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ OffsetToXCoord
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the distance from frame.left than an offset INTO this a row would occupy. Eg 0 would be kLeftIndent pixels in, 1
// would be kLeftIndent+kMonaco9CharWidth etc.
UInt16 CHexEditorPane::OffsetToXCoord(
	CDataStore::TDataOffset			inOffset)
{
	UInt16							result;
	Rect							frame;
	
	if (!CalcLocalFrameRect(frame))
		return 0;

	if (mDisplayAsHex)
	{
		result=kLeftIndent+(k4ByteHexBlockPixelWidth*(inOffset/4));
	
		// Any bytes left over after dividing by 4 would be in a two byte block after the 4 byte columns
		UInt16						remainder=inOffset%4;
		
		if (remainder)
			result+=remainder*(2*kMonaco9CharWidth);
	}
	else
	{
		result=inOffset*kMonaco9CharWidth+kLeftIndent;
	}
	
	return result+frame.left;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PointToOffset
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the offset of the current point as bytes into the data
CDataStore::TDataOffset CHexEditorPane::PointToOffset(
	Point							inLocalPoint)
{
	CDataStore::TDataOffset			rowBytes=CalcMaxBytesPerRow();
	TRowIndex						rowIndex;
	Rect							localFrame;
	CDataStore::TDataOffset			bytesIntoRow=0;
	
	CalcLocalFrameRect(localFrame);
	
	inLocalPoint.v-=localFrame.top;
	inLocalPoint.h-=localFrame.left;
	
	if (!rowBytes)
		return 0;
	
	if (inLocalPoint.v<localFrame.top)
		return 0;
	if (inLocalPoint.v>localFrame.bottom)
		return mDataStore->GetLength();
		
	rowIndex=inLocalPoint.v/kRowHeight;	
	inLocalPoint.h-=kLeftIndent;
	
	if (inLocalPoint.h>0)
	{
		if (mDisplayAsHex)
		{
			UInt16				space=inLocalPoint.h;
			UInt16				charsIn,blocksIn,halfBlocksIn=0;
			bool				halfblockAvailable=inLocalPoint.h > ((rowBytes/4)*k4ByteHexBlockPixelWidth);
			
			blocksIn=space/k4ByteHexBlockPixelWidth;
			space=space%k4ByteHexBlockPixelWidth;
			
			if (halfblockAvailable)
			{
				halfBlocksIn=space/k2ByteHexBlockPixelWidth;
				space=space%k2ByteHexBlockPixelWidth;
			}
			
			charsIn=space/(2*kMonaco9CharWidth);
			space=space%(2*kMonaco9CharWidth);
			
			// If the space left is more than half a byte then round up else leave as is (rounded down)
			if (space>=kMonaco9CharWidth)
				charsIn++;
				
			bytesIntoRow=blocksIn*4+halfBlocksIn*2+charsIn;
		}
		else
		{
			bytesIntoRow=inLocalPoint.h/kMonaco9CharWidth;
			
			// If the space left is more than half a byte then round up else leave as is (rounded down)
			if ((inLocalPoint.h%kMonaco9CharWidth)>=(kMonaco9CharWidth/2))
				bytesIntoRow++;
		}
	}
	
	// Is the bytes into row too big?
	if (bytesIntoRow>rowBytes)
		bytesIntoRow=rowBytes;
	
	return bytesIntoRow + rowIndex*rowBytes;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BeTarget
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Refreshes the bits which change according to focus
void CHexEditorPane::BeTarget()
{
	Try_
	{
		StRegion				region;
		Rect					frame;

		if (!mSuperView)
			return;
		
		if (!CalcLocalFrameRect(frame) || !FocusDraw())
			return;
		
		// Add the frame region
		region+=frame;
		FInsetRect(frame,1,1);
		region-=frame;
				
		if (mSelStart==mSelEnd)
		{
			// Add the caret region
			StRegion			caretRgn(GetRangeRegion(mSelStart,mSelEnd+1),false);
			Rect				caretRect; // =(**(RgnHandle)caretRgn).rgnBBox;
			
			caretRect=caretRgn.Bounds();
			
			caretRect.left-=1;
			caretRect.right=caretRect.left+1;
			caretRect.bottom=caretRect.top+kCaretHeight;
			
			region+=caretRect;
		}
		
		mOfficialTargetStatus=true;			// This is used to replace IsTarget as IsTarget returns true during DontBeTarget

		// Note add the "targetted" region, this is done as the active rgn contains the inactive rgn and so we get both.
		StRegion				hiliteRgn(GetRangeRegion(mSelStart,mSelEnd),false);
		region+=hiliteRgn;
		
		// Translate into port coords
		Point					point={0,0};
		
		LocalToPortPoint(point);
		
		::OffsetRgn(region,point.h,point.v);
		
		mSuperView->Draw(region);
	}
	Catch_(err)
	{
		mOfficialTargetStatus=true;
		Draw(0L);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DontBeTarget
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Refreshes the bits which change according to focus
void CHexEditorPane::DontBeTarget()
{
	Try_
	{
		StRegion				region;
		Rect					frame;

		if (!mSuperView)
			return;
		
		if (!CalcLocalFrameRect(frame) || !FocusDraw())
			return;
		
		// Add the frame region
		region+=frame;
		FInsetRect(frame,1,1);
		region-=frame;
				
		if (mSelStart==mSelEnd)
		{
			// Add the caret region
			StRegion			caretRgn(GetRangeRegion(mSelStart,mSelEnd+1),false);
			Rect				caretRect=caretRgn.Bounds(); //(**(RgnHandle)caretRgn).rgnBBox;
			
			caretRect.left-=1;
			caretRect.right=caretRect.left+1;
			caretRect.bottom=caretRect.top+kCaretHeight;
			
			region+=caretRect;
		}
		
		// Note add the "targetted" region, this is done as the active rgn contains the inactive rgn and so we get both.
		StRegion				hiliteRgn(GetRangeRegion(mSelStart,mSelEnd),false);
		region+=hiliteRgn;
		
		// Translate into port coords
		Point					point={0,0};
		
		LocalToPortPoint(point);
		
		::OffsetRgn(region,point.h,point.v);
		
		mOfficialTargetStatus=false;			// This is used to replace IsTarget as IsTarget returns true during DontBeTarget

		mSuperView->Draw(region);
	}
	Catch_(err)
	{
		mOfficialTargetStatus=false;
		Draw(0L);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RedrawDataRange
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Redraws all lines which intersect the given range. Used when changing the text via key presses. Redraws the caret also.
void CHexEditorPane::RedrawDataRange(
	CDataStore::TDataOffset	inStart,
	CDataStore::TDataOffset	inEnd)
{
	// if the end to be drawn is the end of the data then expand to regiong to be the entire row
	if (inEnd==mDataStore->GetLength())
		inEnd+=CalcMaxBytesPerRow();

	StRegion				theRegion(GetRangeRegion(inStart,inEnd),false);
	Point					point={0,0};
	
	LocalToPortPoint(point);
	::OffsetRgn(theRegion,point.h,point.v);
	
	mSuperView->Draw(theRegion);

	if (mSelStart==mSelEnd)
		DrawCaret(mOfficialTargetStatus ? (mCaretPhase ? kCaretOn : kCaretOff) : kCaretDimmed);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ResizeFrameBy
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Overriden to avoid refreshing all of the data if only the height of the frame has changed.
/*void CHexEditorPane::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	if (inWidthDelta)
		inheritedPane::ResizeFrameBy(inWidthDelta,inHeightDelta,inRefresh);
	else
	{
		StValueChanger<Boolean>		change(mRefreshAllWhenResized,false);
		
		inheritedPane::ResizeFrameBy(inWidthDelta,inHeightDelta,inRefresh);
	}
}*/

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws the contents of the parent hex view as a hexidecimal dump
void CHexEditorPane::DrawSelf()
{	
	if (!mDataStore)
		return;

	Rect						frame;
	CDataStore::TDataOffset		rowStartOffset,bytesPerRow,charactersPerRow;
	unsigned char				*charsBuffer=0L;
	
	if (!CalcLocalFrameRect(frame))
		return;
		
	bytesPerRow=CalcMaxBytesPerRow();
	
	if (!bytesPerRow)
		return;
		
	if (mDisplayAsHex)
		charactersPerRow=bytesPerRow*2;				// each byte renders as two characters on screen
	else
		charactersPerRow=bytesPerRow;

	// Only draw the hex which intersects with the update rgn
	Try_
	{
		RgnHandle	localUpdateRgnH = GetLocalUpdateRgn();
		Rect		updateRect;// = (**localUpdateRgnH).rgnBBox;
		GetRegionBounds(localUpdateRgnH,&updateRect);
		::DisposeRgn(localUpdateRgnH);
		
		if (::EmptyRect(&updateRect))
			updateRect=frame;

		TRowIndex	startRowIndex=updateRect.top/kRowHeight;
		TRowIndex	endRowIndex=updateRect.bottom/kRowHeight+(updateRect.bottom%kRowHeight ? 1 : 0);		// divide and round up for the end row
		UTextSaver	safe(mFontId,9,0,srcCopy);
		
		charsBuffer=new unsigned char[charactersPerRow];		// holds the data as binary, then hex form.

		if (!charsBuffer)
			return;

		ApplyForeAndBackColors();
		
		if (mEditing)
		{
			if (!mOfficialTargetStatus)
			{
				UForeColourSaver		safe;
				RGBColour				colour=cDisabledFrame;
			
				RGBForeColour(&colour);
				::FrameRect(&frame);
			}
			else
				::FrameRect(&frame);
		}

		// Get the offset into the data store for the rows to draw	
		rowStartOffset=startRowIndex*bytesPerRow;
		
		Rect							thisRow;
		
		FSetRect(thisRow,frame.top+(startRowIndex*kRowHeight),frame.left+kHilightIndent,frame.top+((startRowIndex+1)*kRowHeight),frame.right-kHilightIndent);

		for (TRowIndex rowIndex=startRowIndex; rowIndex<endRowIndex; rowIndex++)
		{
			// Position the pen
			::MoveTo(thisRow.left-kHilightIndent+kLeftIndent,thisRow.bottom-kFontDescent);
			
			// Erase the row to clear any hilight which was there - don't erase the top or bottom of the frame
			if ((thisRow.top==frame.top) && (thisRow.bottom==frame.bottom))
			{
				thisRow.top++,thisRow.bottom--;
				::EraseRect(&thisRow);
				thisRow.top--,thisRow.bottom++;
			}
			else if (thisRow.top==frame.top)
			{
				thisRow.top++;
				::EraseRect(&thisRow);
				thisRow.top--;
			}
			else if (thisRow.bottom==frame.bottom)
			{
				thisRow.bottom--;
				::EraseRect(&thisRow);
				thisRow.bottom++;
			}
			else
				::EraseRect(&thisRow);

			// Read the binary data into the buffer
			OSErr			err=mDataStore->GetBytes(rowStartOffset,charsBuffer,bytesPerRow);
			if (err && err!=readErr)		// read err just means we reached the end of the data
				Throw_(err);

			if (mDisplayAsHex)
			{
				// Format the data as hex
				::BinToHex(charsBuffer,charsBuffer,bytesPerRow);

				// Draw the hex it blocks
				CDataStore::TDataOffset		dataLeftToDraw=bytesPerRow*2;
				CDataStore::TDataOffset		offset=0;

				// Blocks of 4 bytes
				while (dataLeftToDraw>=8)
				{
					::DrawText(charsBuffer,offset,8);
					Move(kBlockSpaceWidth,0);
					offset+=8;
					dataLeftToDraw-=8;				
				}
		
				// Draw the final block (will be a block of 2 bytes if anything)
				::DrawText(charsBuffer,offset,dataLeftToDraw);
			}
			else
			{
				ReplaceInvisibles(charsBuffer,bytesPerRow);
				::DrawText(charsBuffer,0,bytesPerRow);
			}
		
			// Increase to the next row of hex
			rowStartOffset+=bytesPerRow;
			FOffset(thisRow,0,kRowHeight);
		}
		
		if (mEditing)
			DrawHilight();
	}
	Catch_(err)
	{
		SignalPStr_("\pError accessing data store in CHexEditorPane::DrawSelf()");
	}
	
	delete [] charsBuffer;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawCaret
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws the caret in it's current phase. If the phase is off then it's drawn in the background colour, else in the fore
// ground colour. Assumes draw is focussed
void CHexEditorPane::DrawCaret(
	TCaretState				inState)
{
	Try_
	{
		if (mSelStart==mSelEnd)
		{
			StRegion		rangeRgn(GetRangeRegion(mSelStart,mSelEnd+1),false);		// Quick but inefficient way of working out where the caret should be - draw on left side of selection range
			
			// Caret is visible, it will be either black, white or grey.
			switch (inState)
			{
				case kCaretOn:
					::RGBForeColour(&TColourPresets::kBlack);
					break;
				
				case kCaretOff:
					::RGBForeColour(&TColourPresets::kWhite);
					break;
					
				case kCaretDimmed:
					{
						RGBColour		temp=cDisabledFrame;
						::RGBForeColour(&temp);
					}
					break;
			}

			FocusDraw();
			
			Rect		rangeRgnRect=rangeRgn.Bounds();
			
			::MoveTo(rangeRgnRect.left-1,rangeRgnRect.top);
			::Line(0,rangeRgnRect.bottom-rangeRgnRect.top-1);
		}		
	}
	Catch_(err)
	{
	
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawHilight
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draw's the selection hilight as either an outline or a sold invert depending on whether this pane is the current target
// or not. Assumes focus of draw.
void CHexEditorPane::DrawHilight()
{
	Try_
	{
		if (mSelStart==mSelEnd)
			DrawCaret(mOfficialTargetStatus ? (mCaretPhase ? kCaretOn : kCaretOff) : kCaretDimmed);
		else
		{
			StRegion			theRegion(GetRangeRegion(mSelStart,mSelEnd),false);

			if (!mOfficialTargetStatus)
			{
				StRegion		copy(theRegion);
				
				::InsetRgn(copy,1,1);
				
				theRegion-=copy;
			}
			
			BetterInvertRgn(theRegion);
		}
	}
	Catch_(err)
	{
//		SignalPStr_("\pCHexEditorPane::DrawHilight() failed to draw hilight, exception thrown);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CalcMaxBytesPerRow
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the max number of bytes which can be display in hex form on a row given this panes font settings and width
CDataStore::TDataOffset	CHexEditorPane::CalcMaxBytesPerRow()
{
	SDimension16				frameSize;
	CDataStore::TDataOffset		result;
	SInt16						numBlocks,remainder;
	
	GetFrameSize(frameSize);

	frameSize.width-=(kLeftIndent+kRightIndent);

	if (frameSize.width<0)
		return 0;

	if (mDisplayAsHex)
	{
		numBlocks=frameSize.width/k4ByteHexBlockPixelWidth;
		remainder=frameSize.width%k4ByteHexBlockPixelWidth;
		
		result=numBlocks*4;
		
		if (remainder>=k2ByteHexBlockPixelWidth)
			result+=2;
	}
	else
	{
		numBlocks=frameSize.width/k4ByteAsciiBlockPixelWidth;
		remainder=frameSize.width%k4ByteAsciiBlockPixelWidth;
		
		result=numBlocks*4;
		
		if (remainder>=k2ByteAsciiBlockPixelWidth)
			result+=2;
	}
	
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetHexView
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the hex view which contains the data this pane relies on. By default it's the super view.
// Returns nil if can't find it.
CHexView *CHexEditorPane::GetHexView()
{
	return dynamic_cast<CHexView*>(GetSuperView());
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClickSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// When clicked on it becomes the target if editing is enabled. If editing is already in progress the click is further
// processed to allow selection.
void CHexEditorPane::ClickSelf(
	const SMouseDownEvent	&inMouseDown)
{
	CHexView			*view=GetHexView();
	
	if (!view)
		return;
	
	if (view->IsEditable())
	{
		if (!view->IsEditing())
		{
			view->StartEdit();		// Refreshes this pane
			SwitchTarget(this);		// Redraws this pane - don't need to refresh
		}
		else
		{
			if (SwitchTarget(this))
				ClickLoop(inMouseDown);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClickLoop
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// A loop which executes whilst the mouse is down. It handles drag selection of the selection.
// This really should implement proper mac UI guidelines with respect to shift clicks - double clicks etc but for now it's
// just move the cursor to where you click and extend if you drag.
void CHexEditorPane::ClickLoop(
	const SMouseDownEvent		&inMouseDown)
{
	CDataStore::TDataOffset		anchorPoint;
	CDataStore::TDataOffset		offset;
	Point						localMouse=inMouseDown.whereLocal;
	CHexView					*view=GetHexView();
	
	if (!view)
		return;

	offset=anchorPoint=PointToOffset(localMouse);

	do
	{
		view->SetSelection(Lesser(anchorPoint,offset),Greater(anchorPoint,offset));
		
		::GetMouse(&localMouse);
		
		offset=PointToOffset(localMouse);
	
	} while (StillDown());
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HandleKey
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Processes all input key presses, assumes key has been filtered.
// Also handles forward delete and backward delete and cursor movement.
void CHexEditorPane::HandleKey(
	unsigned char				inKey)
{
	CHexView					*parentView=GetHexView();
	CDataStore::TDataOffset		viewOffset=0xFFFFFFFF,rowBytes=CalcMaxBytesPerRow();
	
	if ((!parentView) || (!rowBytes))
		return;
	
	switch (inKey)
	{
		case char_LeftArrow:
			if (mSelStart!=mSelEnd)								// Selection?
				parentView->SetSelection(mSelStart,mSelStart);
			else if (mSelStart>0)
				parentView->SetSelection(mSelStart-1,mSelStart-1);
			viewOffset=mSelStart;
			break;
			
		case char_RightArrow:
			if (mSelStart!=mSelEnd)								// Selection?
				parentView->SetSelection(mSelEnd,mSelEnd);
			else if (mSelEnd<mDataStore->GetLength())
				parentView->SetSelection(mSelEnd+1,mSelEnd+1);
			viewOffset=mSelEnd;
			break;
			
		case char_UpArrow:
			if (mSelStart!=mSelEnd)
				parentView->SetSelection(mSelStart,mSelStart);
			else if (mSelStart>rowBytes)
				parentView->SetSelection(mSelStart-rowBytes,mSelStart-rowBytes);
			else
				parentView->SetSelection(0,0);
			break;
			
		case char_DownArrow:
			if (mSelStart!=mSelEnd)
				parentView->SetSelection(mSelEnd,mSelEnd);
			else if (mSelEnd<=(mDataStore->GetLength()-rowBytes))
				parentView->SetSelection(mSelEnd+rowBytes,mSelEnd+rowBytes);
			else
				parentView->SetSelection(mDataStore->GetLength(),mDataStore->GetLength());
			break;
		
		case char_Backspace:
			{
				if (mSelStart!=mSelEnd)
				{
					CDataStore::TDataOffset			start=mSelStart,end=mSelEnd;
				
					parentView->AdjustSelection(mSelStart,mSelStart);
					parentView->DeleteDataRange(start,end);
				}
				else if (mSelStart>0)
				{
					CDataStore::TDataOffset			delPos=mSelStart;

					parentView->AdjustSelection(mSelStart-1,mSelStart-1);
					parentView->DeleteDataRange(delPos-1,delPos);
				}
			}
			break;
		
		case char_Escape:
			GetHexView()->StopEdit(false);
			break;

		case char_Return:
			GetHexView()->StopEdit(true);
			break;
		
		default:
			{
				// insert the pressed key into the data
				if (mDisplayAsHex)
				{
					char		key=inKey;
				
					// Only valid hex chars get here
					CharToHex(&key);		// convert to a number between 0 and 15

					// if we're typing the second half of a nibble then get the first half and merge it
					if (mWaitingSecondNibble)
					{
						// Get previous nibble
						unsigned char		val;
						CDataStore::TDataOffset		size=1;
						
						ThrowIfOSErr_(mDataStore->GetBytes(mSelStart-1,&val,size));
						
						val<<=4;		// shift by 4
						val|=key;		// merge
						
						parentView->InsertData(mSelStart-1,mSelEnd,&val,1);
						
						mWaitingSecondNibble=false;
					}
					else
					{
						CDataStore::TDataOffset			start=mSelStart,end=mSelEnd;

						parentView->InsertData(start,end,(unsigned char*)&key,1);					
						parentView->AdjustSelection(mSelStart+1,mSelStart+1);
						mWaitingSecondNibble=true;
					}
				}
				else
				{
					CDataStore::TDataOffset			start=mSelStart,end=mSelEnd;

					parentView->InsertData(start,end,&inKey,1);
					parentView->AdjustSelection(mSelStart+1,mSelStart+1);
				}
			}
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HandleKeyPress
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This is ripped from LEditField with only minor modifications.
// ********* mark changes
// Basically stopped using typing actions.
Boolean CHexEditorPane::HandleKeyPress(
	const EventRecord&	inKeyEvent)
{
	if (!mEditing)
		return false;

	Boolean		keyHandled	 = true;
	EKeyStatus	theKeyStatus = keyStatus_Input;
	UInt16		theChar		 = (UInt16) (inKeyEvent.message & charCodeMask);
	LCommander	*theTarget	 = GetTarget();
	CHexView	*parent=GetHexView();															// *************
	
	if (!parent)
		return false;
	
	if (inKeyEvent.modifiers & cmdKey) {	// Always pass up when the command
		theKeyStatus = keyStatus_PassUp;	//   key is down
	
	} else {								// Hardcoded keyfilter								// ************

		theKeyStatus = keyStatus_PassUp;
		
		if (UKeyFilters::IsTEDeleteKey(inKeyEvent.message)) {
			theKeyStatus = keyStatus_TEDelete;
			
		} else if (UKeyFilters::IsTECursorKey(inKeyEvent.message)) {
			theKeyStatus = keyStatus_TECursor;
			
		} else if (UKeyFilters::IsExtraEditKey(inKeyEvent.message)) {
			theKeyStatus = keyStatus_ExtraEdit;
			
		} else if (UKeyFilters::IsPrintingChar(theChar)) {
		
			if ((!mDisplayAsHex) || (UKeyFilters::IsNumberChar(theChar) || (ToUpper(theChar)>='A' && ToUpper(theChar)<='F'))) {
				theKeyStatus = keyStatus_Input;
				
			} else {
				theKeyStatus = keyStatus_Reject;
			}
		}
		else if (UKeyFilters::IsActionKey(theChar))
			theKeyStatus=keyStatus_Input;
	}
	
	StFocusAndClipIfHidden	focus(this);
	
											// Save whether current selection
											//   is empty or not
	bool beforeSel = mSelStart==mSelEnd;														// *************
	
	switch (theKeyStatus) {
	
		case keyStatus_Input:
			HandleKey(theChar);
			UserChangedText();
			break;
			
		case keyStatus_TEDelete: {
			if (mSelEnd > 0) {																	// *************
				HandleKey(char_Backspace);
				UserChangedText();
			}
			break;
		}
			
		case keyStatus_TECursor: {
			HandleKey(theChar);
			break;
		}
			
		case keyStatus_ExtraEdit: {
			switch (theChar) {
				
				case char_Home:
					//::TESetSelect(0, 0, mTextEditH);
					parent->SetSelection(0,0);													// **************
					break;
					
				case char_End:
//					::TESetSelect(max_Int16, max_Int16, mTextEditH);
					parent->SetSelection(max_Int16,max_Int16);									// **************
					break;
					
				case char_FwdDelete:
					if (mSelStart < mDataStore->GetLength()) {						
						HandleKey(char_FwdDelete);
						UserChangedText();
					}
					break;
					
				default:
					keyHandled = LCommander::HandleKeyPress(inKeyEvent);
					break;
			}
			break;
		}
			
		case keyStatus_Reject:
			::SysBeep(1);
			break;
			
		case keyStatus_PassUp:
			keyHandled = LCommander::HandleKeyPress(inKeyEvent);
			break;
	}
	
		// If Target is the same before and after keystroke and
		// the selection has changed from empty to not empty or from
		// not empty to empty, we need to update menu commands. Some
		// command such as Cut and Copy depend on whether or not
		// any text is selected.
	
	if ( (theTarget == GetTarget()) &&
		 (beforeSel != (mSelStart == mSelEnd)) ) {
		 
		SetUpdateCommandStatus(true);
	}
	
	return keyHandled;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SpendTime
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Periodically changes the TE phase. Note assumes inMacEvent.when == LMGetTicks() for instantanous event. I mean in set
// selection mLastCaretToggleTime is loaded with LMGetTicks() and here it's loaded with inMacEvent.when so I'm saying I expect
// this to be reporting similar values in the same units etc.
void CHexEditorPane::SpendTime(
	const EventRecord&	inMacEvent)
{
	if (!mEditing)
		return;
		
	if ((mLastCaretToggleTime+kCaretFlashTime)<inMacEvent.when)
	{
		mLastCaretToggleTime=inMacEvent.when;
		mCaretPhase=!mCaretPhase;
		
		if (mOfficialTargetStatus)		// if we're not targetted the appearance of a caret doesn't change when it flashes
			DrawCaret(mCaretPhase ? kCaretOn : kCaretOff);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetEditStatus
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This is called to change from being an editable view to a non editable view.
// On setting the edit mode a box is drawn around it and the hilight/caret is shown.
void CHexEditorPane::SetEditStatus(
	bool		inEditing)
{
	mEditing=inEditing;
	
	if ((!mEditing) && IsTarget())
		SwitchTarget(GetSuperCommander());
	
	Refresh();
}
