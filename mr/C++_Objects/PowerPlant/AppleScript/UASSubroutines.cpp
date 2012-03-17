// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UASSubroutines.cpp
// © Mark Tully and TNT Software 1998-1999
// 14/12/98
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

#include			"MR Debug.h"
#include			<UAppleEventsMgr.h>
#include			<TArrayIterator.h>
#include			<LString.h>
#include			"UASSubroutines.h"
#include			"Utility Objects.h"
#include			"Marks Routines.h"
#include			"TNewHandleStream.h"

TArray<Handle>		UASSubroutines::sSubroutineInfos;

/*
	Class designed to allow easy dispatch of an apple event onto a AppleScript sub routine inside a script context.
	By using a sub routine info resource it can take an apple event and convert it into a sub routine event which can be
	executed in a context.
	It can also take a subroutine event and extract the original ae number so that if a sub routine gets continued into
	the application it can be processed easily.
*/

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadSubroutineInfo									/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adds the handle to an array of handles of sub routine infos thus making it available to the class. The handle is kept in
// mem until UnloadAllSubroutineInfos is called at which point it is released or disposed of depending on whether it's a
// resource handle or not.
// Don't add the same handle twice as it will mess up on release.
void UASSubroutines::LoadSubroutineInfo(
	Handle				inHandle)
{
	// Check the version of the resource
	if (*(short*)*inHandle!=kCurrentVersion)
	{
		SignalPStr_("\pUASSubroutines::LoadSubroutineInfo\nAttempt to load out of date resource format.");
		Throw_(paramErr);
	}

	sSubroutineInfos.AddItem(inHandle);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadSubroutineInfo									/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Loads up the apple script sub routine info resource you specify.
// Pass -1 for the res file ref to get from the current resource file.
void UASSubroutines::LoadSubroutineInfo(
	short				inResFileRef,
	ResIDT				inResId)
{
	UResFetcher			resource(inResFileRef,ResType_ASSubroutineInfo,inResId);
		
	LoadSubroutineInfo(resource);
	resource.SetReleaseFlag(false);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ UnloadAllSubroutineInfos								Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Disposes of all the handles which have been loaded.
// Calls release resource or dispose handle accordingly.
void UASSubroutines::UnloadAllSubroutineInfos()
{
	TArrayIterator<Handle>							iter(sSubroutineInfos);
	Handle											handle;
	
	while (iter.Next(handle))
		BetterDisposeHandle(handle);
		
	sSubroutineInfos.RemoveAllItemsAfter(0);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LocateSubroutineByAENumber							/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Looks through all the sub routine infos to find the sub routine numbered and then returns the handle and offset where it
// was found.
// Throws a -1 if it can't find it.
void UASSubroutines::LocateSubroutineByAENumber(
	SInt32				inAENumber,
	Handle				&outHandle,
	SInt32				&outMarker)
{
	// Index through all the loaded sub routine infos and look if it has the given sub routine name defined.
	TArrayIterator<Handle>							iter(sSubroutineInfos);
	Handle											handle;
	
	while (iter.Next(handle))
	{
		TNewHandleStream							stream(handle,false);
		short										version;
		short										numSubroutineInfos;
		short										numParameters;
		SInt32										aeNumber;
		LStr255										subRoutineName;
		unsigned char								subMode;
		SInt32										marker;
		
		// Parse the stream
		stream >> version;
		stream >> numSubroutineInfos;
	
		if (version!=kCurrentVersion)
			Throw_(paramErr);
	
		for (short index=0; index<numSubroutineInfos; index++)
		{		
			marker=stream.GetMarker();
		
			stream >> aeNumber;
			
			if (aeNumber==inAENumber)
			{
				// Found it
				outHandle=handle;
				outMarker=marker;
				return;
			}
			
			stream >> subRoutineName;	
			stream >> subMode;
			stream >> numParameters;
			stream.SetMarker(numParameters*kSizeOfParameterItem,streamFrom_Marker);
		}		
	}
	
	Throw_(-1);	// Can't find it
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LocateSubroutineByName							/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Looks through all the sub routine infos to find the sub routine named and then returns the handle and offset where it
// was found.
// Throws a -1 if it can't find it.
void UASSubroutines::LocateSubroutineByName(
	const StringPtr		inString,
	Handle				&outHandle,
	SInt32				&outMarker)
{
	// Index through all the loaded sub routine infos and look if it has the given sub routine name defined.
	TArrayIterator<Handle>							iter(sSubroutineInfos);
	Handle											handle;
	
	while (iter.Next(handle))
	{
		TNewHandleStream							stream(handle,false);
		short										version;
		short										numSubroutineInfos;
		short										numParameters;
		SInt32										aeNumber;
		LStr255										subRoutineName;
		unsigned char								subMode;
		SInt32										marker;
		
		// Parse the stream
		stream >> version;
		stream >> numSubroutineInfos;
	
		if (version!=kCurrentVersion)
			Throw_(paramErr);
	
		for (short index=0; index<numSubroutineInfos; index++)
		{		
			marker=stream.GetMarker();
		
			stream >> aeNumber;
			stream >> subRoutineName;
			
			if (subRoutineName==inString)
			{
				// Found it
				outHandle=handle;
				outMarker=marker;
				return;
			}
				
			stream >> subMode;
			stream >> numParameters;
			stream.SetMarker(numParameters*kSizeOfParameterItem,streamFrom_Marker);
		}		
	}
	
	Throw_(-1);	// Can't find it
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetDirectObjectFromSubroutineEvent					/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The direct object for a sub routine is not always stored in the keyDirectObject. In positional sub routines this is used
// for the list of parameters for example. By using the sub routine info table in reverse the direct object can be extracted
// from the event. The aeNumber is also returned as it is often useful.
// Throws leaves outDirectObject alone if it can't find a direct object and just returns the AE number. So make sure you
// set outDirectObject to null on entry then if it's null on exit you know a direct ob wasn't found.
// Throws if it can't find aecode or direct object.
SInt32 UASSubroutines::GetDirectObjectFromSubroutineEvent(
	const AppleEvent	&inAppleEvent,
	AEDesc				&outDirectObject)
{
	Handle			handle;
	SInt32			offset;
	SInt32			aeNumber;
	Str255			matchName;
	Size			actualSize;
	LStr255			subroutineName;
	unsigned char	subMode;
	SInt16			numParameters;
	DescType		dontCare;

	// Extract the name in the sub routine
	ThrowIfOSErr_(::AEGetParamPtr(&inAppleEvent,keyASSubroutineName,typeChar,&dontCare,(Ptr)&matchName[1],255,&actualSize));
	matchName[0]=actualSize;

	// Find the info on this sub routine
	Try_
	{
		LocateSubroutineByName(matchName,handle,offset);
	}
	Catch_(err)
	{
		Throw_(errAENoSuchObject);
	}
	
	TNewHandleStream		stream(handle,false);
	stream.SetMarker(offset,streamFrom_Start);
	
	// Now find the direct object type
	stream >> aeNumber;
	stream >> subroutineName;
	stream >> subMode;
	stream >> numParameters;

	for (SInt16 paramIndex=0; paramIndex<numParameters; paramIndex++)
	{
		DescType			aeKeyword,subroutineKeyword;
		
		stream >> aeKeyword;
		stream >> subroutineKeyword;

		if (aeKeyword==keyDirectObject)
		{
			// found it, extract the desc
			if (subMode)		// using positional parameters
			{
				StAEDescriptor		paramList(inAppleEvent,keyDirectObject,typeWildCard);
				
				ThrowIfOSErr_(::AEGetNthDesc(paramList,paramIndex+1,typeWildCard,&dontCare,&outDirectObject));
				return aeNumber;
			}
			else
			{
				ThrowIfOSErr_(::AEGetParamDesc(&inAppleEvent,subroutineKeyword,typeWildCard,&outDirectObject));
				return aeNumber;
			}
		}
	}
	
	// Couldn't find a direct object - just return the aenumber	
	return aeNumber;
}	
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetAENumberForSubRoutineName							/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Looks up in the loaded sub routine info resources what the AENumber is for a given sub routine name.
// Throws a -1 if it can't find it.
SInt32 UASSubroutines::GetAENumberForSubroutineName(
	const StringPtr		inString)
{
	Handle		handle;
	SInt32		offset;
	SInt32		aeNumber;

	LocateSubroutineByName(inString,handle,offset);
	
	TNewHandleStream		stream(handle,false);
	
	stream.SetMarker(offset,streamFrom_Start);
	
	stream >> aeNumber;
	
	return aeNumber;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetAENumberForSubroutineEvent							/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Same as above but it extracts the name from the event.
SInt32 UASSubroutines::GetAENumberForSubroutineEvent(
	const AppleEvent	&inAppleEvent)
{
	Str255				subroutineName;
	DescType			dontCare;
	Size				actualSize;
	
	// Extract the name in the sub routine
	ThrowIfOSErr_(::AEGetParamPtr(&inAppleEvent,keyASSubroutineName,typeChar,&dontCare,(Ptr)&subroutineName[1],255,&actualSize));
	subroutineName[0]=actualSize;

	return GetAENumberForSubroutineName(subroutineName);
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MakeSubroutineEvent									/*e*/ Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Converts an apple event into a sub routine call. The event number is used to determine what sub routine is created.
// Parameters are fetched by keyword and put into the sub routine call in the order specified in the resource with the
// preposition keywords specified.
void UASSubroutines::MakeSubroutineEvent(
	const AppleEvent	&inAppleEvent,
	SInt32				inAENumber,
	AppleEvent			&outSubroutineEvent)
{
	Handle										handle;
	SInt32										offset;
	
	LocateSubroutineByAENumber(inAENumber,handle,offset);

	TNewHandleStream							stream(handle,false);
	short										numParameters;
	unsigned char								subMode;
	SInt32										aeNumber;
	LStr255										subRoutineName;
	StAEDescriptor								positionalParams;

	stream.SetMarker(offset,streamFrom_Start);

	stream >> aeNumber;
	stream >> subRoutineName;
	stream >> subMode;
	stream >> numParameters;
			
				
	if (subMode)
		ThrowIfOSErr_(::AECreateList(0L,0L,false,&positionalParams.mDesc));
			
	// Assemble the sub routine apple event
	UAppleEventsMgr::MakeAppleEvent(kASAppleScriptSuite,kASSubroutineEvent,outSubroutineEvent);
      
	// Add the name of the subroutine
	ThrowIfOSErr_(::AEPutParamPtr(
							&outSubroutineEvent,
							keyASSubroutineName,
							typeChar,	
							&subRoutineName[1],
							subRoutineName[0]));

	// Transfer the parameters
	for (SInt16 paramIndex=0; paramIndex<numParameters; paramIndex++)
	{
		DescType			wasKeyword,newKeyword;
		StAEDescriptor		desc;
		
		stream >> wasKeyword;
		stream >> newKeyword;
		
		ThrowIfOSErr_(::AEGetParamDesc(&inAppleEvent,wasKeyword,typeWildCard,&desc.mDesc));
		
		if (subMode)		// subMode==true means use positional parameters
			ThrowIfOSErr_(::AEPutDesc(&positionalParams.mDesc,0,&desc.mDesc));
		else
			ThrowIfOSErr_(::AEPutParamDesc(&outSubroutineEvent,newKeyword,&desc.mDesc));
	}
				
	// Add the positional parameters if that is the mode this sub routine is called with
	if (subMode)
		ThrowIfOSErr_(::AEPutParamDesc(&outSubroutineEvent,keyDirectObject,&positionalParams.mDesc));
}







