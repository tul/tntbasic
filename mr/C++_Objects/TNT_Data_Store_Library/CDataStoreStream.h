// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// CDataStoreStream.h
// © Mark Tully and TNT Software 1999
// 22/8/99
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

#include		<LStream.h>
#include		"CDataStoreSection.h"

class CDataStoreStream : public LStream
{
	private:
		// redeclare mMarker and mLength as private to stop sub class access and force them to use accessors.
		// This is done as these are no longer kept updated.
		SInt32								mMarker;
		SInt32								mLength;

	protected:
		CDataStoreSectionMarker				mOffset;
		TSharablePtr<CDataStoreSection>		mSection;

	public:
		enum
		{
			kOffsetMarkerClass='Offs',		// A custom marker class for marking the read/write head in the stream
		};
	
											CDataStoreStream();
											CDataStoreStream(
												CDataStoreSection *inSection);
											CDataStoreStream(
												const CDataStoreStream &inOriginal);


		virtual CMarkableDataStore			*GetDataStore()	const						{ return GetSection() ? GetSection()->GetDataStore() : 0L; }
		
		virtual CDataStoreSection			*GetSection() const							{ return mSection; }
		virtual void						SetSection(
												CDataStoreSection *inSection);

		virtual void						SetMarker(
												SInt32			inOffset,
												EStreamFrom		inFromWhere);
								
		virtual SInt32						GetMarker() const;
		
		virtual void						SetLength(
												SInt32			inLength);
								
		virtual SInt32						GetLength() const;

		virtual ExceptionCode				PutBytes(
												const void		*inBuffer,
												SInt32			&ioByteCount);

		virtual ExceptionCode				GetBytes(
												void			*outBuffer,
												SInt32			&ioByteCount);
};
