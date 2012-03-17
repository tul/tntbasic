// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CFPrefsHelpers.h
// © Mark Tully and TNT Software 2002 -- All Rights Reserved
// 30/4/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

#include "Useful Defines.h"

bool CFPrefsGetString(
	CFStringRef			inKey,
	char				*outValue,
	Size				inBuffSize);
bool CFPrefsSetString(
	CFStringRef			inKey,
	const char			*inValue);
bool CFPrefsGetInt(
	CFStringRef			inKey,
	SInt32				&outValue);
bool CFPrefsSetInt(
	CFStringRef			inKey,
	SInt32				inValue);
bool CFPrefsGetColour(
	CFStringRef			inKey,
	RGBColour			&outColour);
bool CFPrefsSetColour(
	CFStringRef			inKey,
	const RGBColour		&inColour);
SInt32 CFPrefsGetIntArray(
	CFStringRef			inKey,
	SInt32				*outArray,
	SInt32				inMaxEle);
bool CFPrefsSetIntArray(
	CFStringRef			inKey,
	const SInt32		*inArray,
	SInt32				inArrayCount);
bool CFPrefsGetHandle(
	CFStringRef			inKey,
	Handle				&outHandle);
bool CFPrefsSetHandle(
	CFStringRef			inKey,
	Handle				inHandle);
bool CFPrefsGetBool(
	CFStringRef			inKey,
	Boolean				&outValue);
bool CFPrefsGetBool(
	CFStringRef			inKey,
	bool				&outValue);
void CFPrefsSetBool(
	CFStringRef			inKey,
	Boolean				inValue);

CFStringRef CFPrefsGetCurAppId();
bool CFPrefsFlush();