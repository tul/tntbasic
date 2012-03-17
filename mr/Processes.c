// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Processes.c
// Mark Tully
// ??/??/95
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1995, Mark Tully and John Treece-Birch
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

#include	"Marks Routines.h"
#define		timeOut		120L // time out in 60th sec

long GetProcessCreator(ProcessSerialNumber *process)
{
	ProcessInfoRec		poo;
	OSErr				err;
	
	poo.processInfoLength=sizeof(ProcessInfoRec);
	poo.processName=0L; // Don't care
	poo.processAppSpec=0L; // Don't care
	err=GetProcessInformation(process,&poo);
	
	if(err!=noErr) // error
		return(0L);
	else
		return(poo.processSignature);
}

Boolean	ProcessExists(ProcessSerialNumber *process)
{
	ProcessInfoRec		poo;
	OSErr				err;
	
	poo.processInfoLength=sizeof(ProcessInfoRec);
	poo.processName=0L; // Don't care
	poo.processAppSpec=0L; // Don't care
	err=GetProcessInformation(process,&poo);
	
	if(err!=noErr) // error
		return false;
	else
		return true;
}

// pass in the process signiture to switch to
Boolean SwitchToProcess(long process,Boolean wait)
{
	ProcessSerialNumber	testProcess,tempProcess;
	ProcessInfoRecPtr	info;
	ProcessInfoRec		poo;
	Boolean				done=false;
	EventRecord			theEvent;
	long				startProcess;
	OSErr				err;
	unsigned long		until;
	
	info=&poo;
	
	// First, get the current process so we know when we've done a complete circle...
	GetCurrentProcess(&testProcess);
	info->processInfoLength=sizeof(ProcessInfoRec);
	info->processName=0L; // Don't care
	info->processAppSpec=0L; // Don't care
	err=GetProcessInformation(&testProcess,info);
	
	if(err!=noErr) // error
		return(false);
	
	startProcess=info->processSignature;
	
	//err=GetNextProcess(&testProcess); // skip our process
	//if(err!=noErr) // error
	//	return(false);	
		
	while (!done)
	{
		err=GetNextProcess(&testProcess);
		if(err!=noErr) // error
		{
			if (err==-600) // ours was last in list
			{
				// as it failed the PSN is set to 0 meaning start from beginning of list
				err=GetNextProcess(&testProcess);
				if(err!=noErr)
					return(false);
			}
			else
				return(false);
		}
		info->processInfoLength=sizeof(ProcessInfoRec);
		info->processName=0L; // Don't care
		info->processAppSpec=0L; // Don't care
		err=GetProcessInformation(&testProcess,info);
		if(err!=noErr) // error
			return(false);

		if (info->processSignature==process)
		{
			// found it!
			err=SetFrontProcess(&testProcess);
			if(err!=noErr) // error
				return(false);
			until=TickCount()+timeOut;
			
			if (wait)
			{
				done=false;
				while((!done) && (until>TickCount()))
				{
					err=GetFrontProcess(&tempProcess);
					if(err!=noErr) // error
						return(false);
					err=SameProcess(&testProcess,&tempProcess,&done);
					if(err!=noErr) // error
						return(false);
					WaitNextEvent(everyEvent,&theEvent,10L,0); // Give some cpu time out
				}
			
			if (until>TickCount())
				return(false); // timeout occurred, failed
			else
				return(true); // no time out occurred
			}
			else
				return true; // switch ok
		}
		else if (info->processSignature==startProcess)
		{
			// Complete circle
			return(false); // switch failed
		}
	}
	
	return false;
}

// Returns true if this process is frontmost
Boolean IsFrontProcess()
{
	OSErr					err;
	Boolean					same;
	ProcessSerialNumber		thisProcess,frontProcess;

	if (err=GetCurrentProcess(&thisProcess))
		return true;	// default to true on error
	if (err=GetFrontProcess(&frontProcess))
		return true;	// default to true on error
	if (err=SameProcess(&frontProcess,&thisProcess,&same))
		return true;	// default to true on error

	return same;	
}

Boolean WaitForFrontProcess(long process,unsigned long waitFor)
{
	ProcessSerialNumber		current,tempProcess;
	unsigned long			until;
	ProcessInfoRec			info;
	OSErr					err;
	EventRecord				theEvent;
	
	GetCurrentProcess(&current);
	
	until=TickCount()+waitFor;
	
	while(until>TickCount())
	{
		if(GetFrontProcess(&tempProcess))
			return false;
		
		info.processInfoLength=sizeof(ProcessInfoRec);
		info.processName=0L; // Don't care
		info.processAppSpec=0L; // Don't care
		if(GetProcessInformation(&tempProcess,&info))
			return false;
			
		if (info.processSignature==process) // done
			return true;
		
		WaitNextEvent(everyEvent,&theEvent,10L,0); // Give some cpu time out
	}
	
	return false; // timeout occurred, failed
}

Boolean WaitResumeEvent()
{
	EventRecord	theEvent;
	unsigned long	wait=timeOut+TickCount();
	
	while(!WaitNextEvent(osMask,&theEvent,0L,0L))
	{
		if (TickCount()>=wait)
			return false; // timeout occurred
	}
	
	return	true;
}
