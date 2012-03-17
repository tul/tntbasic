// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TFolderIterator.h
// Mark Tully
// 2/8/98
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

#pragma once

#include							"CLinkedListT.h"

typedef class TFolderIteratorStackFrame : public CListElementT<TFolderIteratorStackFrame>
{
	protected:
		static const short					kScanComplete=-1;

	public:
		FSSpec								mFolderSpec;
		short								mCurrentIndex;
		long								mDirId;
		
		/*e*/								TFolderIteratorStackFrame(
													CLinkedListT<TFolderIteratorStackFrame>	&inList,
													FSSpec									&inFolderSpec);
													
		Boolean								IsScanComplete()				{ return mCurrentIndex==kScanComplete; }
		void								SetScanComplete()				{ mCurrentIndex=kScanComplete; }

} TFolderIteratorStackFrame, *TFolderIteratorStackFramePtr;

typedef class TFolderIterator
{
	public:
		typedef unsigned short TFolderIteratorFlags;
		enum
		{
			kResolveFileAliases=1,			// Target file is returned instead of alias. Target file doesn't have to be in the
											// scanned folder.
			kResolveFolderAliases=2,		// Target folder is returned instead of alias. Target folder doesn't have to be
											// in the scanned folder.
			kReturnFiles=4,					// Does your iteration want files?
			kReturnFolders=8,				// Does it want folders?
			kScanSubFolders=16,				// Descend into sub folders. The folder is returned first (if requested) then scanned
			kIgnoreInvisibles=32,			// Ignores invisible items. Doesn't descend into invisible folders nor return
											// any item which is invisible, nor resolve aliases which are invisible.
			kIgnoreBrokenAliases=64,

			kDefaultFlags=kResolveFileAliases|kResolveFolderAliases|kReturnFiles|kScanSubFolders|kIgnoreInvisibles|kIgnoreBrokenAliases
		};

	protected:
		TFolderIteratorFlags						mFlags;
		CLinkedListT<TFolderIteratorStackFrame>		mFoldersList;					// List of all folders iterated and the one we're iterating
		
		virtual void /*e*/					PushStackFrame(
													FSSpec					&inFolder);
		virtual TFolderIteratorStackFrame	*GetCurrentStackFrame();
		virtual void						CurrentStackFrameComplete();
		
		virtual Boolean						HasFolderBeenScanned(
													FSSpec					&inSpec);
		
		virtual Boolean						ShouldReturnItem(
													CInfoPBRec				&inItem);
		virtual Boolean						ShouldResolveAlias(
													CInfoPBRec				&inAliasItem,
													CInfoPBRec				&inTargetItem);
		virtual Boolean						ShouldRecurseInto(
													CInfoPBRec				&inItem);
		
	public:
		/*e*/								TFolderIterator(
													FSSpec					&inFolderToRecurse);
		virtual								~TFolderIterator();

		virtual TFolderIteratorFlags		GetIteratorFlags()									{ return mFlags; }
		virtual void						SetIteratorFlags(
													TFolderIteratorFlags	inNewFlags)			{ mFlags=inNewFlags; }

		virtual Boolean /*e*/				GetNextItem(
													FSSpec					&outItem);
		virtual Boolean /*e*/				GetNextItem(
													CInfoPBRec				&outItem);

} TFolderIterator;
