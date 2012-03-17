// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CSortableOutlineTable.h
// ©ÊMark Tully and TNT Software 1998-1999
// Email : M.C.Tully@durham.ac.uk
// 14/9/98
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

#include						<LOutlineTable.h>

class							LComparator;
class							LStream;

typedef class CSortableOutlineTable : public LOutlineTable
{
	protected:
		LComparator				*mComparator;
		Boolean					mOwnsComparator;

		virtual void			SortOutlineItemsArray(
										LArray			*inItemsArrayToSort);
		virtual void			RefillItemsArray(
										ArrayIndexT		&ioCurrentIndex,
										LArray			*inItemsArray);

		// Disallowed function
		virtual void			InsertItem(
									LOutlineItem*		inOutlineItem,
									LOutlineItem*		inSuperItem,
									LOutlineItem*		inAfterItem,
									Boolean				inRefresh,
									Boolean				inAdjustImageSize)		{ LOutlineTable::InsertItem(inOutlineItem,inSuperItem,inAfterItem,inRefresh,inAdjustImageSize); }

	public:
		enum { class_ID = 'SRot' };

								CSortableOutlineTable(
										LStream*		inStream,
										LComparator		*inComparator=0L,
										Boolean			inAdoptComparator=true);
								~CSortableOutlineTable();
	
		virtual void			SetComparator(
										LComparator		*inComparator,
										Boolean			inAdoptComparator);
		virtual LComparator		*GetComparator()								{ return mComparator; }
		
		virtual void			Sort(
										Boolean			inRefesh);
		
		virtual void			SelectItems(
										LArray			&inArray);
		virtual void			SnapshotSelection(
										LArray			&inArray);

		virtual void			InsertSortedItem(
										LOutlineItem	*inOutlineItem,
										LOutlineItem	*inSuperItem,
										Boolean			inRefresh,
										Boolean			inAdjustImageSize);
} CSortableOutlineTable;