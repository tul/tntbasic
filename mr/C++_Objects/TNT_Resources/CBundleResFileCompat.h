// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CBundleResFileCompat.h
// © Mark Tully 2005
// 22/05/05
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2005, Mark Tully and John Treece-Birch
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

#include "CBundleResFile.h"

class CBundleResFileCompatClassifier
{
	public:
		// this function will allow the bundleresfilecompat class to use the resource type loader/save code for the returned
		// type for the type passed. eg for restypes that are really picts but are stored in another restype, just make
		// classifyrestype return them as 'PICT' and they'll invoke the same loading/saving code
		virtual bool /*e*/				TypesEquiv(
											ResType			inType1,
											ResType			inType2) = 0;
};

class CBundleResFileCompat : public CBundleResFile
{
	private:
		typedef CBundleResFile			inherited;
		
	protected:
		CBundleResFileCompatClassifier	*mClassifier;

		virtual void /*e*/				WriteResourceData(
											ResType			inType,
											TResId			inResId,
											const TResName	inName,
											FSSpec			*inSpec,
											Handle			inHandle);

		virtual Handle /*e*/			ReadResourceData(
											ResType			inType,
											TResId			inResId,
											const TResName	inName,
											FSSpec			*inSpec);
		
		virtual UInt32 /*e*/			GetSizeFromDisk(
											ResType			inType,
											TResId			inId);

		virtual Handle /*e*/			ReadPictFile(
											FSSpec			*inSpec);
		virtual void /*e*/				WritePictFile(
											FSSpec			*inSpec,
											Handle			inHandle);

		virtual bool /*e*/				TypesEquiv(
											ResType			inType1,
											ResType			inType2);

	public:
										CBundleResFileCompat(
											const FSSpec					*inSpec,
											bool							inIsReadOnly,
											CBundleResFileCompatClassifier	*inClassifier) :
											CBundleResFile(inSpec,inIsReadOnly),
											mClassifier(inClassifier)
										{
										}
};
