// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked List Library
// TSharedListElement.h
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

#include					"TLinkedList.h"

#ifdef USE_OLD_LL

// This class of List element should be used when you want to put the same element into multiple linked lists.
// It is used just like the TListElement except that it can be put in more that one list at once.
typedef class TSharedListElement
{
	private:
		TLinkedList			aliasList;								// Linked list of all the aliases which have been placed into lists
		void				*data;

	public:
		
							TSharedListElement(void *data=0L);
		virtual				~TSharedListElement();
		
		TListElementPtr		MakeAlias();
		void				DestroyAllAliases();
		void				DeleteAliasInList(TLinkedListPtr list);
		
		void				*GetData() const;
		void				SetData(void *newData);

} TSharedListElement, *TSharedListElementPtr;

// This class is made by TSharedListElement allowing the shared list element to be placed in multiple
// lists. This alias is handled just like a normal list element except calling get/set data will alter the data in the
// original TSharedListElement. When you want to put a shared element into a list, you make an alias of it and add that to
// the list. Any changes to the data of any one of the aliases affects them all. If the original is destroyed, they are all
// destroyed.
typedef class TSharedListElementAlias : public TListElement
{
	friend class TSharedListElement;

	private:
		TListElement		link;									// Allows it to be put in a list with other aliases to the master element

	protected:
		// This class is not intended to be constructed from anywhere other than in the TSharedListElement so protect
		// it's constructor
							TSharedListElementAlias(TSharedListElement *originalElement);	

	public:
	
		void				*GetData() const;								// These override the SetData/GetData commands to get the master data
		void				SetData(void *newData);

} TSharedListElementAlias, *TSharedListElementAliasPtr;

#else
	#include				"CSharedListElementT.h"
	
	typedef CSharedListElementT<void>		TSharedListElement;
	typedef CSharedListElementAliasT<void>	TSharedListElementAlias;
#endif