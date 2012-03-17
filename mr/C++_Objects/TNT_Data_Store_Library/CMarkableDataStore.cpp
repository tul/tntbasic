// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// CMarkableDataStore.cpp
// © Mark Tully and TNT Software 1999
// 16/1/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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

#include		"CMarkableDataStore.h"
#include		"CDataStoreMarker.h"
#include		"CListIndexerT.h"

/*
	Extends the CDataStore class to allow the use of data markers. This is discussed in Data Store.doc
	The markers are kept in order, if custom ordering is necessary (for example to resolve amiguous cases where the markers
	are marking the same data, but when one resizes which of the others are moved?) a custom sort routine can be used.
	These are implemented via Attachments. When your attachment is executed, only evaluate the markers if the result is
	still set to kUnknown. If it's not then it's been handled and there's no need to take any action.
*/

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊConstructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CMarkableDataStore::CMarkableDataStore(
	CDataStore			*inDataStore) : mMarkersList(StandardMarkerCompareProc,this)
{
	mDataStore=inDataStore;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊDestructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Informs all the markers that the stream is shutting down.
CMarkableDataStore::~CMarkableDataStore()
{
	CListIndexerT<CDataStoreMarker>		indexer(&mMarkersList);
	
	while (CDataStoreMarker *marker=indexer.GetNextData())
		marker->DataStoreDied();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊConnectMarker
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called by a marker when it wants to be connected to the stream
void CMarkableDataStore::ConnectMarker(
	CDataStoreMarker	*inMarker)
{
	mMarkersList.AddOrderedElement(inMarker);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊMarkerAdjustedBy
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This proc is called when a marker has had it's marking position adjusted and it may need reordering in the markers list.
// The marker has it's new value within it, the inAdjustedBy is what it was just adjusted by so that inMarker->GetOffset()-
// inAdjustedBy is what it was before.
void CMarkableDataStore::MarkerAdjustedBy(
	CDataStoreMarker	*inMarker,
	SInt32				inAdjustedBy)
{
	// see if the marker needs moving in the stream
	// this loop looks at all subsequent markers in the stream and moves the element only if it should be moved past them
	if (inAdjustedBy>0)			// marker moved forward in the stream
	{
		CDataStoreMarker			*wasNextMarker=0,*nextMarker=inMarker->GetNextData();
		CDataStore::TDataOffset		marker=inMarker->GetMarker(),nextMarkerOffset;
		bool						hasToMove=false;
		
		while (nextMarker)
		{
			nextMarkerOffset=nextMarker->GetMarker();
			
			if (marker<nextMarkerOffset)
			{
				// we want it before this marker
				if (hasToMove)
					mMarkersList.MoveElement(inMarker,nextMarker,false);
				
				return;
			}
			else if (marker==nextMarkerOffset)
			{
				// have to see if it's more or less by resolving the ambiguity (slow)
				SMarkerSortRequest	sortRequest={inMarker,nextMarker,kUnknown};
		
				ExecuteAttachments(msg_SortDataStoreMarkers,&sortRequest);
		
				switch (sortRequest.result)
				{
					case kUnknown:
					case kEqual:
					case kABeforeB:
						// Equal, unknown or inMarker is before nextMarker, either way move it
						if (hasToMove)
							mMarkersList.MoveElement(inMarker,nextMarker,false);
						return;
						break;

					// default - continue the search
				}				
			}
			
			hasToMove=true;
			wasNextMarker=nextMarker;
			nextMarker=nextMarker->GetNextData();
			
			if (!nextMarker)	// found end of list - move the element to here then return
				mMarkersList.MoveElement(inMarker,wasNextMarker,true);
		}
	}
	else if (inAdjustedBy<0)	// marker moved backwards in the stream 
	{		
		CDataStoreMarker			*wasPrevMarker=0,*prevMarker=inMarker->GetPrevData();
		CDataStore::TDataOffset		marker=inMarker->GetMarker(),prevMarkerOffset;
		bool						hasToMove=false;
		
		while (prevMarker)
		{
			prevMarkerOffset=prevMarker->GetMarker();
			
			if (marker>prevMarkerOffset)
			{
				// we want it after this marker
				if (hasToMove)
					mMarkersList.MoveElement(inMarker,prevMarker,true);
				
				return;
			}
			else if (marker==prevMarkerOffset)
			{
				// have to see if it's more or less by resolving the ambiguity (slow)
				SMarkerSortRequest	sortRequest={inMarker,prevMarker,kUnknown};
		
				ExecuteAttachments(msg_SortDataStoreMarkers,&sortRequest);
		
				switch (sortRequest.result)
				{
					case kUnknown:
					case kEqual:
					case kAAfterB:
						// Equal, unknown or inMarker is after prevMarker, either way move it
						if (hasToMove)
							mMarkersList.MoveElement(inMarker,prevMarker,true);
						return;
						break;

					// default - continue the search
				}				
			}
			
			hasToMove=true;
			wasPrevMarker=prevMarker;
			prevMarker=prevMarker->GetPrevData();
			
			if (!prevMarker)	// found beginning of list - move the element to here then return
				mMarkersList.MoveElement(inMarker,wasPrevMarker,false);		
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊDisconnectMarker
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called by a marker when it wants to be disconnected to the stream
void CMarkableDataStore::DisconnectMarker(
	CDataStoreMarker	*inMarker)
{
	mMarkersList.UnlinkElement(inMarker);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊStandardMarkerCompareProc								Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This proc compares the two markers specified and returns which one is least. This allows the markers list to be ordered.
// The refcon is the CMarkableDataStore which is being sorted.
signed char CMarkableDataStore::StandardMarkerCompareProc(
	CDataStoreMarker	*inDataA,
	CDataStoreMarker	*inDataB,
	unsigned long		inSortKey,
	void				*inRefCon)
{
	if (inDataA->GetMarker()<inDataB->GetMarker())
		return -1;
	else if (inDataA->GetMarker()>inDataB->GetMarker())
		return 1;
	else
	{
		// The markers are marking the same offset - this is ambiguous pass by the attachment to solve the ambiguity
		SMarkerSortRequest	sortRequest={inDataA,inDataB,kUnknown};
		
		((CMarkableDataStore*)inRefCon)->ExecuteAttachments(msg_SortDataStoreMarkers,&sortRequest);
		
		switch (sortRequest.result)
		{
			case kUnknown:
			case kEqual:
				return 0;
				break;
			
			case kABeforeB:
				return -1;
				break;
				
			case kAAfterB:
				return +1;
				break;
		}
	}

	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AdjustMarkers
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adjusts all the markers connected to the stream so they still follow their data.
void CMarkableDataStore::AdjustMarkers(
	const CDataStoreMarker		*inFromMarker,
	CDataStore::TDataOffset		inWasSize,
	CDataStore::TDataOffset		inNewSize)
{
	SInt32								offset=inFromMarker->GetMarker();
	CListIndexerT<CDataStoreMarker>		indexer(&mMarkersList);
	CDataStoreMarker					*marker;
	Size								relChange=inNewSize-inWasSize;
	
	// The dead zone is the data which is lost when a stream is sized down.
	SInt32								deadZoneStart=0;
	SInt32								deadZoneEnd=0;
	
	if (relChange<0)
	{
		deadZoneStart=offset+inNewSize;
		deadZoneEnd=offset+inWasSize;
	}
	
	// Moving onto the next marker ensures that the from marker isn't adjusted
	indexer.StartIndexingAt(inFromMarker->GetNextElement());
	
	while (marker=indexer.GetNextData())
	{
		// Is the marker in the dead zone?
		if (*marker>=deadZoneStart && *marker<deadZoneEnd)
			marker->PositionRemoved(offset,inWasSize,inNewSize);

		// Is the marker in need of moving?
		else if (*marker>=offset)
			marker->SetMarker(marker->GetMarker()+relChange,false);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ResizeFromMarker										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called when the stream needs to be resized. Calls ResizeFromOffsetSelf which must be overridden to resize the stream then
// adjusts all the markers.
void CMarkableDataStore::ResizeFromMarker(
	const CDataStoreMarker		*inMarker,
	CDataStore::TDataOffset		inWasSize,
	CDataStore::TDataOffset		inNewSize)
{
	if (inWasSize==inNewSize)
		return;

	mDataStore->ResizeStore(*inMarker,inWasSize,inNewSize);

	AdjustMarkers(inMarker,inWasSize,inNewSize);	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetBytes
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Maps onto the data stores version
OSErr CMarkableDataStore::GetBytes(
	CDataStore::TDataOffset		inOffset,
	void						*outPtr,
	CDataStore::TDataOffset		&ioCount)
{
	return mDataStore->GetBytes(inOffset,outPtr,ioCount);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PutBytes
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Maps onto the data stores version
OSErr CMarkableDataStore::PutBytes(
	CDataStore::TDataOffset		inOffset,
	const void					*inPtr,
	CDataStore::TDataOffset		&ioCount)
{
	return mDataStore->PutBytes(inOffset,inPtr,ioCount);
}






