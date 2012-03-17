// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Scrap.c
// Mark Tully
// 29/5/96
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

// under carbon the scrapref changes every time the scrap changes. it does the same job that PScrapSctuff->scrapCount did under classic

#if TARGET_API_MAC_CARBON
	ScrapRef	gScrapRef=0;
#else
	long	gScrapCounter=0L;
#endif

// This function returns true if the scraps changed since the last time StoreScrap was called
Boolean ScrapChanged()
{
#if TARGET_API_MAC_CARBON
	ScrapRef	ref;
	
	OSErr		err=::GetCurrentScrap(&ref);
	
	if (err==noErr)
		return ref!=gScrapRef;
	else
		return false;		// error getting scrap, returning false is safest as that will lead to no further messing with the broken scrap
#else
	PScrapStuff	theStuff=InfoScrap();
	
	if (gScrapCounter!=theStuff->scrapCount)
		return true;
	else
		return false;
#endif
}

void StoreScrap()
{
#if TARGET_API_MAC_CARBON
	::GetCurrentScrap(&gScrapRef);
#else
	PScrapStuff	theStuff=InfoScrap();
	
	gScrapCounter=theStuff->scrapCount;
#endif
}

OSErr ClipCutResource(short resFile,ResType theType,short resID)
{
	Handle		theRes=BetterGetResource(resFile,theType,resID);
	OSErr		err;
	short		saveRes=CurResFile();
	
	if (!theRes)
		return resNotFound;
	else
	{
#if TARGET_API_MAC_CARBON
		ScrapRef		ref;
		err=GetCurrentScrap(&ref);
		if (err)
			return err;
#endif		
		
		HLockHi(theRes);

#if TARGET_API_MAC_CARBON
		err=PutScrapFlavor(ref,theType,0,::GetHandleSize(theRes),*theRes);
#else
		err=(OSErr)PutScrap(GetHandleSize(theRes),theType,*theRes);
#endif

		HUnlock(theRes);
		if (err==noErr)
		{
			UseResFile(resFile);
			RemoveResource(theRes);	// REMEMBER: For rmve resource to work the resource MUST be from the current res file
			if (ResError()==noErr)
				DisposeHandle(theRes);
			else
			{
				ReleaseResource(theRes);
				err=rmvResFailed;
			}
			UseResFile(saveRes);
		}
		else
			ReleaseResource(theRes);
	}
	
	return err;
}

OSErr ClipCopyResource(short resFile,ResType theType,short resID)
{
	Handle		theRes=BetterGetResource(resFile,theType,resID);
	OSErr		err;
	
	if (!theRes)
		return resNotFound;
	else
	{
#if TARGET_API_MAC_CARBON
		ScrapRef		ref;
		
		err=GetCurrentScrap(&ref);
		if (err)
			return err;
#endif

		HLockHi(theRes);
#if TARGET_API_MAC_CARBON
		err=PutScrapFlavor(ref,theType,0,::GetHandleSize(theRes),*theRes);
#else
		err=(OSErr)PutScrap(GetHandleSize(theRes),theType,*theRes);
#endif
		HUnlock(theRes);
		ReleaseResource(theRes);
	}
	
	return err;
}

// returns true if the scrap text contains a string of numbers
Boolean ScrapTextIsNumbersOnly()
{
	Str255		tempString;
	short		counter;

#if TARGET_API_MAC_CARBON
	Size			size;
	OSErr			err;
	ScrapRef		ref;
	
	err=GetCurrentScrap(&ref);
	if (err)
		return false;
	
	err=GetScrapFlavorSize(ref,'TEXT',&size);
	if (err)
		return false;

	if (size>255)
		size=255;

	tempString[0]=size;
	err=GetScrapFlavorData(ref,'TEXT',&size,&tempString[1]);
	if (err)
		return false;
		
#else
	Handle		tempHandle;
	long		length,number;
	long		scrapOffset=0;
	
	tempHandle=NewHandleSys(255);
	
	length=GetScrap(tempHandle,'TEXT',&scrapOffset);
	if (length<0)
	{
		DisposeHandle(tempHandle);
		return false;
	}
	BlockMove(*tempHandle,&tempString[1],length);
	DisposeHandle(tempHandle);
	tempString[0]=length;
#endif
	
	// check it's all numbers
	for (counter=1; counter<=tempString[0]; counter++)
	{
		if (!IsAsciiNumber(tempString[counter]))
			return false;
	}
	
	return true;
}

// returns true if the scrap is a number between min and max
Boolean ScrapTextIsNumberInRange(long min,long max)
{
	Str255		tempString;
	short		counter;
	SInt32		number;

#if TARGET_API_MAC_CARBON
	Size			size;
	OSErr			err;
	ScrapRef		ref;
	
	err=GetCurrentScrap(&ref);
	if (err)
		return false;
	
	err=GetScrapFlavorSize(ref,'TEXT',&size);
	if (err)
		return false;

	if (size>255)
		size=255;

	tempString[0]=size;
	err=GetScrapFlavorData(ref,'TEXT',&size,&tempString[1]);
	if (err)
		return false;
		
#else
	Handle		tempHandle;
	long		length,number;
	long		scrapOffset=0;
	
	tempHandle=NewHandleSys(255);
	
	length=GetScrap(tempHandle,'TEXT',&scrapOffset);
	if (length<0)
	{
		DisposeHandle(tempHandle);
		return false;
	}
	BlockMove(*tempHandle,&tempString[1],length);
	DisposeHandle(tempHandle);
	tempString[0]=length;
#endif

	// check it's all numbers
	for (counter=1; counter<=tempString[0]; counter++)
	{
		if (!IsAsciiNumber(tempString[counter]))
			return false;
	}
	
	// convert to a number
	StringToNum(tempString,&number);
	if (!InRange(number,min,max))
		return false;
	return true;
}

// returns true if pasting into the passed te would produce a number between min and max
Boolean PasteProducesNumber(WindowPtr window,TEHandle te,long min,long max)
{
	Str255			string;
	Size			size=255;
	long			num;

	if (!ScrapTextIsNumbersOnly())
		return false;

	if (!SimulateKeyPressInWindow(window,te,0L,kSpecialPasteChar,(Ptr)&string[1],&size))
		return false;

	string[0]=size;
	StringToNum(string,&num);
	
	return InRange(num,min,max);
}


