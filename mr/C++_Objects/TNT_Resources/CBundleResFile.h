// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CBundleResFile.h
// © Mark Tully 2005
// 11/05/05
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

#include "CResourceContainer.h"

class CBundleResFile : public CResourceContainer
{
	friend class CBundleResource;

	protected:
		FSSpec							mResFile;			// fsspec to the resfile directory
		
		virtual CResource /*e*/			*LoadResource(
											ResType			inType,
											TResId			inId);
		virtual void					UnloadResource(
											CResource		*InResource);
		virtual void /*e*/				ChangeResourceInfoSelf(
											ResType			inWasType,
											TResId			inWasId,
											ResType			inNewType,
											TResId			inNewId,
											const TResName	inNewName);
		virtual void /*e*/				AddResourceSelf(
											ResType			inType,
											TResId			inId,
											const TResName	inName,
											Handle			inDataHandle,
											bool			inAdopt);
		virtual void /*e*/				DeleteResourceSelf(
											ResType			inType,
											TResId			inId);
		virtual void /*e*/				FlushSelf();
		virtual UInt32 /*e*/			GetSizeFromDisk(
											ResType			inType,
											TResId			inId);

		bool							IsFolderAResType(
											const FSSpec	*inSpec,
											ResType			*outType);
		SInt32							GatherResources(
											ResType			inType,
											Handle			*outHandle);

		bool /*e*/						GetFSSpecForResType(
											ResType			inType,
											FSSpec			*outSpec);
		bool /*e*/						GetFSSpecForRes(
											ResType			inType,
											TResId			inResId,
											FSSpec			*outSpec);
		void /*e*/						SaveHandleToFile(
											ResType			inType,
											TResId			inResId,
											const TResName	inName,
											Handle			inHandle,
											bool			inCanCreate);
		
		void /*e*/						CreateResType(
											ResType			inType);

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

		void							MacifyFileName(
											Str255			ioFileName);

	public:
										CBundleResFile(
											const FSSpec	*inSpec,
											bool			inIsReadOnly);

		virtual SInt32 /*e*/			CountResourcesSelf(
											ResType			inType);
		virtual Handle /*e*/			GetAllResourceIdsAndNamesSelf(
											ResType			inType,
											SInt32			&outCount);
		virtual void /*e*/				GetFileLocation(
											FSSpec			*outSpec);
		virtual Handle /*e*/			GetAllResTypesSelf();
		virtual SInt32 /*e*/			CountResTypesSelf();
};
