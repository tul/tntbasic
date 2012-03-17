// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CInputManager.cpp
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

/*
	Wraps Input Sprocket in a class. The class loads the element needs in from a resource and it can then be polled very
	easily
*/

#include		"TNT_Debugging.h"
#include		"CInputManager.h"

#if COMPILE_ISP_CODE

#include		"Useful Defines.h"
#include		"Utility Objects.h"
#include		"UTBException.h"
#include		"TNewHandleStream.h"
#include		"Root.h"
#include		"CApplication.h"
#include		"CResource.h"
#include		"CResourceContainer.h"
#include		"UResources.h"

ISpElementReference			*CInputManager::sElements=0L;
ISpNeed						*CInputManager::sNeeds=0L;
short						CInputManager::sNumOfElements=0;

#ifdef USE_HID_WIZARD
bool						CInputManager::sUseSprockets=true;
#else
bool						CInputManager::sUseSprockets=((ISpGetVersion)!=(void*)kUnresolvedCFragSymbolAddress);
#endif

bool						CInputManager::sInputActive=false;
bool						CInputManager::sInputValid=false;
bool						CInputManager::sPausedState=false;

void CInputManager::Startup()
{
	if (sUseSprockets)
	{
#ifdef USE_HID_WIZARD
		InitHIDWizard();
#endif

		sInputValid=false;
		sInputActive=false;
		sPausedState=false;
		::ISpStartup();
	}
}

void CInputManager::Shutdown()
{
	if (sUseSprockets)
	{
		sPausedState=false;
		DestroyInput();
		::ISpShutdown();
	}
}

void CInputManager::LoadInputResource(
	CResourceContainer	*inResContainer,
	TResId				inResId,
	OSType				inCreator)
{
	if (sUseSprockets)
	{
		// If there was some input already allocated then delete it
		DestroyInput();

		::ISpResume();
			
		// Activate the keyboard and mouse
		::ISpDevices_ActivateClass(kISpDeviceClass_Mouse);
		::ISpDevices_ActivateClass(kISpDeviceClass_Keyboard);

		Try_
		{
			// Create the sNeeds structures from the file
			if (inResContainer->ResourceExists(kInputSprocketResType,inResId))
			{
				StTNTResource		ires(inResContainer,kInputSprocketResType,inResId);
				TNewHandleStream	stream(ires->GetReadOnlyDataHandle(),false);
				short				version;

				// Parse the resource
				stream >> version;
				
				if (version==0)
				{
					stream >> sNumOfElements;
					
					if (!sNumOfElements)
						UTBException::ThrowNoInputElementsDefined();		// will cause an error in ISpElement_NewVirtualFromNeeds

					sNeeds=new ISpNeed[sNumOfElements];
					sElements=new ISpElementReference[sNumOfElements];

					for (short n=0; n<sNumOfElements; n++)
					{
						stream >> sNeeds[n].name;
						stream >> sNeeds[n].iconSuiteResourceId;
						stream >> sNeeds[n].theKind;
						stream >> sNeeds[n].theLabel;
						
						sNeeds[n].playerNum=0;
						sNeeds[n].group=0;
						sNeeds[n].flags=0;
						sNeeds[n].reserved1=0;
						sNeeds[n].reserved2=0;
						sNeeds[n].reserved3=0;
					}
				}
				else if (version==1)
				{
					stream >> sNumOfElements;

					if (!sNumOfElements)
						UTBException::ThrowNoInputElementsDefined();		// will cause an error in ISpElement_NewVirtualFromNeeds

					sNeeds=new ISpNeed[sNumOfElements];
					sElements=new ISpElementReference[sNumOfElements];

					for (short n=0; n<sNumOfElements; n++)
					{
						stream >> sNeeds[n].name;
						stream >> sNeeds[n].theLabel;
						
						switch (sNeeds[n].theLabel)
						{
							case 'fire': sNeeds[n].theKind=kISpElementKind_Button; break;
							// Note, kISpElementKind_Movement is a complex input element. It
							// returns two analogue readings of the x,y axis and an overall
							// direction indicator.
							// TB's poll input returns an int, so we have to use dpads instead.
							// kISpElementKind_DPad
							case 'move': sNeeds[n].theKind=kISpElementKind_DPad; break;
							case 'xaxi': sNeeds[n].theKind=kISpElementKind_Axis; break;
							case 'yaxi': sNeeds[n].theKind=kISpElementKind_Axis; break;
							default: sNeeds[n].theKind=kISpElementKind_Button; break;
						}
						
						sNeeds[n].iconSuiteResourceId=0;
						sNeeds[n].playerNum=0;
						sNeeds[n].group=0;
						sNeeds[n].flags=0;
						sNeeds[n].reserved1=0;
						sNeeds[n].reserved2=0;
						sNeeds[n].reserved3=0;
					}
				}
				else
				{
					UTBException::ThrowCouldNotLoadInput();
				}
			}
			else
				UTBException::ThrowCouldNotLoadInput();

			OSStatus	err;
			
			err=::ISpElement_NewVirtualFromNeeds(sNumOfElements,sNeeds,sElements,0);			
			if (err)
				UTBException::ThrowCouldNotInitialiseInput();

			{
				UResFileSaver	saver(CApplication::sApplication->GetAppResFile());
				
				err=::ISpInit(sNumOfElements,sNeeds,sElements,inCreator,1,0,128,0);
				if (err)
					UTBException::ThrowCouldNotInitialiseInput();
			}		
		}
		Catch_(err)
		{
			sInputValid=true;
			sInputActive=true;
			SuspendInput();
			DestroyInput();
			if (ErrCode_(err)==kTBErr_NoInputElementsDefined)
				throw;
			UTBException::ThrowCouldNotInitialiseInput();
		}

		sInputValid=true;
		sInputActive=true;
		
		SuspendInput();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//			¥ Poll											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
/*
	Polls the input of a simple state element. Returns 0 if the element is out of range.
	Index is zero based.

	For a button it returns:
	kISpButtonUp				= 0,
	kISpButtonDown				= 1

	For a dpad it returns:
	kISpPadIdle					= 0,
	kISpPadLeft					= 1,
	kISpPadUpLeft				= 2,
	kISpPadUp					= 3,
	kISpPadUpRight				= 4,
	kISpPadRight				= 5,
	kISpPadDownRight			= 6,
	kISpPadDown					= 7,
	kISpPadDownLeft				= 8

	For an axis it returns in the range:
	#define	kISpAxisMinimum  -100
	#define	kISpAxisMiddle   0
	#define	kISpAxisMaximum  100
*/
SInt32 CInputManager::Poll(
	UInt16		 inIndex)
{
	if (sUseSprockets)
	{
		if (sInputActive)
		{
			if ((inIndex<sNumOfElements) && (inIndex>=0))
			{
				UInt32		input;
					
				OSStatus	err=::ISpElement_GetSimpleState(sElements[inIndex],&input);
			
				if (err)
				{
					ECHO("Failed to poll input(" << inIndex << "), err code :" << err);
					UTBException::ThrowCouldNotPollInput();
				}
			
				switch (sNeeds[inIndex].theKind)
				{
					case kISpElementKind_Button:
					case kISpElementKind_DPad:
						return input;
						break;
					
					case kISpElementKind_Axis:
						input/=(0xFFFFFFFF/200);
						
						return input-100;
						break;
					
					// I think these are all complex state elements and can't be handled by this
					case kISpElementKind_Delta:
					case kISpElementKind_Movement:
					case kISpElementKind_Virtual:
					default:
						return 0;
						break;
				}
			}
		}
	}

	return 0;
}

void CInputManager::Edit()
{
	if (sUseSprockets)
	{
		if (sInputValid)
		{
			UPortSaver		safe;		
			
			if (!sInputActive)
			{
				::ISpResume();
			
				::ISpDevices_ActivateClass(kISpDeviceClass_Mouse);
				::ISpDevices_ActivateClass(kISpDeviceClass_Keyboard);
			}
				
			OSStatus	err=::ISpConfigure(NULL);	
			
			if (err)
				UTBException::ThrowCouldNotConfigureInput();
			
			if (!sInputActive)
			{
				::ISpDevices_DeactivateClass(kISpDeviceClass_Mouse);
				::ISpDevices_DeactivateClass(kISpDeviceClass_Keyboard);
			
				::ISpSuspend();
			}
		}
	}
}

void CInputManager::SuspendInput()
{
	if (sUseSprockets)
	{
		if (sInputActive)
		{
			::ISpDevices_DeactivateClass(kISpDeviceClass_Mouse);
			::ISpDevices_DeactivateClass(kISpDeviceClass_Keyboard);
		
			::ISpSuspend();
			
			sInputActive=false;
		}
	}
}

void CInputManager::ResumeInput()
{
	if (sUseSprockets)
	{
		if ((sInputValid) && (!sInputActive))
		{
			// Build the sElements for polling
			::ISpResume();
			
			// Activate the keyboard and mouse
			::ISpDevices_ActivateClass(kISpDeviceClass_Mouse);
			::ISpDevices_ActivateClass(kISpDeviceClass_Keyboard);

			sInputActive=true;
		}
	}
}

void CInputManager::GamePaused()
{
	if (sUseSprockets)
	{
		sPausedState=sInputActive;
		
		if ((sInputValid) && (sInputActive))
		{
			// Deactivate the keyboard and mouse
			::ISpDevices_DeactivateClass(kISpDeviceClass_Mouse);
			::ISpDevices_DeactivateClass(kISpDeviceClass_Keyboard);
		
			::ISpSuspend();
		
			sInputActive=false;
		}
	}
}

void CInputManager::GameResumed()
{
	if (sUseSprockets)
	{
		if ((sInputValid) && (sPausedState))
		{
			::ISpResume();
		
			// Activate the keyboard and mouse
			::ISpDevices_ActivateClass(kISpDeviceClass_Mouse);
			::ISpDevices_ActivateClass(kISpDeviceClass_Keyboard);
		
			sInputActive=sPausedState;
		}
	}
}

void CInputManager::DestroyInput()
{
	if (sUseSprockets)
	{
		if (sInputValid)
		{
			delete [] sElements;
			delete [] sNeeds;

			sNumOfElements=0;
			sElements=0L;
			sNeeds=0L;

			sInputActive=false;
			sInputValid=false;
			::ISpStop();
		}
	}
}

#endif