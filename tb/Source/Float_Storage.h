// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Float_Storage.h
// © Mark Tully 2000
// 25/4/99
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


// Abstract class for storing an float
class CFloatStorage
{
	public:
		virtual TTBFloat /*e*/			GetValue(
											CProgram		&ioProgram)		=0;
		virtual void /*e*/				SetValue(
											CProgram		&ioProgram,
											TTBFloat		inInt)			=0;
};

// A symbol for an float for a symbol table
class CFloatSymbol : public CVariableSymbol, public CFloatStorage
{
	protected:
	
	public:
								CFloatSymbol(
									const char		*inString) : CVariableSymbol(CVariableSymbol::kFloat,inString) {}

		TTBFloat				GetValue(
									CProgram	&ioProgram);
		void					SetValue(
									CProgram	&ioProgram,
									TTBFloat	inInt);
};

// A symbol table for floats
class CFloatSymbolTable : public CSymbolTableT<CFloatSymbol>
{
};

class CFloatArrayStore;

// A float array, stored as an array symbol in the array symbol table
class CFloatArraySymbol : public CArraySymbol
{
	public:
								CFloatArraySymbol(
									const char		*inString) : CArraySymbol(CVariableSymbol::kFloatArray,inString) {}

		CFloatArrayStore		&GetFloatArrayStorage(
									CProgram				&ioState);
};


// An item from a float array
class CFloatArrayItem : public CArrayItem, public CFloatStorage
{
	protected:
	
	public:
								CFloatArrayItem(
									CFloatArraySymbol			*inArray,
									CGeneralExpression			*inIndex) :
									CArrayItem(inArray,inIndex)
									{}
									
				
		TTBFloat	/*e*/		GetValue(
									CProgram		&ioProgram);
		void /*e*/				SetValue(
									CProgram		&ioProgram,
									TTBFloat		inInt);
};

// This class stores the floats in a CArrayStorage class
class CFloatArrayStore : public CArrayStoreT<TTBFloat>
{
	public:
		virtual void /*e*/		AllocateStorage(
									CProgram		&ioState,
									UInt32			inNumElements);
};
