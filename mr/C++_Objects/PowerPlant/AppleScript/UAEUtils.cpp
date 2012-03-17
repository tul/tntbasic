// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UAEUtils.cpp
// Mark Tully
// 5/9/99
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
	Provides routines for manipulating apple events. Relies on the PowerPlant classes for at least some of the
	routines.
*/

#include			"TNT_Debugging.h"
#include			"UAEUtils.h"
#include			<LModelObject.h>
#include			<UExtractFromAEDesc.h>
#include			<UAppleEventsMgr.h>
#include			<LModelDirector.h>

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetInsertionTarget									Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This is a routine for finding out where the user is attempting to create the new object.
// It is not a replacement function for powerplant and is lifted straight from pp code. (in the HandleCreateAppleEvent
// routine). It can return nil which means no target can be found. It can also throw an error if there is a problem
// resolve the reference.
LModelObject *UAEUtils::GetInsertionTarget(
	AEDesc			&insertTarget)		// insertTarget - generally from the keyInsertHere parameter of an AE
{
	LModelObject	*container=0L;

	switch (insertTarget.descriptorType)
	{
		case typeInsertionLoc:
		{
			OSType			insertPosition;
			LModelObject	*target;
			
			StAEDescriptor	positionD(insertTarget, keyAEPosition, typeEnumeration);
			UExtractFromAEDesc::TheEnum(positionD, insertPosition);
			StAEDescriptor	objectD(insertTarget, keyAEObject, typeObjectSpecifier),
							tokenD;
			OSErr err = LModelDirector::Resolve(objectD, tokenD.mDesc);
			ThrowIfOSErr_(err);
			target = LModelObject::GetModelFromToken(tokenD);
			target = target->GetInsertionTarget(insertPosition);
			container = target->GetInsertionContainer(insertPosition);
			break;
		}

		case typeNull:
			// The "insert here" parameter is supposed be required. However,
			// it seems reasonable to let it be optional and use the
			// beginning of the Application as the default location.
			container = LModelObject::GetDefaultModel();	
			break;
			
		case typeObjectSpecifier:
		{
			//	The AppleEvent was coded and resolved (using lazy objects)
			//	in such a way that the target IS the element to be created.
			//	Its container should recognize this so let its container
			//	finish the creation (setting the parameters).
			LModelObject	*target;
			StAEDescriptor	tokenD;
			OSErr err = LModelDirector::Resolve(insertTarget, tokenD.mDesc);
			ThrowIfOSErr_(err);
			target = LModelObject::GetModelFromToken(tokenD);
			target->GetInsertionTarget(typeNull);			
			container = target->GetInsertionContainer(typeNull);
			if (container == nil)
				container = LModelObject::GetDefaultModel();
			break;
		}

		default:
			// Unknown object location type
			Throw_(errAEBadKeyForm);
			break;
	}
	
	return container;
}