// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// SE Patch Skeloton.c
// Mark Tully
// 3/4/96
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

// This is a simple skeleton to base 68K (not test with native patches) SytemEvent() patches on
// Compile to a SMALL CODE RESOURCE with type INIT INIT -4048. The resource should be LOCKED and
// marked SYSTEM HEAP.

#include	"Marks Routines.h"

// Mixed mode crap
#ifndef powerc // if 68K
	#include <A4Stuff.h>
#else // if powerPC
	// for Metrowerks' linker, this defines the interface for main().
	ProcInfoType __procinfo = kCStackBased | RESULT_SIZE(kNoByteCode);

	#define SetCurrentA4() 0
	#define SetA4(x) 0
#endif

pascal short ReplacementSystemEvent(EventRecord *theEvent);
#define kSystemEventTrapNumber 0xA9B2
#define	uppSystemEventProcInfo  kPascalStackBased | RESULT_SIZE(kTwoByteCode) | STACK_ROUTINE_PARAMETER(1, kFourByteCode)

// globals
pascal short (*gOldSystemEvent)(EventRecord *theEvent);
pascal short NewSystemEvent(EventRecord *theEvent);

#define NiceReturn()		SetZone(oldZone); SetA4(oldA4); return

main()
{
		UniversalProcPtr	newSystemEventAddress;
	long				oldA4;
	THz					oldZone;
		
	oldA4 = SetCurrentA4(); // Set up A4, so we can access our globals.
	oldZone = GetZone();
	SetZone(SystemZone());
	
	// deatach out code resource so that we stay around after code exits
	DetachResource(GetResource('INIT',-4048));
	
	// Remember the old implementation of SystemEvent
	gOldSystemEvent = (void *) GetToolTrapAddress(kSystemEventTrapNumber);
	
	// Patch outselves over that
	newSystemEventAddress = NewRoutineDescriptor((ProcPtr) &ReplacementSystemEvent,uppSystemEventProcInfo,GetCurrentISA());
	SetToolTrapAddress(newSystemEventAddress, kSystemEventTrapNumber);
	
	NiceReturn();
}

pascal short ReplacementSystemEvent(EventRecord *theEvent)
{
	// Set up A4, so we can access our globals.
	long	oldA4;
	short	result;
	Boolean	done=false;

	oldA4 = SetCurrentA4();

	// Insert custom code here
	
	if (!done) // If we haven't handled this event then pass it to the real SystemEvent
	{
		// Call the old SystemEvent:
		#ifndef powerc
			result = gOldSystemEvent(theEvent);
		#else
			result = CallUniversalProc((UniversalProcPtr)gOldSystemEvent, uppSystemEventProcInfo, theEvent);
		#endif
	}
	else
		result=true; // tell it the event has been handled by the system

	// And restore the value of A4 on the way out.
	SetA4(oldA4);

	return result;
}












