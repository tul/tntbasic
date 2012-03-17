// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CHierarchicalGeometry.cpp
// © Mark Tully and TNT Software 1998
// 19/12/98
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1998, Mark Tully and John Treece-Birch
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

#include "CHierarchicalGeometry.h"

// ===========================================================================
//		¥ SCellPaneInfo
// ===========================================================================
//	Private struct used to describe the cell-to-pane relationship.

struct SCellPaneInfo {

public:
	LPane*				mPane;
	SBooleanRect		mCellFrameBindings;

};

static SCellPaneInfo emptyCell = { nil, { false, false, false, false } };


// ===========================================================================

#pragma mark ¥¥¥ CHierarchicalGeometry ¥¥¥

// ---------------------------------------------------------------------------
//	¥ CHierarchicalGeometry()
// ---------------------------------------------------------------------------
//	Constructor

CHierarchicalGeometry::CHierarchicalGeometry(
	LTableView*		inTableView,
	UInt16			inColWidth,
	UInt16			inRowHeight)

: LTableMultiGeometry(inTableView, inColWidth, inRowHeight),
  mCellPanes(inTableView, sizeof (SCellPaneInfo))

{
}


// ---------------------------------------------------------------------------
//	¥ ~CHierarchicalGeometry
// ---------------------------------------------------------------------------
//	Destructor

CHierarchicalGeometry::~CHierarchicalGeometry()
{
}


// ===========================================================================

#pragma mark -
#pragma mark ¥¥ pane accessors

// ---------------------------------------------------------------------------
//	¥ GetCellPane
// ---------------------------------------------------------------------------
//	Return the pane that is installed in a cell.

LPane*
CHierarchicalGeometry::GetCellPane(
	const STableCell&	inCell)
{
	SCellPaneInfo info = emptyCell;
	UInt32 size = sizeof (info);
	mCellPanes.GetCellData(inCell, &info, size);
	return info.mPane;
}


// ---------------------------------------------------------------------------
//	¥ SetCellPane
// ---------------------------------------------------------------------------
//	Install a pane in a cell. The cell's current frame bindings are used
//	as the cell bindings. If a pane already exists in this cell, it is
//	deleted and replaced with the new pane. Pass nil for inPane to delete
//	any existing pane.
//
//	NOTE: The pane must already be installed in the table view and in the
//	correct location relative to the cell.

void
CHierarchicalGeometry::SetCellPane(
	const STableCell&	inCell,
	LPane*				inPane)
{

	// Make sure pane is already installed in table view.
	
	if (inPane->GetSuperView() != mTableView) {
		SignalPStr_("\pCell pane must already by installed in table view.");
		return;
	}
	
	// Delete any pane that may be in this cell now.
	
	RemoveFromGeometry(inCell);
	LPane* oldPane = GetCellPane(inCell);
	if ((oldPane != nil) && (oldPane != inPane)) {
		oldPane->Refresh();
	}
	
	// Now install the new pane.
	
	SCellPaneInfo info = { inPane, { false, false, false, false } };
	if (inPane != nil) {
		SBooleanRect noBinding = { false, false, false, false };
		inPane->GetFrameBinding(info.mCellFrameBindings);
		inPane->SetFrameBinding(noBinding);
	}
	mCellPanes.SetCellData(inCell, &info, sizeof (info));
	
}


// ===========================================================================

#pragma mark -
#pragma mark ¥¥ overrides to adjust pane locations

// ---------------------------------------------------------------------------
//	¥ InsertCols
// ---------------------------------------------------------------------------
//	Overriden to move panes in the columns to the right of the insertion.

void
CHierarchicalGeometry::InsertCols(
	UInt32			inHowMany,
	TableIndexT		inAfterCol)
{

	// Update the cell width array.

	LTableMultiGeometry::InsertCols(inHowMany, inAfterCol);
	
	// Update the cell pane array.
	
	mCellPanes.InsertCols(inHowMany, inAfterCol, &emptyCell, sizeof (emptyCell));
	
	// Update pane locations.
	
	MoveColumnsBy(inAfterCol + inHowMany, (SInt16) (inHowMany * mDefaultColWidth));
	
}


// ---------------------------------------------------------------------------
//	¥ InsertRows
// ---------------------------------------------------------------------------
//	Overriden to move panes below the insertion.

void
CHierarchicalGeometry::InsertRows(
	UInt32			inHowMany,
	TableIndexT		inAfterRow)
{

	// Update the cell height array.

	LTableMultiGeometry::InsertRows(inHowMany, inAfterRow);
	
	// Update the cell pane array.
	
	mCellPanes.InsertRows(inHowMany, inAfterRow, &emptyCell, sizeof (emptyCell));
	
	// Update pane locations.
	
	MoveRowsBy(inAfterRow + inHowMany, (SInt16) (inHowMany * mDefaultRowHeight));

}


// ---------------------------------------------------------------------------
//	¥ RemoveCols
// ---------------------------------------------------------------------------
//	Overriden to delete any panes in the columns being deleted, and to
//	move panes in columns to the right of the deletion.

void
CHierarchicalGeometry::RemoveCols(
	UInt32			inHowMany,
	TableIndexT		inFromCol)
{

	// Calculate the width of the removed columns.
	
	SInt32 deletedWidth = 0;
	TableIndexT column = inHowMany + inFromCol;
	while (--column >= inFromCol) {
		UInt16 width = 0;
		mColWidths.FetchItemAt((ArrayIndexT) column, &width);
		deletedWidth += width;
	}

	// Delete affected panes.
	
	TableIndexT rows, cols;
	mTableView->GetTableSize(rows, cols);
	
/*	STableCell cell;
	for (cell.row = 1; cell.row <= rows; cell.row++) {
		for (cell.col = inFromCol; cell.col < inFromCol + inHowMany; cell.col++) {
			
			// The TableView's CellToIndex routine will be calculating
			// based on the new number of columns. Since we haven't deleted
			// the columns from our array storage yet, we have to compensate
			// for the deleted columns.
			
			STableCell oldCellIndex = cell;
			oldCellIndex.col += (oldCellIndex.row-1) * inHowMany;
			
			LPane* oldPane = GetCellPane(oldCellIndex);
			if (oldPane != nil)
				delete oldPane;
		}
	}*/
	
	// Update the cell width array.

	LTableMultiGeometry::RemoveCols(inHowMany, inFromCol);
	
	// Update the cell pane array.
	
	mCellPanes.RemoveCols(inHowMany, inFromCol);
	
	// Update pane locations.
	
	MoveColumnsBy(inFromCol, (SInt16) -deletedWidth);

}


// ---------------------------------------------------------------------------
//	¥ RemoveRows
// ---------------------------------------------------------------------------
//	Overriden to delete any panes in the columns being deleted, and to
//	move panes in columns to the right of the deletion.

void
CHierarchicalGeometry::RemoveRows(
	UInt32				inHowMany,
	TableIndexT			inFromRow)
{

	// Calculate the height of the removed rows.
	
	SInt32 deletedHeight = 0;
	TableIndexT row = inHowMany + inFromRow;
	while (--row >= inFromRow) {
		UInt16 height = 0;
		mRowHeights.FetchItemAt((ArrayIndexT) row, &height);
		deletedHeight += height;
	}

	// Delete affected panes.
	
	TableIndexT rows, cols;
	mTableView->GetTableSize(rows, cols);
	
	STableCell cell;
	for (cell.col = 1; cell.col <= cols; cell.col++) {
		for (cell.row = inFromRow; cell.row < inFromRow + inHowMany; cell.row++) {
			RemoveFromGeometry(cell);
		}
	}
	
	// Update the cell height array.

	LTableMultiGeometry::RemoveRows(inHowMany, inFromRow);
	
	// Update the cell pane array.
	
	mCellPanes.RemoveRows(inHowMany, inFromRow);
	
	// Update pane locations.
	
	MoveRowsBy(inFromRow, (SInt16) -deletedHeight);

}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RemoveFromGeometry
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Restores the pane bindings of a cell to what they were before it was installed in the pane. Called when a pane is being
// removed from the geometry.
void CHierarchicalGeometry::RemoveFromGeometry(
	const STableCell		&inCell)
{
	SCellPaneInfo info = emptyCell;
	UInt32 size = sizeof (info);
	mCellPanes.GetCellData(inCell, &info, size);

	if (info.mPane)
		info.mPane->SetFrameBinding(info.mCellFrameBindings);
}

// ---------------------------------------------------------------------------
//	¥ SetColWidth
// ---------------------------------------------------------------------------
//	Overriden to resize any cells in the affected columns and move cells
//	in the columns to the right of the resizing.

void
CHierarchicalGeometry::SetColWidth(
	UInt16				inWidth,
	TableIndexT			inFromCol,
	TableIndexT			inToCol)
{

	// Resize panes in affected columns.
	
	TableIndexT rows, cols;
	mTableView->GetTableSize(rows, cols);
	
	SInt32 sumDelta = 0;
	STableCell cell;
	for (cell.col = inFromCol; cell.col <= inToCol; cell.col++) {
		SInt32 widthDelta = inWidth - GetColWidth(cell.col);
		for (cell.row = 1; cell.row <= rows; cell.row++) {
			MoveCellBy(cell, (SInt16) sumDelta, 0);
			ResizeCellBy(cell, (SInt16) widthDelta, 0);
		}
		sumDelta += widthDelta;
	}

	// Update the column width array.

	LTableMultiGeometry::SetColWidth(inWidth, inFromCol, inToCol);
	
	// Move panes in rows after affected columns.
	
	MoveColumnsBy(inToCol + 1, (SInt16) sumDelta);
	
}


// ---------------------------------------------------------------------------
//	¥ SetRowHeight
// ---------------------------------------------------------------------------
//	Overriden to resize any cells in the affected rows and move cells
//	in the rows below the resizing.

void
CHierarchicalGeometry::SetRowHeight(
	UInt16				inHeight,
	TableIndexT			inFromRow,
	TableIndexT			inToRow)
{

	// Resize panes in the affected rows.
	
	TableIndexT rows, cols;
	mTableView->GetTableSize(rows, cols);
	
	SInt32 sumDelta = 0;
	STableCell cell;
	for (cell.row = inFromRow; cell.row <= inToRow; cell.row++) {
		SInt32 heightDelta = inHeight - GetRowHeight(cell.row);
		for (cell.col = 1; cell.col <= cols; cell.col++) {
			MoveCellBy(cell, 0, (SInt16) sumDelta);
			ResizeCellBy(cell, 0, (SInt16) heightDelta);
		}
		sumDelta += heightDelta;
	}


	// Adjust the row height array.

	LTableMultiGeometry::SetRowHeight(inHeight, inFromRow, inToRow);

	// Move panes in rows after the change.
	
	MoveRowsBy(inToRow + 1, (SInt16) sumDelta);

}


// ===========================================================================

#pragma mark -
#pragma mark ¥¥ resizing helpers

// ---------------------------------------------------------------------------
//	¥ MoveColumnsBy											[protected]
// ---------------------------------------------------------------------------
//	Move all cell panes in columns to the right of a specified column.

void
CHierarchicalGeometry::MoveColumnsBy(
	TableIndexT		inStartCol,
	SInt16			inHorizDelta)
{
	TableIndexT rows, cols;
	mTableView->GetTableSize(rows, cols);
	
	STableCell cell;
	
	for (cell.row = 1; cell.row <= rows; cell.row++) {
		for (cell.col = inStartCol; cell.col <= cols; cell.col++) {
			MoveCellBy(cell, inHorizDelta, 0);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ MoveRowsBy											[protected]
// ---------------------------------------------------------------------------
//	Move all cell panes in rows below a specified column.

void
CHierarchicalGeometry::MoveRowsBy(
	TableIndexT		inStartRow,
	SInt16			inVertDelta)
{
	TableIndexT rows, cols;
	mTableView->GetTableSize(rows, cols);
	
	STableCell cell;
	
	for (cell.row = inStartRow; cell.row <= rows; cell.row++) {
		for (cell.col = 1; cell.col <= cols; cell.col++) {
			MoveCellBy(cell, 0, inVertDelta);
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ MoveCellBy											[protected]
// ---------------------------------------------------------------------------
//	If a cell pane exists for this cell, move it by the specified amount.

void
CHierarchicalGeometry::MoveCellBy(
	const STableCell&	inCell,
	SInt16				inHorizDelta,
	SInt16				inVertDelta)
{
	LPane* cellPane = GetCellPane(inCell);
	if (cellPane != nil)
		cellPane->MoveBy(inHorizDelta, inVertDelta, false);
}


// ---------------------------------------------------------------------------
//	¥ ResizeCellBy											[protected]
// ---------------------------------------------------------------------------
//	If a cell pane exists for this cell, resize it by the specified amount.

void
CHierarchicalGeometry::ResizeCellBy(
	const STableCell&	inCell,
	SInt16				inCellWidthDelta,
	SInt16				inCellHeightDelta)
{
	
	// Get the cell information. If no pane, nothing needs to be done.

	SCellPaneInfo info;
	UInt32 size = sizeof (info);
	mCellPanes.GetCellData(inCell, &info, size);

	if (info.mPane == nil)
		return;
	
	// Resize or move the pane as if the superview itself was resizing.
	// Code adapted from LPane::AdaptToSuperFrameSize.
	
	// When a cell changes size, the cell pane may move or change
	// size, depending on how each of its sides is "bound" to
	// that of its cell. A side that is bound always remains
	// the same distance from the corresponding side of its cell.
		
	SInt32 widthDelta = 0;
	SInt32 heightDelta = 0;
	SInt32 horizDelta = 0;
	SInt32 vertDelta = 0;

	if (info.mCellFrameBindings.right) {
		if (info.mCellFrameBindings.left) {

			// Both right and left are bound. Pane resizes horizontally.

			widthDelta = inCellWidthDelta;

		} else {

			// Right bound, left free. Pane moves horizontally.

			horizDelta = inCellWidthDelta;
		}
	}
	
	if (info.mCellFrameBindings.bottom) {
		if (info.mCellFrameBindings.top) {

			// Both bottom and top are bound. Pane resizes vertically.

			heightDelta = inCellHeightDelta;

		} else {

			// Bottom bound, left free. Pane moves vertically.

			vertDelta = inCellHeightDelta;
		}
	}

	// Now move or resize as requested.
	
	if ((widthDelta != 0) || (heightDelta != 0))
		info.mPane->ResizeFrameBy((SInt16) widthDelta, (SInt16) heightDelta, false);
	
	if ((horizDelta != 0) || (vertDelta != 0))
		info.mPane->MoveBy(horizDelta, vertDelta, false);

}
