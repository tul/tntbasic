// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CKeyQueue.cpp
// © Mark Tully 2003
// 20/9/03
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2003, Mark Tully and John Treece-Birch
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

/*
	A simple queue class for keeping a queue of key presses. The presses recorded as SKeyboardEvents, which
	can be created from either TNT Basic rawkeycodes (comprising of scancode, charcode and modifiers) or
	just scancode and modifiers.
	
	mQEnd should point to the place to store the next key press
	mQStart should point to the first key to pop from the queue
	if mQStart==mQEnd then then q is empty
*/

#include "CKeyQueue.h"
#include "Useful Defines.h"

UInt32 CKeyQueue::sKeyMapState=0;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EventAlloc												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// places a SKeyboardEvent entry into the queue and returns it for filling out
CKeyQueue::SKeyboardEvent &CKeyQueue::EventAlloc()
{
	AssertThrow_(mQEnd>=0 && mQEnd<kMaxKeysInQueue);
	
	SKeyboardEvent	&key=mQ[mQEnd];

	mQEnd++;
	if (mQEnd>=kMaxKeysInQueue)
		mQEnd=0;

	if (mQEnd==mQStart)
	{
		// drop key from q
		mQStart++;
		if (mQStart>=kMaxKeysInQueue)
			mQStart=0;

		AssertThrow_(mQStart>=0 && mQStart<kMaxKeysInQueue);
	}	
	
	return key;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ QueueKeyByScancode										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// enters a key into the queue. this function should be used if you don't know the charcode of the key
void CKeyQueue::QueueKeyByScancode(
	UInt32		inScancode,
	UInt32		inModifiers,
	bool		inIsRepeating)
{
	SKeyboardEvent	&key=EventAlloc();
	
	key.dataType=SKeyboardEvent::kDataIsRawcodeMissingCharCode;
	key.rawcode.b.tbMods=inIsRepeating ? kTBKeyModRepeatingKey : 0;
	AssertThrow_((inModifiers&0xFFFF00FF)==0);		// we expect all modifiers to be in the 2nd byte
	key.rawcode.b.macMods=(inModifiers>>8)&0xFF;
	key.rawcode.b.scancode=inScancode;
	key.rawcode.b.charcode=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ QueueKeyByRawcode											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// enters a key into the queue.
void CKeyQueue::QueueKeyByRawcode(
	UInt32		inRawcode,
	bool		inIsRepeating)
{
	SKeyboardEvent	&key=EventAlloc();
	
	key.dataType=SKeyboardEvent::kDataIsRawcode;
	key.rawcode.rawcode=inRawcode;
	if (inIsRepeating)
		key.rawcode.b.tbMods|=kTBKeyModRepeatingKey;
	else
		key.rawcode.b.tbMods&=~kTBKeyModRepeatingKey;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PopKey													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// pops a key from the q. returns true if popped, false if q empty.
bool CKeyQueue::PopKey(
	UInt32		&outRawKey)
{
	bool		result=PeekKey(outRawKey);
	
	if (result)
	{
		mQStart++;
		if (mQStart>=kMaxKeysInQueue)
			mQStart=0;
	}
	
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PeekKey													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CKeyQueue::PeekKey(
	UInt32		&outRawKey)
{
	if (mQStart!=mQEnd)
	{
		AssertThrow_(mQStart>=0 && mQStart<kMaxKeysInQueue);
	
		SKeyboardEvent	&key=mQ[mQStart];
		
		switch (key.dataType)
		{
			case SKeyboardEvent::kDataIsRawcode:
				outRawKey=key.rawcode.rawcode;
				break;
				
			case SKeyboardEvent::kDataIsRawcodeMissingCharCode:
				key.rawcode.b.charcode=ScancodeAndModsToCharcode(key.rawcode.b.scancode,key.rawcode.b.macMods);
				key.dataType=SKeyboardEvent::kDataIsRawcode;
				outRawKey=key.rawcode.rawcode;
				break;
				
			default:
				Debug_("bad keyboard event type");
				break;
		}
		return true;
	}
	else
	{
		outRawKey=0;
		return false;
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ScancodeAndModsToCharcode						Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// convers a scancode and modifier keys to an ascii char code
UInt8 CKeyQueue::ScancodeAndModsToCharcode(
	UInt8		inScancode,
	UInt8		inModifiers)
{
	UInt16		code;
	UInt8		chr=0;
	
	// there appears to be a bug of some kind in getting the char code for the keys on the numeric key pad, don't
	// know why but the num key pad should be constant (i hope!) across all keyboards so I'll give this a go until
	// someone complains...
	switch (inScancode)
	{
		case 0x4C:		chr=char_Enter;		break;
		case 0x41:		chr='.';			break;
		case 0x52:		chr='0';			break;
		case 0x53:		chr='1';			break;
		case 0x54:		chr='2';			break;
		case 0x55:		chr='3';			break;
		case 0x56:		chr='4';			break;
		case 0x57:		chr='5';			break;
		case 0x58:		chr='6';			break;
		case 0x59:		chr='7';			break;
		case 0x5B:		chr='8';			break;
		case 0x5C:		chr='9';			break;
		case 0x51:		chr='=';			break;
		case 0x4B:		chr='/';			break;
		case 0x43:		chr='*';			break;
		case 0x45:		chr='+';			break;
		case 0x4E:		chr='-';			break;
		case 0x47:		chr=char_Clear;		break;		// nb : clear has same char code as escape
				
		case kUpSC:		chr=char_UpArrow;	break;			
		case kDownSC:	chr=char_DownArrow; break;			
		case kLeftSC:	chr=char_LeftArrow; break;		
		case kRightSC:	chr=char_RightArrow;break;
		
		case 0x73:		chr=char_Home;		break;
		case 0x77:		chr=char_End;		break;
		case 0x74:		chr=char_PageUp;	break;
		case 0x79:		chr=char_PageDown;	break;
		
		case 0x75:		chr=char_FwdDelete;	break;
		case 0x72:		chr=char_Help;		break;
	}
	
	if (!chr)
	{
		// bits 0-6 contain the scancode
		// bit 7 indicates if this is a keyup (1) or down (0)
		// bits 8-15 have the same interpretation as the modifiers field of the event structure
		code=(UInt16(inScancode&0x3F))|(UInt16(inModifiers)<<8);
		
		UInt32		result=::KeyTranslate((const void*)::GetScriptManagerVariable(smKCHRCache),code,&sKeyMapState);
		
		// hi byte first
		// 0 - reserved
		// 1 - chr 1
		// 2 - reserved
		// 3 - chr 2
		chr=result&0xFF;

//		std::printf("Translated scancode 0x%X with mods 0x%X got chars '%c' '%c' '%c' '%c'\n",SInt32(inScancode),inModifiers,result&0xFF,(result>>8)&0xFF,(result>>16)&0xFF,(result>>24)&0xFF);
	}
//	else
//		std::printf("Remap 0x%X to char '%c'\n",SInt32(inScancode),chr);
	
	return chr;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Flush
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// clears all keys from the key queue - some more might get pushed again pretty quick if the user is holding them down and
// the program allows key repreating
void CKeyQueue::Flush()
{
	mQStart=mQEnd=0;
}
