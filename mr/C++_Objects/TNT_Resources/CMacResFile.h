// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CMacResFile.h
// © Mark Tully and TNT Software 2003 -- All Rights Reserved
// 17/2/03
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2003, Mark Tully and John Treece-Birch
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

class CMacResFile : public CResourceContainer
{
	protected:
		SInt16							mMacResFileRef;
		bool							mShouldClose;
		
		virtual CResource /*e*/			*LoadResource(
											ResType			inType,
											TResId			inId);
		virtual void					UnloadResource(
											CResource	*InResource);
		virtual void					ChangeResourceInfoSelf(
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
													
	public:
										CMacResFile(
											SInt16		inMacResFile,
											bool		inShouldClose);
		virtual							~CMacResFile();
	
		virtual SInt32 /*e*/			CountResourcesSelf(
											ResType		inType);
		virtual Handle /*e*/			GetAllResourceIdsAndNamesSelf(
											ResType		inType,
											SInt32		&outCount);		
		virtual Handle /*e*/			GetAllResTypesSelf();
		virtual SInt32 /*e*/			CountResTypesSelf();
		virtual UInt32 /*e*/			GetSizeFromDisk(
											ResType			inType,
											TResId			inId);
		virtual void /*e*/				GetFileLocation(
											FSSpec			*outSpec);

		CResource						*GetLoadedResourceByHandle(
											Handle		inHandle);

		SInt16							GetMacResFileRef()		{ return mMacResFileRef; }
};
