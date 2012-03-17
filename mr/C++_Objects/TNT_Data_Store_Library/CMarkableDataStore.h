// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// CMarkableDataStore.h
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

#pragma once

#include				"CSortedLinkedListT.h"
#include				"CDataStore.h"
#include				<LAttachable.h>

class					CDataStoreMarker;

const MessageT			msg_SortDataStoreMarkers	= 960;			// SMarkerSortRequest*

enum EMarkerOrder
{
	kUnknown,			// Starts as this - if your sorter can't resolve the ambiguity then leave as this
	kEqual,				// Markers are equal
	kABeforeB,			// A<B
	kAAfterB			// A>B
};

struct SMarkerSortRequest
{
	CDataStoreMarker		*markerA,*markerB;
	EMarkerOrder			result;
};

class CMarkableDataStore : public LAttachable
{
	friend class														CDataStoreMarker;

	protected:
		CSortedLinkedListT<CDataStoreMarker>							mMarkersList;
		CDataStore														*mDataStore;

		virtual void					ConnectMarker(
											CDataStoreMarker			*inMarker);
		virtual void					DisconnectMarker(
											CDataStoreMarker			*inMarker);

		virtual void					AdjustMarkers(
											const CDataStoreMarker		*inMarker,
											CDataStore::TDataOffset		inWasSize,
											CDataStore::TDataOffset		inNewSize);

		static signed char				StandardMarkerCompareProc(
											CDataStoreMarker			*inDataA,
											CDataStoreMarker			*inDataB,
											unsigned long				inSortKey,
											void						*inRefCon);

		virtual void					MarkerAdjustedBy(
											CDataStoreMarker			*inMarker,
											SInt32						inAdjustedBy);

	public:
										CMarkableDataStore(
											CDataStore					*inStore=0L);
		virtual							~CMarkableDataStore();
	
		CDataStore						*GetDataStore()	const							{ return mDataStore; }
		void							SetDataStore(
											CDataStore					*inStore)	{ mDataStore=inStore; }
	
		virtual void /*e*/				ResizeFromMarker(
											const CDataStoreMarker		*inDataStoreMarker,
											CDataStore::TDataOffset		inWasSize,
											CDataStore::TDataOffset		inNewSize);
		
		virtual OSErr					GetBytes(
											CDataStore::TDataOffset		inOffset,
											void						*outPtr,
											CDataStore::TDataOffset		&ioCount);

		virtual OSErr					PutBytes(
											CDataStore::TDataOffset		inOffset,
											const void					*inPtr,
											CDataStore::TDataOffset		&ioCount);
};