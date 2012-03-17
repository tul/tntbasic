// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CResourceContainer.h
// © Mark Tully and TNT Software 2003 -- All Rights Reserved
// 16/2/03
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

#include "ResourceTypes.h"
#include "CLinkedListT.h"

class CResource;
class CResContainerDirCache;

class CResourceContainer
{
	friend class CResource;
	
	protected:
		enum
		{
			kRCFlag_ReadOnly		= (1<<0),
			kRCFlag_NeedsFlush		= (1<<1),
			kRCFlag_RequiresResIds	= (1<<2)
		};
		UInt32							mFlags;		// see RCFlag_ enum		
		CLinkedListT<CResource>			mLoadedResources;

		CResContainerDirCache			*mCache;

		virtual CResource				*GetLoadedResource(
											ResType 		inType,
											TResId			inId);

		virtual CResource /*e*/			*LoadResource(
											ResType			inType,
											TResId			inId) = 0;
		virtual void					UnloadResource(
											CResource		*InResource) = 0;
		virtual void /*e*/				ChangeResourceInfoSelf(
											ResType			inWasType,
											TResId			inWasId,
											ResType			inNewType,
											TResId			inNewId,
											const TResName	inNewName) = 0;
		virtual void /*e*/				AddResourceSelf(
											ResType			inType,
											TResId			inId,
											const TResName	inName,
											Handle			inDataHandle,
											bool			inAdopt) = 0;
		virtual void /*e*/				DeleteResourceSelf(
											ResType			inType,
											TResId			inId) = 0;
		virtual void /*e*/				FlushSelf() = 0;
		virtual SInt32 /*e*/			CountResourcesSelf(
											ResType			inType) = 0;
		virtual Handle /*e*/			GetAllResourceIdsAndNamesSelf(
											ResType			inType,
											SInt32			&outCount) = 0;
		virtual Handle /*e*/			GetAllResTypesSelf() = 0;
		virtual SInt32 /*e*/			CountResTypesSelf() = 0;
		
		virtual void /*e*/				ResourceChanged(
											CResource		*inResource);

		virtual void /*e*/				ThrowResLoadFailed(
											OSStatus		inErrCode,
											ResType			inType,
											TResId			inId);

		// cached versions
		// most return a bool conveying whether or not cache was available
		virtual bool /*e*/				ResourceExistsCache(
											ResType			inType,
											TResId			inId,
											bool			*outExists);
		virtual void /*e*/				ChangeResourceInfoCache(
											ResType			inWasType,
											TResId			inWasId,
											ResType			inNewType,
											TResId			inNewId,
											const TResName	inNewName);
		virtual void /*e*/				AddResourceCache(
											ResType			inType,
											TResId			inId,
											const TResName	inName);
		virtual void /*e*/				DeleteResourceCache(
											ResType			inType,
											TResId			inId);
		virtual bool /*e*/				GetResIdForNameCache(
											ResType			inType,
											const TResName	inName,
											TResId			*outId);
		virtual bool /*e*/				GetNameForResIdCache(
											ResType			inType,
											TResId			inResId,
											TResName		outName,
											bool			*outFound);
		virtual bool /*e*/				CountResourcesCache(
											ResType			inType,
											SInt32			*outCount);
		virtual bool /*e*/				GetAllResourceIdsAndNamesCache(
											ResType			inType,
											Handle			*outHandle,
											SInt32			*outCount);
		virtual bool /*e*/				GetAllResTypesCache(
											Handle			*outHandle);
		virtual bool /*e*/				CountResTypesCache(
											SInt32			*outCount);


	public:
										CResourceContainer() :
											mFlags(0),
											mCache(NULL)
											{}
		virtual							~CResourceContainer();
		
		virtual void /*e*/				CreateAndInitCache();
											
		// Resource access
		virtual TResourceReference /*e*/GetResource(
											ResType			inType,
											TResId			inId);
		virtual Handle /*e*/			CopyResourceContent(
											ResType			inType,
											TResId			inId);
		virtual SInt32 /*e*/			CountResources(
											ResType			inType);
		virtual Handle /*e*/			GetAllResourceIdsAndNames(
											ResType			inType,
											SInt32			&outCount);
		virtual UInt32 /*e*/			GetSizeFromDisk(
											ResType			inType,
											TResId			inId);
		virtual bool /*e*/				ResourceExists(
											ResType			inType,
											TResId			inId);
		virtual bool					SameResource(
											ResType			inTypeA,
											TResId			inTypeAId,
											ResType			inTypeB,
											TResId			inTypeBId);
		virtual void /*e*/				ChangeResourceInfo(
											ResType			inWasType,
											TResId			inWasId,
											ResType			inNewType,
											TResId			inNewId,
											const TResName	inNewName);
		virtual void /*e*/				AddResource(
											ResType			inType,
											TResId			inId,
											const TResName	inName,
											Handle			inDataHandle,
											bool			inAdopt=true);
		virtual void /*e*/				DeleteResource(
											ResType			inType,
											TResId			inId);
		virtual void /*e*/				Flush();
		virtual bool /*e*/				ResourceInUse(
											ResType				inType,
											TResId				inId);
		virtual void /*e*/				CopyContentsFrom(
											CResourceContainer	*inCopyFrom);
		virtual TResId /*e*/			GetResIdForName(
											ResType				inType,
											const TResName		inName);
		virtual bool /*e*/				GetNameForResId(
											ResType				inType,
											TResId				inResId,
											TResName			outName);
											
		virtual void /*e*/				GetFileLocation(
											FSSpec			*outSpec) = 0;

		// Type access		
		virtual Handle /*e*/			GetAllResTypes();
		virtual SInt32 /*e*/			CountResTypes();
		
		// Container access
		inline bool						IsReadOnly()		{ return (mFlags&kRCFlag_ReadOnly)!=0; }
		inline bool						NeedsFlush()		{ return (mFlags&kRCFlag_NeedsFlush)!=0; }
		inline bool						RequiresResIds()	{ return (mFlags&kRCFlag_RequiresResIds)!=0; }
};
