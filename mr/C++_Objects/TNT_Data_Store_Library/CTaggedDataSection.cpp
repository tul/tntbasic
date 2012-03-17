// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// CTaggedDataSection.cpp
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

/*
	The tagged data section is a section of data which is preluded by a 4 byte tag id and a 4 byte size count.
	
	When the section is created the start marker is adjust to be after the tag header, ie at the start of the data. This is
	done so that if a stream connects it will access the tag content rather than having the tag header to skip over.
*/

#include		"CTaggedDataSection.h"
#include		"CTaggedDataGroup.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Use the flags to specify whether the tag id and tag length are stored in the data. This constructor makes the section the
// root section of the store
CTaggedDataSection::CTaggedDataSection(
	CMarkableDataStore		*inStore,
	CDataStore::TDataOffset	inStartOffset,
	CDataStore::TDataOffset	inLength,
	TTagFlags				inFlags,
	TTagId					inTagId) :
	CDataStoreSection(inStore,inStartOffset,inLength),
	mLink(this)
{
	mTagIndex=0;		// Index is set by parent group after construction
	mTagId=inTagId;
	mTagFlags=inFlags;
	ProcessHeader();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Use the flags to specify whether the tag id and tag length are stored in the data.
CTaggedDataSection::CTaggedDataSection(
	CTaggedDataGroup		*inParentGroup,
	CDataStore::TDataOffset	inStartOffset,
	CDataStore::TDataOffset	inLength,
	TTagFlags				inFlags,
	TTagId					inTagId) :
	CDataStoreSection(inParentGroup,inStartOffset,inLength),
	mLink(this)
{
	mTagIndex=0;		// Index is set by parent group after construction
	mTagId=inTagId;
	mTagFlags=inFlags;
	ProcessHeader();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ OrderSubSections
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Orders the sections by the order that they appear in the store.
EMarkerOrder CTaggedDataSection::OrderSubSections(
	CDataStoreSection		*inSectionA,
	CDataStoreSection		*inSectionB)
{
	CTaggedDataSection		*sectionA=dynamic_cast<CTaggedDataSection*>(inSectionA),*sectionB=dynamic_cast<CTaggedDataSection*>(inSectionB);
	
	if (sectionA->GetTagIndex()<sectionB->GetTagIndex())
		return kABeforeB;
	else
		return kAAfterB;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetTagId											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Changes the tag id to the one passed. If store tag in data is set then the tag is store in the data.
void CTaggedDataSection::SetTagId(
	TTagId			inTagId)
{
	CDataStore::TDataOffset		ioCount=sizeof(inTagId);

	mTagId=inTagId;

	if (mTagFlags&kTF_StoreTagId)
	{
		inTagId=::CFSwapInt32HostToBig(inTagId);
		ThrowIfOSErr_(GetDataStore()->PutBytes(mTagFlags&kTF_StoreLength ? mStart-sizeof(inTagId)-sizeof(CDataStore::TDataOffset) : mStart-sizeof(inTagId),&inTagId,ioCount));
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StoreLength										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Writes the length specified into the tag header as the data length. if the tag's flags indicate the length shouldn't be
// stored then this routine does nothing.
void CTaggedDataSection::StoreLength(
	CDataStore::TDataOffset		inLength)
{
	CDataStore::TDataOffset		ioCount=sizeof(inLength);

	if (mTagFlags&kTF_StoreLength)
	{
		inLength=::CFSwapInt32HostToBig(inLength);
		ThrowIfOSErr_(GetDataStore()->PutBytes(mStart-sizeof(inLength),&inLength,ioCount));
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ProcessHeader										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Operates according to the flags in mTagFlags. Will create the header if necessary storing the current values of the tag id
// and tag length. If not creating the header it will read values for the tag id and tag length and adjust the section
// length accordingly.
void CTaggedDataSection::ProcessHeader()
{
	Size			tagHeaderSize=0;
	
	if ((mTagFlags&kTF_StoreTagId)!=0)
		tagHeaderSize+=sizeof(TTagId);
	if ((mTagFlags&kTF_StoreLength)!=0)
		tagHeaderSize+=sizeof(Size);

	if ((mTagFlags&kTF_CreateTagHeader)!=0 && tagHeaderSize)
	{
		// Insert the tag header before the existing data, then write it out and move the section start data to
		// the beginning of the data.
		mStart.ResizeFromMarker(0,tagHeaderSize);

		// Move the start marker past the tag header
		mStart.SetMarker(mStart.GetMarker()+tagHeaderSize);

		// Store the data into the tag header, these routines obey the tag flags and so do nothing if store tagid and store
		// length aren't set.
		SetTagId(mTagId);
		StoreLength(GetLength());
	}
	else
	{
		Size						sizeOffset=0;
		CDataStore::TDataOffset		ioCount;
	
		if ((mTagFlags&kTF_StoreTagId)!=0)
		{
			// Read the tag id from the data
			ioCount=sizeof(TTagId);
			ThrowIfOSErr_(GetDataStore()->GetBytes(mStart,&mTagId,ioCount));
			mTagId=::CFSwapInt32BigToHost(mTagId);
			sizeOffset=sizeof(TTagId);
		}
		if ((mTagFlags&kTF_StoreLength)!=0)
		{
			CDataStore::TDataOffset		size;
		
			// Read the size from the data
			ioCount=sizeof(CDataStore::TDataOffset);
			ThrowIfOSErr_(GetDataStore()->GetBytes(mStart+sizeOffset,&size,ioCount));
			size=::CFSwapInt32BigToHost(size);
			
			// Adjust the end marker to point to the correct place according to the size written into the data
			mEnd.SetMarker(mStart+tagHeaderSize+size);
		}

		// Move the start marker past the tag header
		mStart.SetMarker(mStart.GetMarker()+tagHeaderSize);		
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetLength										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Writes the new length into the tag header
void CTaggedDataSection::SetLength(
	CDataStore::TDataOffset	inLength)
{
	CDataStoreSection::SetLength(inLength);
	StoreLength(GetLength());
}
