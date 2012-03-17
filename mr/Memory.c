// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Memory.c
// Mark Tully
// 28/11/95
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

#include 	"Marks Routines.h"

#define		high	true
#define		low		false

// Search for memory of length MatchLen inside the Ptrs start and end and return the address
Ptr FindHex(Ptr match,long matchLen,Ptr start,Ptr end)
{
	long 	matched=0L;
	Ptr		pos;
	
	for (pos=start; pos<=end; pos++)
	{
		if (*pos==*(match+matched)) // found a match, increase matched count
			matched++;
		else
			matched=0L;
			
		if (matched==matchLen)
			return pos-matched+1;
	}
	return 0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BinToHex
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Converts binary data into a ascii hex string
// Length to do is the number of bytes of binary data to translate
// Note that the hex buffer must be at least twice the size of length to do
void BinToHex(
	void			*inBinaryBuffer,
	void			*outHexBuffer,
	Size			inLengthToDo)
{
	signed long		counter;

//	for (counter=0,hexPosition=0; counter<lengthToDo; counter++,hexPosition+=2)
//		CharToHexPair(((unsigned char*)binaryData)[counter],&(((unsigned char*)hexBuffer)[hexPosition]));

	// Change to looping backwards so that the same buffer can be used for input and output without the operations writing
	// overitself
	for (counter=inLengthToDo-1; counter>=0; counter--)
		CharToHexPair(((unsigned char*)inBinaryBuffer)[counter],&(((unsigned char*)outHexBuffer)[counter*2]));
}

// Converts a string of ascii hex digits into binary data
// Length is the number to translate and recieves translated
void StringToHex(Ptr theString,Ptr destMem,short *length)
{
	short	pos;
	char 	upper,lower,start=0;
	
	if ((*length)&2) // if theres an odd number of digits the first one needs putting into the
	{				// lower first nibble
		start=1; // now start on the second byte
		
		lower=*(theString); // get byte
		if (!CharToHex(&lower))
		{
			*length=0; // failed completly at the first byte
			return;
		}
		
		destMem[0]=lower; // first byte done	
	}	
		
	for (pos=start; pos< *length; pos+=2)
	{
		upper=*(pos+theString);
		lower=*(pos+1+theString);
		
		if (!CharToHex(&upper) || !CharToHex(&lower)) // convert both values into hex
			break;
		
		upper<<=4; // shift the upper value 4 bits left to make it into the upper nibble
		
		// merge two values into the destination
		destMem[pos/2+start] = upper|lower;
	}
	
	(*length)=pos/2+start;
	
}

Boolean CharToHex(char *temp)
{
		if(((*temp)>='a') && ((*temp)<='z')) // if it's a little letter
			(*temp)-=32; // make it a cap
		
		if (((*temp)>'F') || (((*temp)<'A') && ((*temp)>'9')) || ((*temp)<'0')) // invalid hex digit			
			return false;	
			
		if ((*temp)>='A' && (*temp)<='F') // letter
			(*temp)=(*temp)-'A'+10;
		else
			(*temp)=(*temp)-'0';
			
		return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FilterChars
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Removes all chars which match inStrip from the handle and then shrinks the handle the respective number of bytes
void FilterChars(
	char			inChar,
	Handle			ioHandle)
{
	Size			readOffset=0,writeOffset=0;
	Size			size=GetHandleSize(ioHandle);
	
	for (readOffset=0; readOffset<size; readOffset++)
	{
		if ((*ioHandle)[readOffset]==inChar)
		{
			readOffset++;
			continue;
		}
		
		(*ioHandle)[writeOffset++]=(*ioHandle)[readOffset];
	}
	
	// shrink the handle now
	SetHandleSize(ioHandle,writeOffset);
}

// sizes a handle and inits the new memory
OSErr BetterSetHandleSize(Handle theHandle,Size newSize,char fillByte)
{
	Size		wasSize=GetHandleSize(theHandle);
	
	if (wasSize!=newSize)
	{		
		SetHandleSize(theHandle,newSize);
		if (GetHandleSize(theHandle)==newSize)
		{	
			// fill the new memory with fillByte
			if (newSize>wasSize)
				FillMem((*theHandle)+wasSize,(*theHandle)+newSize,fillByte);
		}
		else
		{
			OSErr	theErr=MemError();
			
			if (!theErr)
				theErr=memFullErr;
			return theErr;
		}
	}
	
	return noErr;
}

OSErr ResizeHandleWithInsert(Handle handle,unsigned long offset,Size wasSize,Size newSize)
{
	if (newSize!=wasSize)
	{
		OSErr				theErr;
		Size				oldHandleSize=GetHandleSize(handle);
		signed short		amountToGrowBy=newSize-wasSize;

		if (amountToGrowBy<0)
		{
			// move the data from the end to fill the new gap, then shrink the handle
			BlockMoveData((*handle)+wasSize+offset,(*handle)+newSize+offset,oldHandleSize-offset-wasSize);
		}
		
		theErr=BetterSetHandleSize(handle,oldHandleSize+amountToGrowBy,0);
		if (theErr)
			return theErr;
		
		if (amountToGrowBy>0)
		{
			// move the data from the middle to the right
			BlockMoveData((*handle)+wasSize+offset,(*handle)+newSize+offset,oldHandleSize-offset-wasSize);
		}
	}
	
	return noErr;
}

void FillMem(Ptr start,Ptr finish,char theFiller)
{
	while (start<finish)
		*(start++)=theFiller;
}

// fills mem with pascal string
void FillMemString(Ptr start,Ptr finish,StringPtr theFiller)
{
	short		counterA=1;

	while (start<=finish)
	{
		*(start++)=theFiller[counterA++];
		if (counterA>theFiller[0])
			counterA=1;
	}
}

Boolean IsVMOn()
 {
	long	feature;
	
	feature = 0;
	if (Gestalt(gestaltVMAttr, &feature) == noErr)
	{
		if (feature & (1<<gestaltVMPresent))
			return true;
	}
	return false;
}

OSErr BetterMemError()
{
	OSErr	err=MemError();
	if (err)
		return err;
	else
		return memFullErr;
}
