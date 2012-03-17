// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CASAttachableModelObject.cpp
// © Mark Tully and TNT Software 1998-1999
// 13/11/98
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
#include			<UExtractFromAEDesc.h>
#include			<UAppleEventsMgr.h>
#include			"CASAttachableModelObject.h"
#include			"CASAttachableModelDirector.h"
#include			"CASIChain.h"
#include			"TNT AS Registry.h"
#include			"UASSubroutines.h"
#include			"UScripts.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//			¥ÊConstructor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CASAttachableModelObject::CASAttachableModelObject() :
	mLink(this)
{
	mAttachedScript=kOSANullScript;
	SetModelFlags(mfGetReturnsReference);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//			¥ÊConstructor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CASAttachableModelObject::CASAttachableModelObject(
	LModelObject			*inSuperModel,
	DescType				inKind) :
	inheritedModelObject(inSuperModel,inKind),
	mLink(this)
{
	SetModelFlags(mfGetReturnsReference);
	mAttachedScript=kOSANullScript;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//			¥ÊDestructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Dispose of the script
CASAttachableModelObject::~CASAttachableModelObject()
{
	if (mAttachedScript!=kOSANullScript)
		::OSADispose(GetAttachableModelDirector()->GetScriptingComponent(),mAttachedScript);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//			¥ÊGetAttachableModelDirector						Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Maps onto the CASAttachableModelDirector's call of the same name. Provided for convinience.
CASAttachableModelDirector *CASAttachableModelObject::GetAttachableModelDirector()
{
	return CASAttachableModelDirector::GetAttachableModelDirector();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//			¥ÊBuildIChain										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Builds the inheritance chain for this model object. Override it if the inheritance chain is different for a certain model
// object or a particular apple event. If the chain is left empty then the event is passed straight to the standard ae
// handler.
// The iChain is empty on input.
void CASAttachableModelObject::BuildIChain(
	const AppleEvent		&inAppleEvent,
	CASIChain				&outIChain)
{
	TListElementPtr					alias=mLink.MakeAlias();
	
	ThrowIfMemFull_(alias);
	
	outIChain.mIChain.AppendElement(alias);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//			¥ÊPreHandleAppleEvent								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This routine attempts to get the attached script to handle the apple event, if it doesn't have an attached script then
// it throws errAEAppleEventNotHandled which causes the default handler to be called.
// This routine assembles the inheritance chain (i-chain) so that apple script's continue statement works correctly.
void CASAttachableModelObject::PreHandleAppleEvent(
	const AppleEvent		&inAppleEvent,
	AppleEvent				&outReply,
	SInt32					inAENumber)
{
	switch (inAENumber)
	{
		// Some apple events can't be overriden, get data and set data for example
		case ae_GetData:
		case ae_SetData:
			Throw_(errAEEventNotHandled);
			break;
			
		default:
			{
				// Get the attached script to handle it, if there is one, if not it will propagate towards the default
				// apple event handler which is called when the end of the chain is reached.
			
				// Make iChain	
				CASIChain				iChain(inAENumber);
	
				BuildIChain(inAppleEvent,iChain);
				GetAttachableModelDirector()->HandleResumeDispatch(inAppleEvent,outReply,&iChain);
			}
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ HandleAppleEvent										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Captures any unhandled sub routine calls and passes them through the handler again with the sub routine aenumber resolved.
void CASAttachableModelObject::HandleAppleEvent(
	const AppleEvent&	inAppleEvent,
	AppleEvent&			outAEReply,
	AEDesc&				outResult,
	long				inAENumber)
{
	switch (inAENumber)
	{
		case ae_ASSubroutine:
			long				aeNumber;
			aeNumber=UASSubroutines::GetAENumberForSubroutineEvent(inAppleEvent);
			// Recurse - so calling the overriden version of this proc with the new aeNumber
			if (aeNumber!=ae_ASSubroutine)
				HandleAppleEvent(inAppleEvent,outAEReply,outResult,aeNumber);
			break;

		default:
			inheritedModelObject::HandleAppleEvent(inAppleEvent,outAEReply,outResult,inAENumber);
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//			¥ÊExecuteEventInAttachmentAll						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Checks if there's a default sub model to handle a given event and if not handles the event in it's own attachment
void CASAttachableModelObject::ExecuteEventInAttachmentAll(
									const AppleEvent		&inAppleEvent,
									AppleEvent				&outReply,
									AEDesc					&outResult,
									SInt32					inAENumber)
{
	Boolean	handled=false;

	if (GetDefaultSubModel())
	{
		CASAttachableModelObject	*defSub=dynamic_cast<CASAttachableModelObject*>(GetDefaultSubModel());
	
		if (defSub)
		{
			defSub->ExecuteEventInAttachmentAll(inAppleEvent,outReply,outResult,inAENumber);
			handled=true;
		}
	}

	if (!handled)
		ExecuteEventInAttachment(inAppleEvent,outReply,outResult,inAENumber);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//			¥ÊExecuteEventInAttachment							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Attempts to execute the event specified in the context of the attached script.
// The ResumeDispatchProc is not adjusted as it would normally need to be if the the event is to be continued up the inheritance
// chain towards the main apple event handler in cases where the event is not handled.
void CASAttachableModelObject::ExecuteEventInAttachment(
	const AppleEvent		&inAppleEvent,
	AppleEvent				&outReply,
	AEDesc					&outResult,
	SInt32					inAENumber)
{
	OSErr						err=noErr;

	if (mAttachedScript==kOSANullScript)
		Throw_(errAEEventNotHandled);

	err=::OSADoEvent(GetAttachableModelDirector()->GetScriptingComponent(),&inAppleEvent,mAttachedScript,kOSAModeAlwaysInteract,&outReply);
		
	if (err==errOSAScriptError)
	{
		// It will be quite common for script errors to occur. We can get the actual message from the scripting component
		// and pass it back to the caller for them. They will not be able to get this message unless they're this app.
		err=UScripts::OSAErrorToAEDesc(GetAttachableModelDirector()->GetScriptingComponent(),outReply);
	}
		
	ThrowIfOSErr_(err);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetAEProperty											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Allows apple events to get the script associated with this object. if there is no script then outProperty desc is left
// as it is which will be null.
void CASAttachableModelObject::GetAEProperty(
	DescType		inProperty,
	const AEDesc	&inRequestedType,
	AEDesc			&outPropertyDesc) const
{
	switch (inProperty)
	{
		case pScript:
			if (mAttachedScript!=kOSANullScript)
			{
				DescType			wantType=typeWildCard;
				
				if (inRequestedType.descriptorType!=typeNull)
					UExtractFromAEDesc::TheType(inRequestedType,wantType);
			
				if (wantType==typeChar || wantType==typeIntlText)
				{
					// if caller wants text, we need to de-compile the script
					ThrowIfOSErr_(::OSAGetSource(	GetAttachableModelDirector()->GetScriptingComponent(),
													mAttachedScript,
													wantType,
													&outPropertyDesc));
				}
				else
				{
					if (wantType==typeWildCard)
						wantType=typeOSAGenericStorage;
					
					// Store the script into the reply removing the parent script
					ThrowIfOSErr_(::OSAStore(		GetAttachableModelDirector()->GetScriptingComponent(),
													mAttachedScript,
													wantType,
													kOSAModeDontStoreParent,
													&outPropertyDesc));
				}
			}
			else
			{
				outPropertyDesc.descriptorType=typeNull;
				outPropertyDesc.dataHandle=0L;
			}
			break;
			
		default:
			inheritedModelObject::GetAEProperty(inProperty,inRequestedType,outPropertyDesc);
			break;	
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetAEProperty											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Allows apple events to change the script which is associated with this object. Setting it to a null desc will clear the
// script.
void CASAttachableModelObject::SetAEProperty(
									DescType		inProperty,
									const AEDesc&	inValue,
									AEDesc&			outAEReply)
{
	switch (inProperty)
	{
		case pScript:
			SetScript(inValue);	
			break;
			
		default:
			inheritedModelObject::SetAEProperty(inProperty,inValue,outAEReply);
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetScript												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called to change the script attached to this model object. If it's set to a null desc then the script is cleared.
void CASAttachableModelObject::SetScript(
	const AEDesc			&inValue)
{
	OSErr					err=noErr;
	OSAID					theValueId=kOSANullScript;

	// Are we setting the script from, source code, if so we need to compile it
	if (inValue.descriptorType==typeChar || inValue.descriptorType==typeIntlText)
		err=::OSACompile(	GetAttachableModelDirector()->GetScriptingComponent(),
							&inValue,
							kOSAModeCompileIntoContext,
							&theValueId);
	else if (inValue.descriptorType!=typeNull)
	{
		// it it's not text, we assume the script is already compiled
		err=::OSALoad(		GetAttachableModelDirector()->GetScriptingComponent(),
							&inValue,
							kOSAModeNull,
							&theValueId);
		
		// the following section strips any existing parent script:
		// It is not currently necessary but demonstrates the technique
	/*	if (err == noErr) 
		{ 
			AEDesc		newData;
			err = OSAStore(gScriptingComponent, theValueID, 
								typeOSAGenericStorage, 
								kOSAModeDontStoreParent, 
								&newData);
			if (err == noErr) 
			{
				OSADispose(gScriptingComponent, theValueID);
				theValueID = kOSANullScript;
				err = (OSErr)OSALoad(gScriptingComponent, &newData, 
											kOSAModeNull, &theValueID);
				AEDisposeDesc(&newData);
			}
		}*/
	}

	// We have the new script now, dispose of the previous one
	if (err==noErr)
		SetScript(theValueId);	
	ThrowIfOSErr_(err);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetScript
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Version of set script which takes a script id instead of a AEDesc
void CASAttachableModelObject::SetScript(
	OSAID				inScriptId)
{
	if (mAttachedScript!=kOSANullScript)
	{
		OSErr	err;
		err=::OSADispose(GetAttachableModelDirector()->GetScriptingComponent(),mAttachedScript);
	}
	mAttachedScript=inScriptId;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AEPropertyExists
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the properties that this element supports.
bool CASAttachableModelObject::AEPropertyExists(
	DescType	inProperty) const
{
	bool	res=true;

	switch (inProperty)
	{
		case pScript:
			break;
	
		default:
			res=inheritedModelObject::AEPropertyExists(inProperty);
			break;
	}
	
	return res;
}
