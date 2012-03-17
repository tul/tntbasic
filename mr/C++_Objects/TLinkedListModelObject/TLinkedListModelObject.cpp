// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TLinkedListModelObject.cpp
// Mark Tully
// 19/6/98
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

#include				"TLinkedListModelObject.h"

TLinkedListModelObject::TLinkedListModelObject()
{
	listWasAllocated=false;
	linkedList=0L;
}

TLinkedListModelObject::~TLinkedListModelObject()
{
	if (listWasAllocated)
		delete linkedList;
}

/*e*/
void TLinkedListModelObject::AddSubModel(LModelObject *inSubModel)
{
	if (!subModelsList)
		return;
		
	TLinkedListElementPtr		ele=GetLink(inSubModel);
}

/*e*/
void TLinkedListModelObject::RemoveSubModel(LModelObject *inSubModel)
{
	if (!subModelsList)
		return;
	
	TLinkedListElementPtr		ele=GetLink(inSubModel);

	
}

Int32 TLinkedListModelObject::CountSubModels(DescType inModelID) const
{
	Int32	count = 0;
	
	if (GetDefaultSubModel())
		count+=GetDefaultSubModel()->CountSubModels(inModelID);

	if (subModelsList)
	{
		TListIndexer<LModelObject>			indexer(subModelsList);
		LModelObjectPtr						modelObject;
		
		while (modelObject=indexer.GetNextData())
		{
			if ((modelObject->GetModelKind()==inModelID) || (inModelID==typeWildCard))
				count++;
		}
	}
	
	return count;
}

/*e*/
void TLinkedListModelObject::SetUseSubModelList(Boolean inUseSubModelList)
{
	if (!inUseSubModelList)
	{
		if (listWasAllocated)
			delete linkedList;
		
		linkedList=0L;
	}
	else
	{
		if (!linkedList)
		{
			linkedList=new TLinkedList;
			ThrowIfMemFull_(linkedList);
			listWasAllocated=true;
		}
	}
}