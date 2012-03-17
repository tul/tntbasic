// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked List Library
// TLinkedList.cpp
// Mark Tully
// 28/1/98
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

#ifdef USE_OLD_LL

#include		"TLinkedList.h"
#include		"TListIndexer.h"

// This library provides a fast double linked list. As it's in C++ it will be able to do things
// that the older LinkedLists.c couldn't do. For example, put a TListElement in a class and
// set the elements data to point at the class head. This is a good way of linking objects
// and aslong as the construction of the object containing the TListElement succeeds the
// actual adding of the element to the list cannot fail as adding elements no longer allocates
// memory.
// If you destroy the list element, directly or by destroying the object which contains it,
// the element is automagically removed from the linked list. I think that's cool.
// Create yourself a TLinkedList somewhere, this is the list head. Use it's procs to manage
// the list.
// If you wish to index through the elements in the linked list, you create yourself a
// TListIndexer and pass it the list to index through. It automatically prepares to index
// from the start of the list.
// You can change this by calling StartIndexingAt() after construction.
// The indexer can only work on the list it was constructed to work on, so don't give it
// elements from other lists and expect it to be able to index from them.

// How to index
// ------------
//		{
//			TListIndexer		indexer(&list);
//			MyDataTypePtr		dataPtr;
//			
//			while (dataPtr=(MyDataTypePtr)indexer.GetNextData())
//			{
//				// *** operate on data ptr ***
//			}
//		}
//
//		...or using the new template based class...
//
//		{
//			TListIndexerT<MyDataType>	indexer(&list);
//			MyDataTypePtr		dataPtr;
//
//			while (dataPtr=indexer.GetNextData())
//			{
//				// *** operate on data ptr ***
//			}
//		}

// See further notes down by the TListIndexer code.

// How to use TSharedListElement
// -----------------------------
// The problem with simply putting a TListElement in you object and adding it to a list is that it can only be added to
// one list. If you want to put your object into two lists at the same time you have a problem. You could create two
// TListElements in your object, but what if you don't know how many lists it will be put into?
// The solution is to use a TSharedListElement instead of a TListElement.
// When you want to add your object to a list do this:
// {
// 		TListElementPtr		newLinkPtr=object->sharedListElement.MakeAlias();
//		
//		if (!newLinkPtr)
//		{
//			// Lack of RAM
//			return;
//		}
//		theList.LinkElement(newLinkPtr);
// }
// The disadvantage is that the make alias command can run out of memory, whereas if you had multiple TListElements you
// can't run out of memory.
// The alias is, as it's type suggests, just a TListElementPtr and works just the same. If you want to remove the alias from
// a list you've added it to, you need to delete it. If you don't delete the aliases when you unlink them then you're losing
// RAM.
// Perhaps the best way to unlink the alias if you are going to delete the alias (which is in most cases what you will do)
// would be to call the TSharableListElement::DeleteAliasFromList() proc.
// As well as that proc, there is a TLinkedList::DeleteAllLinks proc. This deletes all the links. For aliases this is fine,
// but for other types of links, like an normal link, it won't delete the memory pointed to by the dataPtr or anything so
// make sure you know when to use it.
// When the original TSharedListElement is destroyed, it takes down all the aliases which are left (even if you've
// unlinked them from the respective lists but not deleted them). When I say "takes down" I mean they are unlinked from the
// lists and their memory is released.

// Specifing a compare proc
// ------------------------
// A compare proc can be used so sort data and also to do a binary search of linked lists.
// The compare proc is of the following form
// signed char CompareProc(void *dataA,void *dataB,unsigned long sortKey,void *refCon)
// The sortKey and refCon are set for the list you are operating on when you create a TSortedLinkedList object. You can
// change them later. The sortKey is designed to be used to decide how to sort the data, ie if the data had multiple field
// the sort key might be 1 = sort by name, 2 = sort by id, 3 = sort by size etc. The refCon and sort key can be whatever you
// want though as they aren't used by the object.
// The compare proc return -1 if dataA<dataB, 1 if dataA>dataB and 0 if dataA==dataB.

// ********* TListElement *********
#pragma mark ---- TListElement

TListElement::TListElement(void *data)
{
	next=0L;
	prev=0L;
	bossList=0L;
	this->data=data;
}

// This proc returns the index this element is in it's boss list.
// The index is not constant, for an element, if you add or delete and element before this one it's index changes.
TListIndex TListElement::GetCurrentIndex() const
{
	TListIndex		index=0L;
	TListElementPtr	ptr=(TListElementPtr)this;
	
	while (ptr=ptr->GetPrevElement())
		index++;
	
	return index;
}

void *TListElement::GetData() const
{
	return data;
}

void TListElement::SetData(void *data)
{
	this->data=data;
}

// this will unlink the data from the list, however, if the listelement is the first one in the
// list then the list ptr must be set to point at the elements next, else the list will be
// buggered.
void TListElement::Unlink()
{
	if (prev)
		prev->next=next;
	if (next)
		next->prev=prev;
	prev=0L;
	next=0L;
	bossList=0L;
}

TLinkedList *TListElement::GetBossList() const
{
	return bossList;
}

TListElement* TListElement::GetPrevElement() const
{
	return prev;
}

TListElement* TListElement::GetNextElement() const
{
	return next;
}

void *TListElement::GetNextData() const
{
	if (next)
		return next->GetData();
	else
		return 0L;	
}

void *TListElement::GetPrevData() const
{
	if (prev)
		return prev->GetData();
	else
		return 0L;
}

TListElement::~TListElement()
{
	if (bossList)
		bossList->UnlinkElement(this);
	else
		Unlink();
}

// ********* TLinkedList *********
#pragma mark ---- TLinkedList

TLinkedList::TLinkedList()
{
	list=0L;
	indexersList=0L;
}

TLinkedList::~TLinkedList()
{
	TListElementPtr		elePtr=list;
	
	// go to every element linked in this list and set the boss list to 0L
	while (elePtr)
	{
		elePtr->bossList=0L;
		elePtr=elePtr->next;
	}
	
	// now go to all the indexers attached and inform them that the list is gone
	TListIndexerPtr		indexerPtr;
	
	elePtr=indexersList;
	while (elePtr)
	{
		indexerPtr=(TListIndexerPtr)elePtr->GetData();
		indexerPtr->ListDestroyed();
		elePtr=elePtr->next;
	}
}

// adds an element to the start of a list
void TLinkedList::LinkElement(TListElement *addMe)
{
	addMe->next=list;
	addMe->prev=0L;
	addMe->bossList=this;
	if (list)
		list->prev=addMe;
	list=addMe;
}

// This is used to move an element in the list. It must be in the list. If not it does nothing.
// Pass an element specifiying the new position. If you pass 0L then if afterPositionElement is true it goes to the end of
// the list, else it goes to the start of the list.
TListIndex TLinkedList::MoveElement(TListElementPtr listEle,TListElementPtr newPosition,Boolean afterPositionElement)
{
	if (!listEle)
		return 0;

	if (listEle->GetBossList()!=this)
		return 0;

	UnlinkElement(listEle);
	return InsertElement(listEle,newPosition,afterPositionElement);
}

// Use this proc to move an element in a linked list to a different position.
TListIndex TLinkedList::MoveElement(TListElementPtr listEle,TListIndex newPosition,Boolean afterPositionIndex)
{
	if (!listEle)
		return 0;

	if (listEle->GetBossList()!=this)
		return 0;

	TListIndex		wasPosition=listEle->GetCurrentIndex();

	if (wasPosition==newPosition)
		return wasPosition;

	if (wasPosition<newPosition)
		newPosition--;
		
	UnlinkElement(listEle);
	return InsertElement(listEle,newPosition,afterPositionIndex);
}

// ---- Static
// Swaps two elements. They can be in different lists.
void TLinkedList::SwapElements(TListElementPtr listEleA,TListElementPtr listEleB)
{
	if (listEleA==listEleB)
		return;

	TLinkedListPtr			listA,listB;
	TListIndex				indexA,indexB;
	
	listA=listEleA->GetBossList();
	listB=listEleB->GetBossList();
	indexA=listEleA->GetCurrentIndex();
	indexB=listEleB->GetCurrentIndex();
	
	listA->UnlinkElement(listEleA);
	listB->UnlinkElement(listEleB);
		
	if (listA==listB)
	{
		// If we're inserting them both into the same list then care must be taken to add the one with the smaller index
		// first
		if (indexA<indexB)
		{
			listA->InsertElement(listEleB,indexA,false);
			listB->InsertElement(listEleA,indexB,false);
		}
		else
		{
			listB->InsertElement(listEleA,indexB,false);
			listA->InsertElement(listEleB,indexA,false);
		}
	}
	else
	{
		listA->InsertElement(listEleB,indexA,false);
		listB->InsertElement(listEleA,indexB,false);
	}
}

// Use this proc to insert an element before/after a given element in the linked list. If the position is 0L then the new
// element is added to the end of the list if adterPositionelement is true, else it's put at the start of the list.
TListIndex TLinkedList::InsertElement(TListElement *addMe,TListElement *position,Boolean afterPositionElement)
{
	TListIndex			finalPosition;

	if (!position)
	{
		if (afterPositionElement)
			return AppendElement(addMe);
		else
		{
			LinkElement(addMe);
			return 0;
		}
	}

	// link the element in
	if (afterPositionElement)
	{
		// put it after the indexed element
		if (position->next)
			position->next->prev=addMe;
		addMe->next=position->next;
		addMe->prev=position;
		position->next=addMe;
		finalPosition=position->GetCurrentIndex()+1;
	}
	else
	{
		// put it before the indexed element
		if (position->prev)
			position->prev->next=addMe;
		else
		{
			// first element, set the lists first element ptr
			list=addMe;
		}
		addMe->prev=position->prev;
		addMe->next=position;
		position->prev=addMe;
		finalPosition=addMe->GetCurrentIndex();
	}
	addMe->bossList=this;
	
	return finalPosition;
}

// this proc inserts the passed element into the linked list at the given position. You can choose to insert it before
// or after the given index. It returns the new index which you should know anyway. If the position index is too high
// it's inserted at the end of the list instead.
TListIndex TLinkedList::InsertElement(TListElement *addMe,TListIndex positionIndex,Boolean afterPositionIndex)
{
	if (positionIndex>=CountElements())
		return AppendElement(addMe);

	return InsertElement(addMe,GetNthElement(positionIndex),afterPositionIndex);
}

// adds an element to the end of the list, it returns the new index where 0 is the first in the
// list
TListIndex TLinkedList::AppendElement(TListElement *addMe)
{
	TListElementPtr		*temp=&list;
	TListElementPtr		prev=0L;
	TListIndex			count=0;
	
	while (*temp)
	{
		count++;
		prev=*temp;
		temp=&(**temp).next;
	}
	*temp=addMe;
	addMe->bossList=this;
	addMe->next=0L;
	addMe->prev=prev;

	return count;
}

TListElementPtr TLinkedList::FindNthElementByData(void *dataPtr,TListIndex index) const
{
	TListIndexer		indexer((TLinkedListPtr)this);
	TListElementPtr		listEle;
	TListIndex			found=0;
	
	while (listEle=indexer.GetNextElement())
	{
		if (listEle->GetData()==dataPtr)
		{
			if (index==found)
				return listEle;
			else
				found++;
		}
	}
	
	return 0L;
}

// This proc deletes all the links in a list. This is generally not a good thing to do as it won't release the memory pointed
// to in the data. So only use this if you are deleting the links without deleting the data, an example case would be if you
// had a list full of aliases to sharable elements.
void TLinkedList::DeleteAllLinks()
{
	TListElementPtr		elementPtr;
	TListIndexer		indexer(this);
	
	while (elementPtr=indexer.GetNextElement())
		delete elementPtr;
}

void TLinkedList::BroadcastUnlink(TListElement *unlink)
{
	TListElementPtr		elePtr=indexersList;
	TListIndexerPtr		indexerPtr;
	
	while (elePtr)
	{
		indexerPtr=(TListIndexerPtr)elePtr->GetData();
		indexerPtr->ElementUnlinking(unlink);
		elePtr=elePtr->next;
	}
}

void TLinkedList::UnlinkElement(TListElement *unLink)
{
	if (unLink)
	{
		// inform all indexers that the given element is unlinking
		BroadcastUnlink(unLink);		
		
		// if the first element is going, stop pointing at it
		if (list==unLink)
			list=unLink->next;
		else if (indexersList==unLink)
			indexersList=unLink->next;
		
		unLink->Unlink();
	}
}

void TLinkedList::UnlinkElement(TListIndex index)
{
	TListElementPtr	ele=GetNthElement(index);
	
	if (ele)
		UnlinkElement(ele);
}

TListElementPtr TLinkedList::GetLastElement() const
{
	TListElementPtr	ele=list;
	
	while (ele && ele->next)
		ele=ele->next;
	
	return ele;
}

TListElementPtr TLinkedList::GetFirstElement() const
{
	return list;
}

// returns 0L if the index was too big
TListElementPtr	TLinkedList::GetNthElement(TListIndex index) const
{
	TListElementPtr	ele=list;
	TListIndex		count=0;
	
	while (ele && index!=count)
	{
		ele=ele->next;
		count++;
	}
	
	return ele;
}

void *TLinkedList::GetNthElementData(TListIndex index) const
{
	TListElementPtr	ele=GetNthElement(index);

	if (ele)
		return ele->GetData();
	else
		return 0L;
}

TListIndex TLinkedList::CountElements() const
{
	TListElementPtr	temp=list;
	TListIndex		count=0;
	
	while(temp)
	{
		temp=temp->next;
		count++;
	}

	return count;
}

void TLinkedList::LinkIndexer(TListIndexerBase *indexer)
{
	indexer->link.SetData(indexer);
	
	indexer->link.next=indexersList;
	indexer->link.prev=0L;
	indexer->link.bossList=this;
	if (indexersList)
		indexersList->prev=&indexer->link;
	indexersList=&indexer->link;
}

#endif