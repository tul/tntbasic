// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// CTaggedDataGroup.cpp
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
	A subclass of the the CDataStoreSection class.
	This section divides it's contents into sub sections according to the following format
		4 byte tag code
		4 byte size code
			data
			  .
			  .
			  .
		next tag
	
	Each tag comes out as a CTaggedDataSection (or a subclass of this class if you override MakeTaggedDataSection). You can
	manipulate the sections which come out using CDataStoreStreams.
	
	Note : CTaggedDataSection and CTaggedDataGroup delete themselves when LSharable indicates there are no users. To keep
	a reference to a CTaggedDataSection or CTaggedDataGroup you must use a TSharablePtr<CTaggedDataSection/CTaggedDataGroup>.
*/

#include			"TNT_Debugging.h"
#include			"CTaggedDataGroup.h"
#include			"TNT_Errors.h"

struct STagHeader
{
	CTaggedDataSection::TTagId	tagId;
	CDataStore::TDataOffset		tagLength;
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The constructor expects the start offset to point to a tag header for the first tag to be processed. It can point to a
// tag header if the tag header flags are set. It makes this tag group the root section of the store
CTaggedDataGroup::CTaggedDataGroup(
	CMarkableDataStore			*inStore,
	CDataStore::TDataOffset		inStartOffset,
	CDataStore::TDataOffset		inLength,
	TTagFlags					inFlags,
	CTaggedDataSection::TTagId	inTagId) :
	CTaggedDataSection(inStore,inStartOffset,inLength,inFlags,inTagId),
	mConstructedTags(SortTags)
{
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The constructor expects the start offset to point to a tag header for the first tag to be processed. It can point to a
// tag header if the tag header flags are set.
CTaggedDataGroup::CTaggedDataGroup(
	CTaggedDataGroup		*inParentGroup,
	CDataStore::TDataOffset	inStartOffset,
	CDataStore::TDataOffset	inLength,
	TTagFlags				inFlags,
	TTagId					inTagId) :
	CTaggedDataSection(inParentGroup,inStartOffset,inLength,inFlags,inTagId),
	mConstructedTags(SortTags)
{
}

// reads a tag header from the stream and endian flips it as necessary
void CTaggedDataGroup::ReadTagHeader(
	CDataStore::TDataOffset	inOffset,
	STagHeader				*outHeader,
	CDataStore::TDataOffset	&ioLength)
{
	ThrowIfOSErr_(GetDataStore()->GetBytes(inOffset,outHeader,ioLength));

	outHeader->tagId=::CFSwapInt32BigToHost(outHeader->tagId);
	outHeader->tagLength=::CFSwapInt32BigToHost(outHeader->tagLength);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CountTags									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the number of tags in this tag group. This involves indexing through the store.
CTaggedDataGroup::TTagIndex CTaggedDataGroup::CountTags()
{
	CDataStore::TDataOffset	indexOffset=GetStartOffset();
	TTagIndex				count=0;
	STagHeader				tagHeader;
	CDataStore::TDataOffset	ioCount=sizeof(STagHeader);
	
	while (indexOffset<mEnd)
	{
		ReadTagHeader(indexOffset,&tagHeader,ioCount);
	
		indexOffset+=sizeof(STagHeader);
		indexOffset+=tagHeader.tagLength;
		count++;
	}
	
	return count;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNthTag									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the nth tag in this tag group. If a data section has not already been created then one is created and stored in
// a linked list. Throws tag not found error if, well the tag isn't found really. Indexes from 0.
CTaggedDataSection *CTaggedDataGroup::GetNthTag(
	TTagIndex				inTagIndex)
{
	if (CTaggedDataSection *tag=FindConstructedTagByIndex(inTagIndex))
		return tag;

	CDataStore::TDataOffset	indexOffset=GetStartOffset();
	TTagIndex				count=0;
	STagHeader				tagHeader;
	CDataStore::TDataOffset	ioCount=sizeof(STagHeader);
	
	while (indexOffset<mEnd)
	{
		ReadTagHeader(indexOffset,&tagHeader,ioCount);
	
		if (count==inTagIndex)
		{
			// Found the tag
			return MakeTag(count,tagHeader.tagId,indexOffset);
		}
	
		indexOffset+=sizeof(STagHeader);
		indexOffset+=tagHeader.tagLength;
		count++;
	}

	// That tag doesn't exist
	Throw_(kTNTErr_TagNotFound);
	
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetTag									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns a tag looked up by id
CTaggedDataSection *CTaggedDataGroup::GetTag(
	TTagId					inTagId)
{
	if (CTaggedDataSection *tag=FindConstructedTagById(inTagId))
		return tag;

	CDataStore::TDataOffset	indexOffset=GetStartOffset();
	TTagIndex				count=0;
	STagHeader				tagHeader;
	CDataStore::TDataOffset	ioCount=sizeof(STagHeader);
	
	while (indexOffset<mEnd)
	{
		ReadTagHeader(indexOffset,&tagHeader,ioCount);
	
		if (tagHeader.tagId==inTagId)
		{
			// Found the tag
			return MakeTag(count,tagHeader.tagId,indexOffset);
		}
	
		indexOffset+=sizeof(STagHeader);
		indexOffset+=tagHeader.tagLength;
		count++;
	}

	// That tag doesn't exist
	Throw_(kTNTErr_TagNotFound);
	
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetTagInfo								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the tag header offset and the tag length of the requested tag. Throws if it doesn't exist.
void CTaggedDataGroup::GetTagInfo(
	TTagId						inTagId,
	CDataStore::TDataOffset		&outTagHeaderOffset,
	CDataStore::TDataOffset		&outTagLength)
{
	CDataStore::TDataOffset	indexOffset=GetStartOffset();
	STagHeader				tagHeader;
	CDataStore::TDataOffset	ioCount=sizeof(STagHeader);
	
	while (indexOffset<mEnd)
	{
		ReadTagHeader(indexOffset,&tagHeader,ioCount);
	
		if (tagHeader.tagId==inTagId)
		{
			// Found the tag
			outTagHeaderOffset=indexOffset;
			outTagLength=tagHeader.tagLength;
			return;
		}
	
		indexOffset+=sizeof(STagHeader);
		indexOffset+=tagHeader.tagLength;
	}

	// That tag doesn't exist
	Throw_(kTNTErr_TagNotFound);	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MakeTag									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Create a tag with the given properties, override to create sub classes of CTaggedDataSection for your tags.
CTaggedDataSection *CTaggedDataGroup::MakeTag(
	TTagIndex				inTagIndex,
	TTagId					inTagId,
	CDataStore::TDataOffset	inTagHeaderOffset)
{
	CTaggedDataSection	*tag;
	
	ThrowIfMemFull_(tag=new CTaggedDataSection(this,inTagHeaderOffset));
	
	tag->SetTagIndex(inTagIndex);
	mConstructedTags.AddOrderedElement(&tag->mLink);
	
	return tag;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FindConstructedTagByIndex
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Finds a tag with the specified index
CTaggedDataSection *CTaggedDataGroup::FindConstructedTagByIndex(
	TTagIndex				inTagIndex)
{
	CListIndexerT<CTaggedDataSection>	indexer(&mConstructedTags);
	
	while (CTaggedDataSection *tag=indexer.GetNextData())
	{
		if (tag->GetTagIndex()==inTagIndex)
			return tag;
	}
	
	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FindConstructedTagById
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Finds a tag with the specified id
CTaggedDataSection *CTaggedDataGroup::FindConstructedTagById(
	TTagId					inTagId)
{
	CListIndexerT<CTaggedDataSection>	indexer(&mConstructedTags);
	
	while (CTaggedDataSection *tag=indexer.GetNextData())
	{
		if (tag->GetTagId()==inTagId)
			return tag;
	}
	
	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InsertTag											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Inserts an empty tag of the specified tag id into the data. The position of the tag is arbitary. No check is made as
// to whether the tag already exists.
void CTaggedDataGroup::InsertTag(
	TTagId						inTagId)
{
	CDataStore::TDataOffset		offset,length;

	InsertTag(inTagId,offset,length);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InsertTag											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// As above except returns the address of the new tag ready for passing to a CDataSection constructor.
void CTaggedDataGroup::InsertTag(
	TTagId						inTagId,
	CDataStore::TDataOffset		&outTagHeaderOffset,
	CDataStore::TDataOffset		&outTagLength)
{
	ThrowIfNil_(mDataStore);

	outTagHeaderOffset=GetLength();
	outTagLength=0;

	STagHeader					tagHeader={inTagId,0L};
	CDataStore::TDataOffset		ioCount=sizeof(tagHeader);

	SetLength(GetLength()+sizeof(tagHeader));
	
	tagHeader.tagId=::CFSwapInt32HostToBig(tagHeader.tagId);
	tagHeader.tagLength=::CFSwapInt32HostToBig(tagHeader.tagLength);
	mDataStore->PutBytes(outTagHeaderOffset+GetStartOffset(),&tagHeader,ioCount);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SortTags											Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Orders tags in the constructed tags list
signed char CTaggedDataGroup::SortTags(
	CTaggedDataSection			*inTagA,
	CTaggedDataSection			*inTagB,
	unsigned long				inSortKey,
	void						*inRefCon)
{
	if (inTagA->GetTagIndex()<inTagB->GetTagIndex())
		return -1;
	else
		return +1;
}
