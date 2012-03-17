// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CHierarchicalTable.h
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

#include		<LTableView.h>
#include		"CHierarchicalGroup.h"

class			LStream;

class CHierarchicalTable : public LTableView, public virtual CHierarchicalGroup
{
	friend void CHierarchicalElement::GetElementCell(STableCell &outCell);;

	protected:
		typedef LTableView		inheritedTable;

		UInt16					mRowHeight;
		
		virtual void			CalcInsertPosition(
									CHierarchicalElement	*inElement,
									STableCell				&outPosition);
	
		virtual void /*e*/		InsertElementIntoTable(
									CHierarchicalElement							*inElement,
									CHierarchicalGroup::TVisibilityOperation		inOp);
		virtual void			RemoveElementFromTable(
									CHierarchicalElement							*inElement,
									CHierarchicalGroup::TVisibilityOperation		inOp);

		virtual void			AdjustColToFrame();
		virtual void			ResizeFrameBy(
									SInt16		inWidthDelta,
									SInt16		inHeightDelta,
									Boolean		inRefresh);
	
		virtual void			ClickCell(
									const STableCell		&inCell,
									const SMouseDownEvent	&inMouseDown);

		virtual void			FitPaneToCell(
									LPane						*inPane,
									STableCell					&inCell);
		virtual void			PutInTable(
									CHierarchicalElement		*inElement,
									STableCell					&inCell);
		virtual void			RemoveFromTable(
									CHierarchicalElement		*inElement);
	public:
		enum 					{ class_ID = FOUR_CHAR_CODE('hrTb') };

		/*e*/					CHierarchicalTable(
									LStream						*inStream);

		virtual void			HiliteSelection(
									Boolean						inActively,
									Boolean						inHilite);
		virtual void			HiliteCell(
									const STableCell			&inCell,
									Boolean						inHilite);

		virtual void /*e*/		FinishCreateSelf();

		virtual void /*e*/		GetSelection(
									CLinkedListT<CHierarchicalElement>				&outSelection,
									bool											inRemoveSubElements);

		virtual void /*e*/		RemoveSubElements(
									CLinkedListT<CHierarchicalElement>				&ioSelection);

		virtual void /*e*/		SubElementVisibilityChanged(
									CHierarchicalElement							*inElement,
									CHierarchicalGroup::TVisibilityOperation		inOp);

		virtual LPane			*GetCellPane(
									const STableCell		&inCell) const;

		virtual CHierarchicalElement *GetCellElement(
									const STableCell		&inCell) const;
};