// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Apple Events.c
// Mark Tully
// 17/12/95
// 6/6/96
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

#include	"TNT_Debugging.h"
#include	"Marks Routines.h"
#include	<UAppleEventsMgr.h>
#include	<UExtractFromAEDesc.h>

// in
// keyword
// appleevent
// dest pointer
// max size, comes out as size got
OSErr FetchParam(AEKeyword plop,AppleEvent *appIn,Ptr dest,DescType theType,Size *len)
{
	DescType	actualType;
	
	return AEGetParamPtr(appIn, plop, theType, &actualType, dest, *len, len);			
}

OSErr FetchParamAnySize(AEKeyword plop,AppleEvent *appIn,Ptr *dest,DescType theType)
{
	OSErr	err;
	Size	size;
	DescType	actualType;

	err=AESizeOfParam(appIn,plop, &actualType, &size);
	if (err)
		return err;
	
	*dest=NewPtr(size);
	
	err=AEGetParamPtr(appIn, plop, theType, &actualType, *dest, size, &size);

	return err;
}

long FetchNum(AppleEvent *appIn,AEKeyword theKey)
{
	DescType	actualType;
	long		data;
	Size		theSize;
	
	return AEGetParamPtr(appIn,theKey,typeLongInteger,&actualType,(Ptr)&data,sizeof(long),&theSize);
}

OSErr FetchAEErr(AppleEvent *appIn)
{
	OSErr	theErr=FetchNum(appIn,keyErrorNumber);
	
	if (theErr==errAEDescNotFound)
		return noErr;
	else
		return theErr;
}

#if !TARGET_API_MAC_CARBON
// session ids are depreciated under carbon - don't use this code anymore anyway so it's not worth updating

OSErr GetSendersAddress(AppleEvent *event,AEAddressDesc *targ)
{
	Size		len=sizeof(long);
	DescType	actualType;
	long		sessionID;
	OSErr		err;
	
	err=AEGetAttributePtr(event, keyAddressAttr, typeSessionID, &actualType, &sessionID, len, &len);
	if (err)
		return err;
	
	// Create the address desriptor
	return AECreateDesc(typeSessionID,(Ptr)&sessionID,sizeof(long), targ);
}

// High level command to select a target and return it in a TargetID structure
OSErr SelectTarget(StringPtr prompt,StringPtr appsList,TargetID *targ,PPCFilterUPP theProc)
{
	PortInfoRec		portInfo;	// This will return the port selected
	LocationNameRec	locRec;		// This returns the location of the target (computer address)
	OSErr			err;
	
	err=PPCBrowser(prompt,appsList,false,&locRec,&portInfo,theProc,"\p");
		
	targ->sessionID=0L;			// We don't use this in this example
	targ->name=portInfo.name;	// The port we want to sent it to
	targ->location=locRec;		// The computer the port is on
	
	return err;
}
#endif

// Mark Tully
// 29/1/96

// Initailise for apple events, simpley checks their availible and, if specified inits the PPC tools
Boolean InitializeAE(Boolean ppc)
{
	long 	value; // Store the gestalt result
	OSErr	err;
	
	// Check that we have apple events
	Gestalt(gestaltAppleEventsAttr,&value);
	if(!(value&(1<<gestaltAppleEventsPresent)))
		return false;

#if !TARGET_API_MAC_CARBON
	if (ppc)
	{
		err=PPCInit();
		if (err)
			return false;
	}
#endif
		
	return true;
}

OSErr MRInstallAEHandlers(AEInstallStructPtr installUs,short numToInstall)
{
	OSErr	err = noErr;
	short	count;

	for (count=0; count<numToInstall; count++)
	{
		err = AEInstallEventHandler(installUs[count].theEventClass,
									installUs[count].theEventID,
									NewAEEventHandlerUPP(installUs[count].theHandler),
									0L,false);
		if (err)
			return err;
	}
	return err;
}

#if !TARGET_API_MAC_CARBON
// target ids are depreciated under carbon

// This is used for sending a simple event requiring no data or reply
OSErr SendAppleEvent(TargetID *targ,AEEventClass aeClass,AEEventID aeID)
{
	return BigSendAppleEvent(targ,aeClass,aeID,0L,0,0,false,true);
}

OSErr BigSendAppleEvent(TargetID *targ,AEEventClass aeClass,AEEventID aeID,Ptr *data,short dataLen,DescType dataType,Boolean returnData,Boolean checkForHandlerErr)
{
	AEAddressDesc	targDesc={typeNull, nil};	// Desc for target
	AppleEvent		appOut={typeNull, nil},appIn={typeNull, nil};
	short			sendMode=kAENeverInteract;
	OSErr			err;
									
	if (returnData || checkForHandlerErr)
		sendMode+=kAEWaitReply;
	else
		sendMode+=kAENoReply;
									
	// Create the address desriptor
	err=AECreateDesc(typeTargetID,(Ptr)targ,sizeof(TargetID), &targDesc);
	if (err)
		return err;
		
	// now create the apple event which will be sent
	err=AECreateAppleEvent(aeClass,aeID,&targDesc,kAutoGenerateReturnID,kAnyTransactionID,&appOut);	
	if (err)
	{
		AEDisposeDesc(&targDesc);
		return err;
	}
	
	if (data && *data)
	{
		err=AEPutParamPtr(&appOut,keyDirectObject,typeChar,*data,dataLen);
		DisposePtr(*data);
		*data=0L;
		if (err)
		{
			AEDisposeDesc(&appOut);
			AEDisposeDesc(&targDesc);
		}
	}
		
	// Send the apple event	
	err=AESend(&appOut,&appIn,sendMode,kAENormalPriority,kAEDefaultTimeout,0L,0L);		
	if (err)
	{
		AEDisposeDesc(&appOut); 		// get rid of the apple events
		AEDisposeDesc(&appIn);
		AEDisposeDesc(&targDesc);	// and the address descriptor
	}
		
	if (returnData) // get info from reply event
		err=FetchParamAnySize(keyDirectObject,&appIn,data,dataType);
	
	// Get the error returned if any
	if (!err && checkForHandlerErr)
		err=FetchAEErr(&appIn);
	
	// The apple event has been sent, dispose of descriptors
	AEDisposeDesc(&appOut); 		// get rid of the apple events
	AEDisposeDesc(&appIn);
	AEDisposeDesc(&targDesc);	// and the address descriptor
	
	return err;
}

#endif

// 1/4/96
// Mark Tully

// Counts how many files are in the apple event
short CountFiles(AppleEvent *message)
{
	AEDesc		fileListDesc;
	long		numFiles;
	OSErr		err;
	
	/* extract the list of aliases into fileListDesc */
	err = AEGetKeyDesc(message, keyDirectObject, typeAEList, &fileListDesc);
	if (err!=noErr)
		return 0;
		
	/* count the list elements */
	err = AECountItems(&fileListDesc, &numFiles);

	AEDisposeDesc(&fileListDesc);
	
	return	err ? 0 : numFiles; 
}

// 20/2/96
// Mark Tully

// Extracts all the files from a Finder print or open apple event and call the routine specified
// on each of them
OSErr ForEachFileDo(const AppleEvent *message,AEFileProc theirProc)
{
	OSErr		err = noErr;
	AEDesc		fileListDesc;
	long		numFiles;
	DescType	actualType;
	long		actualSize;
	AEKeyword	actualKeyword;
	FSSpec		oneFile;
	long		index;
							
	/* extract the list of aliases into fileListDesc */
	err = AEGetKeyDesc(message, keyDirectObject, typeAEList, &fileListDesc);
	if (err!=noErr)
		return err;
		
	/* count the list elements */
	err = AECountItems(&fileListDesc, &numFiles);
	if (err != noErr)
	{
		AEDisposeDesc(&fileListDesc);
		return err;
	}
	
	/* get each from list and process it */		
	for (index = 1; index <= numFiles; index ++)
	{
		err = AEGetNthPtr(&fileListDesc, index, typeFSS, &actualKeyword,
							&actualType, (Ptr)&oneFile, sizeof(oneFile), &actualSize);
		if (err != noErr)
		{
			AEDisposeDesc(&fileListDesc);
			return err;
		}
		
		/* oneFile contains FSSpec of file in question */
		err=theirProc(&oneFile);
		if (err)
			break;
	}
	
	AEDisposeDesc(&fileListDesc);
	return err;
}

/*e*/
// This proc takes the AEDesc you pass it and attempts to coerce it into the type specified, if it can't then it doesn't
// throw. It only throws if an error other than can't coerce occurs.
// Passing a null descriptor as inRequestedType means make no changes
void CoerceInPlaceDesc(AEDesc &inDesc,const AEDesc &inRequestedType)
{
	if (inRequestedType.descriptorType!=typeNull)
	{
		DescType			requestedType;

		UExtractFromAEDesc::TheType(inRequestedType,requestedType);
		
		CoerceInPlaceType(inDesc,requestedType);
	}
}

/*e*/
void CoerceInPlaceType(AEDesc &inDesc,DescType toType)
{
	if (toType==typeWildCard || toType==typeNull)
		return;

	AEDesc				resultDesc={typeNull,0L};
	OSErr				err=::AECoerceDesc(&inDesc,toType,&resultDesc);

	if (err && err!=errAECoercionFail)
		Throw_(err);
		
	if (!err)
	{
		AEDisposeDesc(&inDesc);
		inDesc=resultDesc;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetAEDataHandle										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the AE data handle from the desc. You have to dispose of this handle.
Handle GetAEDataHandle(
	const AEDesc	&inDesc)
{
	Size		size;

	size=::AEGetDescDataSize(&inDesc);
	
	Handle		h=::NewHandle(size);
	ThrowIfMemFull_(h);
	
	HLock(h);
	OSErr	err=::AEGetDescData(&inDesc,*h,size);
	HUnlock(h);
	
	if (err)
	{
		DisposeHandle(h);
		Throw_(err);
	}

	return h;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CoerceWithReply										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Version of AECoerceDesc which fills out a reply ae with any error which occurs and then throws.
void CoerceWithReply(
	const AEDesc	&inDesc,
	AEDesc			&outDesc,
	DescType		inDestType,
	AppleEvent		&outReply)
{
	Try_
	{
		if (inDestType==typeWildCard || inDestType==typeNull)
		{
			ThrowIfOSErr_(::AEDuplicateDesc(&inDesc,&outDesc));
			return;
		}
		ThrowIfOSErr_(::AECoerceDesc(&inDesc,inDestType,&outDesc));
	}
	Catch_(err)
	{
		// Error occurred add to the AE reply
		if (ErrCode_(err)==errAECoercionFail)
		{
			try
			{
				StAEDescriptor		typeDesc(inDestType);

				::AEPutKeyDesc(&outReply,kOSAErrorOffendingObject,&inDesc);
				::AEPutKeyDesc(&outReply,kOSAErrorExpectedType,&typeDesc.mDesc);
			}
			catch(...)		// StAEDescriptor can throw - don't let it
			{
			}
		}
		
		throw;
	}
}

/*e*/
// This routine retrieves the property you ask for if it is available under the keyAEPropData parameter
// Returns true if it was available, false if not. Throws on error
Boolean GetPropertyFromAppleEvent(const AppleEvent &inAppleEvent,AEDesc &property,DescType propertyCode,DescType desiredType)
{
	// Initialize the reply
	property.descriptorType=typeNull;
	property.dataHandle=0L;

	// The the properties descriptor
	StAEDescriptor props;
	props.GetOptionalParamDesc(inAppleEvent,keyAEPropData,typeAERecord);
	if (props.mDesc.dataHandle)
	{	
		// From that properties descriptor get the required property
		OSErr err=::AEGetKeyDesc((AEDesc*)props,propertyCode,desiredType,&property);
		if (err)
		{
			if (err!=errAEDescNotFound)
				Throw_(err);
		}
		else
			return true;	
	}
	
	return false;
}