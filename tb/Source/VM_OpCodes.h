// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// VM_OpCodes.h
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

#include "TNTBasic_Types.h"

// typedefs to work with the virtual machine
typedef UInt32		TVMOpCode;
union TVMOperand
{
	float		f;
	SInt32		i;
};

// a single VM instruction consists of an operand code and a single parameter
struct SVMInstruction
{
	TVMOpCode			op;		// op code
	TVMOperand			data;		// operand
	

						SVMInstruction(
							TVMOpCode		inOp,
							float			inF) :
							op(inOp)
							{
								data.f=inF;
							}
							
						SVMInstruction(
							TVMOpCode		inOp,
							SInt32			inI) :
							op(inOp)
							{
								data.i=inI;
							}
							
						SVMInstruction(
							TVMOpCode		inOp,
							char			inC) :
							op(inOp)
							{
								data.i=inC;
							}
	
	private:
							SVMInstruction(const SVMInstruction &inA) {}
};

// op-codes for virtual machine
enum	// TInstruction
{
	kIns_Nop,			// do nowt, shouldn't really end up in programs
	kIns_LoadRegister,	// load a register, either local, global or special
	kIns_SaveRegister,	// save a register, data is reg to save to
	kIns_LoadI,			// load int literal, data is sint32 to push onto the stack
	kIns_LoadF,			// load float literal, data is a float to push onto the stack
	kIns_Add,			// add, adds last to numbers on the stack, pops them both and pushes the result
	kIns_Sub,
	kIns_Mul,
	kIns_Div,
	kIns_Power,
	kIns_Neg,			// negates the last loaded element on the stack
	kIns_BNZ,			// Branch if not zero, looks at last value and brances to label in data if it's non zero
	kIns_BNZI,			// Branch if not zero immediate, same as above but goes to data as address
	kIns_BRAI,			// Branch immediate, unconditional branch to address in data
	kIns_LT,			// comparison ops, takes last two and replaces with a 1 or 0
	kIns_GT,
	kIns_LTE,
	kIns_GTE,
	kIns_EQ,
	kIns_NEQ,
	kIns_Jump,			// jump to label in data
	kIns_Halt,			// terminates the virtual machine
	
	kIns_FuncCall		// calls function with index <data>. Must be registered with VM.
};

typedef void (*TVMFunction)(class CVMVirtualMachine *);

#define	kVMMaxFunctionNameLength	50

struct SVMFunctionEntry
{
	char			funcName[kVMMaxFunctionNameLength];
	TVMFunction		funcPtr;
};

struct SVMStackElement
{
	enum EType
	{
		kInt,
		kFloat
	};
	UInt32				type;		// do we need a type? vm code generator should be able to produce code so that no runtime type checking is needed?
	
	union
	{
		TTBInteger		i;
		TTBFloat		f;
	};

						SVMStackElement(
							float		inData) :
							type(kFloat),
							f(inData)
							{
							}
	
						SVMStackElement(
							SInt32		inData) :
							type(kInt),
							i(inData)
							{
							}
};
