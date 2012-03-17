// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked List Library
// TSharedListElement.cpp
// Mark Tully
// 25/6/97
// 24/3/98
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

// For usage notes see the top of TLinkedList.cpp

#ifdef USE_OLD_LL

#include		"TSharedListElement.h"
#include		"TListIndexer.h"

TSharedListElement::TSharedListElement(void *data)
{
	this->data=data;
}

// This proc deletes an alias from a list freeing it's memory and unlinking it
void TSharedListElement::DeleteAliasInList(TLinkedListPtr list)
{
	TListIndexer				indexer(&aliasList);
	TSharedListElementAliasPtr	aliasPtr;
	
	while (aliasPtr=(TSharedListElementAliasPtr)indexer.GetNextData())
	{
		if (aliasPtr->GetBossList()==list)
		{
			delete aliasPtr;
			return;
		}
	}
}

void *TSharedListElement::GetData() const
{
	return data;
}

void TSharedListElement::SetData(void *newData)
{
	data=newData;
}

// This routine makes an alias of the shared list element which is suitable to be added to a linked list. It will work
// exactly like an normal element except all data access commands access the data in the original.
TListElementPtr TSharedListElement::MakeAlias()
{
	TSharedListElementAliasPtr		newAlias;
	
	newAlias=new TSharedListElementAlias(this);
	if (newAlias)
	{
		// Add the alias to the list of aliases
		newAlias->link.SetData(newAlias);
		aliasList.LinkElement(&newAlias->link);
	}
	
	return newAlias;
}

void TSharedListElement::DestroyAllAliases()
{
	// Go through the alias list and delete all the aliases
	// This has the effect of unlinking them too
	TListIndexer				indexer(&aliasList);
	TListElementPtr				link;
	TSharedListElementAliasPtr	aliasPtr;
	
	// This gets the link which is a member of the TSharedListElementAlias. Deleting the member won't delete the original. We
	// need to get the TSharedListElementAliasPtr from this link then delete that.
	while (link=indexer.GetNextElement())
	{
		aliasPtr=(TSharedListElementAliasPtr)link->GetData();
		delete aliasPtr;
	}
}

TSharedListElement::~TSharedListElement()
{
	DestroyAllAliases();
}

#pragma mark ---- TSharedListElementAlias

TSharedListElementAlias::TSharedListElementAlias(TSharedListElement *masterElement)
{
	// Store the master element's address, we need it to get the data
	TListElement::SetData(masterElement);
}

void *TSharedListElementAlias::GetData() const
{
	TSharedListElementPtr		masterElement=(TSharedListElement*)TListElement::GetData();
	
	if (masterElement)
		return masterElement->GetData();
	else
		return 0L;
}

void TSharedListElementAlias::SetData(void *newData)
{
	TSharedListElementPtr		masterElement=(TSharedListElement*)TListElement::GetData();
	
	if (masterElement)
		masterElement->SetData(newData);
}

#endif