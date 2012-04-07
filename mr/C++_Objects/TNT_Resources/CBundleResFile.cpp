// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CBundleResFile.cpp
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

/*
	implements res file access from directories - which are referred to as bundles when treating them as a single file

	IMPORTANT
	To maintain consistency and compatibility, if a file is missing from the bundle a resNotFound exception should be
	thrown and NOT a fnfErr.
*/

#include "CBundleResFile.h"
#include "CBundleResource.h"
#include "TFolderIterator.h"
#include "TNewHandleStream.h"
#include "UResources.h"
#include "Marks Routines.h"
#include "Utility Objects.h"
#include "MoreFiles.h"
#include "MoreFilesExtras.h"
#include "UResourceMgr.h"
#include "Root.h"

static const TFolderIterator::TFolderIteratorFlags		kResTypeIteratingFlags=TFolderIterator::kResolveFolderAliases|TFolderIterator::kReturnFolders|TFolderIterator::kIgnoreBrokenAliases|TFolderIterator::kIgnoreInvisibles;
static const TFolderIterator::TFolderIteratorFlags		kResIteratingFlags=TFolderIterator::kResolveFileAliases|TFolderIterator::kReturnFiles|TFolderIterator::kIgnoreBrokenAliases|TFolderIterator::kIgnoreInvisibles;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CBundleResFile::CBundleResFile(
	const FSSpec	*inSpec,
	bool			inIsReadOnly)
{
	if (inIsReadOnly)
	{
		mFlags|=kRCFlag_ReadOnly;
	}
	mFlags|=kRCFlag_RequiresResIds;
	::BlockMoveData(inSpec,&mResFile,sizeof(mResFile));
	
	// get some cache going
	CreateAndInitCache();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadResource								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CResource *CBundleResFile::LoadResource(
	ResType			inType,
	TResId			inId)
{
	// load the file into a handle
	CResource		*res=NULL;

	Try_
	{
		FSSpec		spec;
		if (!GetFSSpecForRes(inType,inId,&spec))
		{
			Throw_(resNotFound);
		}

		// get res name from the res
		Str255				fullName;
		ThrowIfOSErr_(::GetLongFileName(&spec,fullName));
		CResourceName		name(fullName);
		TResName			resName;
		name.ResName(resName);

		// load into a handle
		Handle				handle=ReadResourceData(inType,inId,resName,&spec);
		UHandleReleaser		rel(handle);

#if TARGET_RT_LITTLE_ENDIAN
		// it's ok to modify this data directly here as the handle was newly allocated and read from disk
		ApplyEndianFlip(inType,handle,true,false);
#endif
		
		res=new CBundleResource(this,inType,inId,resName,handle,true);
		ThrowIfMemFull_(res);

		rel.SetReleaseFlag(false);
	}
	Catch_(err)
	{
		ThrowResLoadFailed(ErrCode_(err),inType,inId);
	}
	return res;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ UnloadResource
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBundleResFile::UnloadResource(
	CResource		*inResource)
{
	delete inResource;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ChangeResourceInfoSelf					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// moves the resource passed to a different id/name/type
void CBundleResFile::ChangeResourceInfoSelf(
	ResType			inWasType,
	TResId			inWasId,
	ResType			inNewType,
	TResId			inNewId,
	const TResName	inNewName)
{
	FSSpec			oldSpec,newSpec;

	CreateResType(inNewType);
	if (!GetFSSpecForRes(inWasType,inWasId,&oldSpec))
	{
		Throw_(resNotFound);
	}
	if (GetFSSpecForRes(inNewType,inNewId,&newSpec) && !CompareFSSpecs(&oldSpec,&newSpec))
	{
		Throw_(errFSForkExists);
	}
	else
	{
		CResource		*res=GetLoadedResource(inWasType,inWasId);

		// TODO long file name support - seems to work already ?
		TResName		newName;
		CResourceName::MakeName(inNewId,inNewName,newName);

		MacifyFileName(newName);
		
		ThrowIfOSErr_(HMoveRenameCompat(oldSpec.vRefNum,oldSpec.parID,oldSpec.name,newSpec.parID,NULL,newName));

		if (res)
		{
			res->UpdateInfo(inNewType,inNewId,inNewName);
		}
	}
}

// will apply an endian flip to the data if it is a format that is endian flipped by the OS 'automagically'
// this is done to remain compatable with Mac OS Resource files, which also have this behaviour
// if allowed to modify the original handle, then it is flipped in place
// if not, a new handle is allocated and returned, and the caller is responsible for disposing of it
// if flipped in place, null is returned, indicating there is no additional handle to release
// inDataIsLittleEndian indicates the endianess of the data contained in inHandle currently
Handle CBundleResFile::ApplyEndianFlip(
	ResType			inType,
	Handle			inHandle,
	bool			inCanModifyOriginalHandle,
	bool			inDataIsLittleEndian)
{
	Handle			result=0;

	if (UResFlip::HasFlipper(inType))
	{
		Handle		flip=inHandle;

		//ECHO("Flipping resource of type "  << ((SInt8*)&inType)[0] << ((SInt8*)&inType)[1] << ((SInt8*)&inType)[2] << ((SInt8*)&inType)[3] << "\n");
		if (!inCanModifyOriginalHandle)
		{
			result=inHandle;
			ThrowIfOSErr_(::HandToHand(&result));
			flip=result;
		}
		UHandleReleaser	rel(result);
		UHandleLocker	lock(flip);
		OSStatus	err=::CoreEndianFlipData(
			kCoreEndianResourceManagerDomain,
			inType,
			0,
			*flip,
			::GetHandleSize(flip),
			inDataIsLittleEndian);
		ThrowIfOSErr_(err);
		rel.Release();
	}

	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddResourceSelf							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// handle is adopted immediately
// handle can be null meaning create an empty resource
void CBundleResFile::AddResourceSelf(
	ResType			inType,
	TResId			inId,
	const TResName	inName,
	Handle			inDataHandle,
	bool			inAdopt)
{
	UHandleReleaser	rel(inDataHandle,inAdopt);

#if TARGET_RT_LITTLE_ENDIAN
	// if we're a little endian machine writing to our big endian file format, perform an endian flip on the data if necessary
	Handle			newHandle;
	newHandle=ApplyEndianFlip(inType,inDataHandle,inAdopt,true);
	UHandleReleaser	rel2(newHandle,true);
	if (newHandle)
	{
		inDataHandle=newHandle;
	}
#endif
	
	SaveHandleToFile(inType,inId,inName,inDataHandle,true);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DeleteResourceSelf						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// deletes a resource
void CBundleResFile::DeleteResourceSelf(
	ResType			inType,
	TResId			inId)
{
	FSSpec			spec;
	if (!GetFSSpecForRes(inType,inId,&spec))
	{
		Throw_(resNotFound);
	}
	else
	{
		ThrowIfOSErr_(::FSpDelete(&spec));
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FlushSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBundleResFile::FlushSelf()
{
	// nothing to do - files all automatically flush themselves as necessary
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CountResourcesSelf						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 CBundleResFile::CountResourcesSelf(
	ResType			inType)
{
	return GatherResources(inType,NULL);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GatherResources							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the number of resources in a folder
// if outHandle is != null then it also returns a handle stream with all the resource details in
SInt32 CBundleResFile::GatherResources(
	ResType			inType,
	Handle			*outHandle)
{
	FSSpec			spec;
	SInt32			count=0;
	CNewHandleStream	stream;

	// handle should be unallocated
	Assert_(outHandle==NULL || *outHandle==0);

//	ECHO("GatherResources() type " << ((SInt8*)&inType)[0] << ((SInt8*)&inType)[1] << ((SInt8*)&inType)[2] << ((SInt8*)&inType)[3]);

	if (GetFSSpecForResType(inType,&spec))
	{
//		ECHO("Got folder");

		TFolderIterator		iter(spec);
		FSSpec				item;

		iter.SetIteratorFlags(kResIteratingFlags);

		while (iter.GetNextItem(item))
		{
			Str255				longName;
			ThrowIfOSErr_(::GetLongFileName(&item,longName));

//			ECHOINDENT("File: " << longName);
			
			CResourceName		name(longName);
			if (name.ResIdAvailable())
			{
				count++;

				if (outHandle)
				{
					TResName		resName;
					stream << name.ResId() << name.ResName(resName);
				}
			}
			else
			{
//				ECHOINDENT("(no resid)");
			}
		}
	}
	else
	{
//		ECHO("No folder");
	}

	if (outHandle)
	{
		*outHandle=stream.DetachDataHandle();
		if (!*outHandle)
		{
			Assert_(count==0);
			*outHandle=::NewHandle(0);		// empty handle rather than nil handle (will happen if no resources)
			ThrowIfMemFull_(*outHandle);
		}
	}

	return count;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetAllResourceIdsAndNamesSelf				/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
Handle CBundleResFile::GetAllResourceIdsAndNamesSelf(
	ResType			inType,
	SInt32			&outCount)
{
	Handle			myHandle=0;
	outCount=GatherResources(inType,&myHandle);
	return myHandle;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetSizeFromDisk							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the size of a resource on disk without loading it into memory
UInt32 CBundleResFile::GetSizeFromDisk(
	ResType			inType,
	TResId			inId)
{
	FSSpec			spec;
	long			dataSize=0,rsrcSize=0;
	UInt32			size=0;

	if (!GetFSSpecForRes(inType,inId,&spec))
	{
		ThrowResLoadFailed(resNotFound,inType,inId);
	}

	ThrowIfOSErr_(FSpGetFileSize(&spec,&dataSize,&rsrcSize));
	
	size=dataSize;

	return size;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetFileLocation							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CBundleResFile::GetFileLocation(
	FSSpec			*outSpec)
{
	::BlockMoveData(&mResFile,outSpec,sizeof(*outSpec));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetAllResTypesSelf						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
Handle CBundleResFile::GetAllResTypesSelf()
{
	CNewHandleStream	stream;
	TFolderIterator		iter(mResFile);
	FSSpec				item;
	SInt32				count=0;

	iter.SetIteratorFlags(kResTypeIteratingFlags);

	while (iter.GetNextItem(item))
	{
		ResType		type;
		if (IsFolderAResType(&item,&type))
		{
			stream << type;
		}
	}

	return stream.DetachDataHandle();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsFolderAResType
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns true if the folder passed is a restype folder
// this currently means it has a 4 char name
bool CBundleResFile::IsFolderAResType(
	const FSSpec		*inSpec,
	ResType				*outType)
{
	bool	ok=(inSpec->name[0]<=sizeof(ResType));
	if (ok && outType)
	{
		*outType='    ';
		::BlockMoveData(&inSpec->name[1],outType,Lesser(sizeof(*outType),inSpec->name[0]));
		*outType=CFSwapInt32BigToHost(*outType);
	}
	return ok;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CountResTypesSelf							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 CBundleResFile::CountResTypesSelf()
{
	TFolderIterator		iter(mResFile);
	FSSpec				item;
	SInt32				count=0;

	iter.SetIteratorFlags(kResTypeIteratingFlags);

	while (iter.GetNextItem(item))
	{
		if (IsFolderAResType(&item,NULL))
		{
			count++;
		}
	}

	return count;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetFSSpecForResType
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the FSSpec for a given res type
// if the res type has null chrs in it these are clipped off the end when producing the folder name
// eg 'snd ' 'snd\0' == 'snd'
// returns true if a folder was found - returns false if not
// if the spec doesn't exist - it returns false and fills out the spec correctly - ready to create the dir
bool CBundleResFile::GetFSSpecForResType(
	ResType			inType,
	FSSpec			*outSpec)
{
	Str32			resType;

	::BlockMoveData(&mResFile,outSpec,sizeof(*outSpec));
	resType[sizeof(ResType)+1]=0;
	inType=CFSwapInt32HostToBig(inType);
	::BlockMoveData(&inType,&resType[1],sizeof(inType));
	resType[0]=0;
	for (int i=0; i<sizeof(ResType); i++)
	{
		if (resType[1+i]==' ' || resType[1+i]==0)
		{
			break;
		}
		else
		{
			resType[0]++;
		}
	}

	OSErr		err=MakeFSSpecFromFileSpec(outSpec,resType);
	Boolean		exists;

	if (err==fnfErr)
	{
		exists=false;
	}
	else
	{
		ThrowIfOSErr_(err);
		ThrowIfOSErr_(FSpSpecExists(outSpec,&exists));
	}

	return exists;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetFSSpecForRes									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the FSSpec for a given restype,resid pair
// if the file doesn't exist it returns false, otherwise it returns true and fills out the spec
bool CBundleResFile::GetFSSpecForRes(
	ResType			inType,
	TResId			inResId,
	FSSpec			*outSpec)
{
	bool			found=false;
	TResName		resName;

	found=GetNameForResId(inType,inResId,resName);

	if (!found)
	{
		resName[0]=0;
	}

	// whether its found or not, create an fsspec for it
	// this allows creation of new res files easily

	TResName		fileName;

	CResourceName::MakeName(inResId,resName,fileName);
	if (GetFSSpecForResType(inType,outSpec))		// NB: restype dir might not exist if this is the first res in that type
	{
		OSErr			err;
		
		if (found)
		{
			err=::MakeFSSpecFromFileSpecLongName(outSpec,fileName);
		}
		else
		{
			// NB: if not found can't use the long name versions of the file accessors as we can't encode a long file name for a non-existant file into an FSSpec
			err=::MakeFSSpecFromFileSpec(outSpec,fileName);
		}

		if (err==fnfErr)
		{
			found=false;
		}
		else
		{
			ThrowIfOSErr_(err);
		}
	}
	else
	{
		found=false;
	}

	return found;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CreateResType										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// creates the res type dir for the res type passed. does nothing if it already exists. throws on error
void CBundleResFile::CreateResType(
	ResType				inType)
{
	FSSpec			resTypeSpec;
	if (!GetFSSpecForResType(inType,&resTypeSpec))
	{
		long	dirID;
		// TODO: long file name support not used here - but the file names involved for dirs are short anyway
		ThrowIfOSErr_(FSpDirCreate(&resTypeSpec,smSystemScript,&dirID));
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SaveHandleToFile									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// saves a handle to a file resource. if inCanCreate==true then the file is created if it's not there.
// inName is used when creating a file to give it a name - it can be NULL
// inHandle can be NULL to create an emtpy file
void CBundleResFile::SaveHandleToFile(
	ResType				inType,
	TResId				inResId,
	const TResName		inName,
	Handle				inHandle,
	bool				inCanCreate)
{
	// create the res file dir if it's not present
	if (inCanCreate)
	{
		CreateResType(inType);
	}
	
	FSSpec		spec;

	// create the file if it's not present
	if (!GetFSSpecForRes(inType,inResId,&spec))
	{
		if (inCanCreate)
		{
			TResName		newName;
			
			// create the file - the name will have been set to just be the resid by default - set it to the name
			// passed if any
			if (inName)
			{
				CResourceName::MakeName(inResId,inName,newName);
			}
			else
			{
				::NumToString(inResId,newName);
			}
			
			// make the file name friendly to HFS
			MacifyFileName(newName);
			
			OSType		creator;
			FSSpec		parentDir;
			GetAppCreatorCode(&creator);
			if (GetFSSpecForResType(inType,&parentDir))
			{
//				ThrowIfOSErr_(::FSpCreate(&spec,creator,inType,smSystemScript));
				ThrowIfOSErr_(::CreateFileLongName(&parentDir,newName,creator,inType,&spec));
			}
		}
		else
		{
			Throw_(resNotFound);
		}
	}

	WriteResourceData(inType,inResId,inName,&spec,inHandle);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ WriteResourceData									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// actually writes the resource data to the fsspec passed
// you can transform the data at this point by overriding this func
// use a head patch to mod the handle before returning it or replace the file access completely
void CBundleResFile::WriteResourceData(
	ResType			inType,
	TResId			inResId,
	const TResName	inName,
	FSSpec			*inSpec,
	Handle			inHandle)
{
	short				fRef=-1;
	SInt32				size=inHandle ? ::GetHandleSize(inHandle) : 0;
	long				written=size;

	ThrowIfOSErr_(FSpOpenDF(inSpec,fsWrPerm,&fRef));

	Try_
	{
		ThrowIfOSErr_(SetEOF(fRef,size));

		if (size>0)
		{
			UHandleLocker		lock(inHandle);
			ThrowIfOSErr_(FSWrite(fRef,&written,*inHandle));
		}
		Assert_(written==size);
		//ThrowIfOSErr_(FlushFile(fRef));		// NB: not flushing here - file is flushed when it's closed automatically - IMPORTANT: catalog info not flushed until FlushVol() called apparently
		ThrowIfOSErr_(FSClose(fRef));
	}
	Catch_(err)
	{
		FSClose(fRef);
		throw;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReadResourceData									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// actually reads the resource data from the fsspec passed
// you can transform the data at this point by overriding this func
// use a tail patch to mod the handle before returning it or replace the file access completely
Handle CBundleResFile::ReadResourceData(
	ResType		inType,
	TResId		inResId,
	const TResName	inName,
	FSSpec		*inSpec)
{
	long		dataSize,rsrcSize;
	ThrowIfOSErr_(FSpGetFileSize(inSpec,&dataSize,&rsrcSize));

	Handle		handle=::NewHandle(dataSize);
	ThrowIfMemFull_(handle);

	UHandleReleaser	rel(handle);

	short		fRef=-1;
	long		count=dataSize;

	ThrowIfOSErr_(FSpOpenDF(inSpec,fsRdPerm,&fRef));

	HLock(handle);
	ThrowIfOSErr_(FSRead(fRef,&count,*handle));
	HUnlock(handle);

	ThrowIfOSErr_(FSClose(fRef));

	Assert_(count==dataSize);	// check we read all the data

	rel.SetReleaseFlag(false);
	return handle;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MacifyFileName
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// get rid of any chrs that will upset the file system
// dunno if there's an official api for this, i can't find one
void CBundleResFile::MacifyFileName(
	Str255		ioFileName)
{
	for (int i=0; i<ioFileName[0]; i++)
	{
		if (ioFileName[1+i]==':' || ioFileName[1+i]=='/')
		{
			ioFileName[1+i]='-';
		}
	}
}
