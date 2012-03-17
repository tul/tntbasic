// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked List Library
// TSortedLinkedList.cpp
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

#ifdef USE_OLD_LL

#include		"TSortedLinkedList.h"
#include		"TListIndexer.h"

TSortedLinkedList::TSortedLinkedList(TListCompareProc compareProc,void *refCon,unsigned long sortKey,Boolean ascend)
{
	this->compareProc=compareProc;
	compareRefCon=refCon;
	this->sortKey=sortKey;
	MakeAscending(ascend);
}

TListIndex TSortedLinkedList::AddOrderedElement(TListElementPtr listElement)
{
	TListIndexer		indexer(this);
	void				*dataBeingAdded=listElement->GetData();
	
	while (TListElementPtr elePtr=indexer.GetNextElement())
	{
		if (CompareData(dataBeingAdded,elePtr->GetData())==-1)
			return InsertElement(listElement,elePtr,false);
	}
	
	return AppendElement(listElement);
}

// This compares data taking into account whether the list is ascending or descending. If ascendDescend is 1 then it's
// ascent, if it's -1 then it's descent.
signed char TSortedLinkedList::CompareData(void *dataA,void *dataB)
{
	return ascendDescend*compareProc(dataA,dataB,sortKey,compareRefCon);
}

void TSortedLinkedList::LinearSelectionSortList()
{
	void				*minData;
	TListElementPtr		minElement,indexElement;	
	TListElementPtr		startElement=GetNthElement(0);
	
	while (startElement)
	{
		minData=startElement->GetData();
		minElement=startElement;
		indexElement=startElement->GetNextElement();
		
		while (indexElement)
		{
			if (CompareData(indexElement->GetData(),minData)==-1)
			{
				minElement=indexElement;
				minData=indexElement->GetData();
			}
			indexElement=indexElement->GetNextElement();
		}
	
		SwapElements(minElement,startElement);
		startElement=minElement->GetNextElement();
	}	
}

#endif









