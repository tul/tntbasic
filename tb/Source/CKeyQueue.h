// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CKeyQueue.h
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

#pragma once

// raw key format

// mac os modifiers in upper 8 bits
// 

class CKeyQueue
{
	public:
		enum
		{
			kTBKeyModRepeatingKey		= (1<<0)
		};

		union STBRawKeycode		// union to allow easy access of bits in raw code
		{
			struct
			{
				UInt8		tbMods,macMods,scancode,charcode;
			} b;
			UInt32			rawcode;
		};	

		static const UInt32		kCharCodeShift			=	0;
		static const UInt32		kScancodeShift			=	8;
		static const UInt32		kMacKeyModifiersShift	=	16;
		static const UInt32		kTBKeyModifiersShift	=	24;
		static const UInt32		kMaxKeysInQueue			=	20;
		
		void					QueueKeyByScancode(
									UInt32		inScancode,
									UInt32		inModifiers,
									bool		inIsRepeating);
		void					QueueKeyByRawcode(
									UInt32		inRawcode,
									bool		inIsRepeating);
		bool					PopKey(
									UInt32		&outRawKey);
		bool					PeekKey(
									UInt32		&outRawKey);
		void					Flush();

		static UInt8			ScancodeAndModsToCharcode(
									UInt8		inScancode,
									UInt8		inModifiers);
									
	protected:
		struct SKeyboardEvent
		{
			enum
			{
				kDataIsRawcodeMissingCharCode,
				kDataIsRawcode
			}				dataType;
			STBRawKeycode	rawcode;
		};

		static UInt32			sKeyMapState;
		UInt16					mQStart,mQEnd;
		SKeyboardEvent			mQ[kMaxKeysInQueue];
		
		SKeyboardEvent			&EventAlloc();

	public:
								CKeyQueue() :
									mQStart(0),
									mQEnd(0)
									{}
};