// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Keys.c
// Mark Tully
// 26/11/95
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

// Tells you if a certain key is held down give it the KEYCODE NOT THE ASCII CODE
short IsPressed(unsigned short k )
// k =  any keyboard scan code, 0-127
{
	//KeyMap km;
	unsigned char km[16];

	#if TARGET_API_MAC_CARBON
		#if __GNUC__
			::GetKeys((BigEndianUInt32*)km);
		#else
			::GetKeys((long*)km);
		#endif
	#else
		GetKeys((unsigned long *)km);
	#endif
	
	return ( ( km[k>>3] >> (k & 7) ) & 1);
}

void KeyToString (long keyMessage,char string[4],short resId)
{
	// This routine should be used with the string resource from "Marks routines.rsrc" which is 
	// an edited version of str# 133 from marathon
	
	char	theChar=keyMessage&charCodeMask;
	char	keyCode=(keyMessage&keyCodeMask)>>8;
	Str255	tempString;
	
	if (theChar!=0x10)
	{
		GetIndString(tempString,resId,keyCode+1);
		string[1]=tempString[1];
		string[2]=tempString[2];
		string[3]=tempString[3];
		string[0]=tempString[0];
		if (string[0]>3)
			string[0]=3;
	}
	
	/* // un rem to return scan codes
	if (theChar!=0x10) // f key	not allowed (can't read them from event record, all the same)
	{
		NumToString((long)keyCode,(unsigned char *)string);	
	}*/
}

// Simulate the pressing of keys by using system event. (Must give it the trap address of system
// event ie the original one which you haven't patched)
void TypeString(StringPtr theString)
{
	unsigned char	count;
	
	for (count=1; count<=theString[0]; count++)
		PostEvent(keyDown,theString[count]);
}

Boolean CommandPeriod()
{
	return (IsPressed(kCommandKeySC) && IsPressed(47));
}




