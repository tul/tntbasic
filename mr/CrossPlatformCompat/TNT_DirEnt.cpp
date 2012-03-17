// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// TNT_DirEnt.c
// © Mark Tully 2001
// 17/5/01
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2001, Mark Tully and John Treece-Birch
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
	On unix and windows dirent.h contains some directory scanning routines. This file is not available in MSL.
	TNT_DirEnt.h and .cpp provide the routines in this header for the mac os.
	
	The standard library directory streams functions are 
	
	DIR	*opendir(char *inName);		// takes dir in workin directory. return 0 for error, puts error code in errno (see errno.h)
	dirent *readdir(DIR *inDir);	// returns next entry - nil for no more entries
	void rewinddir();				// starts over
	int closedir(DIR *inCloseme);	// return 0 for no error
*/

#include "TNT_Debugging.h"
#include "Marks Routines.h"
#include "TNT_DirEnt.h"
#include "TFolderIterator.h"

#include <errno.h>

OSErr GetFSSpecOfWorkingDir(
	FSSpec		&outSpec,
	SInt32		&outDirNum);

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetFSSpecOfWorkingDir
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the fsspc of the working directory and it's dir num.
OSErr GetFSSpecOfWorkingDir(
	FSSpec		&outSpec,
	SInt32		&outDirNum)
{
	WDPBRec			wdpb;
	OSErr			err;
	
	wdpb.ioNamePtr = 0;
	err = PBHGetVolSync(&wdpb);
	
	if (err)
		return err;

	return GetDirFSSpec(wdpb.ioWDVRefNum,wdpb.ioWDDirID,&outSpec);
}

#define BailOnError(err)		if (err) { errno=(err); return 0L; }

namespace std
{

class DIR
{
	protected:
		TFolderIterator		it;
		dirent				ent;
	
	public:
						DIR(
							FSSpec	&inSpec) :
							it(inSpec)
							{
								// don't want sub folders to be recursed into
								TFolderIterator::TFolderIteratorFlags		flags=it.GetIteratorFlags();
								
								flags&=~TFolderIterator::kScanSubFolders;	// don't decend into sub folders
								flags|=TFolderIterator::kReturnFolders;
								
								it.SetIteratorFlags(flags);
							}
			
		void Rewind()
		{
		
		}
		
		dirent *NextEntry()
		{
			Try_
			{
				Str32			name;
				CInfoPBRec		block;
				
				block.hFileInfo.ioNamePtr=name;
				
				if (it.GetNextItem(block))
				{
					::CopyPAsCStr(name,(StringPtr)ent.d_name);
					ent.d_namlen=name[0];
					ent.d_type=(block.hFileInfo.ioFlAttrib & ioDirMask)!=0 ? DT_DIR : DT_REG;
					return &ent;
				}
			}
			Catch_(err)
			{
				errno=ErrCode_(err);
			}
		
			return 0;
		}
};


DIR *opendir(
	char	*inName)
{
	// Get the fsspec of the current working directory
	FSSpec	workingDir;
	SInt32	dirId;
	OSErr	err=::GetFSSpecOfWorkingDir(workingDir,dirId);

	BailOnError(err);

	// get the fsspec of the inName dir inside the working directory
	if (inName)
	{
		Str255	dirName;
		::CopyCAsPStr((StringPtr)inName,dirName);
		
		err=::MakeFSSpecFromFileSpec(&workingDir,dirName);
		BailOnError(err);
	}

	DIR		*d=new DIR(workingDir);
	if (!d)
		BailOnError(memFullErr);

	return d;
}

dirent *readdir(
	DIR		*inDir)
{
	AssertThrow_(inDir);
	return inDir->NextEntry();
}

void rewinddir(
	DIR		*inDir)
{
	AssertThrow_(inDir);
	inDir->Rewind();
}

int closedir(
	DIR		*inDir)
{
	delete inDir;
	return 0;
}

}