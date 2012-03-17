// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CImageTable.cpp
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

/*
	A table class which allows drag and drop of arbitary image cells. Cells can be reordered and reflowed when the table is
	resized.
	
	The table supports 2 modes, either single column mode or wrapping mode. In single column mode there's only a single
	column (how about that?). In multicolumn mode the cells flow from left to right and rewrap when the table is resized
	(like in a word processor). Rows are added/removed as needed according to how many cells there are.
	
	Don't alter num rows/cols yourself cos that will screw the table up; cos it maps a continuous stream of data to cells
	and breaking its mapping won't be a good thing.
*/

#include		"TNT_Debugging.h"
#include		"CImageTable.h"
#include		<LTableMonoGeometry.h>
#include		"Marks Routines.h"
#include		"Utility Objects.h"
#include		"CStdDragAndDrop.h"
#include		"CImageTableSelector.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CImageTable::CImageTable(
	LStream*	inStream) :
	LTableView(inStream),
	mIBarLoc(0),
	mIBarOnRhs(false)
{
	SInt32			colMode,dragMode,canDupeCells;
	
	*inStream >> colMode >> dragMode >> canDupeCells >> mCellWidth >> mCellHeight >> mTextTraitsId;
	
	mColMode=(EColumnMode)colMode;
	mDragMode=(EDragAndDropMode)mDragMode;
	mSupportsCellDupe=canDupeCells!=0;
	mCustomHilite=true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊDestructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Delete all cells
CImageTable::~CImageTable()
{
	ClearTable();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊClearTable
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Resets the table to what it was when it was constructed
void CImageTable::ClearTable()
{
	STableCell			cell;
	CImageTableSelector	*its=dynamic_cast<CImageTableSelector*>(mTableSelector);
	
	AssertThrow_(its);
	
	for (cell.col=1; cell.col<=mCols; cell.col++)
	{
		for (cell.row=1; cell.row<=mRows; cell.row++)
		{
			CImageCell	*ic=GetCellImage(cell);
			its->CellDied(ic);
			delete ic;
		}
	}
	
	if (mRows)	
		RemoveRows(mRows,1,false);
	if (mColMode==kSingleVColumn)
	{
		if (mCols>1)
			RemoveCols(mCols-1,2,false);
	}
	else
	{
//		if (mCols)	// leave the cols in place
//			RemoveCols(mCols,1,false);
	}
		
	mImageCells.RemoveAllItemsAfter(0);
	Refresh();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FinishCreateSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Create the storage and geometry for the table
void CImageTable::FinishCreateSelf()
{
	SetTableGeometry(new LTableMonoGeometry(this, mCellWidth, mCellHeight));
	SetTableSelector(new CImageTableSelector(this));
	
	CImageCell		*null=0l;
	
	if (mColMode==kSingleVColumn)
		InsertCols(1,0,&null,sizeof(null),false);	// a single column
	else
		ReflowCells();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReflowCells
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adds/removes columns and reflows the cells within
void CImageTable::ReflowCells()
{
	Rect		frame;
	
	if (!CalcLocalFrameRect(frame))
		return;
	
	TableIndexT	numCols=(frame.right-frame.left)/mCellWidth;
	TableIndexT	wasCols=mCols;
	
	if (numCols==0)
		numCols=1;
	
	if (numCols==wasCols)
		return; // no change

	// number of columns has increased	
	if (numCols>wasCols)
	{
		// columns increased, add more cols on the right hand size and then move all the cells up
		CImageCell	*null=0;
		InsertCols(numCols-wasCols,mCols,&null,sizeof(null),false);
	}
	else
		RemoveCols(wasCols-numCols,mCols-(wasCols-numCols),false);

	// that's all the cells shifted, now dump any extra rows from the bottom of the table which are no longer in use
	TableIndexT	numRows=CountImageCells()/numCols;		
	if (CountImageCells()%numCols)
		numRows++;
	
	if (numRows<mRows)
		RemoveRows(mRows-numRows,mRows-(mRows-numRows),false);
	else if (numRows>mRows)
		InsertRows(numRows-mRows,mRows,false);
	
	Refresh();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ResizeFrameBy
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Overriden to make the name coloumn wider
void CImageTable::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	if (mColMode==kSingleVColumn)
	{
		inheritedTable::ResizeFrameBy(inWidthDelta,inHeightDelta,false);	// Don't refresh as SetColWidth does it
		
		SInt16		newWidth=GetColWidth(1)+inWidthDelta;

		// keep the cells square
		SetColWidth(newWidth,1,1);
		SetRowHeight(newWidth,1,mRows);
	}
	else
	{
		inheritedTable::ResizeFrameBy(inWidthDelta,inHeightDelta,inRefresh);
		ReflowCells();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetCellImage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns nil if cell out of range, else it's the image cell in question. Image cell can be null if empty cell
CImageCell *CImageTable::GetCellImage(
	const STableCell	&inCell) const
{
	CImageCell		*imageCell=0;
	TableIndexT		index;

	CellToIndex(inCell,index);

	mImageCells.FetchItemAt(index,imageCell);

	return imageCell;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetCellImage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the cell, as soon as a cell is in the table the table owns it
void CImageTable::SetCellImage(
	const STableCell	&inCell,
	CImageCell			*inIC)
{
	CImageCell		*wasCell=0;
	TableIndexT		index;

	CellToIndex(inCell,index);

	if (index && (index<=mImageCells.GetCount()))
	{
		wasCell=mImageCells[index];
		
		if (wasCell)
		{
			CImageTableSelector		*its=dynamic_cast<CImageTableSelector*>(mTableSelector);
			if (its)
				its->CellDied(wasCell);
		}
		
		delete wasCell;
		mImageCells[index]=inIC;

		if (inIC)
			inIC->mImageTable=this;
	}
	else
	{
		SignalString_("CImageTable::SetCellImage() called on non existant cell");
		delete inIC;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReleaseCellImage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the image cell from a cell location and then stores null in the location. Null values can be returned for empty
// cells
CImageCell *CImageTable::ReleaseCellImage(
	const STableCell	&inCell)
{
	TableIndexT		index;
	CImageCell		*wasCell=0;

	CellToIndex(inCell,index);

	if (index && (index<=mImageCells.GetCount()))
	{
		wasCell=mImageCells[index];
		mImageCells[index]=0;
	}

	if (wasCell)
	{
		CImageTableSelector		*its=dynamic_cast<CImageTableSelector*>(mTableSelector);
		if (its)
			its->CellDied(wasCell);
	}

	return wasCell;	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FindCellFor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the cell for an image, null for none
void CImageTable::FindCellFor(
	CImageCell			*inIm,
	STableCell			&outCell)
{
	outCell.SetCell(0,0);
	
	ArrayIndexT			max=mImageCells.GetCount();
	
	while (max--)
	{
		if (mImageCells[max+1]==inIm)
		{
			IndexToCell(max+1,outCell);
			return;
		}
	}
}
	
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawCell
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CImageTable::DrawCell(
	const STableCell		&inCell,
	const Rect				&inLocalRect)
{
	CImageCell			*imageC=GetCellImage(inCell);
	
	if (imageC)
		imageC->DrawCell(inCell,inLocalRect,CellIsSelected(inCell),IsEnabled());
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawBackground
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Erases in white
void CImageTable::DrawBackground()
{
	Rect	frame;
	CalcLocalFrameRect(frame);
	
	{
		UForeColourSaver		safe(TColourPresets::kWhite);
		
		::PaintRect(&frame);
	}	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ApplyForeAndBackColors
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Black and white baby
void CImageTable::ApplyForeAndBackColors() const
{
	::RGBForeColor(&TColourPresets::kBlack);
	::RGBBackColor(&TColourPresets::kWhite);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Overridden to erase areas outside of cells
void CImageTable::DrawSelf()
{
	Rect			eraseRect1,eraseRect2;
	Rect			cellRect;

	// Get the frame of the list	as of pp 2.2 the entire table is erased by DrawBackground
/*	if (!CalcLocalFrameRect(eraseRect1))
		return;
	eraseRect2=eraseRect1;

	ApplyForeAndBackColors();
			
	GetLocalCellRect(STableCell(mRows,mCols),cellRect);
	eraseRect1.top=cellRect.bottom;
	::EraseRect(&eraseRect1);
	
	eraseRect2.left=cellRect.right;
	::EraseRect(&eraseRect2);
	
	// finally we must erase a section of the last row
	TableIndexT		numEmptyOnLastRow=mCols-(CountImageCells()%mCols);
	
	cellRect.left=cellRect.right-numEmptyOnLastRow*mCellWidth;
	
	::EraseRect(&cellRect);*/

	inheritedTable::DrawSelf();
	
	if (GetInsertBar())	// update the i bar
		DrawIBar(mIBarLoc,mIBarOnRhs,true);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HiliteCellActively
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CImageTable::HiliteCellActively(
	const STableCell	&inCell,
	Boolean				inHilite)
{
	Rect	rect;
	if (GetLocalCellRect(inCell,rect) && FocusExposed())
	{
		CImageCell			*imageC=GetCellImage(inCell);
		
		if (imageC)
			imageC->DrawCell(inCell,rect,inHilite,true);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HiliteCellInactively
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CImageTable::HiliteCellInactively(
	const STableCell	&inCell,
	Boolean				inHilite)
{
	Rect	rect;
	if (GetLocalCellRect(inCell,rect) && FocusExposed())
	{
		CImageCell			*imageC=GetCellImage(inCell);
		
		if (imageC)
			imageC->DrawCell(inCell,rect,inHilite,false);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Click													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Dispatches a click on this cell, calls double click if it's a double click.
void CImageCell::Click(
	const STableCell			&inCell,
	const SMouseDownEvent		&inMouseDown)
{
	Boolean wasSelected = mImageTable->CellIsSelected(inCell);
	LTableSelector		*ts=mImageTable->GetTableSelector();
	
	AssertThrow_(ts);

	if (!wasSelected)
	{
		ts->ClickSelect(inCell,inMouseDown);
		mImageTable->UpdatePort();
	}
	
	// See if mouse moves. If it does, track a drag from it
	if (::StillDown() &&
		LDragAndDrop::DragAndDropIsPresent() &&
		::WaitMouseMoved(inMouseDown.macEvent.where)) {
		mImageTable->TrackDrag(inCell, inMouseDown);
		return;
	}

	// Mouse didn't move, see if this was a double-click.	
	if (LPane::GetClickCount() > 1)
	{
		DoubleClick(inCell, inMouseDown);
		return;
	}

	if (wasSelected)
		ts->ClickSelect(inCell,inMouseDown);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClickSelf												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Passes the click to the cell
void CImageTable::ClickSelf(
	const SMouseDownEvent&	inMouseDown)
{
	STableCell				hitCell;
	SPoint32				imagePt;
	
	LocalToImagePoint(inMouseDown.whereLocal, imagePt);
	
	if (GetCellHitBy(imagePt, hitCell))
	{
		CImageCell	*image=GetCellImage(hitCell);
		
		if (image)
			image->Click(hitCell,inMouseDown);
	}
//	else
//		TrackEmptyClick(hitCell, inMouseDown, emptyDrawInfo);		// click is outside all cells, try drag selection
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DeleteCell
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Removes the cell specified and deletes the image
void CImageTable::DeleteCell(
	const STableCell	&inCell,
	bool				inRefresh)
{
	if (!IsValidCell(inCell))
		return;

	UnselectCell(inCell);

	TableIndexT	index;

	if (mColMode==kSingleVColumn)
	{
		SetCellImage(inCell,0);	// clear the cell
		CellToIndex(inCell,index);
		mImageCells.RemoveItemsAt(1,index);
		RemoveRows(1,inCell.row,inRefresh);
	}
	else
	{	
		SetCellImage(inCell,0);	// clear the cell
		CellToIndex(inCell,index);
		mImageCells.RemoveItemsAt(1,index);
		ReflowCells();
	}	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InsertImageCell
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Inserts an image at the specified position. inIndex is 1 based
void CImageTable::InsertImageCell(
	CImageCell		*inImageCell,
	TableIndexT		inIndex,
	bool			inRefresh)
{
	CImageCell			*null=0;

	AssertThrow_(mCols);
	
	mImageCells.InsertItemsAt(1,inIndex,inImageCell);
	if (inImageCell)
		inImageCell->mImageTable=this;

	if (mColMode==kSingleVColumn)
		InsertRows(1,inIndex-1,&null,sizeof(null),inRefresh);
	else
	{
		// insert another row to the table if the bottom row is full.
		ArrayIndexT	rowsInUse=CountImageCells()/mCols;
		if (CountImageCells()%mCols)
			rowsInUse++;
		
		if (rowsInUse>mRows)
			InsertRows(1,mRows,&null,sizeof(null),inRefresh);			
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CountImageCells
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the number of image cells currently in use. If there are null cells in the table they're still counted.
TableIndexT	CImageTable::CountImageCells() const
{
	return mImageCells.GetCount();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CountSelectedImageCells
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TableIndexT	CImageTable::CountSelectedImageCells() const
{
	STableCell		cell;
	TableIndexT		count=0;
	
	while (GetNextSelectedCell(cell))
		count++;
		
	return count;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetCellImage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The index is converted into a cell spec by assuming left to right, top to bottom index ordering.
// Index is 1 based. Remember that the result can be nil.
CImageCell *CImageTable::GetCellImage(
	ArrayIndexT	inIndex) const
{
	STableCell		cell;
	
	IndexToCell(inIndex,cell);

	return GetCellImage(cell);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ TrackDrag
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CImageTable::TrackDrag(
	const STableCell&		/* inCell */,
	const SMouseDownEvent&	/* inMouseDown */)
{
	// override hook
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MakeDragRegion							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CImageTable::MakeDragRegion(
	DragReference		inDragRef,
	RgnHandle			inRegion)
{
	StRegion			globalRegion(inRegion,false);
	
	globalRegion.Release();	// Stops it disposing of the region

	// Can't use GetNextSelectedCell as it returns multiple for a single row as a row has 2 cells on it
	ArrayIndexT			max=CountImageCells();
	STableCell			cell;
	Rect				cellRect;
	
	while (GetNextSelectedCell(cell))
	{
		CImageCell		*cellImage=GetCellImage(cell);
		
		if (cellImage && GetLocalCellRect(cell,cellRect))
			cellImage->GetDragRegion(cell,cellRect,globalRegion);
	}

	// convert region to global coords
	Point	zero={0,0};

	LocalToPortPoint(zero);
	PortToGlobalPoint(zero);
	::OffsetRgn(globalRegion,zero.h,zero.v);

	// Clip the drag rgn to the visible part of the table so the image doesn't grab data from outside of our pane
	Rect		clipRect;
	
	if (CalcPortExposedRect(clipRect))
	{
		PortToGlobalPoint(topLeft(clipRect));
		PortToGlobalPoint(botRight(clipRect));
		globalRegion&=clipRect;
	}

	// Get the drag image
	Point		imageOffsetPt={0,0};
		
#if TARGET_API_MAC_CARBON
	if (UEnvironment::IsRunningOSX())
	{
		// Under OS X, I can't see a way to use the contents of my window as the drag image, don't think it's allowed
		// I think we'd need to open a separate gworld for storing the image in?
	}
	else
	{
		PixMapHandle	map;

		map=::GetPortPixMap(GetMacPort());
		::SetDragImage(inDragRef,map,globalRegion,imageOffsetPt,0L);
	}
#else
	::SetDragImage(inDragRef,((CWindowPtr)GetMacPort())->portPixMap,globalRegion,imageOffsetPt,0L);
#endif
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetDragRegion							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Gets the default drag region for an image cell which is simply the cell rect
// The region is in local coords, assume the view is correctly focussed
void CImageCell::GetDragRegion(
	const STableCell	&inCell,
	const Rect			&inCellRect,
	StRegion			&outLocalRgn)
{
	Rect	temp=inCellRect;
	
	outLocalRgn+=temp;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShowInsertBar
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Takes the point and figures where the ibar should go
void CImageTable::ShowInsertBar(
	Point		inPoint)
{
	if (mColMode!=kWrappingColumns)
		SignalString_("Support for single column ibar unimplemented");
	
	// calc where the ibar should go
	SPoint32	imagePoint;
	STableCell	cell;
	bool		rhs=false;			// should the ibar go on the left or right hand side of the cell we're calculating?
	
	LocalToImagePoint(inPoint,imagePoint);

	cell.row=imagePoint.v/mCellHeight+1;
	cell.col=imagePoint.h/mCellWidth+1;
		
	if ((imagePoint.h%mCellWidth)>(mCellWidth/2))		// if it's more than half way then round up
		rhs=true;

	// Now get the cell index
	TableIndexT	index;
	CellToIndex(cell,index);
	
	// If they point to null space of the rhs of the table then make sure the insert point is placed on the previous line
	// rather than wrapping to the beginning of the next line
	if (cell.col>mCols)
	{
		rhs=true;
		index--;
	}
	
	// off the end of the table
	if ((index+(rhs ? 1 : 0))>CountImageCells())
	{
		index=CountImageCells();
		rhs=true;
	}

	// Has the cell index changed?
	if ((index+(rhs ? 1 : 0))!=(mIBarLoc+(mIBarOnRhs ? 1 : 0)))
	{
		ClearInsertBar();
		mIBarLoc=index;
		mIBarOnRhs=rhs;
		
		DrawIBar(mIBarLoc,mIBarOnRhs,true);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawIBar
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Low level ibar rendering routine. If the hilite is on pass true
void CImageTable::DrawIBar(
	TableIndexT		inIndex,
	bool			inOnRhs,
	bool			inOn)
{
	if (!FocusDraw())
		return;

	// Draw the insert bar at the specified loc
	Rect		rect;
	STableCell	cell;
	Rect		frameRect;
	
	IndexToCell(inIndex,cell);
	
	GetLocalCellRect(cell,rect);
	
	CalcLocalFrameRect(frameRect);
	
	// if hiliting is on mask it off
	{
		CStdDragAndDrop		*dropArea=dynamic_cast<CStdDragAndDrop*>(this);
		
		if (dropArea && dropArea->IsHilited())
			FInsetRect(frameRect,kDnDBorderWidth,kDnDBorderWidth);
	}
	
	if (inOnRhs)
	{
		rect.left=rect.right-kIBarWidth;
		FOffset(rect,kIBarWidth/2,0);			// put the ibar right on the line

		if (rect.right>=frameRect.right)
			::OffsetRect(&rect,-(rect.right-frameRect.right),0);
	}
	else
	{
		rect.right=rect.left+kIBarWidth;
		FOffset(rect,-kIBarWidth/2,0);		// put the ibar right on the line

		if (rect.left<frameRect.left)		// drawing flush with left side, will interfere with d&d border, must avoid that
			::OffsetRect(&rect,frameRect.left-rect.left,0);
	}
	
	if (rect.top<frameRect.top)
		rect.top+=frameRect.top-rect.top;

	if (rect.bottom>=frameRect.bottom)
		rect.bottom-=rect.bottom-frameRect.bottom;		
		
	if (inOn)
	{
		::RGBForeColour(&TColourPresets::kBlack);

#if TARGET_API_MAC_CARBON
		Pattern		grey;
		
		::PenPat(::GetQDGlobalsGray(&grey));
#else
		::PenPat(&qd.gray);
#endif
		::PaintRect(&rect);
		::PenNormal();
	}
	else
	{
		RefreshRect(rect);
		UpdatePort();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClearInsertBar
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Removes the ibar from the table
void CImageTable::ClearInsertBar()
{
	if (!(mIBarLoc || mIBarOnRhs))
		return;

	TableIndexT		temp=mIBarLoc;
	bool			temp2=mIBarOnRhs;

	mIBarLoc=0;			// clear these before erasing the ibar otherwise it doesn't actually get erased as the update routine
	mIBarOnRhs=false;	// just redraws it
		
	DrawIBar(temp,temp2,false);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetInsertBar
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the ibar location from the last drop. Returns 0 for none, else it's the index that the dropped item should
// occupy
TableIndexT	CImageTable::GetInsertBar()
{
	return mIBarLoc + (mIBarOnRhs ? 1 : 0);
}
