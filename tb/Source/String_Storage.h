// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// String_Storage.h
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

// Abstract class for storing a string
class CStrStorage
{
	public:
		virtual CCString				&Access(
											CProgram	&ioState) = 0;	// Allows read and write of the resultant CString
};

// A symbol representing a string
class CStrSymbol : public CVariableSymbol, public CStrStorage
{
	public:
								CStrSymbol(
									const char		*inString) : CVariableSymbol(CVariableSymbol::kString,inString) {}

		CCString				&Access(
									CProgram	&ioState);
};

class CStrArrayStore;

// A str array, stored as an array symbol in the array symbol table
class CStrArraySymbol : public CArraySymbol
{
	public:
								CStrArraySymbol(
									const char		*inString) : CArraySymbol(CVariableSymbol::kStringArray,inString) {}

		CStrArrayStore			&GetStrArrayStorage(
									CProgram		&ioState);
};

// An item from a str array
class CStrArrayItem : public CArrayItem, public CStrStorage
{
	protected:
	
	public:
								CStrArrayItem(
									CStrArraySymbol				*inArray,
									CGeneralExpression		*inIndex) :
									CArrayItem(inArray,inIndex)
									{}									
				
		CCString				&Access(
									CProgram	&ioState);
};

// A str storage
class CStrArrayStore : public CArrayStoreT<CCString*>
{	
	protected:
		void						DeleteStrings(
										ArrayIndexT		inNum,
										ArrayIndexT		inStartingFrom);
		void						CreateStrings(
										ArrayIndexT		inNum,
										ArrayIndexT		inStartingFrom);
		void						ClearStrings(
										ArrayIndexT		inNum,
										ArrayIndexT		inStartingFrom);

	public:
		virtual						~CStrArrayStore();
		
		virtual void /*e*/			AllocateStorage(
										CProgram		&ioState,
										UInt32			inNumElements);
};