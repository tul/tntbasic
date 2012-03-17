// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CResContainerDirCache.h
// © Mark Tully 2005
// 24/06/05
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

#include "ResourceTypes.h"
#include <hash_map.h>

struct SResContents
{
	SInt32		count;
	Handle		contents;
};

class CResContainerDirCache
{
	protected:
		typedef hash_map<ResType,SResContents>	CResHash;
		CResHash							mResContentsHash;
		Handle								mResTypes;
	
	public:
							CResContainerDirCache();
		virtual				~CResContainerDirCache();

		bool /*e*/			RetrieveResTypeList(
									Handle			*outHandle);
		bool /*e*/			RetrieveResList(
									ResType			inResType,
									SInt32			*outCount,
									Handle			*outHandle);
		void /*e*/			SetResTypeList(
									Handle			inHandle);
		void /*e*/			SetResList(
									ResType			inResType,
									SInt32			inCount,
									Handle			inHandle);

		void /*e*/			AddResType(
									ResType			inResType);
		void /*e*/			AddResource(
									ResType			inResType,
									TResId			inResId,
									const TResName	inResName);
		bool /*e*/			RemoveResource(
									ResType			inResType,
									TResId			inResId);
									
		bool /*e*/			ResourceExists(
									ResType			inResType,
									TResId			inResId);
		TResId /*e*/		GetResIdForName(
									ResType			inResType,
									const TResName	inResName);
		bool /*e*/			GetNameForResId(
									ResType			inResType,
									TResId			inId,
									TResName		outName);
		SInt32 /*e*/		CountResources(
									ResType			inResType);
};
