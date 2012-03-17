// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Messages.c
// Mark Tully
// 17/2/96
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

#include	"Marks Routines.h"

#define		timeOut	120L // 2 sec to read mess

DialogPtr	gDLog=0L;
Rect		gDLogRect={0,0,38,332},gMessRect={12,2,28,330};

OpenMessBox()
{
	CenterRectInRect(&gDLogRect,&qd.screenBits.bounds);
	
	gDLog=NewDialog(0L,&gDLogRect,"\p",false,dBoxProc,(WindowPtr)-1,false,'MESS',0L);
}

DoMessage(StringPtr mess)
{
	GrafPtr		savePort;
	EventRecord	theEvent;
	long		until;
	
	if (!gDLog)
	{
		OpenMessBox();
		if (!gDLog)
			SysBeep(0);
	}

	GetPort(&savePort);
	SetPort(gDLog);
	
	ShowWindow(gDLog);
	
	TETextBox(&mess[1],mess[0],&gMessRect,teJustCenter);
	
	until=LMGetTicks()+timeOut;
	
	do
	{
		WaitNextEvent(everyEvent,&theEvent,20L,0L);
		if (theEvent.what==updateEvt)
		{
			if ((WindowPtr)theEvent.message==gDLog)
			{
				BeginUpdate(gDLog);
				EndUpdate(gDLog);
			}
		}
	}while(theEvent.what!=mouseDown && theEvent.what!=keyDown && theEvent.what!=autoKey && until>LMGetTicks());

	HideWindow(gDLog);
	
	SetPort(savePort);
}

CloseMessBox()
{
	DisposeDialog(gDLog);
}