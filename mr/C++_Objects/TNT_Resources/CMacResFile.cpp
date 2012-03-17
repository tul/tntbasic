// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CMacResFile.cpp
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

#include "CMacResFile.h"
#include "CMacResource.h"
#include "UResources.h"
#include "Marks Routines.h"
#include "Utility Objects.h"
#include "TNewHandleStream.h"
#include <cstdio>
#include <cstring>
#include "MoreFiles.h"
#include "MoreFilesExtras.h"
#include "Root.h"

/*
	Implements support for a traditional mac resource file
	
	General warning
	You don't want to call the BetterGetResource functions if you're already holding a handle
	to the given resource, as it can potentially release the handle.
*/

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CMacResFile::CMacResFile(
	SInt16		inMacResFile,
	bool		inShouldClose) :
	mMacResFileRef(inMacResFile),
	mShouldClose(inShouldClose)
{
	AssertThrow_(inMacResFile!=0 && inMacResFile!=-1);
	
	// check write permission
	Boolean		readOnly=true;
	ThrowIfOSErr_(::IsFileReadOnly(inMacResFile,&readOnly));
	if (readOnly)
		mFlags|=kRCFlag_ReadOnly;
	mFlags|=kRCFlag_RequiresResIds;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CMacResFile::~CMacResFile()
{
	if (mShouldClose)
	{
		::CloseResFile(mMacResFileRef);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadResource							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CResource *CMacResFile::LoadResource(
	ResType			inType,
	TResId			inId)
{
	Try_
	{
		Handle				handle;
		TResName			temp;
		
		handle=::BetterGetResource(mMacResFileRef,inType,inId);
	
		if (!handle)
			Throw_(::BetterGetResErr());
			
		::HNoPurge(handle);
		::HUnlock(handle);

		SInt16		dontCareId;
		ResType		dontCareType;
		TResName	name;
		::GetResInfo(handle,&dontCareId,&dontCareType,name);
		ThrowIfResError_();
		
		CResource		*res=NULL;
		Try_
		{
			res=new CMacResource(this,inType,inId,name,handle);
			ThrowIfMemFull_(res);
		}
		Catch_(err)
		{
			::ReleaseResource(handle);
			throw;
		}
		return res;		
	}
	Catch_(err)
	{
		ThrowResLoadFailed(ErrCode_(err),inType,inId);
	}
	
	Debug_("null resource!");
	return NULL;		// shouldn't get here
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ UnloadResource
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// no throw
void CMacResFile::UnloadResource(
	CResource	*inResource)
{
	delete inResource;
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CountResourcesSelf					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 CMacResFile::CountResourcesSelf(
	ResType		inType)
{
	UResFileSaver	safe(mMacResFileRef);
	SInt32			count=::Count1Resources(inType);
	return count;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetLoadedResourceByHandle				/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// searches the loaded resources for a resource that holds the given handle
CResource *CMacResFile::GetLoadedResourceByHandle(
	Handle		inHandle)
{
	CListIndexerT<CResource>		indexer(&mLoadedResources);
	
	while (CMacResource *res=static_cast<CMacResource*>(indexer.GetNextData()))
	{
		if (res->GetReadOnlyDataHandle()==inHandle)
			return res;
	}
	
	return NULL;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetAllResourceNamesSelf				/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the names of all resources in the container. format is stream << res id << pstrname
Handle CMacResFile::GetAllResourceIdsAndNamesSelf(
	ResType		inType,
	SInt32		&outCount)
{
	CNewHandleStream	stream;
	UResFileSaver		safe(mMacResFileRef);
	UResLoadSaver		safe2(false);
	
	outCount=CountResources(inType);
	
	for (SInt32 index=0; index<outCount; index++)
	{
		Handle			h=::Get1IndResource(inType,index+1);
		if (!h)
			ThrowResLoadFailed(memFullErr,inType,kNullResId);
		
		SInt16			id;
		ResType			type;
		unsigned char	name[257];
		::GetResInfo(h,&id,&type,name);
		ThrowIfResError_();
		
		stream << TResId(id) << name;

		if (!GetLoadedResourceByHandle(h))
			::ReleaseResource(h);		
	}
	
	return stream.DetachDataHandle();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetAllResTypesSelf					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
Handle CMacResFile::GetAllResTypesSelf()
{
	UResFileSaver		safe(mMacResFileRef);
	SInt32				count=::Count1Types();
	CNewHandleStream	stream;
	
	for (SInt32 i=1; i<=count; i++)
	{
		ResType		type=NULL;
		::Get1IndType(&type,i);
		stream << type;
	}
	
	return stream.DetachDataHandle();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CountResTypesSelf						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 CMacResFile::CountResTypesSelf()
{
	UResFileSaver		safe(mMacResFileRef);
	SInt32				count=::Count1Types();
	return count;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetSizeFromDisk								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// can be overridden to get the size from disk without loading the resource into memory
UInt32 CMacResFile::GetSizeFromDisk(
	ResType			inType,
	TResId			inId)
{
	CResource		*res=GetLoadedResource(inType,inId);
	
	if (res)
		return res->GetSize();
	else
	{
		UResLoadSaver	safe2(false);
		Handle			handle=::BetterGetResource(mMacResFileRef,inType,inId);
		UInt32			size;
		
		size=::GetResourceSizeOnDisk(handle);
		::ReleaseResource(handle);
		return size;
	}	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ NameFromResId									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// gets a resource name from an id. returns true and the name if found, false and an empty string if not
// throws on other error.
#if 0
bool CMacResFile::NameFromResId(
	ResType			inType,
	TResId			inResId,
	TResName		outName)
{
	UResFileSaver	safe(mMacResFileRef);
	Handle			h;
	{
		UResLoadSaver	safe2(false);
		h=::Get1Resource(inType,inResId);
	}
	
	if (h)
	{
		SInt16		id;
		ResType		type;
		Str255		resName;
		
		::GetResInfo(h,&id,&type,resName);
		if (!GetLoadedResourceByHandle(h))
			::ReleaseResource(h);
		
		CResourceName::MakeName(inResId,resName,outName);

		return true;
	}
	outName[0]=0;
	return false;
}
#endif

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ChangeResourceInfoSelf								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// changes a resources name/type
void CMacResFile::ChangeResourceInfoSelf(
	ResType			inWasType,
	TResId			inWasId,
	ResType			inNewType,
	TResId			inNewId,
	const TResName	inNewName)
{
	CResource		*res=GetLoadedResource(inWasType,inWasId);

	if (inWasType==inNewType)
	{	
		Handle			h=NULL;
		
		Try_
		{
			UResFileSaver	safe(mMacResFileRef);		
			short			tempAttrs;

			{
				UResLoadSaver	load(false);
				h=::Get1Resource(inWasType,inWasId);
				if (!h)
					Throw_(resNotFound);
			}
			tempAttrs=::GetResAttrs(h);

			::SetResAttrs(h,0);

			// ResInfo is different, store the res info in the resource file
			::SetResInfo(h,inNewId,inNewName);
			ThrowIfResError_();

			::SetResAttrs(h,tempAttrs);
		
			::ChangedResource(h);
			ThrowIfResError_();

			if (res)
				res->UpdateInfo(inNewType,inNewId,inNewName);
		}
		Catch_(err)
		{
			if (!res)
				::ReleaseResource(h);
			throw;
		}

		if (!res)
			::ReleaseResource(h);		
	}
	else
	{
		UResFileSaver	safe(mMacResFileRef);		
		Handle			h=::Get1Resource(inWasType,inWasId);
		if (!h)
			Throw_(resNotFound);

		Try_
		{		
			Handle			copy=h;
			ThrowIfOSErr_(::HandToHand(&copy));
			
			AddResource(inNewType,inNewId,inNewName,copy);
		}
		Catch_(err)
		{
			if (!res)
				::ReleaseResource(h);
			throw;
		}
		if (!res)
			::ReleaseResource(h);
		h=NULL;
		
		CHandleResource	*hres=dynamic_cast<CHandleResource*>(res);
		if (hres)
		{
			// get the new resource handle
			Handle newHandle=::Get1Resource(inNewType,inNewId);
			
			if (!newHandle)		// couldn't get the new resource, so delete it
			{
				DeleteResource(inNewType,inNewId);
				Throw_(memFullErr);
			}
			UHandleReleaser		rel(newHandle);
			hres->DetachHandle();		// stops the handle being released and stuff - that can be done when it's deleted
			hres->UpdateHandleAndInfo(inNewType,inNewId,inNewName,newHandle,true);
			rel.SetReleaseFlag(false);	// newHandle now owned by res
		}
		
		// finally, delete the old resource (res is no longer using it)
		DeleteResource(inWasType,inWasId);				
	}

	mFlags|=kRCFlag_NeedsFlush;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddResourceSelf										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// adds a resource.
// it has already been verified that this resource doesn't exist.
// the handle is owned by this function as soon as it's passed in
// the handle can be null meaning create an empty resource
void CMacResFile::AddResourceSelf(
	ResType			inType,
	TResId			inId,
	const TResName	inName,
	Handle			inDataHandle,
	bool			inAdopt)
{
	UResFileSaver	safe(mMacResFileRef);
	TResName		name;
	TResId			resId;

	if (!inAdopt && inDataHandle)
	{
		// AddResource adopts the handle, so if we're not supposed to adopt it dupe it now
		ThrowIfOSErr_(::HandToHand(&inDataHandle));
	}
	
	if (!inDataHandle)
	{
		inDataHandle=::NewHandle(0);
		ThrowIfMemFull_(inDataHandle);
	}

	UHandleReleaser	rel(inDataHandle);
		
	::AddResource(inDataHandle,inType,inId,inName);
	ThrowIfResError_();
	mFlags|=kRCFlag_NeedsFlush;
	::WriteResource(inDataHandle);
	ThrowIfResError_();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DeleteResourceSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// it has been already verified that this resource exists and isn't being used
void CMacResFile::DeleteResourceSelf(
	ResType			inType,
	TResId			inId)
{
	Handle			theRes=0L;
	UResFileSaver	safe(mMacResFileRef);
		
	::SetResLoad(false);
	theRes=::Get1Resource(inType,inId);
	::SetResLoad(true);
	
	if (theRes)
	{
		OSErr	err;
		::RemoveResource(theRes);
		err=::ResError();
		if (err==noErr)
		{
			::DisposeHandle(theRes);
			mFlags|=kRCFlag_NeedsFlush;
		}
		else
		{
			ReleaseResource(theRes);
			ThrowOSErr_(err);
		}
	}
	else
		Throw_(resNotFound);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FlushSelf												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// flushes changes to disk
void CMacResFile::FlushSelf()
{
	::UpdateResFile(mMacResFileRef);
	ThrowIfResError_();
	ThrowIfOSErr_(::FlushFile(mMacResFileRef));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetFileLocation										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the file location of the res file
void CMacResFile::GetFileLocation(
	FSSpec			*outSpec)
{
	ThrowIfOSErr_(::FSpGetFileLocation(mMacResFileRef,outSpec));
}
