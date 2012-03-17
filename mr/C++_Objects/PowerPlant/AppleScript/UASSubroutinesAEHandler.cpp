// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UASSubroutinesAEHandler.cpp
// © Mark Tully and TNT Software 1999
// 6/1/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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
	This utility class complements the UASSubroutines class.
	It implements a handler for subroutine apple events. When it receives one it obtains the keydirect param and the
	ae number of the subroutine and then processes it as normal.
	
	If it can't find a target for the AS subroutine it uses the default subroutine model to handle it. If that's null it
	uses the default model from LModelDirector to handle it. See StASSubroutineTarget for more info.
*/

#include	"MR Debug.h"
#include	<LModelObject.h>
#include	<LModelDirector.h>
#include	<UAppleEventsMgr.h>
#include	"UASSubroutinesAEHandler.h"
#include	"UASSubroutines.h"
#include	"TNT AS Registry.h"

StAEObjectSpecifier		UASSubroutinesAEHandler::sDefaultSubroutineModel;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetDefaultSubroutineModel									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the default model for handling unowned applescript subroutines.
void UASSubroutinesAEHandler::GetDefaultSubroutineModel(
	AEDesc			&outDesc)
{
	ThrowIfOSErr_(::AEDuplicateDesc(sDefaultSubroutineModel,&outDesc));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetDefaultSubroutineModel									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UASSubroutinesAEHandler::SetDefaultSubroutineModel(
	AEDesc			&inDesc)
{
	sDefaultSubroutineModel.Dispose();
	ThrowIfOSErr_(::AEDuplicateDesc(&inDesc,sDefaultSubroutineModel));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InstallCallBacks											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Installs the subroutine ae handler
void UASSubroutinesAEHandler::InstallCallBacks()
{
	ThrowIfOSErr_(::AEInstallEventHandler(	kASAppleScriptSuite,
											kASSubroutineEvent,
											NewAEEventHandlerUPP(SubroutineAEHandler),
											ae_ASSubroutine,
											false));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SubroutineAEHandler
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Finds the direct object and resolves it then process the ae as normal.
pascal OSErr UASSubroutinesAEHandler::SubroutineAEHandler(
	const AppleEvent*	inAppleEvent,
	AppleEvent*			outAEReply,
	SInt32				inRefCon)
{
	Try_
	{
		LModelDirector		*mDirector=LModelDirector::GetModelDirector();
		StAEDescriptor		directObj;
		StAEDescriptor		directObjToken;
		StAEDescriptor		theResult;
		SInt32				aeNumber;
		OSErr				err=noErr;
		
		// Get direct object parameter
		Try_
		{
			aeNumber=UASSubroutines::GetDirectObjectFromSubroutineEvent(*inAppleEvent,directObj.mDesc);
			// directObj may be null
		}
		Catch_(inErr)
		{
			err=ErrCode_(inErr);
		}

		if (directObj.IsNotNull())
		{	// Direct object exists. Try to resolve
			//   it to a Token. It's OK for the
			//   resolve to fail, so we turn off
			//   throw debugging to prevent annoying
			//   breaks.
			StDisableDebugThrow_();
			err = mDirector->Resolve(directObj, directObjToken);
		}
		
		if ( (directObj.IsNull()) ||
			 (err == errAENotAnObjSpec) ||
			 (err == errAENoSuchObject) ) {
										// Direct object parameter not present
										//   or is not an object specifier.
										//   Let the default model handle it.
			
			// Pass it to the by the default subroutine model
			Try_
			{
				if (sDefaultSubroutineModel.IsNotNull())
				{
					sDefaultSubroutineModel.Resolve()->HandleAppleEventAll(*inAppleEvent,*outAEReply,theResult,aeNumber);
					return noErr;		// Handled OK
				}
			}
			Catch_(err)
			{
				// event not handled is OK - just pass to the default model instead.
				if (ErrCode_(err)!=errAEEventNotHandled)
					throw;
			}
			
			(LModelObject::GetDefaultModel())->
				HandleAppleEventAll(*inAppleEvent, *outAEReply, theResult, aeNumber);
			
		} else if (err == noErr) {	
										// Process this event using the Token
										//   representing the direct direct
			mDirector->ProcessTokenizedEvent(*inAppleEvent, directObjToken,
									theResult, *outAEReply, aeNumber);
		} else {
			Throw_(err);
		}
										// Put result code in Reply
		if ( (theResult.IsNotNull()) &&
			 (outAEReply->descriptorType != typeNull) ) {
			 
			err = ::AEPutParamDesc(outAEReply, keyAEResult, theResult);
			ThrowIfOSErr_(err);
		}
	}
	Catch_(err)
	{
		return ErrCode_(err);
	}
	
	return noErr;
}