// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked List Library
// TListIndexer.cpp
// Mark Tully
// 24/3/98
// 24/3/98	: Split into seperate files
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

// For usage notes see the top of TLinkedList.cpp

#include		"TListIndexer.h"

#ifdef USE_OLD_LL

// ********* TListIndexer *********
#pragma mark ---- TListIndexer
// This object is used to index through the linked list. When constructing you must pass a list to index through. This is
// the only list which this indexer can index through.
// You then pass a boolean, if true (or ommitting it makes it default to true), indexing starts from the beginning.
// If false, indexing starts from the end.
// You can then use the index commands to get the data and index forwards (GetNextData()), get the data and index backwards
// (GetPrevData()), get the TListElementPtr and index forward or backward (GetNextElement()/GetPrevElement()).
// These procs return the requested information on the current indexed element and then either the next or the previous
// element becomes the index for the following call. If they return 0L then there are no more elements.
// (Also note that GetxxxxData() can return 0L if your data is, in fact, 0L. If this can happen, use GetxxxxElement() and
// then use GetElementData()).
// The indexing is pretty robust. If you delete the element which is currently the target of the index, (will be returned
// by the next GetxxxxData or GetxxxxElement) the index will automatically advance. You can control whether the the index
// advances forwards or backwards by using SetUnlinkAdvanceDirection(). Passing true will mean that the index will move
// onto the next element and false will mean that it will move onto the previous element. If you are starting indexing
// from the beginning then it defaults to true, else it defaults to false.
// Now you can use either a TListIndexer object which returns void * for GetxxxxData or you can use TListIndexerT<type> where
// GetxxxxData returns type*.

TListIndexerBase::TListIndexerBase(TLinkedListPtr listToIndex,Boolean indexFromStart)
{
	list=listToIndex;
	indexElement=0L;
	
	if (list)
		list->LinkIndexer(this);
	
	SetUnlinkAdvanceDirection(indexFromStart);
	if (indexFromStart)
		RewindIndexing();
	else
		FastForwardIndexing();
}

// Use this to set the direction which the index will move in if you unlink the element which is under index. true will
// process onto the next element, false onto the previous element. Which one you move to depends on the directions you
// are indexing through the list in. If you're going from end to start you would pass false, if you're going from start
// to end you would pass true. Note that it defaults to the direction you specifiy to the constructor.
void TListIndexerBase::SetUnlinkAdvanceDirection(Boolean direction)
{
	unlinkDirection=direction;
}

Boolean TListIndexerBase::GetUnlinkAdvanceDirection()
{
	return unlinkDirection;
}

// This is called when an element is being removed from the list. We check if it's our index element and advance the index
void TListIndexerBase::ElementUnlinking(TListElement *unlink)
{
	if (unlink==indexElement)
	{
		if (unlinkDirection)
			indexElement=indexElement->GetNextElement();
		else
			indexElement=indexElement->GetPrevElement();
	}
}

// Called when the list being operated on is destroyed (ie deleted)
void TListIndexerBase::ListDestroyed()
{
	list=0L;
}

void TListIndexerBase::RewindIndexing()
{
	if (list)
		indexElement=list->GetFirstElement();
}

void TListIndexerBase::FastForwardIndexing()
{
	if (list)
		indexElement=list->GetLastElement();
}

void TListIndexerBase::StartIndexingAt(TListElement *start)
{
	indexElement=start;
}

void TListIndexerBase::StartIndexingAt(TListIndex index)
{
	if (list)
		indexElement=list->GetNthElement(index);
}

TListElementPtr TListIndexerBase::GetNextElement()
{
	TListElementPtr		temp=indexElement;

	if (indexElement)
		indexElement=indexElement->GetNextElement();
	
	return temp;
}

TListElementPtr TListIndexerBase::GetPrevElement()
{
	TListElementPtr		temp=indexElement;

	if (indexElement)
		indexElement=indexElement->GetPrevElement();
	
	return temp;
}

#endif