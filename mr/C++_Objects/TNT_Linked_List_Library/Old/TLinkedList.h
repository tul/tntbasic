// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked Lists Library
// TLinkedList.h
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

#ifdef USE_OLD_LL

class TLinkedList;
typedef unsigned long TListIndex;

typedef class TListElement
{
	friend class TLinkedList;

	private:		
		TListElement		*next;
		TListElement		*prev;
		class TLinkedList	*bossList;
		
		void				*data;
		void				Unlink();
			
	public:
							TListElement(void *data=0L);
		virtual 			~TListElement();
		virtual void		*GetData() const;
		virtual void		SetData(void *newData);
		TListElement*		GetPrevElement() const;
		TListElement*		GetNextElement() const;
		void				*GetNextData() const;
		void				*GetPrevData() const;
		TLinkedList			*GetBossList() const;
		TListIndex			GetCurrentIndex() const;
} TListElement, *TListElementPtr, TListLink, *TListLinkPtr;

typedef class TLinkedList
{
	// The listindexer's constructor must be able to add itself to the indexer's list
	friend class TListIndexerBase;

	private:
		TListElementPtr	indexersList;
		TListElementPtr	list;
		
		void			BroadcastUnlink(TListElement *unlink);
		void			LinkIndexer(TListIndexerBase *indexer);
				
	public:
						TLinkedList();
		virtual			~TLinkedList();
		
		// adding elements
		void			LinkElement(TListElement *addMe);
		TListIndex		AppendElement(TListElement *addMe);
		TListIndex		InsertElement(TListElement *addMe,TListIndex positionIndex,Boolean afterPositionIndex);
		TListIndex		InsertElement(TListElement *addMe,TListElement *position,Boolean afterPositionElement);

		// Moving elements
		TListIndex		MoveElement(TListElement *listEle,TListIndex newPosition,Boolean afterPositionIndex);
		TListIndex		MoveElement(TListElement *listEle,TListElement *newPosition,Boolean afterPositionElement);
		static void		SwapElements(TListElementPtr listEleA,TListElementPtr listEleB);

		// removing elements
		void			UnlinkElement(TListElement *me);
		void			UnlinkElement(TListIndex index);
		void			DeleteAllLinks();
	
		// element access
		TListElementPtr	GetNthElement(TListIndex index) const;
		void			*GetNthElementData(TListIndex index) const;
		TListElementPtr GetLastElement() const;
		TListElementPtr GetFirstElement() const;
		
		TListElementPtr FindNthElementByData(void *dataPtr,TListIndex index) const;
		TListIndex		CountElements() const;
		
} TLinkedList, *TLinkedListPtr;

// Deletes all links on destruction
class TDeletingLinkedList : public TLinkedList
{
	public:
		virtual			~TDeletingLinkedList()	{ DeleteAllLinks(); }
};

#else
	#include		"CLinkedListT.h"
	
	typedef CListElementT<void>			TListElement;
	typedef CLinkedListT<void>			TLinkedList;
	typedef CDeletingLinkedListT<void>	TDeletingListList;
	typedef TLinkedList					*TLinkedListPtr;
	typedef TListElement				*TListElementPtr;
	typedef TListElement				TListLink;
	typedef TListElement				*TListLinkPtr;
#endif