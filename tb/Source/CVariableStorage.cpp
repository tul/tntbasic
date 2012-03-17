// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CVariableStorage.cpp
// © Mark Tully 2000
// 21/1/00
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

/*
	This class provides storage for variables of all types for a stack frame. The symbol table maps variable names to an
	index into the current variable storage table.
*/

#include		"TNT_Debugging.h"
#include		<TArrayIterator.h>
#include		"CVariableStorage.h"
#include		"Integer_Storage.h"
#include		"String_Storage.h"
#include		"Float_Storage.h"
#include		"CVariableSymbol.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Allocates the storage for the values for a given variable table
CVariableStorage::CVariableStorage(
	CVariableSymbolTable		&inTable)
{
	ArrayIndexT		intCounts=inTable.CountInts();
	ArrayIndexT		strCounts=inTable.CountStrs();
	ArrayIndexT		floatCounts=inTable.CountFloats();
	ArrayIndexT		intArraysCounts=inTable.CountIntArrays();
	ArrayIndexT		strArraysCounts=inTable.CountStrArrays();
	ArrayIndexT		floatArraysCounts=inTable.CountFloatArrays();

	mIntegers.InsertItemsAt(intCounts,LArray::index_Last,(TTBInteger)0);
	mStrings.InsertItemsAt(strCounts,LArray::index_Last,(CCString*)0);
	mFloats.InsertItemsAt(floatCounts,LArray::index_Last,(TTBFloat)0);
	mIntegerArrays.InsertItemsAt(intArraysCounts,LArray::index_Last,(CIntegerArrayStore*)0);
	mStringArrays.InsertItemsAt(strArraysCounts,LArray::index_Last,(CStrArrayStore*)0);
	mFloatArrays.InsertItemsAt(floatArraysCounts,LArray::index_Last,(CFloatArrayStore*)0);
	
	Try_
	{
		// Allocate array storage
		for (ArrayIndexT index=1; index<=floatArraysCounts; index++)
		{
			mFloatArrays[index]=new CFloatArrayStore;
			ThrowIfMemFull_(mFloatArrays[index]);
		}		
		for (ArrayIndexT index=1; index<=strArraysCounts; index++)
		{
			mStringArrays[index]=new CStrArrayStore;
			ThrowIfMemFull_(mStringArrays[index]);
		}
		for (ArrayIndexT index=1; index<=intArraysCounts; index++)
		{
			mIntegerArrays[index]=new CIntegerArrayStore;
			ThrowIfMemFull_(mIntegerArrays[index]);
		}

		// Allocate str storage
		for (ArrayIndexT index=1; index<=strCounts; index++)
		{
			mStrings[index]=new CCString;
			ThrowIfMemFull_(mStrings[index]);
		}
	}
	Catch_(err)
	{
		// Delete parially constructed data structures.
		Kill();
		throw;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Kill
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Delete strings and arrays from storage.
void CVariableStorage::Kill()
{
	TArrayIterator<CCString*>		iter(mStrings);
	CCString						*str;
	
	while (iter.Next(str))
		delete str;
		
	TArrayIterator< CStrArrayStore *>		iter4(mStringArrays);
	CStrArrayStore							*strArray;
	
	while (iter4.Next(strArray))
		delete strArray;
	
	TArrayIterator< CIntegerArrayStore *>	iter3(mIntegerArrays);
	CIntegerArrayStore						*intArray;

	while (iter3.Next(intArray))
		delete intArray;
	
	TArrayIterator< CFloatArrayStore *>		iter5(mFloatArrays);
	CFloatArrayStore						*floatArray;
	
	while (iter5.Next(floatArray))
		delete floatArray;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Delete strings and arrays from storage.
CVariableStorage::~CVariableStorage()
{
	Kill();
}


