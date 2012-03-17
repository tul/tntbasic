// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// CTaggedDataSection.h
// © Mark Tully and TNT Software 1999
// 28/8/99
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

#include		"CDataStoreSection.h"
#include		"CListElementT.h"
#include		"CMarkableDataStore.h"

class			CTaggedDataGroup;

class CTaggedDataSection : public CDataStoreSection
{
	friend class CTaggedDataGroup;				// Needs to access the tag index for sorting purposes

	public:
		typedef OSType							TTagId;
	
		// The tag flags control what data is stored in the tag header.
		// If not storing the length, or not storing the tag id, you must specify the initial values to the constructor.
		typedef UInt16							TTagFlags;
		enum
		{
			kTF_StoreTagId=1,					// A 4 byte tag id is stored at the beginning of the tag header
			kTF_StoreLength=2,					// A 4 byte tag length is stored after the tag id
			kTF_CreateTagHeader=4				// The tag header should be created and initialized to the values passed
		};
	
	protected:
		TListIndex								mTagIndex;					// The position in the parent tag group
		TTagId									mTagId;
	
		TTagFlags								mTagFlags;
		CListElementT<CTaggedDataSection>		mLink;

		virtual TListIndex						GetTagIndex()								{ return mTagIndex; }
		virtual void							SetTagIndex(
													TListIndex				inTagIndex)		{ mTagIndex=inTagIndex; }

		virtual void /*e*/						ProcessHeader();
		virtual void /*e*/						StoreLength(
													CDataStore::TDataOffset	inLength);

		virtual EMarkerOrder					OrderSubSections(
													CDataStoreSection		*inSectionA,
													CDataStoreSection		*inSectionB);

	public:
		/*e*/									CTaggedDataSection(
													CMarkableDataStore		*inStore,
													CDataStore::TDataOffset	inStartOffset,
													CDataStore::TDataOffset	inLength=0,
													TTagFlags				inFlags=0,
													TTagId					inTagId=0);

		/*e*/									CTaggedDataSection(
													CTaggedDataGroup		*inParentGroup,
													CDataStore::TDataOffset	inStartOffset,
													CDataStore::TDataOffset	inLength=0L,
													TTagFlags				inFlags=kTF_StoreLength+kTF_StoreTagId,
													TTagId					inTagId=0L);

		virtual TTagId							GetTagId()										{ return mTagId; }
		virtual	void /*e*/						SetTagId(
													TTagId					inTagId);
		
		virtual void /*e*/						SetLength(
													CDataStore::TDataOffset	inLength);
};