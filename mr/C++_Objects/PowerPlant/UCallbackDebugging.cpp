// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UCallbackDebugging.cpp
// © Mark Tully and TNT Software 1999
// 22/6/99
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
	This is a special implementation of UDebugging which checks for a special proc ptr which can be used to display
	a message. If the ptr is nil (ie unset) then it does nothing.

	Changes to standard UDebugging
		1. AlertThrowAt and AlertSignalAt
		2. Add globals and accessors
		3. Changed included file from UDebugging.h to UCallbackDebugging.h
*/

#include		"UCallbackDebugging.h"

TAlertDebugCallBack		gThrowAlertCallBack=0L,gSignalAlertCallBack=0L;

void SetDebugCallBacks(
	TAlertDebugCallBack		inThrowCallBack,
	TAlertDebugCallBack		inSignalCallBack)
{
	gThrowAlertCallBack=inThrowCallBack;
	gSignalAlertCallBack=inSignalCallBack;
}

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <PP_Constants.h>
#include <Dialogs.h>

PP_Begin_Namespace_PowerPlant

	// Global variables for specifying the action to take for
	// a Throw and a Signal

EDebugAction	UDebugging::gDebugThrow  = debugAction_Nothing;
EDebugAction	UDebugging::gDebugSignal = debugAction_Nothing;


const	short	ALRT_ThrowAt	= 251;
const	short	ALRT_SignalAt	= 252;


namespace UDebugging {

	unsigned char*	LoadPStrFromCStr(
							Str255			outPStr,
							const char*		inCStr);
}


// ---------------------------------------------------------------------------
//	¥ LoadPStrFromCStr
// ---------------------------------------------------------------------------
//	Copy contents of a C string into a Pascal string
//
//	Returns a pointer to the Pascal string

unsigned char*
UDebugging::LoadPStrFromCStr(
	Str255			outPStr,
	const char*		inCStr)
{
		// Find length of C string by searching for the terminating
		// null character. However, don't bother to look past 255
		// characters, since that's all that a Pascal string can hold.
		
	unsigned char	strLength = 0;
	
	while ( (strLength < 255)  &&  (inCStr[strLength] != '\0') ) {
		strLength += 1;
	}
	
	outPStr[0] = strLength;
	::BlockMoveData(inCStr, outPStr + 1, strLength);
	
	return outPStr;
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AlertThrowAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Throw occurs

void
UDebugging::AlertThrowAt(
	ConstStringPtr		inError,
	ConstStringPtr		inFile,			// Pascal string
	long				inLine)
{
	if (gThrowAlertCallBack)
	{
		Str15	lineStr;
		::NumToString(inLine, lineStr);
		
	//	::ParamText(inError, inFile, lineStr, Str_Empty);
	//	::StopAlert(ALRT_ThrowAt, nil);
		(*gThrowAlertCallBack)(inError,inFile,lineStr);
	}
}


// ---------------------------------------------------------------------------
//	¥ AlertThrowAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Throw occurs

void
UDebugging::AlertThrowAt(
	ConstStringPtr		inError,
	const char*			inFile,			// C string
	long				inLine)
{
	Str255	fileStr;

	AlertThrowAt(inError, LoadPStrFromCStr(fileStr, inFile), inLine);
}

#pragma mark -

// ---------------------------------------------------------------------------
//	¥ AlertSignalAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalAt(
	ConstStringPtr		inTestStr,		// Pascal string
	ConstStringPtr		inFile,			// Pascal string
	long				inLine)
{
	if (gSignalAlertCallBack)
	{
		Str15	lineStr;
		::NumToString(inLine, lineStr);
		
		//::ParamText(inTestStr, inFile, lineStr, Str_Empty);
		//::StopAlert(ALRT_SignalAt, nil);
		(*gSignalAlertCallBack)(inTestStr,inFile,lineStr);
	}
}


// ---------------------------------------------------------------------------
//	¥ AlertSignalAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalAt(
	ConstStringPtr		inTestStr,		// Pascal string
	const char*			inFile,			// C string
	long				inLine)
{
	Str255	fileStr;
	
	AlertSignalAt(inTestStr,
				  LoadPStrFromCStr(fileStr, inFile),
				  inLine);
}


// ---------------------------------------------------------------------------
//	¥ AlertSignalAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalAt(
	const char*			inTestStr,		// C string
	ConstStringPtr		inFile,			// Pascal string
	long				inLine)
{
	Str255	testStr;
	
	AlertSignalAt(LoadPStrFromCStr(testStr, inTestStr),
				  inFile,
				  inLine);
}


// ---------------------------------------------------------------------------
//	¥ AlertSignalAt
// ---------------------------------------------------------------------------
//	Display an Alert Box when a Signal occurs

void
UDebugging::AlertSignalAt(
	const char*			inTestStr,		// C string
	const char*			inFile,			// C string
	long				inLine)
{
	Str255	testStr;
	Str255	fileStr;
	
	AlertSignalAt(LoadPStrFromCStr(testStr, inTestStr),
				  LoadPStrFromCStr(fileStr, inFile),
				  inLine);
}


PP_End_Namespace_PowerPlant
