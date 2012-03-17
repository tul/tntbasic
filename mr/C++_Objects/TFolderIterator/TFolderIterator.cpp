// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TFolderIterator.cpp
// Mark Tully
// 2/8/98
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This is a C++ version of the old Marks Routines ScanDirectory proc.
/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1998, Mark Tully and John Treece-Birch
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
// This is a C++ version of the old Marks Routines ScanDirectory proc.
// Better in that it can resolve aliases for you, delivers the contents a file/folder at a time without you having to supply
// a proc. Watches out for infinate loops due to aliases resolving to already iterated folders and stuff.

// Implementation
// --------------
// TFolderIteratorStackFrame is used to hold the position in a folder of the current recursion. This would normally be held
// in a stack based variable (normally meaning if we had a recursion loop). As we are turing a recursiv loop into an
// iterative one we need to store stack variables ourselves to simulate recursion.

// When GetCurrentStackFrame is called, the first stack frame is got and used to find the next item. If there are no stack
// frames then the iteration is complete. The stack frames have an index recorded with them. If the index is kScanComplete
// then the folder is done and is not scanned further. It is left in the list however so we know not to recurse into that
// folder again.

#include "TNT_Debugging.h"
#include "MoreFilesExtras.h"

#include "TFolderIterator.h"
#include "TListIndexer.h"

#include "Marks Routines.h"


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ TFolderIteratorStackFrame Constructor			/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Constructs a stack frame, and puts it in the list you pass at the beginning.
TFolderIteratorStackFrame::TFolderIteratorStackFrame(CLinkedListT<TFolderIteratorStackFrame> &inList,FSSpec &inFolderSpec) : CListElementT<TFolderIteratorStackFrame>(this)
{
	mFolderSpec=inFolderSpec;
	mCurrentIndex=1;
	inList.LinkElement(this);
	
	// Get the dir id, if this is not a dir then throw an error
	Boolean			isDirectory;
	ThrowIfOSErr_(FSpGetDirectoryID(&inFolderSpec,&mDirId,&isDirectory));
	if (!isDirectory)
		Throw_(dirNFErr);

}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ TFolderIterator Constructor					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TFolderIterator::TFolderIterator(FSSpec &inFolderToRecurse)
{
	mFlags=kDefaultFlags;
	PushStackFrame(inFolderToRecurse);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ TFolderIterator Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TFolderIterator::~TFolderIterator()
{
	mFoldersList.DeleteAllLinks();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PushStackFrame								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This makes the folder you pass become the folder to be iterated on the next pass. Next GetNextItem call will return item
// 1 in this folder. If however this folder has already been iterated then it is not pushed again and this proc does nothing.
// Once a folder is added in this way it will not be recursed into again.
// You shouldn't need to override this, instead override ShouldRecurseInto.
void TFolderIterator::PushStackFrame(FSSpec &inFolder)
{
	if (!HasFolderBeenScanned(inFolder))
	{
		TFolderIteratorStackFramePtr			frame=new TFolderIteratorStackFrame(mFoldersList,inFolder);
		ThrowIfMemFull_(frame);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetCurrentStackFrame
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Gets the last item to be pushed on the stack which hasn't been fully iterated.
// You shouldn't need to override this.
TFolderIteratorStackFrame *TFolderIterator::GetCurrentStackFrame()
{
	CListIndexerT<TFolderIteratorStackFrame>		indexer(&mFoldersList);
	
	while (TFolderIteratorStackFramePtr ptr=indexer.GetNextData())
	{
		if (!ptr->IsScanComplete())
			return ptr;
	}
	
	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CurrentStackFrameComplete
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Marks the current folder as done. Next call to GetNextItem will return the following item in the parent folder.
// You shouldn't need to override this.
void TFolderIterator::CurrentStackFrameComplete()
{
	TFolderIteratorStackFramePtr		ptr=GetCurrentStackFrame();
	if (ptr)
		ptr->SetScanComplete();
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HasFolderBeenScanned
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns true if a given folder has been scanned or is in the progress of being scanned.
// You shouldn't really need to override this.
Boolean TFolderIterator::HasFolderBeenScanned(FSSpec &inSpec)
{
	CListIndexerT<TFolderIteratorStackFrame>		indexer(&mFoldersList);
	
	while (TFolderIteratorStackFramePtr ptr=indexer.GetNextData())
	{
		if (inSpec.vRefNum==ptr->mFolderSpec.vRefNum && inSpec.parID==ptr->mFolderSpec.parID && CmpPStrNoCase(inSpec.name,ptr->mFolderSpec.name))
			return true;
	}
	
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShouldReturnItem								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Override this to provide filtering of the items to be returned
Boolean TFolderIterator::ShouldReturnItem(CInfoPBRec &inItem)
{
	if (mFlags&kIgnoreInvisibles && inItem.hFileInfo.ioFlFndrInfo.fdFlags&fInvisible)
		return false;
	if (mFlags&kIgnoreInvisibles && inItem.hFileInfo.ioNamePtr[1]=='.')
		return false;
		
	if (inItem.hFileInfo.ioFlAttrib & ioDirMask)
		return (mFlags&kReturnFolders)!=0;
	else
		return (mFlags&kReturnFiles)!=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShouldResolveAlias							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Override this to control which aliases get resolved
Boolean TFolderIterator::ShouldResolveAlias(CInfoPBRec &inAliasItem,CInfoPBRec &inTargetItem)
{
	if (mFlags&kIgnoreInvisibles && inAliasItem.hFileInfo.ioFlFndrInfo.fdFlags&fInvisible)
		return false;
	if (mFlags&kIgnoreInvisibles && inTargetItem.hFileInfo.ioFlFndrInfo.fdFlags&fInvisible)
		return false;
	if (mFlags&kIgnoreInvisibles && inAliasItem.hFileInfo.ioNamePtr[1]=='.')
		return false;
	if (mFlags&kIgnoreInvisibles && inTargetItem.hFileInfo.ioNamePtr[1]=='.')
		return false;
	
	if (inTargetItem.hFileInfo.ioFlAttrib & ioDirMask)
		return (mFlags&kResolveFolderAliases)!=0;
	else
		return (mFlags&kResolveFileAliases)!=0;

}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShouldRecurseInto								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Override this to control which folders get recursed into
Boolean TFolderIterator::ShouldRecurseInto(CInfoPBRec &inItem)
{
	if (mFlags&kIgnoreInvisibles && inItem.hFileInfo.ioFlFndrInfo.fdFlags&fInvisible)
		return false;
	if (mFlags&kIgnoreInvisibles && inItem.hFileInfo.ioNamePtr[1]=='.')
		return false;

	return (mFlags&kScanSubFolders)!=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNextItem									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Same as the routine below except that it returns each item as an FSSpec. Use this if you don't want the additional info.
Boolean TFolderIterator::GetNextItem(FSSpec &outItem)
{
	Str32				string;
	CInfoPBRec			pb;
	Boolean				moreToCome;
	
	pb.hFileInfo.ioNamePtr=string;
	if (moreToCome=GetNextItem(pb))
		ThrowIfOSErr_(CInfoToFSSpec(&pb,&outItem));
	
	return				moreToCome;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNextItem									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// If you want the file name then put a Str32 ptr in the name field of outItem. Otherwise, set it to 0L. No other fields
// need initializing.
Boolean TFolderIterator::GetNextItem(CInfoPBRec &outItem)
{
	Boolean									gotItem=false;
	
	do
	{
		TFolderIteratorStackFramePtr			currentFrame=GetCurrentStackFrame();
		if (!currentFrame)
			return false;						// Iteration is complete

		// Got the stack frame, fill out the variables
		outItem.hFileInfo.ioVRefNum=currentFrame->mFolderSpec.vRefNum;
		outItem.hFileInfo.ioDirID=currentFrame->mDirId;
		outItem.hFileInfo.ioFDirIndex=currentFrame->mCurrentIndex++;
		
		// Get the OS to give us the lowdown on the specified item
		OSErr err=PBGetCatInfo(&outItem,false);
		
		// fnf is when there are no files left in this stack frame, access denied can occur when scanning servers, ignore
		// both.
		if ((err==fnfErr) || (err==afpAccessDenied))
		{
			CurrentStackFrameComplete();
			continue;
		}
		else if (err)
			Throw_(err);						// IO Err
		
		// Firstly, resolve it if it's an alias, but only if we've been told to
		if (GetIteratorFlags()&(kResolveFileAliases|kResolveFolderAliases))
		{
			Boolean				wasAliased,targetIsFolder;
			FSSpec				targetSpec;
			
			ThrowIfOSErr_(CInfoToFSSpec(&outItem,&targetSpec));
			
			err=ResolveAliasFile(&targetSpec,true,&targetIsFolder,&wasAliased);
			if (err && ((GetIteratorFlags()&kIgnoreBrokenAliases)==false))
				Throw_(err);
			else if (err)
			{
				// alias err skip it
				continue;
			}
			
			// If it was an alias then get the cinfo on the target, but only if we should have resolved it
			if (wasAliased)
			{
				CInfoPBRec			targetItem;
				
				targetItem.hFileInfo.ioVRefNum=targetSpec.vRefNum;
				targetItem.hFileInfo.ioDirID=targetSpec.parID;
				targetItem.hFileInfo.ioNamePtr=targetSpec.name;
				targetItem.hFileInfo.ioFDirIndex=0;						// Query the item which is named
			
				// The alias should be resolved, get info on the target item now.
				ThrowIfOSErr_(PBGetCatInfo(&targetItem,false));
				
				if (ShouldResolveAlias(outItem,targetItem))
				{
					StringPtr		tempString=outItem.hFileInfo.ioNamePtr;
					outItem=targetItem;
					outItem.hFileInfo.ioNamePtr=tempString;
					if (outItem.hFileInfo.ioNamePtr)
						CopyPStr(targetItem.hFileInfo.ioNamePtr,outItem.hFileInfo.ioNamePtr);
				}
			}
		}
		
		// Now what have we got as the item?
		if (outItem.hFileInfo.ioFlAttrib & ioDirMask)
		{
			// A folder, do we recurse into it?
			if (ShouldRecurseInto(outItem))
			{
				// Yep, add it to the stack, we'll get the first item in this folder next time around
				FSSpec					tempSpec;
				
				ThrowIfOSErr_(CInfoToFSSpec(&outItem,&tempSpec));
				PushStackFrame(tempSpec);
			}
		}
		
		// Do we return the item we now have
		gotItem=ShouldReturnItem(outItem);
		
	} while (!gotItem);
	
	return true;	// More to come, please come again!
}

