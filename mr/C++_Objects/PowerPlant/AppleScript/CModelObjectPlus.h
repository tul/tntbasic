// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CModelObjectPlus.h
// © Mark Tully and TNT Software 1998
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

#pragma once

#include			<LModelObject.h>

class CModelObjectPlus : public LModelObject
{
	private:
		typedef LModelObject		inheritedModelObject;
	
	public:
		typedef UInt16				TModelObjectFlags;
		enum
		{
			mfNoFlags=0,
			mfGetReturnsReference=1,					// PowerPlant makes a get action on an object return it's contents,
														// setting this flags makes it returns a reference to the object 
														// instead.
			mfSpecifyByName=2,							// When making specifiers for this object, do them by name.
			mfSpecifyById=4,							// When making specifiers for this object, do them by id - must override GetAEProperty to support pID
		};
	
	protected:
		TModelObjectFlags			mFlags;
	
		virtual void /*e*/			MakeSelfSpecifier(
										AEDesc&				inSuperSpecifier,
										AEDesc&				outSelfSpecifier) const;
	
	public:
		/*e*/						CModelObjectPlus();
		/*e*/						CModelObjectPlus(
										LModelObject		*inSuperModel,
										DescType			inKind = typeNull);
	
		virtual void				SetModelFlags(
										TModelObjectFlags	inNewFlags)			{ mFlags=inNewFlags; }
		virtual TModelObjectFlags	GetModelFlags()								{ return mFlags; }

		virtual bool				IsOfKind(
										DescType			inModelKind) const;

		virtual bool /*e*/			AddPropertyToRecord(
										AEKeyword			inKeyword,
										AERecord			&outRecord,
										bool				inThrowIfMissing);

		virtual void /*e*/			SendDeleteAE(
										AppleEvent			&outReply,
										bool				inRecord);

		virtual void /*e*/			SendSelfAEWithReply(
										AEEventClass		inEventClass,
										AEEventID			inEventID,
										AppleEvent			&outReply,
										bool				inRecord);

		virtual void /*e*/			HandleAppleEvent(
										const AppleEvent	&inAppleEvent,
										AppleEvent			&outAEReply,
										AEDesc				&outResult,
										SInt32				inAENumber);
};


