/***************************************************************************************************

BLAST VBL.c
Extension of BLASTª engine
Mark Tully
© TNT Software 1996
All right reserved
26/4/96
17/5/96	:	Made the 68K version work and locked the PPC versions memory.
18/5/96	:	Removed the installation of the evil self modifiing code from the 68K version and so
			stoped the jumping from the sysheap into the app heap code. I don't know why GURUS
			said that this had to be done as it didn't explain it. No other source I can find
			anywhere does it and it crashes half the time if you do do it. So why do it?

This provides functions for installing and communicating with VBL tasks.
NB: Procedures preceeded by BL_ are internal and should not be called from your game.

SECTION 1 - PPC 60Hz Interrupt task
SECTION 2 - 68K 60Hz Interrupt task

***************************************************************************************************/

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

#include	"BLAST.h"
#include	<Retrace.h>

// ********* SECTION 1 - PPC 60Hz Interrupt task *********

#ifdef	powerc

// Installs a 60Hz Interrupt task on PPC
// Based on Pg 352 of Tricks
OSErr Install60HzTask(ExVBLTaskPtr *vblRec,void *refCon,short vblCount,ProcPtr theTask)
{
	OSErr		theErr=noErr;

	// Allocate the task record in the system heap so that it can't get switched out of ram
	*vblRec=(ExVBLTaskPtr)NewPtrSysClear((long)sizeof(ExtendedVBLTask));
	if(*vblRec)
	{
		if (IsVMOn())
			LockMemory((Ptr)*vblRec,(long)sizeof(ExtendedVBLTask));
		
		(**vblRec).refCon=refCon;
		(**vblRec).vblRec.vblAddr=NewVBLProc(PreProcess60HzTask); // resets the counter then call the custom routine
		(**vblRec).vblRec.vblCount=vblCount;
		(**vblRec).vblRec.qType=vType;
		//(**vblRec).vblRec.qLink=0L;
		(**vblRec).vblRec.vblPhase=0;
		// Stick the address of the custom routine in a slot after the vbl task
		(**vblRec).customProc=(void(*)(ExtendedVBLTask*))theTask;
		// Stick the frequency after that so it can be reset each vbl
		(**vblRec).masterVblCount=vblCount;
		
		theErr=VInstall((QElemPtr)*vblRec);
		if (theErr)
		{
			if((**vblRec).vblRec.vblAddr)
				DisposeRoutineDescriptor((VBLUPP)(**vblRec).vblRec.vblAddr);
			if (IsVMOn())
				UnlockMemory((Ptr)*vblRec,(long)sizeof(ExtendedVBLTask));
			DisposePtr((Ptr)*vblRec);
			*vblRec=0L;
		}
	}
	else
		theErr=memFullErr;

	return theErr;
}

void Remove60HzTask(ExVBLTaskPtr *vblRec)
{
	if (*vblRec)
	{
		VRemove((QElemPtr)*vblRec);
		if ((**vblRec).vblRec.vblAddr)
			DisposeRoutineDescriptor((VBLUPP)(**vblRec).vblRec.vblAddr);
		
		if (IsVMOn())
			UnlockMemory((Ptr)*vblRec,(long)sizeof(ExtendedVBLTask));
		DisposePtr((Ptr)*vblRec);
		*vblRec=0L;
	}
}

pascal void PreProcess60HzTask(ExVBLTaskPtr vblRec)
{	
	vblRec->customProc(vblRec);
	
	// Gets the original vbl count from the special place we hid it after the vbl record
	// NB It is being done afterward so that the custom routine can change the vbl rate by
	// modifying the correct field. It can also change the custom procedure if it wishes
	// It should change masterVblCount and customProc, it should NOT CHANGE ANYTHING IN THE
	// VBLREC PART OF THE EXTENDEDVBLTASK.
	vblRec->vblRec.vblCount=vblRec->masterVblCount;
}

#endif

// ********* SECTION 2 - 68K 60Hz Interrupt task *********

#ifndef powerc

// Based on Pg 350 of Tricks
// Evil selfmodifing - hex written - cache flushing - not as good or easy or fast as the powerpc
// version - 68K pile of code.

// The basics are the smae as the PPC version except a chunk of code is created in the system heap
// which contains instuctions which then call our real code. I'm sure there's a very good reason for
// this but GURUs doesn't expand on it.

OSErr Install60HzTask(ExVBLTaskPtr *vblRec,void *refCon,short vblCount,ProcPtr theTask)
{
	OSErr	theErr;
	long	*taskPtr; 	// a pointer in the app heap which has a space for the storing of the apps A5
						// address at the front. It is followed by an extended Vbl struct.
	struct Jump
	{
		short		bsr_Indirect;
		void		*address;
		long		movea;
		short		jmp_a1;
	};
	
	struct Jump *headJumpPtr;	// This pointer is made to point to some code in the system heap
								// which jumps to the code in the app heap
	
	/* This is the code which will be built into the system heap
		0x6104			bsr.s		6(pc)
		0x0000,0x0000	dc.l		0
		0x225F			movea.l		(a7)+,a1
		0x2251			movea.l		(a1),a1
		0x4ED1			jmp			(a1)
	*/

	// Set up the vbl task
	// Allocate space for the storing of the BLASTª exteneded VBL struct
	taskPtr=(long *)NewPtrSysClear((long)sizeof(ExtendedVBLTask));
	// Allocate space in the system heap for some code to call the Preprocess task
	headJumpPtr=(struct Jump *)NewPtrSys((long)sizeof(struct Jump));
	
	if (taskPtr && headJumpPtr)
	{	
		if (IsVMOn())
		{
			LockMemory(taskPtr,(long)sizeof(ExtendedVBLTask));
			LockMemory(headJumpPtr,(long)sizeof(struct Jump));
		}
		
		// Make the extended task ptr point to the ExtendedVBLTask record
		(*vblRec)=(ExVBLTaskPtr)taskPtr;
		// Now write a bit of assembler into the System heap which gets our address and jumps to it
		(**vblRec).refCon=refCon;
		headJumpPtr->bsr_Indirect=0x6104;
		headJumpPtr->address=PreProcess60HzTask;
		headJumpPtr->movea=0x225F2251L;
		headJumpPtr->jmp_a1=0x4ED1;
		
		(**vblRec).vblRec.vblAddr=(VBLUPP)PreProcess60HzTask;//headJumpPtr
		(**vblRec).vblRec.vblCount=vblCount;
		(**vblRec).vblRec.qType=vType;
		(**vblRec).vblRec.qLink=0L;
		(**vblRec).vblRec.vblPhase=0;
		(**vblRec).masterVblCount=vblCount;
		(**vblRec).customProc=(void(*)(ExtendedVBLTask*))theTask;
		(**vblRec).appA5=SetCurrentA5();
		
		// Now that that is all set up the cache must be flushed because of the evil self modifing code
		if (LMGetCPUFlag()>=2)
		{
			FlushInstructionCache();
			FlushDataCache();
		}
		theErr=VInstall((QElemPtr)*vblRec);
	}
	else
	{
		if (headJumpPtr)
		{
			if (IsVMOn())
				UnlockMemory((Ptr)headJumpPtr,(long)sizeof(struct Jump));
			DisposePtr((Ptr)headJumpPtr);
		}
		if (taskPtr)
		{
			if(IsVMOn())
				UnlockMemory(taskPtr,(long)sizeof(ExtendedVBLTask)+sizeof(long));
			DisposePtr((Ptr)taskPtr);
		}
		*vblRec=0L;
		theErr=memFullErr;
	}
	return theErr;
}

Remove60HzTask(ExVBLTaskPtr *vblRec)
{
	if (*vblRec)
	{
		VRemove((QElemPtr)*vblRec);
		if ((**vblRec).vblRec.vblAddr)
			DisposePtr((Ptr)(**vblRec).vblRec.vblAddr);
		DisposePtr((Ptr)(*vblRec));
		*vblRec=0L;
	}
}

// A0 points to the vbl rec for this task on entry
// This pragma makes the proc get its pointer from A0 instead of from the stack (which is bad)
#pragma parameter PreProcess60HzTask(__A0)

static pascal PreProcess60HzTask(ExVBLTaskPtr vblRec) //ExVBLTaskPtr vblRec
{
	long			saveA5;
	
	saveA5=SetA5(vblRec->appA5); // give us our globals!
	
	vblRec->customProc(vblRec); // call the custom routine
	
	vblRec->vblRec.vblCount=vblRec->masterVblCount;
	SetA5(saveA5);
}

#endif