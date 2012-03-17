// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Prefereneces.c
// High level routines for the common task of managing prefs files
// Mark Tully
// 18/3/96
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

#include	"Marks Routines.h"

// pass init=true to create the file if it doesn't exist
// is init comes back true then the file did have to be created and so should be initialized
OSErr GetPrefsFile(StringPtr prefFileName,short *fRefNum,OSType creator,OSType fileType,Boolean *init,Boolean getResFork)
{	
	OSErr	err;
	long	dirID;
	short	vRefNum;
	
	*fRefNum=0;
	
	err=FindFolder(kOnSystemDisk,kPreferencesFolderType,kCreateFolder,&vRefNum,&dirID);
	if (err)
		return err;
		
	if (getResFork)
	{
		*fRefNum=HOpenResFile(vRefNum,dirID,prefFileName,fsCurPerm);
		
		err=noErr;
		
		if (*fRefNum==-1)
		{
			err=ResError();
			if (!err)
				err=-1;
		}
	}
	else
		err=HOpen(vRefNum,dirID,prefFileName,fsCurPerm,fRefNum);

	if (err==fnfErr && *init) 
	{
		// No prefs file, create a new one
		if (getResFork)
		{
			HCreateResFile(vRefNum,dirID,prefFileName);
			err=ResError();
		}
		else
			err=HCreate(vRefNum,dirID,prefFileName,creator,fileType);
		if (err)
			return err;
			
		if (getResFork)
		{
			*fRefNum=HOpenResFile(vRefNum,dirID,prefFileName,fsCurPerm);
			
			err=noErr;
			
			if (*fRefNum==-1)
			{
				err=ResError();
				if (!err)
					err=-1;
			}
		}
		else
			err=HOpen(vRefNum,dirID,prefFileName,fsCurPerm,fRefNum);
		
		if (err)
			return err;
			
		*init=true; // needs to be initialised
	}
	else if (err!=noErr)
		return err;
	else
		*init=false; // opened OK / did not get created
		
	return noErr;
}

// true = vis, false = invis
OSErr SetPrefsFileVisibility(StringPtr name,Boolean visible)
{
	OSErr	err;
	short	vRefNum;
	long	dirID;
	FInfo	finfo;
	
	err=FindFolder(kOnSystemDisk,kPreferencesFolderType,kCreateFolder,&vRefNum,&dirID);
	if (err)
		return err;
	
	err=HGetFInfo(vRefNum,dirID,name,&finfo);
	if (err)
		return err;
	
	if (visible)
		finfo.fdFlags&=!fInvisible;
	else
		finfo.fdFlags!=fInvisible;
	
	err=HSetFInfo(vRefNum,dirID,name,&finfo);
	if (err)
		return err;
		
	return FlushVol("\p",vRefNum);
}

// true vis, false invis
OSErr SetFilesVisibility(short frefnum,Boolean visible)
{
	FCBPBRec	info;
	OSErr		err;
	FInfo		fInfo;
	Str32		name;
	
	info.ioVRefNum=0;
	info.ioCompletion=0L;
	info.ioFCBIndx=0;
	info.ioRefNum=frefnum;
	info.ioNamePtr=name;
	
	err=PBGetFCBInfoSync(&info);
	if (err)
		return err;

	err=HGetFInfo(info.ioVRefNum,info.ioFCBParID,info.ioNamePtr,&fInfo);
	if (err)
		return err;
		
	if (visible)
		fInfo.fdFlags&=!fInvisible; // clear invisible flag
	else
		fInfo.fdFlags|=fInvisible;
	
	err=HSetFInfo(info.ioVRefNum,info.ioFCBParID,name,&fInfo);
	if (err)
		return err;

	return FlushVol("\p",info.ioVRefNum); // flush the disk casche	
}

OSErr SetFileType(short frefnum,OSType type,OSType creator)
{
	FCBPBRec	info;
	OSErr		err;
	FInfo		fInfo;
	Str32		name;
	
	info.ioVRefNum=0;
	info.ioCompletion=0L;
	info.ioFCBIndx=0;
	info.ioRefNum=frefnum;
	info.ioNamePtr=name;
	
	err=PBGetFCBInfoSync(&info);
	if (err)
		return err;

	err=HGetFInfo(info.ioVRefNum,info.ioFCBParID,info.ioNamePtr,&fInfo);
	if (err)
		return err;
		
	fInfo.fdType=type;
	fInfo.fdCreator=creator;
	
	err=HSetFInfo(info.ioVRefNum,info.ioFCBParID,name,&fInfo);
	return err;
}

// IN:
// Address of the handle to receive the data
// Type of resource
// Id of resource
// size that the resource should be/size which will be fetched
// Address of boolean to return a flag. If returned true then the resource needs initing
// NB: write to the resource again by calling
// ChangedResource(h);
// WriteResource(h);
// where h is the resource handle
OSErr GetPrefsResource(Handle *data,ResType type,short iD,Size size,Boolean *init)
{	
	OSErr	err;

	*data=0L;
	*init=false;
	
	*data=Get1Resource(type,iD);
	// err=ResError(); // can't use res error on a GetResource call, just check for null handles
	
	if (!*data) 
	{
		// presume that that resource is missing, create new
		*init=true;

		*data=NewHandleClear(size);
		if (!*data)
			return memFullErr;
		
		AddResource(*data,type,iD,"\p");
		err=ResError();
		return err;
	}
	
	// make sure that our prefs resource is the correct size.
	if (GetHandleSize(*data)!=size)
	{
		*init=true;
		SetHandleSize(*data,size);
		if (GetHandleSize(*data)!=size)
			return memFullErr;
	}
	
	return noErr;
}
