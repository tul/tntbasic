// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Resources.c
// Mark Tully
// 26/4/96
// 30/5/96
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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

#include		"Marks Routines.h"
#include		"Utility Objects.h"

OSErr	gGetResErr=noErr;

// Pass resFile as -1 to use the current resource file
// true = exists
Boolean ResourceExists(short resFile,ResType resType,short resID)
{
	short 	oldRes=::CurResFile();
	Handle	theRes=0L;
	
	if (resFile!=-1)
	{
		UseResFile(resFile);
		if (ResError())
			return false;
	}
	
	SetResLoad(false);
	theRes=Get1Resource(resType,resID);
	SetResLoad(true);
	UseResFile(oldRes);
	
	if (theRes)
	{
		ReleaseResource(theRes);
		return true;
	}
	else
		return false;
}

// Pass resFile as -1 to use the current resource file
// true = exists
Boolean IndResourceExists(short resFile,ResType resType,short inIndex)
{
	short 	oldRes=::CurResFile();
	Handle	theRes=0L;
	
	if (resFile!=-1)
	{
		UseResFile(resFile);
		if (ResError())
			return false;
	}
	
	SetResLoad(false);
	theRes=Get1IndResource(resType,inIndex);
	SetResLoad(true);
	UseResFile(oldRes);
	
	if (theRes)
	{
		ReleaseResource(theRes);
		return true;
	}
	else
		return false;
}

short BetterUniqueID(short resFile,ResType theType)
{
	short	saveFile=::CurResFile();
	short	id;
	
	UseResFile(resFile);
	id=Unique1ID(theType);
	UseResFile(saveFile);
	
	return id;
}

OSErr BetterGetResErr(void)
{
	return gGetResErr;	
}

// mode is either
// rnStartsWith
// rnContains
// rnIs
// rnEndsWith
// Please note this code is as yet untested
Handle BetterGetResourceByName(short resFile,ResType type,StringPtr name,unsigned char mode,Boolean caseSens)
{
	UResFileSaver		safe(resFile);
	UResLoadSaver		safe2(false);
	short				numRes=Count1Resources(type);
	Str255				resName;
	Handle				handle;
	Boolean				ok=false;
	
	gGetResErr=noErr;
	
	for (short counter=1; counter<numRes; counter++)
	{
		handle=BetterGetIndResource(-1,type,counter);
		if (!handle)
			return 0L;
		
		// got the handle, get the name.
		GetResName(handle,resName);
		
		// got the name, do the check
		switch (mode)
		{
			case rnStartsWith:
				if (name[0]<=resName[0])
					resName[0]=name[0];	// fall through and do a comparison
				else
				{
					ok=false;
					break;
				}
				
			case rnIs:		// exact match
				if (caseSens)
					ok=CmpPStr(name,resName);
				else
					ok=CmpPStrNoCase(name,resName);
				break;
								
			case rnContains:
				ok=(SearchForChars((Ptr)&resName[1],(Ptr)&name[1],resName[0],name[0],caseSens)!=-1);
				break;
				
			case rnEndsWith:
				if (name[0]<=resName[0])
					ok=CmpBuffer((Ptr)&name[1],(Ptr)&resName[name[0]],name[0],caseSens);
				else
					ok=false;
				break;
			
			default:
				ok=false;
				break;
		}
		
		if (ok)
			return handle;
		else
			ReleaseResource(handle);
	}
	
	gGetResErr=resNotFound;
	
	return 0L;
}

void GetResName(Handle handle,Str255 name)
{
	short		id;
	ResType		type;
	
	GetResInfo(handle,&id,&type,name);
}

OSErr BetterGetString(short resFile,short resId,StringPtr string)
{
	OSErr		err;
	Handle		handle;
	
	if (handle=BetterGetResource(resFile,'STR ',resId))
	{
		BlockMoveData(*handle,string,GetHandleSize(handle));
		
		BetterDisposeHandle(handle);
	}
	else
		return BetterGetResErr();
		
	return noErr;
}

Handle BetterGetResource(short resFile,ResType theType,short resID)
{
	short		saveFile=::CurResFile();
	Handle		theHandle;
	
	if (resFile!=-1)
		UseResFile(resFile);
		
	theHandle=Get1Resource(theType,resID);
	if (!theHandle)
	{
		gGetResErr=ResError();
		if (!gGetResErr)
		{
			if (ResourceExists(resFile,theType,resID))
				gGetResErr=memFullErr;
			else
				gGetResErr=resNotFound;
		}
	}
	else
		gGetResErr=noErr;
		
	UseResFile(saveFile);
	
	return theHandle;
}

void BetterReleaseResource(Handle theRes)
{
	if (theRes)
		BetterDisposeHandle(theRes);
}

short BetterGetResAttrs(short resFile,ResType theType,short resID)
{
	short		saveResLoad=LMGetResLoad();
	Handle		theHandle=0L;
	short		theAttrs=0;
	
	SetResLoad(false);
	
	theHandle=BetterGetResource(resFile,theType,resID);
	if (theHandle)
	{
		theAttrs=GetResAttrs(theHandle);
		ReleaseResource(theHandle);
	}
	
	if (saveResLoad)
		SetResLoad(true);
	
	return theAttrs;
}

long BetterSizeResource(short resFile,ResType resType,short resID)
{
	Handle		theHandle;
	long		theSize=0L;
	short		saveResLoad=LMGetResLoad();
	
	SetResLoad(false);
	theHandle=BetterGetResource(resFile,resType,resID);
	if (theHandle)
	{
		theSize=SizeResource(theHandle);
		ReleaseResource(theHandle);
	}
	if (saveResLoad)
		SetResLoad(true);
		
	return theSize;
}

void BetterDisposeHandle(Handle theHandle)
{
	if (!theHandle)
		return;
		
	if (!*theHandle)
	{
		// nil handle, could be either, we can't tell from hgetstate
		ReleaseResource(theHandle);	// resource?
		if (ResError())				// no...
			DisposeHandle(theHandle);
	}
	else
	{
		if (IsResourceHandle(theHandle))
			ReleaseResource(theHandle);
		else
			DisposeHandle(theHandle);
	}
}

Handle BetterGetIndResource(short resFile,ResType theType,short resID)
{
	short		saveFile=::CurResFile();
	Handle		theHandle;
	
	if (resFile!=-1)
		UseResFile(resFile);
	theHandle=Get1IndResource(theType,resID);
	if (!theHandle)
	{
		gGetResErr=ResError();
		if (!gGetResErr)
		{
			if (IndResourceExists(resFile,theType,resID))
				gGetResErr=memFullErr;
			else
				gGetResErr=resNotFound;
		}
	}
	else
		gGetResErr=noErr;
	
	if (resFile!=-1)
		UseResFile(saveFile);
	
	return theHandle;
}

// pass resFile as -1 to use the current file
OSErr BetterAddResource(short resFile,Handle theData,ResType type,short *id,StringPtr name,short mode)
{
	short		oldResFile=::CurResFile();
	OSErr		err=noErr;
	
	if (resFile!=-1)
		UseResFile(resFile);
	// If this resource exists then we may have some problems
	if (ResourceExists(-1,type,*id))
	{
		switch (mode)
		{
			case kOverwriteResource:
				if (!DeleteResource(-1,type,*id))
					err=rmvResFailed;
				break;
				
			case kUniqueID:
				*id=Unique1ID(type);
				break;
				
			case kBottleIt:
				err=userCanceledErr;
				break;
		}
	}
	
	if (!err)
	{
		// Now there is space for the resource lets add it
		AddResource(theData,type,*id,name ? name:"\p");
		err=ResError();
		if (err==noErr)
		{
			WriteResource(theData);
			err=ResError();
		}
	}
	
	UseResFile(oldResFile);
	
	return err;
}

// Deletes the specified resource
// Pass resFile as -1 to use the current resource file
// returns true for deleted
Boolean DeleteResource(short resFile,ResType resType,short resID)
{
	Handle		theRes=0L;
	short		oldRes=::CurResFile();
	
	if (resFile!=-1)
		UseResFile(resFile);
	
	SetResLoad(false);
	theRes=Get1Resource(resType,resID);	
	SetResLoad(true);
	
	if (theRes)
	{
		RemoveResource(theRes);
		if (ResError()==noErr)
		{
			DisposeHandle(theRes);
			return true;
		}
		else
		{
			ReleaseResource(theRes);
			return false;
		}
	}
	else
		return false;
}

Boolean DeleteAllResources(short resFile,ResType theType)
{
	short		saveRes=::CurResFile();
	short		max,count;
	Boolean		all=true;
	Handle		theRes;
		
	UseResFile(resFile);
	max=Count1Resources(theType);
	
	SetResLoad(false);
	for (count=1; count<=max; count++)
	{
		theRes=Get1IndResource(theType,count);
		if (theRes)
		{
			RemoveResource(theRes);
			if (ResError()==noErr)
				DisposeHandle(theRes);
			else
			{
				ReleaseResource(theRes);
				all=false;
			}
		}
		else
			all=false;
	}
	SetResLoad(true);
	
	UseResFile(saveRes);
	
	return all;
}

OSErr BetterCopyResource(short sourceFile,ResType sourceType,short sourceID,Boolean indexing,short destFile,short resMode)
{
	Handle	theRes;
	OSErr	err;
	Str255	name;
	
	if (indexing)
		theRes=BetterGetIndResource(sourceFile,sourceType,sourceID);
	else
		theRes=BetterGetResource(sourceFile,sourceType,sourceID);
	
	if (theRes)
	{
		GetResInfo(theRes,&sourceID,&sourceType,name);
		DetachResource(theRes); // make into a handle
		if ((err=ResError())==noErr) // no errors?
		{
			err=BetterAddResource(destFile,theRes,sourceType,&sourceID,name,resMode);
			if (err)
				DisposeHandle(theRes);
			else
				ReleaseResource(theRes);
		}
		else
			ReleaseResource(theRes);
	}
	else
		return resNotFound;
		
	return err;
}

bool GetNamedResourceId(
	ResType			inType,
	Str255			inString,
	ResID			&outId)
{
	Str255			name;
	ResType			type;
	
	UResLoadSaver	stopLoading(false);
	
	for (ResID n=1; n<=::Count1Resources(inType); n++)
	{
		if (Handle res=::GetIndResource(inType,n))
		{
			::GetResInfo(res,&outId,&type,name);
			::ReleaseResource(res);			// WARNING: This could potentially release a resource which was already loaded into RAM via another GetResource call. This is cos the Resource Manager doesn't do reference counting
			
			if (CmpPStr((unsigned char*)name,inString))
				return true;
		}
	}
	
	outId=0;
	return false;
}
