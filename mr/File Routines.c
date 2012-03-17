// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// File Routines.c
// Mark Tully
// 4/11/95
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1995, Mark Tully and John Treece-Birch
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

#include 		"Marks Routines.h"
#include		"MoreFilesExtras.h"
#include		<Files.h>

// gets the creator code of the current app
OSErr GetAppCreatorCode(OSType *outCreator)
{
 	ProcessSerialNumber PSN;
  	ProcessInfoRec 		info;
	OSErr				err;

	PSN.highLongOfPSN = 0;
	PSN.lowLongOfPSN = kCurrentProcess;
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName = 0L;
	info.processAppSpec = 0L;
	
	err=GetProcessInformation(&PSN, &info);
	if (err==noErr)
	{
		*outCreator=info.processSignature;
	}
	else
	{
		*outCreator='????';
	}
	return err;
}

OSErr GetAppSpec(FSSpec *spec)
{
 	ProcessSerialNumber PSN;
  	ProcessInfoRec 		info;
	OSErr				err;

	PSN.highLongOfPSN = 0;
	PSN.lowLongOfPSN = kCurrentProcess;
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName = 0L;
	info.processAppSpec = spec;
	
	return GetProcessInformation(&PSN, &info); // return the error
}

// 29/2/96

// Opens a file in the applications folder
OSErr OpenFileInFolder(StringPtr fileName,short *fRef,Boolean resFork)
{
	FSSpec		us;
	OSErr		err;
	
	*fRef=0;
	
	err=GetAppSpec(&us);
	if (err)
		return err;
	
	if (resFork) // open up the resource fork of the file	
	{
		*fRef=HOpenResFile(us.vRefNum,us.parID,fileName,fsRdWrPerm);
		err=ResError();
	}
	else
		err=HOpen(us.vRefNum,us.parID,fileName,fsRdWrPerm,fRef);

	return err;
}

// 2/4/96
// Nice folder scanning routine from UMP (with improvements of course) which saves me having to
// write a new one

// Just pass the address of any CInfoPBRec here, you don't need to init it.
OSErr ScanDirectoryCInfo(CInfoPBPtr cipbp,short vRefNum,long dirID,ScanProc fileProc,ScanProc folderProc,long refCon)
{
	Str32		theString;
	StringPtr	saveString;
	short		saveIndex;
	OSErr		err = noErr;
	
	// initialize ourselves
	cipbp->hFileInfo.ioCompletion 	= nil;
	cipbp->hFileInfo.ioFDirIndex  	= 1;
	cipbp->hFileInfo.ioVRefNum 		= vRefNum;
	cipbp->hFileInfo.ioNamePtr 		= (StringPtr)theString;
	
	while (err == noErr) {
		
		// always reset directory id
		cipbp->hFileInfo.ioDirID = dirID;

		// get the info for the next catalog item
		err = PBGetCatInfoSync(cipbp);		
		if (err) goto exit;
		
		// increment the count
		cipbp->hFileInfo.ioFDirIndex++;
		
		// if we are a directory, recurse (IT is a directory not WE (daft UMP man))
		if (cipbp->hFileInfo.ioFlAttrib & ioDirMask) {
			
			// if we are a folder then call the folder proc if we have one
			if (folderProc)
				err = (*folderProc)(cipbp, refCon);
			
			if (!err)
			{
				// save before recursing
				saveIndex = cipbp->hFileInfo.ioFDirIndex;
				saveString = cipbp->hFileInfo.ioNamePtr;
	
				// recurse within the directory
				err = ScanDirectoryCInfo(cipbp, vRefNum, cipbp->hFileInfo.ioDirID, fileProc,folderProc, refCon);
	
				// restore after recursion
				cipbp->hFileInfo.ioFDirIndex = saveIndex;
				cipbp->hFileInfo.ioNamePtr = saveString;
			}
			
		} else {	// it's a file
			
			// call our scanProc if there is one specified
			if (fileProc != nil)
				err = (*fileProc)(cipbp, refCon);
				
		}
	}
	
exit:
	
	// ignore fnfErr and afpAccessDenied errors since fnfErr is what we
	// get when there are no more files left to scan, and afpAccessDenied
	// can just ÒhappenÓ depending on what we are scanning (ie: network
	// volumes)
	if ((err == fnfErr) || (err == afpAccessDenied)) return noErr;
	else return err;
}

// makes a fsspec from a cinfo rec
OSErr CInfoToFSSpec(CInfoPBRec *cipbp,FSSpec *theFile)
{
	return 	FSMakeFSSpec(cipbp->hFileInfo.ioVRefNum,cipbp->hFileInfo.ioFlParID,cipbp->hFileInfo.ioNamePtr,theFile);
}

// and back again
// (can't const the FSSpec as it might write to the name ptr)
void FSSpecToCInfo(FSSpec *theFile,CInfoPBRec *cipbp)
{
	cipbp->hFileInfo.ioCompletion 	= 0L;
	cipbp->hFileInfo.ioFDirIndex  	= 0;
	cipbp->hFileInfo.ioVRefNum 		= theFile->vRefNum;
	cipbp->hFileInfo.ioNamePtr 		= theFile->name;
	cipbp->hFileInfo.ioDirID		= cipbp->hFileInfo.ioFlParID = theFile->parID;
}

// Takes a file refnum and gives a fsspec back
/*OSErr FSpGetFileLocation(short refNum,FSSpec *spec)
{
	FCBPBRec		pb;
	OSErr		error;

	pb.ioNamePtr = spec->name;
	pb.ioVRefNum = 0;
	pb.ioRefNum = refNum;
	pb.ioFCBIndx = 0;
	error = PBGetFCBInfoSync(&pb);
	spec->vRefNum = pb.ioFCBVRefNum;
	spec->parID = pb.ioFCBParID;

	return ( error );
}*/

// Returns if the file is read only
OSErr IsFileReadOnly(short refNum,Boolean *fileIsReadOnly)
{
	FCBPBRec		pb;
	Str32			tempString;
	OSErr			error;

	pb.ioNamePtr=tempString;
	pb.ioVRefNum=0;
	pb.ioRefNum=refNum;
	pb.ioFCBIndx=0;
	error=PBGetFCBInfoSync(&pb);
	
	*fileIsReadOnly=!(pb.ioFCBFlags&(1<<8));
	
	return error;
}


// Gives the size of the file (the sum of the size of the data fork and resource fork)
unsigned long BothForksSize(FSSpec *theFile,OSErr *err)
{
	CInfoPBRec		cib;
	OSErr			theErr;
	
	FSSpecToCInfo(theFile,&cib);
	theErr = PBGetCatInfoSync(&cib);	
	if (err)
		*err=theErr;
		
	return cib.hFileInfo.ioFlLgLen+cib.hFileInfo.ioFlRLgLen;
}

// Gives the size of the file (the sum of the size of the data fork and resource fork)
OSErr ForksExist(FSSpec *theFile, Boolean *outDataForkExists, Boolean *outResForkExists)
{
	CInfoPBRec		cib;
	OSErr			theErr;
	
	FSSpecToCInfo(theFile,&cib);
	theErr = PBGetCatInfoSync(&cib);	
	if (theErr==noErr)
	{
		if (cib.hFileInfo.ioFlAttrib & ioDirMask)
		{
			theErr=notAFileErr;
		}
		else
		{	
			if (outDataForkExists)
			{
				*outDataForkExists=(cib.hFileInfo.ioFlLgLen!=0);			
			}
			if (outResForkExists)
			{
				*outResForkExists=(cib.hFileInfo.ioFlRLgLen!=0);
			}
		}
	}
		
	return theErr;
}

// converts a fsref to a fsspec
OSErr FSRefToFSSpec(
	FSRef		*inRef,
	FSSpec		*outSpec)
{
	OSErr		err;

	if (IsOS9OrGreater())
	{
		err=FSGetCatalogInfo(inRef,kFSCatInfoNone,NULL,NULL,outSpec,NULL);
	}
	else
	{
		err=paramErr;
	}

	return err;
}

// makes a FSSUniStr255 - a long unicode file name - from a Str255
// the Str255 is assumed to be in roman script
OSErr ConvertPStringToHFSUniStr(Str255 inStr, HFSUniStr255 *outUniStr)
{
	OSErr err = noErr;
	CFStringRef tmpStringRef = CFStringCreateWithPascalString( kCFAllocatorDefault, inStr, kCFStringEncodingMacRoman );
	if( tmpStringRef != NULL )
	{
		if( CFStringGetCString( tmpStringRef, (char*)outUniStr->unicode, sizeof(outUniStr->unicode), kCFStringEncodingUnicode ) )
			outUniStr->length = CFStringGetLength( tmpStringRef );
		else
			err = -1;

		CFRelease( tmpStringRef );
	}
	else
		err = -1;

	return err;
}

// takes a spec to a directory and makes a new spec referring to the file inLongName inside that directory
// if long file names aren't available then it will fail if the name passed is too long
OSErr MakeFSSpecFromFileSpecLongName(FSSpec *ioSpec, Str255 inLongName)
{
	if (!IsOS9OrGreater())
	{
		// no long file name support
		if (inLongName[0]>32)
		{
			return errFSNameTooLong;
		}
		else
		{
			return MakeFSSpecFromFileSpec(ioSpec,inLongName);
		}
	}
	else
	{
		FSRef		dirRef;
		OSErr		err;
		
		err=FSpMakeFSRef(ioSpec,&dirRef);
		if (err==noErr)
		{
			FSRef			fRef;
			HFSUniStr255	uniStr;
			err=ConvertPStringToHFSUniStr(inLongName,&uniStr);
			err=FSMakeFSRefUnicode(&dirRef,uniStr.length,uniStr.unicode,kTextEncodingMacRoman,&fRef);
			if (err==noErr)
			{
				err=FSRefToFSSpec(&fRef,ioSpec);
			}
		}
		return err;
	}
}

// This routine takes an fsspec and a file name and does the following
// 1 : If the fsspec is a dir then the fsspec is resolved to fsspec pointing to the file inside the original folder
// 2 : If the fsspec is a file then the fsspec routine does nothing
OSErr MakeFSSpecFromFileSpec(FSSpec *spec,Str32 nameString)
{
	CInfoPBRec		cipbp;
	Str32			theString;
	OSErr			err = noErr;
	
	// initialize ourselves
	cipbp.hFileInfo.ioCompletion 	= nil;
	cipbp.hFileInfo.ioFDirIndex  	= 0;
	cipbp.hFileInfo.ioVRefNum 		= spec->vRefNum;
	cipbp.hFileInfo.ioNamePtr 		= spec->name;
	cipbp.hFileInfo.ioDirID 		= spec->parID;

	err = PBGetCatInfoSync(&cipbp);		
	if (err)
		return err;
	
	// Is this a directory?
	if (cipbp.hFileInfo.ioFlAttrib & ioDirMask)
	{
		// Yep, make a new FSSpec where the directory is the parent
		spec->parID=cipbp.dirInfo.ioDrDirID;
		if (nameString[0]<=63)
		{
			CopyPStr(nameString,spec->name);
		}
		else
		{
			spec->name[0]=0;
		}
	}
	else
	{
		; // Already a file do nothing
	}
	

	return noErr;	
}

// returns
// true = folder
// false = file
OSErr IsFolder(FSSpec *theFolder,Boolean *theResult)
{
	CInfoPBRec		cipbp;
	Str32			theString;
	OSErr			err = noErr;
	
	// initialize ourselves
	cipbp.hFileInfo.ioCompletion 	= nil;
	cipbp.hFileInfo.ioFDirIndex  	= 0;
	cipbp.hFileInfo.ioVRefNum 		= theFolder->vRefNum;
	cipbp.hFileInfo.ioNamePtr 		= theFolder->name;
	cipbp.hFileInfo.ioDirID 		= theFolder->parID;

	err = PBGetCatInfoSync(&cipbp);		
	if (err)
		return err;
	
	*theResult=cipbp.hFileInfo.ioFlAttrib & ioDirMask;
	return noErr;	
}

// all new scan directory, the modified better one.
// give it an FSSpec of a FOLDER and it decends into it and calls the scanproc on each
OSErr ScanDirectoryFSSpec(CInfoPBPtr cipbp,FSSpec *theFolder,ScanProc sp,ScanProc folderProc,long refCon)
{
	OSErr		err = noErr;
	
	// initialize ourselves
	// first step is to decend into the directory we are supposed to be scanning
	cipbp->hFileInfo.ioCompletion 	= nil;
	cipbp->hFileInfo.ioFDirIndex  	= 0;
	cipbp->hFileInfo.ioVRefNum 		= theFolder->vRefNum;
	cipbp->hFileInfo.ioNamePtr 		= theFolder->name;
	cipbp->hFileInfo.ioDirID 		= theFolder->parID;

	err = PBGetCatInfoSync(cipbp);		
	if (err)
	{
		if (err == afpAccessDenied)
			return noErr;
		else
			return err;
	}

	if (cipbp->hFileInfo.ioFlAttrib & ioDirMask) // if it's a directory
		return ScanDirectoryCInfo(cipbp,theFolder->vRefNum,cipbp->hFileInfo.ioDirID, sp,folderProc, refCon);
	else
		return dirNFErr; // wasn't a directory so cannot scan it

}

// 22/7/96
// Flushes a file to disk after a write operation
/*OSErr FlushFile(short fileRef)
{
	ParamBlockRec		pb;
	
	pb.ioParam.ioCompletion=0L;
	pb.ioParam.ioRefNum=fileRef;
	
	return PBFlushFileSync(&pb);
}*/

// does the file/directory exist
OSErr FSpSpecExists(
	FSSpec		*inSpec,
	Boolean		*outExists)
{
	CInfoPBRec		cipbp;
	Str32			theString;
	OSErr			err = noErr;
	
	// initialize ourselves
	cipbp.hFileInfo.ioCompletion 	= nil;
	cipbp.hFileInfo.ioFDirIndex  	= 0;
	cipbp.hFileInfo.ioVRefNum 		= inSpec->vRefNum;
	cipbp.hFileInfo.ioNamePtr 		= inSpec->name;
	cipbp.hFileInfo.ioDirID 		= inSpec->parID;

	err = PBGetCatInfoSync(&cipbp);		
	if (err)
	{
		*outExists=false;
		if (err==fnfErr)
			err=noErr;
	}
	else
	{
		*outExists=true;
	}
	
	return err;	
}

// creates a file from the given spec
// if the file already exists then the mode parameter can say
// kOverwriteFile or kBottleIt
// if resFork is true then a resource fork is also created
OSErr BetterCreateFile(FSSpec *theSpec,Boolean resFork,short mode,OSType creator,OSType type)
{
	OSErr	theErr=noErr;
	
	// delete the file if it is already there
	if (mode==kOverwriteFile)
	{
		theErr=HDelete(theSpec->vRefNum,theSpec->parID,theSpec->name);
		if (theErr==fnfErr)
			theErr=noErr;
	
		// error deleting the previous file
		if (theErr)
			return theErr;
	}
		
	// create a new file
	theErr=HCreate(theSpec->vRefNum,theSpec->parID,theSpec->name,creator,type);
	
	// error creating the file
	if (theErr)
		return theErr;
	
	// create resource fork of said file
	if (resFork)
	{
		HCreateResFile(theSpec->vRefNum,theSpec->parID,theSpec->name);
		theErr=ResError();
		if (theErr)
		{
			// delete the empty data fork
			HDelete(theSpec->vRefNum,theSpec->parID,theSpec->name);
			return theErr;
		}
	}
	
	return noErr;
}

// get a files version, pass -1 for cur res file
// pass NULL for strings you don't want
OSErr GetVersionInfoWithStrs(
	short		inResFile,
	short		*outMajor,
	short		*outSub,
	short		*outSubSub,
	StringPtr	outVerShortText,
	StringPtr	outVerLongText)
{
	Handle	versHandle;
	short	version;
	short	verMajor,verSub,verSubSub;

	versHandle=BetterGetResource(inResFile,'vers',1);
	if (!versHandle)
		return BetterGetResErr();
	
	HLock(versHandle);
	version=*(short*)*versHandle;
	
	if (outVerShortText)
		CopyPString((unsigned char*)((*versHandle)+6),outVerShortText);
	if (outVerLongText)
	{
		unsigned char	*tmp=(unsigned char*)(*versHandle);
		tmp+=6;
		tmp+=*tmp+1;	// skip p str
		CopyPString(tmp,outVerLongText);
	}		
	
	HUnlock(versHandle);
	HPurge(versHandle);					// I don't know whether you are suppose to release version resources, but I know they're purgable

	// now extract the version info
	if (outMajor)
		*outMajor=version>>8L;
	if (outSub)
		*outSub=(version&0x00FF)>>4;
	if (outSubSub)
		*outSubSub=version&0x000F;
	
	return noErr;
}

// use this to get a files version
OSErr GetVersionInfo(short resFile,short *major,short *sub,short *subSub)
{
	return GetVersionInfoWithStrs(resFile,major,sub,subSub,NULL,NULL);
}

// sets res to true if the file you pass is in the system folder
OSErr IsSystemFile(FSSpec *theSpec,Boolean *res)
{
	CInfoPBRec	theRec;
	OSErr		theErr;
	short		vRef;
	long		dirId;
	
	FSSpecToCInfo(theSpec,&theRec);
	if (theErr=PBGetCatInfoSync(&theRec))
		return theErr;
		
	// got the info, get the parId of the system folder and compare
	if (theErr=FindFolder(kOnSystemDisk,kSystemFolderType,false,&vRef,&dirId))
		return theErr;
		
	if (theRec.hFileInfo.ioFlParID==dirId)
		*res=true;
	else
		*res=false;
		
	return noErr;
}

// gets you the types of a file without opening it
OSErr GetCreatorAndType(const FSSpec *file,OSType *creator,OSType *fileType)
{
	CInfoPBRec	theRec;
	OSErr		theErr;
	FSSpec		local=*file;
	
	FSSpecToCInfo(&local,&theRec);
	if (theErr=PBGetCatInfoSync(&theRec))
		return theErr;
	
	*fileType=theRec.hFileInfo.ioFlFndrInfo.fdType;
	*creator=theRec.hFileInfo.ioFlFndrInfo.fdCreator;
	
	return noErr;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetDirFSSpec
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Given a volume reference number and a directory id get the fsspec of that directory
OSErr GetDirFSSpec(
	short	inVRefNum,
	long	inDirID,
	FSSpec	*outSpec)
{
	CInfoPBRec pb;
	OSErr error;

	pb.dirInfo.ioNamePtr = outSpec->name;
	pb.dirInfo.ioVRefNum = inVRefNum;
	pb.dirInfo.ioDrDirID = inDirID;
	pb.dirInfo.ioFDirIndex = -1;	/* get information about ioDirID */
	error = PBGetCatInfoSync(&pb);
	
	if (!error)
	{
		outSpec->vRefNum=inVRefNum;
		outSpec->parID=pb.dirInfo.ioDrParID;
	}
	
	return ( error );
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetModDate
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the modification date of a file/directory. You should be able to pass in inName as nil to operate
// on the dir in inDirID.
// Modified from MoreFileExtras.c::BumpDate
OSErr GetModDate(
	short				inVRefNum,
	long				inDirID,
	ConstStr255Param	inName,
	unsigned long		*outModDate)
{
	CInfoPBRec 	pb;
	StrFileName tempName;
	OSErr		 error;
	unsigned long secs;

	/* Protection against File Sharing problem */
	if ( (inName == NULL) || (inName[0] == 0) )
	{
		tempName[0] = 0;
		pb.hFileInfo.ioNamePtr = tempName;
		pb.hFileInfo.ioFDirIndex = -1;	/* use ioDirID */
	}
	else
	{
		pb.hFileInfo.ioNamePtr = (StringPtr)inName;
		pb.hFileInfo.ioFDirIndex = 0;	/* use ioNamePtr and ioDirID */
	}
	pb.hFileInfo.ioVRefNum = inVRefNum;
	pb.hFileInfo.ioDirID = inDirID;
	error = PBGetCatInfoSync(&pb);
	if ( error == noErr )
		*outModDate=pb.hFileInfo.ioFlMdDat;
	
	return ( error );
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetModDate
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the modification date of a file/directory. You should be able to pass in inName as nil to operate
// on the dir in inDirID.
// Modified from MoreFileExtras.c::BumpDate
OSErr SetModDate(
	short				inVRefNum,
	long				inDirID,
	ConstStr255Param	inName,
	unsigned long		inModDate)
{
	CInfoPBRec 	pb;
	StrFileName tempName;
	OSErr		 error;
	unsigned long secs;

	/* Protection against File Sharing problem */
	if ( (inName == NULL) || (inName[0] == 0) )
	{
		tempName[0] = 0;
		pb.hFileInfo.ioNamePtr = tempName;
		pb.hFileInfo.ioFDirIndex = -1;	/* use ioDirID */
	}
	else
	{
		pb.hFileInfo.ioNamePtr = (StringPtr)inName;
		pb.hFileInfo.ioFDirIndex = 0;	/* use ioNamePtr and ioDirID */
	}
	pb.hFileInfo.ioVRefNum = inVRefNum;
	pb.hFileInfo.ioDirID = inDirID;
	error = PBGetCatInfoSync(&pb);
	if ( error == noErr )
	{
		pb.hFileInfo.ioFlMdDat = inModDate;
		if ( pb.dirInfo.ioNamePtr == tempName )
		{
			pb.hFileInfo.ioDirID = pb.hFileInfo.ioFlParID;
		}
		else
		{
			pb.hFileInfo.ioDirID = inDirID;
		}
		error = PBSetCatInfoSync(&pb);
	}
	
	return ( error );
}

// returns true if currently executing on os 9 or greater
// this is useful to a carbon app as a lot of the fsref file routines are os 9 upwards
Boolean IsOS9OrGreater()
{
	SInt32		sysVersion;
	
	::Gestalt(gestaltSystemVersion, &sysVersion);

	return sysVersion>=0x900;
}

// gets the long file name for a FSSpec (dunno how well this will work with other scripts)
OSErr GetLongFileName(const FSSpec *inSpec,Str255 outName)
{
#if 1
	if (IsOS9OrGreater())
	{
		// FSpMakeFSRef requires mac os 9 or later
		CFURLRef		url=NULL;
		CFStringRef		str=NULL;
		FSRef			ref;
		OSErr			err;

		err=::FSpMakeFSRef(inSpec,&ref);
		if (err)
			return err;
			
		url=CFURLCreateFromFSRef(kCFAllocatorDefault,&ref);
		
		if (!url)
			return -1;
		
		str=CFURLCopyLastPathComponent(url);
		CFRelease(url);
		url=NULL;
		
		if (str)
		{
			if (!CFStringGetPascalString(str,outName,256,kCFStringEncodingMacRoman))
			{
				CFRelease(str);
				return -1;
			}
		
			CFRelease(str);
			str=NULL;
		}
		else
			return -1;
	}
	else
	{
		// use short name
		CopyPStr(inSpec->name,outName);
	}
	
	return noErr;

#else


	// THIS VERSION TENDS TO TRUNCATE THE TEXT TO ABOUT 50 CHRS FOR SOME REASON?
	FSRef				ref;
	OSStatus			err;
	FSCatalogInfo		info;
	UnicodeToTextInfo	uniConv;
	HFSUniStr255		uniStr;

	err=::FSpMakeFSRef(inSpec,&ref);
	if (err)
		return err;

	err=::FSGetCatalogInfo(&ref,kFSCatInfoTextEncoding,&info,&uniStr,NULL,NULL);	
	if (err)
		return err;

	err=::CreateUnicodeToTextInfoByEncoding(info.textEncodingHint,&uniConv);
	if (err)
		return err;

	err=::ConvertFromUnicodeToPString(uniConv,uniStr.length,uniStr.unicode,outName);

	::DisposeUnicodeToTextInfo(&uniConv);
	
	return err;
#endif
}

// returns true if two fsspecs are the same
Boolean CompareFSSpecs(
	FSSpec		*inFile1,
	FSSpec		*inFile2)
{
	return (inFile1->vRefNum==inFile2->vRefNum && inFile1->parID==inFile2->parID && CmpPStrNoCase(inFile1->name,inFile2->name));
}

// changes the creator code and file type for a file
OSErr SetCreatorAndFileType(
	FSSpec		*inSpec,
	OSType		inCreator,
	OSType		inFileType)
{
	FInfo		finfo;
	OSErr		err;
	
	err=FSpGetFInfo(inSpec,&finfo);
	if (err==noErr)
	{
		finfo.fdCreator=inCreator;
		finfo.fdType=inFileType;
		err=FSpSetFInfo(inSpec,&finfo);
	}
	return err;
}

// changes the finder flags for a file
OSErr SetFInfoFlags(
	FSSpec			*inSpec,
	unsigned short	inAddFlags,
	unsigned short	inRemoveFlags)
{
	FInfo		finfo;
	OSErr		err;
	
	err=FSpGetFInfo(inSpec,&finfo);
	if (err==noErr)
	{
		finfo.fdFlags&=~inRemoveFlags;
		finfo.fdFlags|=inAddFlags;
		err=FSpSetFInfo(inSpec,&finfo);
	}
	return err;
}

// changes the finder flags for a dir
OSErr SetDInfoFlags(
	FSSpec			*inSpec,
	unsigned short	inAddFlags,
	unsigned short	inRemoveFlags)
{
	DInfo		dinfo;
	OSErr		err;
	
	err=FSpGetDInfo(inSpec,&dinfo);
	if (err==noErr)
	{
		dinfo.frFlags&=~inRemoveFlags;
		dinfo.frFlags|=inAddFlags;
		err=FSpSetDInfo(inSpec,&dinfo);
	}
	return err;
}


// creates a file and returns the fsspec. uses new file system long file name support if available, otherwise
// returns an error
OSErr CreateFileLongName(
	FSSpec		*inParentDir,
	Str255		inFileName,
	OSType		inCreator,
	OSType		inFileType,
	FSSpec		*outFSSpec)
{
	OSErr		err=noErr;
	
	if (IsOS9OrGreater())
	{
		FSRef		parentDir;
		err=FSpMakeFSRef(inParentDir,&parentDir);
		if (err==noErr)
		{
			HFSUniStr255		uni;
			err=ConvertPStringToHFSUniStr(inFileName,&uni);
			if (err==noErr)
			{
				err=FSCreateFileUnicode(&parentDir,uni.length,uni.unicode,kFSCatInfoNone,NULL,NULL,outFSSpec);
				if (err==noErr)
				{
					// ignore errors from this
					SetCreatorAndFileType(outFSSpec,inCreator,inFileType);
				}
			}
		}
	}
	else
	{
		if (inFileName[0]>32)
		{
			err=errFSNameTooLong;
		}
		else
		{
			FSSpec		copy;
			copy=*inParentDir;
			err=MakeFSSpecFromFileSpec(&copy,inFileName);
			if (err==noErr)
			{
				err=FSpCreate(&copy,inCreator,inFileType,smSystemScript);
			}
		}
	}
	
	return err;
}

// returns the number of open resource files in this processes res map chain
SInt32 CountOpenResourceFiles()
{
	OSErr		err;
	SInt16		it;
	SInt32		count=0;
	
	err=GetTopResourceFile(&it);
	
	while (err==noErr && it)
	{
		count++;
		err=GetNextResourceFile(it,&it);
	}

	return count;
}



////////////////// stolen from pacakagetool apple sample code
//	updated to preserve cur res file across call
/* UpdateRelativeAliasFile updates the alias file located at aliasDest referring to the targetFile.
  relative path information is stored in the new file.  It is appropriate for targetFile to refer
  to either a file or a folder.  */
OSErr UpdateRelativeAliasFile(FSSpec *theAliasFile, FSSpec *targetFile, Boolean createIfNecessary)
{
  CInfoPBRec cat;
  FInfo fndrInfo;
  AliasHandle theAlias;
  Boolean wasChanged;
  short rsrc;
  OSErr err;
  short	wasResFile;
     /* set up locals */
  rsrc = -1;
  wasResFile=CurResFile();
    /* set up the Finder information record */
  BlockZero(&fndrInfo, sizeof(fndrInfo));
  BlockZero(&cat, sizeof(cat));
  cat.dirInfo.ioNamePtr = targetFile->name;
  cat.dirInfo.ioVRefNum = targetFile->vRefNum;
  cat.dirInfo.ioFDirIndex = 0;
  cat.dirInfo.ioDrDirID = targetFile->parID;
  err = PBGetCatInfoSync(&cat);
  if (err != noErr) goto bail;
  if ((cat.dirInfo.ioFlAttrib & 16) == 0) {  /* file alias */
    switch (cat.hFileInfo.ioFlFndrInfo.fdType) {
      case 'APPL': fndrInfo.fdType = kApplicationAliasType; break;
      case 'APPC': fndrInfo.fdType = kApplicationCPAliasType; break;
      case 'APPD': fndrInfo.fdType = kApplicationDAAliasType; break;
      default: fndrInfo.fdType = cat.hFileInfo.ioFlFndrInfo.fdType; break;
    }
    fndrInfo.fdCreator = cat.hFileInfo.ioFlFndrInfo.fdCreator;
  } else {
    fndrInfo.fdType = kContainerFolderAliasType;
    fndrInfo.fdCreator = 'MACS';
  }
  fndrInfo.fdFlags = kIsAlias;
     /* set the file information or the new file */
  err = FSpSetFInfo(theAliasFile, &fndrInfo);
  if ((err == fnfErr) && createIfNecessary) {
    FSpCreateResFile( theAliasFile, fndrInfo.fdCreator, fndrInfo.fdType, smSystemScript);
    if ((err = ResError()) != noErr) goto bail;
    err = FSpSetFInfo( theAliasFile, &fndrInfo);
    if (err != noErr) goto bail;
  } else if (err != noErr) goto bail;
     /* save the resource */
  rsrc = FSpOpenResFile(theAliasFile, fsRdWrPerm);
  if (rsrc == -1) { err = ResError(); goto bail; }
  UseResFile(rsrc);
  theAlias = (AliasHandle) Get1IndResource(rAliasType, 1);
  if (theAlias != NULL) {
    err = UpdateAlias(theAliasFile, targetFile, theAlias, &wasChanged);
    if (err != noErr) goto bail;
    if (wasChanged)
      ChangedResource((Handle) theAlias);
  } else {
    err = NewAlias(theAliasFile, targetFile, &theAlias);
    if (err != noErr) goto bail;
    AddResource((Handle) theAlias, rAliasType, 0, theAliasFile->name);
    if ((err = ResError()) != noErr) goto bail;
  }
  CloseResFile(rsrc);
  rsrc = -1;
    /* done */
  return noErr;
 bail:
  if (rsrc != -1) CloseResFile(rsrc);
  UseResFile(wasResFile);
  return err;
 }
 
 
/////////////////////// also stolen from apple packaging tool example code
/* IdentifyPackage returns true if the file system object refered to
  by *target refers to a package.  If it is a package, then 
  *mainPackageFile is set to refer to the package's main file. */
Boolean IdentifyPackage(FSSpec *target, FSSpec *mainPackageFile)
{
  CInfoPBRec cat;
  OSErr err;
  long packageFolderDirID;
  Str255 name;
  FSSpec aliasFile;
  Boolean targetIsFolder, wasAliased;
    /* check the target's flags */
  cat.dirInfo.ioNamePtr = target->name;
  cat.dirInfo.ioVRefNum = target->vRefNum;
  cat.dirInfo.ioFDirIndex = 0;
  cat.dirInfo.ioDrDirID = target->parID;
  err = PBGetCatInfoSync(&cat);
  if (err != noErr) return false;
  if (((cat.dirInfo.ioFlAttrib & 16) != 0) && ((cat.dirInfo.ioDrUsrWds.frFlags & kHasBundle) != 0)) {
      /* search for a top level alias file */
    packageFolderDirID = cat.dirInfo.ioDrDirID;
    cat.dirInfo.ioNamePtr = name;
    cat.dirInfo.ioVRefNum = target->vRefNum;
    cat.dirInfo.ioFDirIndex = 1;
    cat.dirInfo.ioDrDirID = packageFolderDirID;
      /* find the first alias file in the directory */
    while (PBGetCatInfoSync(&cat) == noErr) {
      if (((cat.dirInfo.ioFlAttrib & 16) == 0) && ((cat.dirInfo.ioDrUsrWds.frFlags & kIsAlias) != 0)) {
        err = FSMakeFSSpec(target->vRefNum, packageFolderDirID, name, &aliasFile);
        if (err != noErr) return false;
        err = ResolveAliasFile(&aliasFile, false, &targetIsFolder, &wasAliased);
        if (err != noErr) return false;
        if (mainPackageFile != NULL)
          *mainPackageFile = aliasFile;
        return true;
      }
      cat.dirInfo.ioFDirIndex++;
      cat.dirInfo.ioDrDirID = packageFolderDirID;
    }
  }
    /* no matching files found */
  return false;
}


