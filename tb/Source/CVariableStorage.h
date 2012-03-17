// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CVariableStorage.h
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

#pragma once

#include			<TArray.h>
#include			"TNTBasic_Types.h"
#include			"CCString.h"
#include			"CVariableSymbol.h"

class				CIntegerArrayStore;
class				CStrArrayStore;
class				CFloatArrayStore;
class				CArrayStorage;
class				CVariableSymbolTable;

// debugging purposes, index 0 to array size -1?
#define				CheckIndex(array,index)		AssertThrow_(index>=0 && index<array.GetCount())

class CVariableStorage
{
	private:
		void						Kill();
			
	protected:
		TArray< TTBInteger >			mIntegers;
		TArray< CCString* >				mStrings;
		TArray< TTBFloat >				mFloats;
		TArray< CIntegerArrayStore* >	mIntegerArrays;
		TArray< CStrArrayStore *>		mStringArrays;
		TArray< CFloatArrayStore *>		mFloatArrays;
		
	public:
		/*e*/						CVariableStorage(
										CVariableSymbolTable	&inTable);
		virtual						~CVariableStorage();
		
		// Indexes are zero based - powerplants are 1 based hence the +1
		TTBInteger					&AccessInteger(
										ArrayIndexT		inIndex)			{ CheckIndex(mIntegers,inIndex); return mIntegers[inIndex+1]; }
		CCString					*AccessString(
										ArrayIndexT		inIndex)			{ CheckIndex(mStrings,inIndex); return mStrings[inIndex+1]; }
		TTBFloat					&AccessFloat(
										ArrayIndexT		inIndex)			{ CheckIndex(mFloats,inIndex); return mFloats[inIndex+1]; }
		CIntegerArrayStore			&GetIntArray(
										ArrayIndexT		inIndex)			{ CheckIndex(mIntegerArrays,inIndex); return *mIntegerArrays[inIndex+1]; }
		CStrArrayStore				&GetStrArray(
										ArrayIndexT		inIndex)			{ CheckIndex(mStringArrays,inIndex); return *mStringArrays[inIndex+1]; }
		CFloatArrayStore			&GetFloatArray(
										ArrayIndexT		inIndex)			{ CheckIndex(mFloatArrays,inIndex); return *mFloatArrays[inIndex+1]; }
		
		// Pointer access
		TTBInteger*					GetIntegerPtr(
										ArrayIndexT		inIndex)			{ CheckIndex(mIntegers,inIndex); return &mIntegers[inIndex+1]; }
		TTBFloat*					GetFloatPtr(
										ArrayIndexT		inIndex)			{ CheckIndex(mFloats,inIndex); return &mFloats[inIndex+1]; }
		CCString**					GetStringPtr(
										ArrayIndexT		inIndex)			{ CheckIndex(mStrings,inIndex); return &mStrings[inIndex+1]; }
		CIntegerArrayStore*			GetIntArrayPtr(
										ArrayIndexT		inIndex)			{ CheckIndex(mIntegerArrays,inIndex); return mIntegerArrays[inIndex+1]; }
		CFloatArrayStore*			GetFloatArrayPtr(
										ArrayIndexT		inIndex)			{ CheckIndex(mFloatArrays,inIndex); return mFloatArrays[inIndex+1]; }
		CStrArrayStore*				GetStringArrayPtr(
										ArrayIndexT		inIndex)			{ CheckIndex(mStringArrays,inIndex); return mStringArrays[inIndex+1]; }
};