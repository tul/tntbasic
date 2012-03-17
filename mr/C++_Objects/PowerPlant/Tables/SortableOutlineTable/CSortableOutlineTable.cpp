// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CSortableOutlineTable.cpp
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

/*
	Provides an implementation of LOutlineTable which is capable of sorting the contents. The sorting is done by a standard
	comparator and so can be done by different sort keys.
	When sorting the deepest groups are sorted first then thier parent groups all the way up to the top level table. That
	way the sorting routine can always rely on an element's sub group being sorted.
	By using derivatives of this table finder like list views can be produced quite easily.
*/
	
#include		"MR Debug.h"
#include		<LFastArrayIterator.h>
#include		<LOutlineItem.h>
#include		"CSortableOutlineTable.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CSortableOutlineTable::CSortableOutlineTable(LStream *inStream,LComparator *inComparator,Boolean inAdoptComparator) :
	LOutlineTable(inStream)
{
	mComparator=inComparator;
	mOwnsComparator=inAdoptComparator;
	
	mFirstLevelItems.SetComparator(mComparator,false);
	mFirstLevelItems.SetKeepSorted(true);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CSortableOutlineTable::~CSortableOutlineTable()
{
	if (mComparator && mOwnsComparator)
		delete mComparator;
}
	
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetComparator
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Use either this or the constructor to specify the comparator used to sort the table. If you tell the table to adopt the
// comparator then it will be responsible for disposing of it.
void CSortableOutlineTable::SetComparator(LComparator *inComparator,Boolean inAdoptComparator)
{
	if (mComparator && mOwnsComparator)
		delete mComparator;
	
	mComparator=inComparator;
	mOwnsComparator=inAdoptComparator;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InsertSortedItem
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Inserts an item into the table keeping the table sorted.
// Makes no attempt to sort the sub group of the item being added. If it has no sub group then thats fine.
void CSortableOutlineTable::InsertSortedItem(	LOutlineItem	*inOutlineItem,
												LOutlineItem	*inSuperItem,
												Boolean			inRefresh,
												Boolean			inAdjustImageSize)
{
	if (!inOutlineItem)
		return;
	
	LArray			*parentArray;
	LOutlineItem	*afterItem=0L;
	
	// Find which array represents the outline items list that this item will be placed in
	// This array is assumed to be sorted (an empty array counts as being sorted)
	if (inSuperItem)
		parentArray=(LArray*)inSuperItem->GetSubItems();	// Returns 0L if no array
	else
		parentArray=&mFirstLevelItems;

	if (parentArray)
	{
		ArrayIndexT		insertPosition=parentArray->FetchInsertIndexOf(&inOutlineItem,sizeof(void**));
	
		if (insertPosition!=1 && insertPosition!=LArray::index_Last)
			afterItem=*(LOutlineItem**)parentArray->GetItemPtr(insertPosition-1);
	}
	
	InsertItem(inOutlineItem,inSuperItem,afterItem,inRefresh,inAdjustImageSize);
	
	// If there was no parent array then this was the first item to be inserted into a parent item. The array will now
	// have been constructed. Set it's comparator, it's keep sorted flag and sort it. As it's only got one item this will
	// cause no change.
	if (!parentArray)
	{
		if (parentArray=(LArray*)inSuperItem->GetSubItems())
		{
			parentArray->SetComparator(mComparator,false);
			parentArray->SetKeepSorted(true);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊSnapshotSelection										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Saves the current selection into the array passsed
void CSortableOutlineTable::SnapshotSelection(LArray &inArray)
{
	STableCell		cell;
	UInt32			numRows,numCols;
	
	GetTableSize(numRows,numCols);
	
	for (cell.row=1; cell.row<=numRows; cell.row++)
	{
		if (CellIsSelected(cell))
		{
			LOutlineItem		*item=FindItemForRow(cell.row);
			inArray.AddItem(&item,sizeof(LOutlineItem*));
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊSelectItems											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Selects all the items in the list. The selection is not cleared first
void CSortableOutlineTable::SelectItems(LArray &inArray)
{
	LFastArrayIterator			indexer(inArray,LFastArrayIterator::from_Start);
	LOutlineItem				*item;
	STableCell					cell;
	
	while (indexer.Next(&item))
	{
		cell.row=FindRowForItem(item);
		SelectCell(cell);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Sort
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Uses LArray's Sort function to sort the list using the comparator you specify. The table is refreshed if you specify.
// The sort first sorts an items sub items before sorting the item itself.
// If there's not enough memory available to store the current selection in an Array the selection will be cleared.
void CSortableOutlineTable::Sort(Boolean inRefresh)
{
	// Snapshot the currently selected items, then deselect them
	LArray			theSelection;
	Try_
	{
		SnapshotSelection(theSelection);
	}
	Catch_(err)
	{
		// Not enough memory to transfere the selection, never mind.
	}
	
	// Clear the selection - it will be wrong once the items have been moved around
	UnselectAllCells();
	
	// Sort the table
	SortOutlineItemsArray(&mFirstLevelItems);

	// Now update the rows array which maps a row index onto an LOutlineItem*.
	// Assuming all the rows which were in the table, still are in the table (they'd better be or LTable view will need
	// the #rows altering) the memory for this array is already completely allocated. All we have to do is refill it with
	// the items in their new positions.
	ArrayIndexT			theIndex=1;
	RefillItemsArray(theIndex,&mFirstLevelItems);
	
	// Now reapply the selection
	SelectItems(theSelection);
	
	if (inRefresh)
		Refresh();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SortOutlineItemsArray
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This private routine is used by Sort.
// Performs a sort on one array of LOutlineItems. It recurses on the sub group of each item so that when the comparator is
// asked to compare groups they will be sorted. The comparator will only be asked to compare objects in the same group.
void CSortableOutlineTable::SortOutlineItemsArray(LArray *inItemsArrayToSort)
{
	// For each item in the array, first sort it's sub group
	LFastArrayIterator			indexer(*inItemsArrayToSort,LFastArrayIterator::from_Start);
	LOutlineItem				*item;
	
	while (indexer.Next(&item))
	{
		if (item->GetSubItems())
			SortOutlineItemsArray((LArray*)item->GetSubItems());
	}

	inItemsArrayToSort->InvalidateSort();
	inItemsArrayToSort->Sort();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RefillItemsArray
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This private routine is called by Sort.
// It refills an Outline tables rows array which maps a given row index onto a outlineitem object ptr. It refills this list
// from the items array you pass it, recursing into any items which have sub groups.
void CSortableOutlineTable::RefillItemsArray(ArrayIndexT &ioCurrentIndex,LArray *inItemsArray)
{
	LFastArrayIterator			indexer(*inItemsArray,LFastArrayIterator::from_Start);
	LOutlineItem				*item;

	while (indexer.Next(&item))
	{
		// First put the item itself into the rows list, then it's sub items
		mOutlineItems.AssignItemsAt(1,ioCurrentIndex++,&item,sizeof(item));
		if (item->GetSubItems())
			RefillItemsArray(ioCurrentIndex,(LArray*)item->GetSubItems());
	}
}


