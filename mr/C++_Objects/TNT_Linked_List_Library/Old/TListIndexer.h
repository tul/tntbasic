// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked Lists Library
// TListIndexer.h
// New Linked List library in C++
// Mark Tully
// 25/6/97
// 24/3/98	: Split into seperate files
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1997, Mark Tully and John Treece-Birch
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

#include			"TLinkedList.h"

#ifdef USE_OLD_LL

typedef class TListIndexerBase
{
	friend class TLinkedList;

	private:
		TListElement			link;				// link so the indexer can be put in a list
		Boolean					unlinkDirection;	// direction that will be moved in if the current element is unlinked

	protected:
		TLinkedList				*list;				// list to index through
		TListElementPtr			indexElement;		// element currently being indexed

		virtual void			ElementUnlinking(TListElementPtr gonnaGo);		// called by the list when an element is to be removed
		virtual void			ListDestroyed();								// called by the list when the list is destoyed
	
	public:
		// Constructor
								TListIndexerBase(TLinkedList *listToIndex,Boolean indexFromStart=true);

		// Unlinked index preference
		virtual void			SetUnlinkAdvanceDirection(Boolean direction);
		virtual Boolean			GetUnlinkAdvanceDirection();

		// Indexing commands
		virtual void			RewindIndexing();
		virtual void			FastForwardIndexing();
		virtual void			StartIndexingAt(TListElement *start);
		virtual void			StartIndexingAt(TListIndex index);
		virtual TListElement	*GetNextElement();	
		virtual TListElement	*GetPrevElement();
		
} TListIndexerBase, *TListIndexerBasePtr;

#else
	#include		"CListIndexerT.h"

	typedef CListIndexerT<void>	TListIndexerBase;

#endif

template <class T>
class TListIndexerT : public TListIndexerBase
{
	public:
								TListIndexerT(TLinkedList *list,Boolean indexFromStart=true) : TListIndexerBase(list,indexFromStart) {}
		virtual T	 			*GetNextData();
		virtual T	 			*GetPrevData();
};

template <class T>
T* TListIndexerT<T>::GetNextData()
{
	TListElementPtr		ele=GetNextElement();
	
	if (ele)
		return (T*)ele->GetData();
	else
		return 0L;
}

template <class T>
T* TListIndexerT<T>::GetPrevData()
{
	TListElementPtr		ele=GetPrevElement();
	
	if (ele)
		return (T*)ele->GetData();
	else
		return 0L;
}

typedef TListIndexerT<void> TListIndexer;
typedef TListIndexer		*TListIndexerPtr;