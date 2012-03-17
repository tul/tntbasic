// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CVMStack.h
// Mark Tully
// 15/2/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

#include "VM_OpCodes.h"

class CVMStack
{
	protected:
		SVMStackElement					*mSP,*mMaxSP;		// these are offsets into the handle
		Handle							mStackHandle;
		
		static const UInt32	kIncreaseBy = sizeof(SVMStackElement) * 100;
		
		void /*e*/						GrowStack();
	
	public:
										CVMStack();
		virtual 						~CVMStack();
		
		inline void						ClearStack()		{ mSP=0; }
		
		inline SVMStackElement /*e*/	&Pop()
		{
			TB_Assert_(mSP);										// check stack ptr is not zero
			return *reinterpret_cast<SVMStackElement*>( (*mStackHandle)+(UInt32)(--mSP) );		// assume each stack element is 8 bytes
		}
		
		inline void	/*e*/				Push(
											const SVMStackElement	&inData)
		{
			if (mSP==mMaxSP)	// maxed out?
				GrowStack();
			
			*reinterpret_cast<SVMStackElement*>( (*mStackHandle)+(UInt32)(mSP++) )=inData;
		}

		inline TTBFloat /*e*/			PopFloat()
		{
			const SVMStackElement		&e=Pop();
			return (e.type==SVMStackElement::kInt) ? TTBFloat(e.i) : e.f;
		}
		
		inline TTBInteger /*e*/			PopInt()
		{
			const SVMStackElement		&e=Pop();
			return (e.type==SVMStackElement::kInt) ? e.i : TTBInteger(e.f);
		}
		
		// pops 2 floats, returns true if either was actually a float, returns false if both were casted to floats
		inline bool /*e*/				PopFloatPair(
											TTBFloat	&outFirst,
											TTBFloat	&outSecond)
		{
			const SVMStackElement		&second=Pop();
			const SVMStackElement		&first=Pop();
			
			if (first.type==SVMStackElement::kFloat)
				outFirst=first.f;
			else
				outFirst=first.i;
				
			if (second.type==SVMStackElement::kFloat)
				outSecond=second.f;
			else
				outSecond=second.i;

			if (second.type==SVMStackElement::kFloat || first.type==SVMStackElement::kFloat)
				return true;
			else
				return false;		
		}
		
		inline UInt32					GetCount()		{ return ((UInt32)mSP)/sizeof(SVMStackElement); }
};