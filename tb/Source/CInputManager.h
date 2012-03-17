// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CInputSprocket.h
// © John Treece-Birch 2000
// 12/1/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
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

#define COMPILE_ISP_CODE				__MWERKS__

#if COMPILE_ISP_CODE

//#define USE_HID_WIZARD

#ifdef USE_HID_WIZARD
#include 	"HIDWizard.h"
#else
#include 	"InputSprocket.h"
#endif

#endif

#include	<PP_Types.h>
#include	"ResourceTypes.h"

class CResourceContainer;

#if COMPILE_ISP_CODE

class CInputManager
{
	protected:
		#define	kInputSprocketResType		'INPT'
					
		static ISpElementReference			*sElements;
		static ISpNeed						*sNeeds;
		static short						sNumOfElements;
		
		static bool							sUseSprockets;
		static bool							sInputActive,sInputValid,sPausedState;
	
		static void							DestroyInput();
	
	public:
	
		static void							Startup();
		static void							Shutdown();

		static void							LoadInputResource(
												CResourceContainer	*inResConc,
												TResId				inResId,
												OSType				inCreator);

		static SInt32						Poll(
												UInt16		inIndex);
	
		static void							Edit();
		
		static void							SuspendInput();
		static void							ResumeInput();
		
		static void							GamePaused();
		static void							GameResumed();

		static bool							ValidInput()			{ return sInputValid; }
};

#else

// disabled for xcode target as os x doesn't support it
class CInputManager
{
	protected:
		#define	kInputSprocketResType		'INPT'
					
		static void							DestroyInput() {}
	
	public:
	
		static void							Startup() {}
		static void							Shutdown() {}

		static void							LoadInputResource(
												CResourceContainer	*inResConc,
												TResId				inResId,
												OSType				inCreator) {}

		static SInt32						Poll(
												UInt16		inIndex) { return 0; }
	
		static void							Edit() {}
		
		static void							SuspendInput() {}
		static void							ResumeInput() {}
		
		static void							GamePaused() {}
		static void							GameResumed() {}

		static bool							ValidInput()			{ return false; }
};

#endif