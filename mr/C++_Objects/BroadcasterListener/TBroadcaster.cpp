// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TBroadcaster.cpp
// Mark Tully
// 26/3/98
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

#include			"TBroadcaster.h"
#include			"TListener.h"
#include			"TListIndexer.h"

TBroadcaster::TBroadcaster(OSType inBroadcasterType,void *inRefCon)
{
	mBroadcasterType=inBroadcasterType;
	mBroadcasterRefCon=inRefCon;
}

TBroadcaster::~TBroadcaster()
{
	// Inform all listeners that this element has been deleted
	TListIndexer		indexer(&mListenersList);
	
	while (TListenerPtr listener=(TListenerPtr)indexer.GetNextData())
		listener->BroadcasterDied(this);
	
	// Now delete all the aliases in the listeners list
	mListenersList.DeleteAllLinks();
}

Boolean TBroadcaster::AddListener(TListenerPtr listener)
{
	TListElementPtr		listEle=listener->link.MakeAlias();

	if (!listEle)
		return false;
		
	mListenersList.LinkElement(listEle);
	
	return true;
}

void TBroadcaster::RemoveListener(TListenerPtr listener)
{
	listener->link.DeleteAliasInList(&mListenersList);
}

void TBroadcaster::BroadcastMessage(TBroadcastMessageClass message,void *param1,void *param2)
{
	TListIndexer		indexer(&mListenersList);
	
	while (TListenerPtr listener=(TListenerPtr)indexer.GetNextData())
		listener->Listen(message,this,param1,param2);
}
