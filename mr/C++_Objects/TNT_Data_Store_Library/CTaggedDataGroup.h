// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// CTaggedDataGroup.h
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

#include		"CTaggedDataSection.h"
#include		"CSortedLinkedListT.h"

struct STagHeader;

class CTaggedDataGroup : public CTaggedDataSection
{
	public:
		typedef UInt16				TTagIndex;

	protected:
		CSortedLinkedListT<CTaggedDataSection>	mConstructedTags;

		void /*e*/					ReadTagHeader(
										CDataStore::TDataOffset		inOffset,
										STagHeader					*outHeader,
										CDataStore::TDataOffset		&ioLength);
	
		virtual CTaggedDataSection	*FindConstructedTagByIndex(
										TTagIndex					inTagIndex);
		virtual CTaggedDataSection	*FindConstructedTagById(
										TTagId						inTagId);
										
		virtual CTaggedDataSection *MakeTag(
										TTagIndex					inTagIndex,
										TTagId						inTagId,
										CDataStore::TDataOffset		inTagHeaderOffset);
		
		static signed char			SortTags(
										CTaggedDataSection			*inTagA,
										CTaggedDataSection			*inTagB,
										unsigned long				inSortKey,
										void						*inRefCon);

	public:

		/*e*/						CTaggedDataGroup(
										CMarkableDataStore			*inStore,
										CDataStore::TDataOffset		inStartOffset,
										CDataStore::TDataOffset		inLength,
										TTagFlags					inFlags=0,
										CTaggedDataSection::TTagId	inTagId=0);
	
		/*e*/						CTaggedDataGroup(
										CTaggedDataGroup			*inParentGroup,
										CDataStore::TDataOffset		inStartOffset,
										CDataStore::TDataOffset		inLength,
										TTagFlags					inFlags=0,
										CTaggedDataSection::TTagId	inTagId=0);

		virtual void /*e*/			GetTagInfo(
												TTagId						inTagId,
												CDataStore::TDataOffset		&outTagHeaderOffset,
												CDataStore::TDataOffset		&outTagLength);

		virtual CTaggedDataSection /*e*/	*GetTag(
												TTagId						inTagId);
		virtual CTaggedDataSection /*e*/	*GetNthTag(
												TTagIndex					inTagIndex);
		virtual TTagIndex /*e*/				CountTags();
		
		virtual void /*e*/					InsertTag(
												TTagId						inTagId,
												CDataStore::TDataOffset		&outTagHeaderOffset,
												CDataStore::TDataOffset		&outTagLength);
		virtual void /*e*/					InsertTag(
												TTagId						inTagId);

};
