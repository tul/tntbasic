// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CHierarchicalGeometry.h
// © Mark Tully and TNT Software 1998
// 16/12/98
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

#pragma once

#include <LTableMultiGeometry.h>
#include <LTableArrayStorage.h>

// ===========================================================================
//		¥ CHierarchicalGeometry
// ===========================================================================
// A variant of LTablePaneHostingGeometry that doesn't delete the panes when
// they're removed.

class CHierarchicalGeometry : public LTableMultiGeometry {

public:
							CHierarchicalGeometry(
									LTableView*		inTableView,
									UInt16			inColWidth,
									UInt16			inRowHeight);
	virtual					~CHierarchicalGeometry();

	// pane accessors
	
	virtual LPane*			GetCellPane(
									const STableCell&	inCell);
	virtual void			SetCellPane(
									const STableCell&	inCell,
									LPane*				inPane);

	// overrides to adjust pane location

	virtual void			InsertCols(
									UInt32				inHowMany,
									TableIndexT			inAfterCol);
	virtual void			InsertRows(
									UInt32				inHowMany,
									TableIndexT			inAfterRow);

	virtual void			RemoveCols(
									UInt32				inHowMany,
									TableIndexT			inFromCol);
	virtual void			RemoveRows(
									UInt32				inHowMany,
									TableIndexT			inFromRow);

	virtual void			SetColWidth(
									UInt16				inWidth,
									TableIndexT			inFromCol,
									TableIndexT			inToCol);
	virtual void			SetRowHeight(
									UInt16				inHeight,
									TableIndexT			inFromRow,
									TableIndexT			inToRow);

	// resizing helpers

protected:
	virtual void			MoveColumnsBy(
									TableIndexT			inStartCol,
									SInt16				inHorizDelta);
	virtual void			MoveRowsBy(
									TableIndexT			inStartRow,
									SInt16				inVertDelta);

	virtual void			MoveCellBy(
									const STableCell&	inCell,
									SInt16				inHorizDelta,
									SInt16				inVertDelta);
	virtual void			ResizeCellBy(
									const STableCell&	inCell,
									SInt16				inWidthDelta,
									SInt16				inHeightDelta);

	virtual void			RemoveFromGeometry(
									const STableCell	&inCell);

	LTableArrayStorage		mCellPanes;
};
