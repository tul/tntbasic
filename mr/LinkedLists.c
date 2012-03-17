// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// LinkedLists.c
// Mark Tully
// 10/2/96
// 23/5/96
// 27/5/96
// 15/6/96 : Added sorting routines and also removed the 62767 element limit. It's now 4,294,967,295
//			 ie the size of an unsigned long.
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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

#include	"Marks Routines.h"

// Sorts the list by doing a linear selection sort. This is crap and slow but the quick sort would
// mean double linking my list which I don't want to do yet as this one is fast enough. (For now...)
Boolean LSSortList(CompareProc theProc,LinkedList *listPtr,void *refCon)
{
	Boolean			swapsMade=false;		// Have any swaps been made
	ElementHandle	bestElement;			// This is the element which is the lowest value element
											// in the current list and so will be swapped with the
											// start element
	ElementHandle	startElement=listPtr;	// This is the element whose place is being tested
	ElementHandle	subElement;				// this is the element who could swap placed with above

	
	// For each element in the list, put it in it's place
	while (*startElement)
	{
		// starting from the next element in the list, check if there's any which are less that the
		// start element, if so, swap them
		subElement=(ElementHandle)&((**startElement).next);
		bestElement=startElement;
		while (*subElement)
		{
			// if this element is less than the best one then make this the best element
			if (theProc((**subElement).data,(**bestElement).data,refCon) < 0) 
				bestElement=subElement;
			
			// advance onto the next subElement
			subElement=(ElementHandle)&((**subElement).next);
		}
		
		// perform the swap if necessary
		if (bestElement!=startElement)
		{
			Ptr	swapPtr;

			swapsMade=true;
			swapPtr=(**bestElement).data;
			(**bestElement).data=(**startElement).data;
			(**startElement).data=swapPtr;
		}
		
		// advance onto the next start element
		startElement=(ElementHandle)&((**startElement).next);
	}
	
	return swapsMade;
}

signed char LongCompareProc(Ptr dataA,Ptr dataB,void *refCon)
{
	if (*(long *)dataA < *(long *)dataB)
		return -1;
	else if (*(long *)dataA > *(long *)dataB)
		return 1;
	else
		return 0;
}

signed char ShortCompareProc(Ptr dataA,Ptr dataB,void *refCon)
{
	if (*(short *)dataA < *(short *)dataB)
		return -1;
	else if (*(short *)dataA > *(short *)dataB)
		return 1;
	else
		return 0;
}

signed char PStringCompareProc(Ptr dataA,Ptr dataB,void *refCon)
{
	Boolean		caseSensitive=(Boolean)refCon;
	
	return RelString((StringPtr)dataA,(StringPtr)dataB,caseSensitive,false); // (thanks Apple!)
}

// front list gets back list added to it
Boolean DuplicateLinkedList(LinkedList *frontList,LinkedList *backList)
{
	return ForEachElementDo(backList,DupeElementProc,(long*)frontList);
}

Boolean DupeElementProc(Ptr dataPtr,long *refCon,unsigned long count)
{
	LinkedList		*toMe=(LinkedList*)refCon;
	
	return AddElement(dataPtr,GetPtrSize(dataPtr),toMe);
}

Size GetTotalDataSize(LinkedList *theList)
{
	Size		size=0L;

	ForEachElementDo(theList,SizeElementProc,(long*)&size);
	return size;
}

Boolean SizeElementProc(Ptr dataPtr,long *refCon,unsigned long count)
{
	(*refCon)+=GetPtrSize(dataPtr);
	return true;
}

Size GetDataSize(LinkedList *theList,unsigned long element)
{
	ElementHandle	moo=GetElement(theList,element);

	if (*moo)
		return GetPtrSize((**moo).data);
	else
		return 0L;
}

Size DumpLinkedList(LinkedList *theList,unsigned char *destPtr,Size bufferSize)
{
	ElementHandle	moo=theList; // the address of a linked list
	Size			copiedSize=0L,elementSize;

	while (*moo)
	{
		elementSize=GetPtrSize((**moo).data);
		if (elementSize>(bufferSize-copiedSize))	// too big!, time to go
			return copiedSize;
		BlockMove((**moo).data,destPtr,elementSize);
		destPtr+=elementSize;
		copiedSize+=elementSize;
		
		// get next element
		moo=(ElementHandle)&((**moo).next);
	}
	
	return copiedSize;
}

// this adds an element to the list in an ordered position. The existing must be ordered obviously
// it returns the position in the list that the data was inserted, or 0xFFFFFFFF for a bosh.
unsigned long AddSortedElement(void *dataPtr,Size dataSize,CompareProc sortProc,void *refCon,LinkedList *theList)
{
	ElementHandle	moo=theList; // the address of a linked list
	unsigned long	pos=0L;
	
	while (*moo)
	{
		// if the new data is > the current data, go onto the next element
		if (sortProc((Ptr)dataPtr,(Ptr)(**moo).data,refCon)>0)
		{
			// onto the next element
			moo=(ElementHandle)&((**moo).next);
			pos++;
		}
		else
			break;	// insert the data here
	}
	
	if (InsertElement(theList,pos,(Ptr)dataPtr,dataSize))
		return pos;
	else
		return -1L;
}

// Adds a linked list element to the end of a linked list
Boolean AddElement(void *dataPtr,Size dataSize,LinkedList *listPtr)
{
	ElementHandle	moo=listPtr; // the address of a linked list
	
	while(*moo) // go through list until found last element
	{
		moo=(ElementHandle)&((**moo).next);
	}
	
	return BetterAddElement((Ptr)dataPtr,dataSize,moo);
}

// moo is the element to insert the data after
Boolean BetterAddElement(Ptr dataPtr,Size dataSize,ElementHandle moo)
{
	// Moo now points to a Ptr to store the next element in. The Ptr is the address of the 'next'
	// field of the last element in the list.
	// Add a new ListElement to the list
	*moo=(ElementPtr)NewPtr(sizeof(ListElement));
	if (!*moo) // failed?
		return false;

	(*moo)->next=0L;
	(*moo)->data=0L;
	
	// Add the data to that element
	(*moo)->data=NewPtr(dataSize);
	if (!(*moo)->data) // failed?
	{
		DisposePtr((Ptr)*moo);
		*moo=0L;
		return false;
	}
	BlockMove(dataPtr,(*moo)->data,dataSize);
	return true;
}

// Copys the data for you
Boolean CopyData(LinkedList *listPtr,Ptr dataDest,Size dataSize,unsigned long num)
{
	ElementHandle	moo=GetElement(listPtr,num);
	
	if (*moo) // if this is 0L then n wasn't in the list
	{
		BlockMove((**moo).data,dataDest,dataSize); // copy it for them
		return true;
	}
	else
		return false;
}

// Gets the element n
ElementHandle GetElement(LinkedList *listPtr,unsigned long n)
{
	ElementHandle	moo=listPtr;
	unsigned long	count=0;
	
	while((*moo) && (count!=n))
	{
		count++;
		moo=(ElementHandle)&((**moo).next);
	}
	
	return moo;
}

// Get you the address of the data in the list
Boolean AddressData(LinkedList *listPtr,Ptr *data,unsigned long num)
{
	ElementHandle		moo=GetElement(listPtr,num);
	
	if (*moo)
	{
		*data=(**moo).data;
		return true;
	}
	else
	{
		*data=0L;
		return false;
	}
}

// this routine indexes through the linked list and calls the custom compare proc with the matching
// data and the data in the list.
// The first data to you proc is the data you're to examine, the second is the data you're searching
// for
// Your proc returns zero for a match.
// This proc returns 0xFFFFFFFF for no matches or the index of the element which matched
unsigned long FindCustomElement(LinkedList *listPtr,void *matchData,void *refCon,CompareProc theProc)
{
	ElementHandle		moo=listPtr;
	unsigned long		count=0L;
	
	while(*moo) // go through list until found last element
	{		
		if (theProc((Ptr)(**moo).data,(Ptr)matchData,refCon)==0)
			return count;
		
		count++;
		
		moo=(ElementHandle)&((**moo).next);
	}
	
	return -1;
}

// finds an element in the list
// returns the index of it or 0xFFFFFFFF if not is list
// dataoffset is how far in the data to start the comparem datasize is how long the compare is in
// bytes
unsigned long FindElement(LinkedList *listPtr,void *findData,unsigned long dataOffset,Size dataSize)
{
	ElementHandle		moo=listPtr;
	unsigned long		count=0L;
	
	while(*moo) // go through list until found last element
	{		
		if (CmpBuffer((Ptr)findData,(Ptr)((**moo).data+dataOffset),dataSize,true))
			return count;
		
		count++;
		
		moo=(ElementHandle)&((**moo).next);
	}
	
	return -1;
}

Boolean RemoveElement(LinkedList *listPtr,unsigned long num)
{
	ElementHandle	moo=listPtr,last=0L;
	unsigned long	count=0;
	
	while((*moo) && (count!=num))
	{
		count++;
		last=moo;
		moo=(ElementHandle)&((**moo).next);
	}
	
	if (*moo) // if this is 0L then n wasn't in the list
	{
		Ptr		dataPtr=(**moo).data;
		Ptr		entityPtr=(Ptr)*moo;
		
		// got it, delete it
		if (last==0L) // if last==0L then this was the first element in the list
			*listPtr=(ElementPtr)(**moo).next; // the start is the next one (which is null if this was also the last one in the list)
		else
			(**last).next=(**moo).next; // bypass element moo (element n)
		
		// Free up the ram
		DisposePtr(dataPtr);
		DisposePtr(entityPtr);
		return true; // it's gone
	}
	else
		return false;
}

unsigned long CountElements(LinkedList *listPtr)
{
	ElementHandle	moo=listPtr;
	unsigned long	count=0;
	
	while(*moo)
	{
		moo=(ElementHandle)&((**moo).next);
		count++;
	}
		
	return count;
}


// Removes a ListElement from the end of the list, copying data if wanted
Boolean RemoveLastElement(LinkedList *listPtr)
{
	ElementHandle	moo=listPtr; // address of linked list

	if (!*moo) // if the list is empty
		return false;
	
	while((**moo).next) // go through list until found second to last element
		moo=(ElementHandle)&((**moo).next);
		
	DisposePtr((**moo).data);
	DisposePtr((Ptr)*moo);
	*moo=0L;
	
	return true;
}

void DestroyList(LinkedList *listPtr)
{
	while(RemoveElement(listPtr,0));
}

// Calls the proc on the Nth element in the list.
// numbered from 0
Boolean ForNthElementDo(LinkedList *listPtr,unsigned long n,ElementProc routine,long *refCon)
{
	ElementHandle	moo=GetElement(listPtr,n);
	
	if (*moo) // if this is 0L then n wasn't in the list
		return routine((**moo).data,refCon,n);
	else
		return false;
}

// inserts before the element you give it. Give it 0 to insert at head of the list. If the number is
// past the end of the list the data is added onto the end (ie pass a large number if you want the
// element added to the end everytime).
Boolean InsertElement(LinkedList *listPtr,unsigned long pos,Ptr dataPtr,Size dataSize)
{	
	ElementHandle		moo=GetElement(listPtr,pos);
	ElementPtr			wasPtr=*moo;
	Boolean				theRes;
	
	// add the data after said element
	theRes=BetterAddElement(dataPtr,dataSize,moo);
	
	// if it was added OK then move moo onto the new element. If it didn't then keep it on the
	// same element.
	if (theRes)
		moo=(ElementHandle)&(**moo).next;
	
	// relink the list
	*moo=wasPtr;
	
	return theRes;
}

// returns false if the ElementProc does
Boolean ForEachElementDo(LinkedList *listPtr,ElementProc routine,long *refCon)
{
	ElementHandle	moo=listPtr; // address of linked list
	unsigned long	count=0;

	while(*moo) // go through list until found last element
	{		
		if (!routine((**moo).data,refCon,count)) // count starts from zero
			return false;
		
		count++;
		
		moo=(ElementHandle)&((**moo).next);
	}
	
	return true;
}

// this is an alternative to for each element do. It indexes through the list but can operate inline
// rather than you needed a serpate proc.
// To use, copy your linked list ptr into a temp variable of type LinkedList. You pass the address
// of this variable to the proc. THIS VARIABLE GETS TRASHED!!!!! DO NOT PASS ORIGINAL LINKED LIST
// POINTER!!!!! It returns the data for the first element and adjusts the ptr, next time it passed
// the data for the seconds element and adjusts the ptr etc. When it returns 0L there's no more
// data.
// example usage:
/*
	void IndexThroughList(LinkedList theList)
	{
		MyDataTypePtr	dataPtr;					// MyDataTypePtr is whatever you want, as GetNextElement returns a void*
	
		while (dataPtr=GetNextElement(&theList))	// NB: Using a local copy of the linked list ptr
		{
			// operate on dataPtr
		}
		
		// Loop done, note, local variable theList is 0L and so invalid for further linkedlist ops
	}
*/	
void *GetNextElement(LinkedList *list)
{
	void		*thePtr=0L;

	if (*list)
	{
		thePtr=(**list).data;
		*list=(ElementPtr)(**list).next;
	}
	return thePtr;
}









