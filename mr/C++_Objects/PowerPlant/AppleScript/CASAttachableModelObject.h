// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CASAttachableModelObject.h
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

#pragma once

#include		"CModelObjectPlus.h"
#include		"TSharedListElement.h"

class			CASAttachableModelDirector;
class			CASIChain;

class CASAttachableModelObject : virtual public CModelObjectPlus
{
	friend class CASIChain;						// Needs to be able to access the mLink member

	private:
		typedef CModelObjectPlus			inheritedModelObject;

	protected:
		OSAID								mAttachedScript;
		TSharedListElement					mLink;
	
		virtual void /*e*/					BuildIChain(
												const AppleEvent		&inAppleEvent,
												CASIChain				&outIChain);
	
	public:
		static CASAttachableModelDirector	*GetAttachableModelDirector();
	
											CASAttachableModelObject();
											CASAttachableModelObject(
												LModelObject			*inSuperModel,
												DescType				inKind = typeNull);

		virtual								~CASAttachableModelObject();
																				
		virtual void /*e*/					PreHandleAppleEvent(
												const AppleEvent		&inAppleEvent,
												AppleEvent				&outReply,
												SInt32					inAENumber);

		virtual void /*e*/					HandleAppleEvent(
												const AppleEvent&		inAppleEvent,
												AppleEvent&				outAEReply,
												AEDesc&					outResult,
												long					inAENumber);

		virtual void /*e*/					ExecuteEventInAttachmentAll(
												const AppleEvent		&inAppleEvent,
												AppleEvent				&outReply,
												AEDesc					&outResult,
												SInt32					inAENumber);
		virtual void /*e*/					ExecuteEventInAttachment(
												const AppleEvent		&inAppleEvent,
												AppleEvent				&outReply,
												AEDesc					&outResult,
												SInt32					inAENumber);

		virtual bool						AEPropertyExists(
												DescType				inProperty) const;
		virtual void /*e*/					SetScript(
												const AEDesc			&inValue);
		virtual void						SetScript(
												OSAID					inScriptId);
		virtual void /*e*/					SetAEProperty(
												DescType				inProperty,
												const AEDesc			&inValue,
												AEDesc					&outAEReply);
		virtual void /*e*/					GetAEProperty(
												DescType				inProperty,
												const AEDesc			&inRequestedType,
												AEDesc					&outPropertyDesc) const;
};