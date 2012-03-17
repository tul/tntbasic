// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CFlexBuffer.h
// © Mark Tully and TNT Software 2000
// 25/6/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
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

#include	"CStdIOStream.h"
#include	"LDataStream.h"

struct yy_buffer_state;

// pass these functions into the constructor for the class to work
// their names depend on the name space used by the flex support code
// this name space is difficult to guess - especially when more than one parser is in use
typedef yy_buffer_state *(*AllocAndSwitchToBufferT)(char *inPtr,SInt32 inSize);
typedef void (*DeallocBufferT)(yy_buffer_state*);
typedef int (*GetCurrentLexOffsetT)();

class CFlexBuffer : public CStdIOStream
{
	protected:
		yy_buffer_state				*mFlexBuffer;
		LDataStream					mDataStream;
		AllocAndSwitchToBufferT		mAllocAndSwitch;
		DeallocBufferT				mDealloc;
		GetCurrentLexOffsetT		mCurrentLexOffset;
	
	public:	
						CFlexBuffer(
							char						*inBuffer,
							SInt32						inSize,
							AllocAndSwitchToBufferT		inA,
							DeallocBufferT				inD,
							GetCurrentLexOffsetT		inO);
		virtual			~CFlexBuffer();
		
		SInt32			GetLastTokenOffset();
		SInt32			GetLastTokenLength();
		
		yy_buffer_state	*GetBuffer()	{ return mFlexBuffer; }
		
		static void		MacToUnixCR(
							char	*inBuffer,
							SInt32	inSize);
};