// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Low level.c
// Mark Tully
// 10/1/96
// 1/5/96
// 21/5/96	:	CPUType() proc written
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

// NOTE:
// if trap > A7FF toolbox routine
// if trap <= A7FF os routine

#include	"Marks Routines.h"

// *****************************************************************************************
// NO FORCE QUIT SECTION
// THIS PART NEEDS REWRITING!!! Signed Mark Tully 1/5/96

#define		kSysErr		0xA9C9 // SysErr trap number

#ifndef powerc
	#define		uppSystemErrProcInfo	kRegisterBased | RESULT_SIZE(kNoByteCode) | REGISTER_ROUTINE_PARAMETER(kRegisterD0, kTwoByteCode)
#else
	#define		uppSystemErrProcInfo	0L
#endif

void (*gOldSysErr)(short theError); // global to store address of the old SysErr routine

typedef void (*TOldSysError)(short theError);

void NoForceQuitProc(short theError); // Don't call this direct

void SetNoForceQuit()
{
	UniversalProcPtr	newSysErr=0L;
	
	gOldSysErr = (TOldSysError) GetToolTrapAddress(kSysErr); // Get address of SysErr trap
	
	// Patch outselves over that
	newSysErr = NewRoutineDescriptor((ProcPtr)&NoForceQuitProc, uppSystemErrProcInfo, GetCurrentISA());
	if (newSysErr)
		SetToolTrapAddress(newSysErr,kSysErr);
}

// This is the routine intended to replace the SysErr routine
// Check if error is dsForcedQuit, if so return with out doing anything. If it isn't call orig
// SysErr
void NoForceQuitProc(short theError)
{
	if (theError==dsForcedQuit)
		ObscureCursor();
	//#ifndef powerc
	//	gOldSysErr(theError);
	//#else
	//	CallUniversalProc((UniversalProcPtr)gOldSysErr, uppSystemErrProcInfo, theError);
	//#endif
}

// *******************************************************************************************

// These routines can be used to get/set the contents of the 68K regs even from PPC code,
// useful for patches which need to preserve regs.

// movem.l d0-d7/a0-a6,([4,a7])
long _Get68kRegs[3] = {0x48F77FFF, 0x01610004, 0x4E754E75};

// movem.l ([4,a7]),d0-d7/a0-a6
long _Set68kRegs[3] = {0x4CF77FFF, 0x01610004, 0x4E754E75};

void Get68KRegs(S68KRegs *regs)
{
	CallUniversalProc((RoutineDescriptor *)_Get68kRegs,upp68KRegsProcInfo,regs);
}

void Set68KRegs(S68KRegs *regs)
{
	CallUniversalProc((RoutineDescriptor *)_Set68kRegs,upp68KRegsProcInfo,regs);
}

short CPUType()
{
	long		type;
	
	Gestalt(gestaltNativeCPUtype,&type);
	return type;
}