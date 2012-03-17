// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Password.c
// Mark Tully
// 28/1/96
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
#define		kGhostChar	''

// Can't remember who wrote this but I use it anyway (with slight modifications to stop flick'a) MT
// Modifications to PasswordKey by Mark Tully


// Bloke who wrote it sez:
// This function is the meat which does the password field magic.
//
// What is passed into this function are: a TEHandle to the visible TERec
// that shows the user the bullets, the character that was pressed, and 
// the password as it was before the key was pressed.  Upon return, the
// password will have been modified to reflect the key that was pressed 
// and the TERec passed in will also have been modified to have the proper
// number of bullets as well as have its selection set correctly.  This
// allows you to have selections in a password field rather than the
// super-lame "backspace deletes the whole string" behavior.
//
// The reason that this creates/destroys the hidden TERec on the fly is
// because I thought it was rather dumb to have the actual item in there
// for enterprising hackers to find & it takes just as much code to move
// the hidden thing offscreen at run-time as it does to create/destroy it.
// If you leave it in the DITL & just move it around, there is always the
// chance that the enterprising hacker will find it & find a way to make
// it visible.  I'm just way too anal I guess.  ;-)

PasswordKey(TEHandle teHndl, char theKey, StringPtr password)
{
   short    start, end, len;
   Rect     hiddenRect;
   Handle      txtHndl;
   TEHandle tmpTE;
   Str255      tmpStr;
   GrafPtr     currPort;
   
   GetPort(&currPort);
   
   // Create a temporary TERec offscreen
   
   hiddenRect = (**teHndl).viewRect;
   OffsetRect(&hiddenRect, currPort->portRect.right+5,currPort->portRect.bottom+5);
   tmpTE = TENew(&hiddenRect, &hiddenRect);
   if(!tmpTE)
      return;
   
   // Set the current password into the hidden TERec
   
   TESetText(&password[1], password[0], tmpTE);
   
   // Set the selection range of our hidden TERec to that
   // of the visible password TERec

   TESetSelect((**teHndl).selStart, (**teHndl).selEnd, tmpTE);
   
   // Process the key in the hidden TERec
   if (theKey!=kForDelChar) // don't let these go in as TE doen't do a delete, it just put in a char
 	  TEKey(theKey, tmpTE);
   
   // Grab the new password out of the hidden TERec
   
   password[0] = (**tmpTE).teLength;
   BlockMove(*((**tmpTE).hText), &password[1], password[0]);
   
   // Create a string of bullets to match the real password
   
   tmpStr[0] = password[0];
   for(len=1;len<=tmpStr[0];len++)
      tmpStr[len] = kGhostChar;

   // Deactivate the visible TERec so no phantom cursors pop up.
   
   TEDeactivate(teHndl);

   // Dump the bullets into the visible TERec
   // This is a better method than was first used by the person who I got the routine off. MT
   
   // His
   // TESetText(&tmpStr[1], tmpStr[0], teHndl); 
   
   // Mine (no flicker)
	// if it was a special which doesn't print on the screen let TE do it's stuff
   	if (theKey==kDownChar || theKey==kUpChar || theKey==kLeftChar || theKey==kRightChar
   			|| theKey==kDeleteChar)
   		TEKey(theKey,teHndl);
   	else if (theKey==kForDelChar)
   		; // nobody gets forward del at the moment as TE doesn't support it yet
   	else
   		TEKey(kGhostChar,teHndl); // otherwise ghost it
   		
   // Set the selection to match that of the hidden TERec
   
   TESetSelect((**tmpTE).selStart, (**tmpTE).selEnd, teHndl);
   
   // Dispose of the hidden TERec
   
   TEDispose(tmpTE);

   // We need to call InvalRect if the visible TEField is empty because
   // of some wierdness I didn't bother to figure out with TextEdit.
   // In all other cases, we just need to update the visible TERec.
   
   if(!tmpStr[0])
      InvalRect(&((**teHndl).viewRect));
      
  // This is not longer need as I modified the way it set the text MT
  // else
  //    TEUpdate(&((**teHndl).viewRect), teHndl);

   // Turn the visible TERec back on.
   
   TEActivate(teHndl);
}

// Mark Tully
// 28/1/96
// High level password requesting routines that works without any resources
// Give it the address of a string to store the password in
Boolean GetPassword(StringPtr password,StringPtr prompt)
{
	DialogPtr		theDialog;
	Rect			dialogFrame={0,0,76,300};
	Rect			passRect={50,10,66,290};
	Rect			messRect={6,10,40,290};
	Rect			passFrame=passRect;
	TEHandle		thePass;
	GrafPtr			savePort;
	EventRecord		theEvent;
	Boolean			done=false;
	Boolean			res=true;
	
	password[0]=0; // clear the string
	
	CenterRectInRect(&dialogFrame,&qd.screenBits.bounds);
	theDialog=NewDialog(0,&dialogFrame,"\p",true,noGrowDocProc,(WindowPtr)-1,true,0L,0L);
	if (!theDialog)
		return false;
	
	GetPort(&savePort);
	SetPort(theDialog);
	
	thePass = TENew(&passRect, &passRect);
	if (!thePass)
	{
		DisposeDialog(theDialog);
		return false;
	}
		
	InsetRect(&passFrame,-2,-2);
	TEActivate(thePass);
	TEAutoView(true, thePass);
		
	while(!done)
	{
		WaitNextEvent(everyEvent, &theEvent, 10L, 0L);
		TEIdle(thePass);
		
		switch(theEvent.what)
		{
			case mouseDown:
				{
					WindowPtr	win;
					
					switch (FindWindow(theEvent.where, &win))
					{
						case inDrag:
							DragWindow(win,theEvent.where,&qd.screenBits.bounds);
							break;
					
						case inContent:
							{
								Point	thePoint=theEvent.where;
								
								GlobalToLocal(&thePoint);
								TEClick(thePoint,(theEvent.modifiers & 0x0200) >> 8,thePass);
								// is the shift key held down
								break;
							}
					
						case inGoAway:
							if (TrackGoAway(win, theEvent.where))
							{
								password[0]=0; // clear the string
								res=false; // user cancelled
								done=true;
							}
							break;
					}
				}
				break;
				
			case keyDown:
			case autoKey:
				{
					char	theChar=theEvent.message & charCodeMask;
					
					if ((theChar==kReturnChar) || (theChar==kEnterChar))
						done=true;
					else
						PasswordKey(thePass, theChar, password);
				}
				break;
		
			case updateEvt:
				{				
					if ((WindowPtr)theEvent.message==theDialog)
					{
						GrafPtr		mooPort;
						
						GetPort(&mooPort);
						SetPort(theDialog);
												
						BeginUpdate(theDialog);
							EraseRect(&((**thePass).viewRect));
							TEUpdate(&((**thePass).viewRect), thePass);
							FrameRect(&passFrame);
							TETextBox(&prompt[1], prompt[0], &messRect, 0);
						EndUpdate(theDialog);
					
						SetPort(mooPort);
					}
				}
				break;
		}
	}
	
	// Display password proc
	//TESetText(&password[1], password[0], thePass);
	//TEUpdate(&((**thePass).viewRect), thePass);
	//while(!Button());
	//FlushEvents(everyEvent,0);
	
	TEDispose(thePass);
	DisposeDialog(theDialog);
	SetPort(savePort);
	
	return res;
}
