// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CBundleResFileCreator.cpp
// © Mark Tully 2005
// 15/5/05
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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
	handles the creation of CBundleResFile resource containers from folders
*/

#include "CBundleResFileCreator.h"
#include "CBundleResFile.h"
#include "Marks Routines.h"
#include "Root.h"
#include <string.h>

static const StringPtr		kBundleExtension="\p.tbproj";	// more generally we should also map other types to be bundle dirs, eg .resbndl
static const StringPtr		kBundleFileInfoFileName="\pInfo File";
static const StringPtr		kBundleFileInfoFileAliasName="\pInfo File Alias";
static const OSType			kBundleOS9FileType='RBun';
static const OSType			kBundleOS9CreatorCode='Hyr2';

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CanOpenFile
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// it can open it if it's a directory with the appropriate extension
bool CBundleResFileCreator::CanOpenFile(
	FSSpec		*inFile)
{
	bool		canOpen=false;

	Try_
	{
		Str255		longStr;
		Boolean		isFolder=false;

		ThrowIfOSErr_(::GetLongFileName(inFile,longStr));
		ThrowIfOSErr_(::IsFolder(inFile,&isFolder));

		if (isFolder)
		{
			canOpen=(longStr[0]>kBundleExtension[0] && std::memcmp(longStr+longStr[0]+1-kBundleExtension[0],kBundleExtension+1,kBundleExtension[0])==0);
			if (!canOpen)
			{
				// see if it's an OS 9 style package with an alias at the top level with our file type
				FSSpec		mainFile;
				
				if (IdentifyPackage(inFile,&mainFile))
				{
					OSType	fileType,creator;
					ThrowIfOSErr_(::GetCreatorAndType(&mainFile,&creator,&fileType));
					canOpen=(fileType==kBundleOS9FileType);
				}
			}
		}
	}
	Catch_(err)
	{
		ECHO("Error occurred trying to decided if '" << (inFile->name) << "' could be read as a bundle - err code : " << ErrCode_(err));
	}

	return canOpen;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ OpenFile														/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// attempts to open the file passed. inOpenReadOnly can be passed to open the file as read only. Caller should check whether a file that
// was requested as non-readonly comes back read only - this happens if it's on a read only volume or if the user don't have write perms.
CResourceContainer *CBundleResFileCreator::OpenFile(
	FSSpec		*inFile,
	bool		inOpenReadOnly)
{
	CResourceContainer		*conc=new CBundleResFile(inFile,inOpenReadOnly);
	ThrowIfMemFull_(conc);

	return conc;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetName
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the name of this file format
StringPtr CBundleResFileCreator::GetName(
	Str63		outStr)
{
	::CopyPStr("\pBundle Resource File",outStr);
	return outStr;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetDefaultExtension
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns the default extension of a file of this type
StringPtr CBundleResFileCreator::GetDefaultExtension(
	Str32		outStr)
{
	::CopyPStr(kBundleExtension,outStr);
	return outStr;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CreateFile									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// creates a bundle res file
void CBundleResFileCreator::CreateFile(
	FSSpec		*inFile)
{
	CreateFile(inFile,kBundleExtension,kBundleOS9CreatorCode,kBundleOS9FileType);		// default to creating a hieroglyph bundle file
}

void CBundleResFileCreator::CreateFile(
	FSSpec				*inFile,
	const StringPtr		inBundleExtension,
	OSType				inCreatorCode,			// these are of use only on mac os 9
	OSType				inFileType)
{
	long		dirID;
	ThrowIfOSErr_(FSpDirCreate(inFile,smSystemScript,&dirID));
	
	// set the package flag so the file doesn't get opened on the mac os 9 finder
	OSErr		err=::SetDInfoFlags(inFile,kHasBundle,kHasBeenInited);		// setting bundle flag on os 9 means it's a package (tn 1188)
	ECHO("Setting finfo - err " << err);
	
	// also, need to create a single alias at the top level to make the os 9 finder treat it as a bundle
	// the alias has to be to a file with the creator code and file type set to our app.
	
	// create dummy file with creator code of cur app + dummy file type which maps to a bundle icon
	ThrowIfOSErr_(::HCreate(inFile->vRefNum,dirID,kBundleFileInfoFileName,inCreatorCode,inFileType));

	// create alias to this file at the top level of the bundle	
	FSSpec			aliasFile,aliasTarget;
	aliasFile.vRefNum=inFile->vRefNum;
	aliasFile.parID=dirID;
	CopyPStr(kBundleFileInfoFileAliasName,aliasFile.name);

	aliasTarget.vRefNum=inFile->vRefNum;
	aliasTarget.parID=dirID;
	CopyPStr(kBundleFileInfoFileName,aliasTarget.name);

	ThrowIfOSErr_(::UpdateRelativeAliasFile(&aliasFile,&aliasTarget,true));
}