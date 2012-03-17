// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Integer_Storage.h
// © Mark Tully 1999
// 23/12/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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

#include			"TNTBasic_Types.h"
#include			"CVariableSymbol.h"
#include			"CArraySymbol.h"
#include			"CArrayItem.h"
#include			"CArrayStoreT.h"

class				CArithExpressionNode;
class				CProgram;

// Abstract class for storing an integer
class CIntegerStorage
{
	public:
		virtual TTBInteger /*e*/		GetValue(
											CProgram		&ioProgram)		=0;
		virtual void /*e*/				SetValue(
											CProgram		&ioProgram,
											TTBInteger		inInt)			=0;
};

// A symbol for an integer for a symbol table
class CIntegerSymbol : public CVariableSymbol, public CIntegerStorage
{
	protected:
	
	public:
								CIntegerSymbol(
									const char		*inString) : CVariableSymbol(CVariableSymbol::kInteger,inString) {}

		TTBInteger				GetValue(
									CProgram	&ioProgram);
		void					SetValue(
									CProgram	&ioProgram,
									TTBInteger	inInt);
};

class CIntegerArrayStore;

// A integer array, stored as an array symbol in the array symbol table
class CIntegerArraySymbol : public CArraySymbol
{
	public:
								CIntegerArraySymbol(
									const char		*inString) : CArraySymbol(CVariableSymbol::kIntegerArray,inString) {}

		CIntegerArrayStore		&GetIntArrayStorage(
									CProgram				&ioState);
};

// An item from an integer array
class CIntegerArrayItem : public CArrayItem, public CIntegerStorage
{
	protected:
	
	public:
								CIntegerArrayItem(
									CIntegerArraySymbol			*inArray,
									CGeneralExpression			*inIndex) :
									CArrayItem(inArray,inIndex)
									{}								
		
		TTBInteger	/*e*/		GetValue(
									CProgram		&ioProgram);
		void /*e*/				SetValue(
									CProgram		&ioProgram,
									TTBInteger		inInt);
};

// This class stores the integers in a CArrayStorage class
class CIntegerArrayStore : public CArrayStoreT<TTBInteger>
{
	public:
		virtual void /*e*/			AllocateStorage(
										CProgram		&ioState,
										UInt32			inNumElements);
};
