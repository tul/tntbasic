// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CResContainerDirCache.cpp
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

/*
	caches the directory listing of a res file
	useful for the slower access res file directories
*/

#include "CResContainerDirCache.h"
#include "TNT_Errors.h"
#include "TNewHandleStream.h"
#include "Utility Objects.h"
#include "TNT_Debugging.h"
#include "Marks Routines.h"

CResContainerDirCache::CResContainerDirCache() :
	mResTypes(NULL)
{
}

CResContainerDirCache::~CResContainerDirCache()
{
	if (mResTypes)
	{
		::DisposeHandle(mResTypes);
		mResTypes=NULL;
	}

	// free all handles referenced from the hash map
	for (CResHash::const_iterator it=mResContentsHash.begin(); it!=mResContentsHash.end(); ++it)
	{
		Handle	h=(*it).second.contents;
		if (h!=NULL)
		{
			::DisposeHandle(h);
		}
	}
}

// returns the res types list that has been cached
bool CResContainerDirCache::RetrieveResTypeList(
	Handle			*outHandle)
{
	if (mResTypes)
	{
		*outHandle=mResTypes;
		return true;
	}
	return false;
}

/*e*/
// retrives the list of resources for a given res type, returns true if it was cached, false if not
bool CResContainerDirCache::RetrieveResList(
	ResType			inResType,
	SInt32			*outCount,
	Handle			*outHandle)
{
	bool			r=false;
	
	try
	{
		CResHash::const_iterator	it=mResContentsHash.find(inResType);

		if (it!=mResContentsHash.end())
		{
			*outCount=(*it).second.count;
			*outHandle=(*it).second.contents;
			r=true;
		}
		else
		{
			*outCount=0;
			*outHandle=NULL;
			r=false;
		}
	}
	catch(...)
	{
		Throw_(kTNTErr_STLException);
	}
	
	return r;
}

// sets the res type list
// handle is adopted
void CResContainerDirCache::SetResTypeList(
	Handle			inHandle)
{
	if (mResTypes)
	{
		::DisposeHandle(mResTypes);
	}
	mResTypes=inHandle;
}

/*e*/
// sets the list of resources for a type
// handle is adopted
void CResContainerDirCache::SetResList(
	ResType			inResType,
	SInt32			inCount,
	Handle			inHandle)
{
	try
	{
		CResHash::const_iterator	it=mResContentsHash.find(inResType);
		SResContents	cur={0,NULL};

		if (it!=mResContentsHash.end())
		{
			cur=(*it).second;
		}
		
		if (cur.contents)
		{
			::DisposeHandle(cur.contents);
		}

		cur.contents=inHandle;
		cur.count=inCount;

		mResContentsHash.insert(CResHash::value_type(inResType,cur));
		
		it=mResContentsHash.find(inResType);
		AssertThrow_(it!=mResContentsHash.end());
		
	}
	catch(...)
	{
		Throw_(kTNTErr_STLException);
	}
}

/*e*/
// adds 1 type to the currently cached res types list
void CResContainerDirCache::AddResType(
	ResType			inResType)
{
	if (mResTypes==NULL)
	{
		mResTypes=::NewHandle(sizeof(ResType));
		ThrowIfMemFull_(mResTypes);
		((ResType*)*mResTypes)[0]=inResType;
	}
	else
	{
		CNewHandleStream	stream(mResTypes,false);
		SInt32				length=stream.GetLength();
		bool				found=false;

		while (stream.GetMarker()<length)
		{
			ResType		type;

			stream >> type;

			if (type==inResType)
			{
				found=true;
				break;
			}
		}

		if (!found)
		{
			stream << inResType;
		}
	}
}

/*e*/
// adds 1 resource to the currently cached resource list for a type
// if a resource with the id passed already exists then it overwrites it
void CResContainerDirCache::AddResource(
	ResType			inResType,
	TResId			inResId,
	const TResName	inResName)
{
	RemoveResource(inResType,inResId);

	Try_
	{
		SResContents		cur={0,NULL};
		CResHash::iterator		it=mResContentsHash.find(inResType);
		CNewHandleStream	stream;
		int					ok;

		if (it!=mResContentsHash.end())
		{
			cur=(*it).second;
			mResContentsHash.erase(it);
		}
		if (cur.contents==NULL)
		{
			cur.contents=::NewHandle(0);
			ThrowIfMemFull_(cur.contents);
		}
		
		stream.SetDisposeHandleFlag(false);
		stream.SetDataHandle(cur.contents);

		cur.count++;
		stream.SetMarker(0,streamFrom_End);
		stream << inResId;
		stream << inResName;

		mResContentsHash.insert(CResHash::value_type(inResType,cur));
	}
	Catch_(err)
	{
		throw;
	}
	catch(...)
	{
		Throw_(kTNTErr_STLException);
	}
}

/*e*/
// removes a resource from the cache
// returns true if it was found
bool CResContainerDirCache::RemoveResource(
	ResType			inResType,
	TResId			inResId)
{
	bool				found=false;
	
	Try_
	{
		SResContents		cur={0,NULL};
		CResHash::iterator	it=mResContentsHash.find(inResType);
		CNewHandleStream	stream;

		if (it!=mResContentsHash.end())
		{
			cur=(*it).second;
		
			if (cur.contents)
			{
				stream.SetDisposeHandleFlag(false);
				stream.SetDataHandle(cur.contents);

				SInt32			length=stream.GetLength();

				for (SInt32 ri=0; ri<cur.count; ri++)
				{
					TResId				id;
					TResName			name;
					SInt32				marker=stream.GetMarker();

					stream >> id;
					stream >> name;

					if (id==inResId)
					{
						// remove this section
						SInt32			curMarker=stream.GetMarker();
						{
							UHandleLocker	locked(cur.contents);
							::BlockMoveData((*cur.contents)+curMarker,(*cur.contents)+marker,length-curMarker);
						}
						stream.SetMarker(marker,streamFrom_Start);
						length-=(curMarker-marker);
						// resize handle + stream internal length
						stream.SetLength(length);
						cur.count--;
						ri--;
						found=true;
						break;
					}
				}

				if (found)
				{
					mResContentsHash.erase(it);
					mResContentsHash.insert(CResHash::value_type(inResType,cur));
				}
			}
		}
	}
	Catch_(err)
	{
		throw;
	}
	catch(...)
	{
		Throw_(kTNTErr_STLException);
	}

	return found;
}

/*e*/
bool CResContainerDirCache::ResourceExists(
	ResType			inResType,
	TResId			inResId)
{
	Handle			h=NULL;
	SInt32			count=0;
	bool			exists=false;
	
	if (RetrieveResList(inResType,&count,&h))
	{
		CNewHandleStream	stream(h,false);
		for (SInt32 i=0; i<count; i++)
		{
			TResId		id;
			TResName	name;
			
			stream >> id;
			stream >> name;
			
			if (id==inResId)
			{
				exists=true;
				break;
			}
		}
	}
	
	return exists;
}

/*e*/
TResId CResContainerDirCache::GetResIdForName(
	ResType			inResType,
	const TResName	inResName)
{
	Handle			h=NULL;
	SInt32			count=0;
	TResId			result=kNullResId;
	
	if (RetrieveResList(inResType,&count,&h))
	{
		CNewHandleStream	stream(h,false);
		for (SInt32 i=0; i<count; i++)
		{
			TResId		id;
			TResName	name;
			
			stream >> id;
			stream >> name;

			if (CmpPStrNoCase(inResName,name))
			{
				result=id;
				break;
			}			
		}
	}
	
	return result;
}

/*e*/
bool CResContainerDirCache::GetNameForResId(
	ResType			inResType,
	TResId			inId,
	TResName		outName)
{
	Handle			h=NULL;
	SInt32			count=0;
	TResId			found=false;
	
	if (RetrieveResList(inResType,&count,&h))
	{
		CNewHandleStream	stream(h,false);
		for (SInt32 i=0; i<count; i++)
		{
			TResId		id;
			
			stream >> id;
			stream >> outName;

			if (inId==id)
			{
				found=true;
				break;
			}			
		}
	}
	
	if (!found)
	{
		outName[0]=0;
	}
	
	return found;
}

/*e*/
SInt32 CResContainerDirCache::CountResources(
	ResType			inResType)
{
	Handle			h=NULL;
	SInt32			count=0;
	
	RetrieveResList(inResType,&count,&h);
	return count;
}
