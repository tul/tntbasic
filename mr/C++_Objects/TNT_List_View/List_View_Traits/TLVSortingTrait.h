// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TLVSortingTrait.h
// Mark Tully
// 10/4/98
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

#include							"MR Debug.h"
#include							"TSortedLinkedList.h"
#include							"TListView.h"

template <class T>
class TLVSortingTrait : public T
{
	private:
	
	protected:
		TSortedLinkedList			*sortedListPtr;

	public:
									TLVSortingTrait(WindowPtr window,const Rect &rect,TSortedLinkedList::TListCompareProc compareProc,void *refCon=0L);
		virtual void				InitSortedLinkedList(TSortedLinkedList::TListCompareProc compareProc,void *refCon,unsigned long sortKey,Boolean ascending);
		
		virtual void /*e?*/			AttachOrderedCell(TListViewCellPtr cell);
		virtual void				SortList();

		// This can get you the list, nb, don't add/remove elements from this list, use AttachCell/DetachCell instead
		// Just use this for accessing refcon and sort keys of the list
		virtual TSortedLinkedList	&GetSortedLinkedList()	{ return *sortedListPtr; }

};

template <class T>
TLVSortingTrait<T>::TLVSortingTrait(WindowPtr window,const Rect &rect,TSortedLinkedList::TListCompareProc compareProc,void *refCon) : T(window,rect,0L,refCon)
{
	sortedListPtr=new TSortedLinkedList(compareProc);
	ThrowIfMemFull_(sortedListPtr);
	
	SpecifyList(sortedListPtr);
}

template <class T>
void TLVSortingTrait<T>::InitSortedLinkedList(TSortedLinkedList::TListCompareProc compareProc,void *refCon,unsigned long sortKey,Boolean ascending)
{	
	sortedListPtr->SetCompareRefCon(refCon);
	sortedListPtr->SetSortKey(sortKey);
	sortedListPtr->MakeAscending(ascending);
	sortedListPtr->SetCompareProc(compareProc);
}

template <class T>
void TLVSortingTrait<T>::AttachOrderedCell(TListViewCellPtr cell)
{
	sortedListPtr->AddOrderedElement(GetCellLink(cell));
	CellAttached(cell);
}

template <class T>
void TLVSortingTrait<T>::SortList()
{
	sortedListPtr->LinearSelectionSortList();

	RecalcCellDistances();

	UpdateListView();
}