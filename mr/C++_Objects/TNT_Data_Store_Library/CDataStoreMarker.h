// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// CDataStoreMarker.h
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

#include		"CListElementT.h"
#include		"CDataStore.h"

class CMarkableDataStore;

class CDataStoreMarker : public CListElementT<CDataStoreMarker>
{
	friend class					CMarkableDataStore;

	protected:
		CMarkableDataStore			*mDataStore;
		CDataStore::TDataOffset		mOffset;
	
		virtual void				DataStoreDied();
		virtual void				PositionRemoved(
										CDataStore::TDataOffset			inOffset,
										CDataStore::TDataOffset			inWasSize,
										CDataStore::TDataOffset			inNewSize);

	public:
									CDataStoreMarker();
									CDataStoreMarker(
										CMarkableDataStore				*inDataStore,
										CDataStore::TDataOffset			inOffset);
		virtual						~CDataStoreMarker();

		inline CDataStore::TDataOffset	GetMarker() const				{ return mOffset; }
		virtual void				SetMarker(
										CDataStore::TDataOffset			inNewMarker,
										bool							inResortMarkersList=true);

		virtual CMarkableDataStore	*GetDataStore() const;
		virtual void				SetDataStore(
										CMarkableDataStore				*inDataStore);

		virtual void /*e*/			ResizeFromMarker(
										CDataStore::TDataOffset			inWasSize,
										CDataStore::TDataOffset			inNewSize);

		virtual OSErr				GetBytes(
										void							*outPtr,
										CDataStore::TDataOffset			&ioCount);

		virtual OSErr				PutBytes(
										const void						*inPtr,
										CDataStore::TDataOffset			&ioCount);

		virtual void				Set(
										CMarkableDataStore				*inDataStore,
										CDataStore::TDataOffset			inOffset);

									operator CDataStore::TDataOffset() const	{ return GetMarker(); }
};