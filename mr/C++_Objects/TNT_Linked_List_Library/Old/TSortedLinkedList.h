// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked List Library
// TSortedLinkedList.h
// Mark Tully
// 24/3/98
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

#include		"TLinkedList.h"

#ifdef USE_OLD_LL

// Sort proc spec
// --------------
// return -1 if dataA<dataB
// return 0 if dataA==dataB
// return 1 if dataA>dataB
// sortKey and refCon are for your use and are not used by the TSortedLinkedList object

typedef class TSortedLinkedList : public TLinkedList
{
	public:
		typedef signed char					(*TListCompareProc)(void *dataA,void *dataB,unsigned long sortKey,void *refCon);

	private:
		void								*compareRefCon;
		unsigned long						sortKey;
		TListCompareProc					compareProc;
		signed char							ascendDescend;
	
	public:
											TSortedLinkedList(TSortedLinkedList::TListCompareProc compareProc,void *refCon=0L,unsigned long sortKey=0L,Boolean ascending=true);
		
		TListCompareProc					GetCompareProc()						{ return compareProc; }
		void								SetCompareProc(TListCompareProc newProc)	{ compareProc=newProc; }
		
		void								*GetCompareRefCon()						{ return compareRefCon; }
		void								SetCompareRefCon(void *newRef)			{ compareRefCon=newRef; }
		
		unsigned long						GetSortKey()							{ return sortKey; }
		void								SetSortKey(unsigned long newKey)		{ sortKey=newKey; }
		
		void								MakeAscending(Boolean ascending)		{ ascendDescend=ascending ? 1 : -1; }
		
		// Use this to add an element to the list in the right place. It assumes the list is sorted, if it's not then
		// it makes no attempt to sort it.
		TListIndex							AddOrderedElement(TListElementPtr listElement);
		signed char							CompareData(void *dataA,void *dataB);
		
		// Sort routines
		void								LinearSelectionSortList();

} TSortedLinkedList, *TSortedLinkedListPtr;

#else
	#include		"CSortedLinkedListT.h"
	
	typedef CSortedLinkedListT<void>		TSortedLinkedList;
#endif