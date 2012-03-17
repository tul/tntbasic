// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CVariableSymbol.h
// © Mark Tully 2000
// 23/1/00
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

#include					"CSymbolTableT.h"

class						CStackFrame;
class						CProgram;

class CVariableSymbol : public CSymbolEntryT<CVariableSymbol>
{
	public:
		enum ESymbolType
		{
			kUndefined,			// Shouldn't actually be set for a symbol in the table but useful as a null return value for
			kInteger,			// some functions
			kString,
			kFloat,
			kIntegerArray,
			kStringArray,
			kFloatArray,
			kUndeclaredPrimitive,	// Undeclared types are returned from the lexer when the user has failed to declare
			kUndeclaredArray,		// They allow the parsing to recover and stop the same var causing an error later.
		};

	protected:
		ESymbolType				mSymbolType;
		bool					mIsRoot;
	
	public:
		// This constructor is needed in order to instantiate the CAssociativeArray::Enter function which creates new symbols
		// using only the name. For our variable table we can't use Enter as this doesn't type the symbol, however we still
		// need this constructor to keep Enter happy, even though we aren't using it. Just signal if this constructor is
		// ever invoked.
								CVariableSymbol(
									const char	*inStr) : CSymbolEntryT<CVariableSymbol>(this,inStr) { SignalPStr_("\pCVariableSymbol illegal constructor called!"); Throw_(-1); }
								CVariableSymbol(
									ESymbolType	inType,
									const char	*inStr) : CSymbolEntryT<CVariableSymbol>(this,inStr), mIsRoot(false), mSymbolType(inType) {}
		
		bool					IsRoot()	{ return mIsRoot; }
		bool					IsUndeclared() { return mSymbolType==kUndefined || mSymbolType==kUndeclaredPrimitive || mSymbolType==kUndeclaredArray; }
		void					MakeRoot()	{ mIsRoot=true; }
		ESymbolType				GetType()	{ return mSymbolType; }
		
		CStackFrame	/*e*/		*GetStackFrame(
									CProgram	&inState);
};

class CVariableSymbolTable : public CSymbolTableT<CVariableSymbol>
{
	private:
		typedef CSymbolTableT<CVariableSymbol> inheritedTable;

	protected:
		ArrayIndexT		mIntCount;
		ArrayIndexT		mStrCount;
		ArrayIndexT		mFloatCount;
		ArrayIndexT		mIntArrayCount;
		ArrayIndexT		mStrArrayCount;
		ArrayIndexT		mFloatArrayCount;

		SInt32			mTempIndex;

	public:
						CVariableSymbolTable() :
							mIntCount(0),
							mStrCount(0),
							mFloatCount(0),
							mIntArrayCount(0),
							mStrArrayCount(0),
							mFloatArrayCount(0),
							mTempIndex(0)
							{}
							
		CVariableSymbol	*Enter(
							const char						*inName,
							CVariableSymbol::ESymbolType	inType);
		CVariableSymbol *EnterTemporary(
							CVariableSymbol::ESymbolType	inType);

		void			TableComplete();
		
		ArrayIndexT		CountInts()			{ return mIntCount; }
		ArrayIndexT		CountStrs()			{ return mStrCount; }
		ArrayIndexT		CountFloats()		{ return mFloatCount; }
		ArrayIndexT		CountIntArrays()	{ return mIntArrayCount; }
		ArrayIndexT		CountStrArrays()	{ return mStrArrayCount; }
		ArrayIndexT		CountFloatArrays()	{ return mFloatArrayCount; }
};