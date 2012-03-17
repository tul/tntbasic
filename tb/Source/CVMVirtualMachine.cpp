// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CVMVirtualMachine.cpp
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

/*
	An implementation of a virtual machine for executing TB assembler programs.
	
	It is a stackbased virtual machine that works with code chunks
*/

#include "CVMVirtualMachine.h"
#include "CVMCodeChunk.h"
#include "TNTBasic_Errors.h"

const char						*CVMVirtualMachine::kUnregisteredFunction="??? unregistered function ???";

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CVMVirtualMachine::~CVMVirtualMachine()
{
	delete mProgram;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetFunctionName								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
const char *CVMVirtualMachine::GetFunctionName(
	SInt32			inFuncIndex)
{
	if (inFuncIndex>=mUsedFunctions)
		return kUnregisteredFunction;
	else
		return mFuncTable[inFuncIndex].funcName;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RegisterFunction								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CVMVirtualMachine::RegisterFunction(
	SVMFunctionEntry	*inEntry)
{
	if (mUsedFunctions==kMaxFunctions)
		Throw_(-1);
	
	mFuncTable[mUsedFunctions++]=*inEntry;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Run											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// runs a program slice, returns true if there's more to run or false if that's your lot
// A slice is defined as:
//		30 jumps (includes 'for' loops and if jumps)
//		After any function call (eg move or anim function)
//		Up until a Pause
bool CVMVirtualMachine::RunSlice()
{
	if (!mProgram)
		return false;

	bool	done=false;
	bool	progComplete=false;
	SInt32	jumps=30;		// allow this many jumps before ending the slice
		
	while (!done)
	{
		switch ((*mProgram)[mPC].op)
		{
			case kIns_LoadRegister:
				{
					SVMStackElement		e(GetReg((*mProgram)[mPC].data.i));
					mStack.Push(e);				
				}
				break;
				
			case kIns_SaveRegister:
				{
					SInt32		a;
					a=mStack.Pop().i;
					SetReg((*mProgram)[mPC].data.i,a);
				}
				break;
		
			case kIns_LoadI:
				{
					SVMStackElement		e((*mProgram)[mPC].data.i);
					mStack.Push(e);
				}
				break;
				
			case kIns_LoadF:
				{
					SVMStackElement		e((*mProgram)[mPC].data.f);
					mStack.Push(e);				
				}
				break;
				
			case kIns_Add:
				{
					TTBFloat	a,b;
					
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(a+b);
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(a)+SInt32(b));
						mStack.Push(e);
					}
				}
				break;

			case kIns_Sub:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(a-b);
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(a)-SInt32(b));
						mStack.Push(e);
					}
				}
				break;
				
			case kIns_Mul:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(a*b);
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(a)*SInt32(b));
						mStack.Push(e);
					}
				}
				break;
				
			case kIns_Div:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(a/b);
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(a)/SInt32(b));
						mStack.Push(e);
					}
				}
				break;
				
			case kIns_EQ:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(SInt32(a==b));
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(SInt32(a)==SInt32(b)));
						mStack.Push(e);
					}
				}
				break;
				
			case kIns_NEQ:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(SInt32(a!=b));
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(SInt32(a)!=SInt32(b)));
						mStack.Push(e);
					}
				}
				break;
				
			case kIns_GT:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(SInt32(a>b));
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(SInt32(a)>SInt32(b)));
						mStack.Push(e);
					}
				}
				break;
				
			case kIns_LT:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(SInt32(a<b));
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(SInt32(a)<SInt32(b)));
						mStack.Push(e);
					}
				}
				break;
				
			case kIns_GTE:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(SInt32(a>=b));
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(SInt32(a)>=SInt32(b)));
						mStack.Push(e);
					}
				}
				break;
				
			case kIns_LTE:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(SInt32(a<=b));
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e(SInt32(SInt32(a)<=SInt32(b)));
						mStack.Push(e);
					}
				}
				break;
				
			case kIns_Neg:
				{
					TTBFloat		a,b;
					const SVMStackElement		&e(mStack.Pop());
					
					if (e.type==SVMStackElement::kInt)
					{
						SVMStackElement		o(-e.i);
						mStack.Push(o);					
					}
					else
					{
						SVMStackElement		o(-e.f);
						mStack.Push(o);					
					}					
				}
				break;
				
			case kIns_Power:
				{
					TTBFloat		a,b;
					if (mStack.PopFloatPair(a,b))
					{					
						SVMStackElement		e(::powf(a,b));
						mStack.Push(e);
					}
					else
					{
						SVMStackElement		e((SInt32) ::pow(SInt32(a),SInt32(b)) );
						mStack.Push(e);
					}
				}
				break;
				

			case kIns_BNZI:
			case kIns_BRAI:
				{
					SInt32		a=1;

					if ((*mProgram)[mPC].op==kIns_BNZI)
						a=mStack.PopInt();

					if (a)
					{
						SInt32	dest=(*mProgram)[mPC].data.i;
						
						if (dest==0xFFFFFFFF)
							ThrowRuntimeError(kTBErr_BadJumpAddress);
						else
						{
							mPC=dest-1;		// NB: Relying on the mPC++ line further down...
							done=(--jumps)<=0;
						}
					}
				}
				break;
				
			case kIns_BNZ:
				{
					SInt32		a;
					a=mStack.PopInt();

					if (a)
					{
						SInt32	dest=mProgram->GetLabelDest((*mProgram)[mPC].data.i);
						
						if (dest==-1)
							ThrowUndefinedLabel((char)(*mProgram)[mPC].data.i);
						else
						{
							mPC=dest;		// NB: Zero based offsets for label. mPC is then ++ further down to give 1 based index
							done=(--jumps)<=0;
						}
					}
				}
				break;	
				
			case kIns_Jump:
				{
					SInt32	dest=mProgram->GetLabelDest((*mProgram)[mPC].data.i);
					
					if (dest==-1)
						ThrowUndefinedLabel((char)(*mProgram)[mPC].data.i);
					else
					{
						mPC=dest;		// NB: Zero based offsets for label. mPC is then ++ further down to give 1 based index
						done=(--jumps)<=0;
					}
				}
				break;	
				
			case kIns_FuncCall:
				mFuncTable[(*mProgram)[mPC].data.i].funcPtr(this);
				done=true;		// end slice
				break;			
				
			case kIns_Halt:
				done=true;
				progComplete=true;
				break;
				
			case kIns_Nop:
				done=true;		// use nops to break up slices
				break;
				
			default:
				{
					char	str[256];
					std::sprintf(str,"Unknown moan VM instruction: %d",int((*mProgram)[mPC].op));
					Debug_(str);
				}
				break;
		}
		mPC++;
	}	
	
	// Print result (what's left on the stack)
	if (progComplete)
	{
		if (mStack.GetCount())
		{
			SInt32	i=0;
			std::cout << "Moan result(s):\n";
			while (mStack.GetCount())
			{
				SInt32	a;
				a=mStack.Pop().i;
				std::cout << "Result " << ++i << ":\t" << a << "\n";
			}
		}
		else
			;	// std::cout << "No results\n";
	}
		
	return !progComplete;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadCode
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// takes ownership of code chunk
void CVMVirtualMachine::LoadCode(
	CVMCodeChunk	*inCode)
{
	delete mProgram;
	mProgram=inCode;
	
	RestartProgram();
}

