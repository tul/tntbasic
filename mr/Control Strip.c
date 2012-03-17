// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Control Strip.c
// Mark Tully
// 11/5/96
// 12/5/96
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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

#include		"Marks Routines.h"
#ifndef powerc
#include		<ControlStrip.h>
#endif

// ********* IsControlStripInstalled *********

enum
{
	csUntested,
	csNotInstalled,
	csInstalled
};

// Remembers if the CS is installed or not so we don't call gestalt every time
char		gCSInstalled=csUntested;
Boolean		gCSHidden=false;

Boolean IsControlStripInstalled()
{
	// If this routine has already been called then we can return quickly
	if (gCSInstalled==csInstalled)
		return true;
	else if (gCSInstalled==csNotInstalled)
		return false;
	
	// Now it must be tested to see if the CS is installed, gestalt will do that nicely
	else if (gCSInstalled==csUntested)
	{
		long		csAttr;
	
		if (Gestalt(gestaltControlStripAttr,&csAttr))
			return false;	// if we get an error return false
		
		// Test the return value
		if(!(csAttr&(1<<gestaltControlStripExists)))
		{
			gCSInstalled=csNotInstalled;
			return false;
		}
		else
		{
			gCSInstalled=csInstalled;
			return true;
		}
	}
	else
		return false;
}

// ********* ShowHideControlStrip *********

/*

	This code was obtained via viewing memory addresses in the debugger
	It calls SBShowHideControlStrip(false)

	clr.b		-(A7)			; 0x4227
	move.w		#$0101,d0		; 0x303C0101
	dc.w		$AAF2			; 0xAAF2
	rts							; 0x4E75
	
	This calls SBShowHideControlStrip(true)

	move.b		#$01,-(A7)		; 0x1F3C0001
	move.w		#$0101,d0		; 0x303C0101
	dc.w		$AAF2			; 0xAAF2
	rts							; 0x4E75
*/

void ShowHideControlStrip(Boolean state)
{
	if (IsControlStripInstalled())
	{
		// control strip is installed, hide it
		
		#ifdef powerc
			
			// This is the code in 68K machine language to call SBShowHideControlStrip
			short	hideCSCode[]={0x4227,0x303C,0x0101,0xAAF2,0x4E75};
			short	showCSCode[]={0x1F3C,0x0001,0x303C,0x0101,0xAAF2,0x4E75};
			
			if (state)
				CallUniversalProc((RoutineDescriptor*)showCSCode,RESULT_SIZE(kNoByteCode));
			else
				CallUniversalProc((RoutineDescriptor*)hideCSCode,RESULT_SIZE(kNoByteCode));
		
		#else
			SBShowHideControlStrip(state);
		#endif
	}
}

// ********* IsControlStripVisible ********

/*
	Is control strip visible does the following to call SBIsControlStripVisible()

	SUBQ.W		#$2,A7
	MOVEQ		#$00,D0
	DC.W		$AAF2
	MOVE.B		(A7)+,D0

*/

#ifndef powerc
	#pragma	parameter __D0 isCSVisibleCode
	pascal Boolean isCSVisibleCode()={0x554F,0x7000,0xAAF2,0x101F}; // as this is an inline no rts is needed
#else
	#define		uppIsCSVisible		kRegisterBased | RESULT_SIZE(SIZE_CODE(sizeof(Boolean))) | REGISTER_RESULT_LOCATION(kRegisterD0)
#endif

Boolean IsControlStripVisible()
{
	if (IsControlStripInstalled())
	{
		#ifdef powerc
			short	isCSVisibleCode[]={0x554F,0x7000,0xAAF2,0x101F,0x4E75};
			return CallUniversalProc((RoutineDescriptor*)isCSVisibleCode,uppIsCSVisible);
		#else
			return isCSVisibleCode();
		#endif
	}
	else
		return false;
}

// ********* HideControlStrip *********
// This routine should be called from your game. It will hide the control strip if there and will
// restore it if it was originally visible upon a call to restore control strip

void HideControlStrip()
{
	if (IsControlStripInstalled() && gCSHidden==false)
	{
		if (IsControlStripVisible())
		{
			ShowHideControlStrip(false);
			gCSHidden=true;					// say we hid it
		}
	}
}

// ********* RestoreControlStrip *********

void RestoreControlStrip()
{
	if (IsControlStripInstalled())
	{
		if (gCSHidden)	// only show it if we hid it
			ShowHideControlStrip(true);
		gCSHidden=false;
	}
}