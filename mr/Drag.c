// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Drag.c
// Mark Tully
// 27/5/96
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

#include	<Drag.h>
#include	"Marks Routines.h"

// Fetches the flavour data from an item if it exists
OSErr FetchFlavorData(DragReference theDrag,ItemReference theItem,FlavorType theType,Handle *theData)
{
	OSErr	err;
	Size	dataSize;
	
	*theData=0L;
	
	// Data's size?
	err=GetFlavorDataSize(theDrag,theItem,theType,&dataSize);
	if (err)
		return err;
		
	// Get the memory for that amount of data
	*theData=NewHandle(dataSize);
	if (*theData==0L)
		return memFullErr;
	
	// Do a data fetch
	HLock(*theData);
	err=GetFlavorData(theDrag,theItem,theType,**theData,&dataSize,0L);
	HUnlock(*theData);
	
	// Did we get the data?
	if (err)
		DisposeHandle(*theData);
	
	return err;
}

// This proc returns true if the specified flavour is availble in the drag items flavours list
Boolean IsFlavourAvailable(DragReference theDrag,ItemReference theItem,FlavourType type)
{
	unsigned short		maxFlavours,flavourIndex;
	FlavourType			theFlavour;

	if (CountDragItemFlavours(theDrag,theItem,&maxFlavours))
		return false;

	for (flavourIndex=1; flavourIndex<=maxFlavours; flavourIndex++)
	{
		if (GetFlavorType(theDrag,theItem,flavourIndex,&theFlavour))
			return false;
	
		if (theFlavour==type)
			return true;
	}
	
	return false;
}

// Use this to only get the data if it's sender only
OSErr FetchSenderOnlyFlavorData(DragReference theDrag,ItemReference theItem,FlavorType theType,Handle *theData)
{
	FlavorFlags		theFlags;
	OSErr			err;
	
	*theData=0L;
	
	if (err=GetFlavorFlags(theDrag,theItem,theType,&theFlags))
		return err;
	
	if (theFlags&flavorSenderOnly)
		return FetchFlavorData(theDrag,theItem,theType,theData);
	else
		return badDragFlavorErr; // that's not from our app but it has the same data type (don't want it then)
}

Boolean DragAndDropPresent()
{
	long		res;
	
	Gestalt(gestaltDragMgrAttr,&res);
	
	return (res&(1<<gestaltAppleEventsPresent));
}

// Returns true if this window is in the same window it started in
Boolean InsideSenderWindow(DragReference theDrag)
{
	DragAttributes	theAtt;
	
	if (GetDragAttributes(theDrag,&theAtt))
		return true; // error, true will probably make their handler exit
	
	return (kDragInsideSenderWindow&theAtt!=0);
}