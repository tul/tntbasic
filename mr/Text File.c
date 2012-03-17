// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Text File.c
// Mark Tully
// File routines for manipulating text files
// 1/11/95
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

OSErr	gTextErr;

// Reads up to a stop char
void ReadUntil(short file,char stopChar)
{
	while((ReadChar(file)!=stopChar) && (gTextErr==noErr))
		;
}

// Gets string between current file pos and stop char
void ReadPString(short file,StringPtr theString,char stopChar)
{
	short io=256;
	
	ReadIntoBuffer(file,(Ptr)theString,stopChar,&io);
	CStrToPStr(theString);
}

// Read into buffer
void ReadIntoBuffer(short file,Ptr here,char stopChar,short *length)
{
	short	len=0;
		
	do
	{
		here[len]=ReadChar(file); // Read one char
		len++; // increase len
	}
	while((here[len-1]!=stopChar) && (gTextErr==noErr) && (len<*length));

	*length=len-2; // set length
	here[len-1]=0; // nullify buffer and overwrite stop char
}

// Reads a  single char from a file
char ReadChar(short file)
{
	long	length=1L;
	char	theChar;
	
	gTextErr=FSRead(file,&length,&theChar);		
	
	return theChar;
}

void WriteChar(short file,char theChar)
{
	long	length=1L;
	
	gTextErr=FSWrite(file,&length,&theChar);			
}

void WritePString(short file,StringPtr theString)
{
	long	io=theString[0];
	
	gTextErr=FSWrite(file,&io,&theString[1]);
}

OSErr GetTextFileErr()
{
	return gTextErr;
}

void SetTextFileErr(OSErr err)
{
	gTextErr=err;
}	
