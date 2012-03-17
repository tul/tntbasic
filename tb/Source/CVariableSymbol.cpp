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

/*
	a variation of the standard symbol entry which allows for flags saying whether the symbol is root and also retrieving
	of the variables stack frame.
	The type of the symbol is also stored allowing a common CVariableSymbol class to be used to represent all variables.
*/

#include		"CVariableSymbol.h"
#include		"CProgram.h"

class CUndeclaredSymbol : public CVariableSymbol
{
	public:
								CUndeclaredSymbol(
									ESymbolType	inType,
									const char *inStr) : CVariableSymbol(inType,inStr) {}
};

class CUndeclaredArray : public CArraySymbol
{
	public:
			CUndeclaredArray(
				ESymbolType inType,
				const char	*inStr) :
				CArraySymbol(inType,inStr) {}
};

#undef ToLower

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CVariableSymbol::GetStackFrame						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the stack frame this variable should be stored in
CStackFrame *CVariableSymbol::GetStackFrame(
	CProgram	&inState)
{
	if (mIsRoot)	// is a root variable?
		return inState.RootStackFrame();
	else
		return inState.CurrentStackFrame();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CVariableSymbolTable::EnterTemporary					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Creates a symbol which can be used for internal value storage. The symbol is entered with a unique name. Don't make them
// global/shared etc 'cos that would break the temporary naming strategy in use here
CVariableSymbol *CVariableSymbolTable::EnterTemporary(
	CVariableSymbol::ESymbolType	inType)
{
	CCString			str("Ætemp"); // <-- I think Æ isn't allowed in an identifier name by default
	Str15				temp;
	
	::NumToString(mTempIndex++,temp);
	str+=temp;
	
	return Enter(str,inType);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CVariableSymbolTable::Enter							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Variant on the standard enter function which takes a type as well as a name
CVariableSymbol *CVariableSymbolTable::Enter(
	const char						*inName,
	CVariableSymbol::ESymbolType	inType)
{
	Assert_(mTableComplete==false);
	
	CCString		tempString(inName);

	tempString.ToLower();

	// fetch it
	CVariableSymbol		*entry=FetchData(tempString);

	if (!entry)
	{
		// not there, create it
		switch (inType)
		{
			case CVariableSymbol::kInteger:
				entry=new CIntegerSymbol(tempString);
				mIntCount++;
				break;
			
			case CVariableSymbol::kString:
				entry=new CStrSymbol(tempString);
				mStrCount++;
				break;
			
			case CVariableSymbol::kFloat:
				entry=new CFloatSymbol(tempString);
				mFloatCount++;
				break;
			
			case CVariableSymbol::kIntegerArray:
				entry=new CIntegerArraySymbol(tempString);
				mIntArrayCount++;
				break;
			
			case CVariableSymbol::kFloatArray:
				entry=new CFloatArraySymbol(tempString);
				mFloatArrayCount++;
				break;
			
			case CVariableSymbol::kStringArray:
				entry=new CStrArraySymbol(tempString);
				mStrArrayCount++;
				break;

			case CVariableSymbol::kUndeclaredArray:
				entry=new CUndeclaredArray(inType,tempString);
				break;
				
			case CVariableSymbol::kUndeclaredPrimitive:
				entry=new CUndeclaredSymbol(inType,tempString);
				break;
				
			default:
				SignalPStr_("\pCVariableSymbolTable::Enter() - unknown symbol type");
				Throw_(-1);
				break;
		}
		
		ThrowIfMemFull_(entry);
		entry->SetParentTable(this);
		
		Try_
		{
			Store(entry);
		}
		Catch_(err)
		{
			delete entry;
			throw;
		}
	}
	
	return entry;	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ TableComplete
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Normally each symbol is assigned an index starting from 0. This routine does things slightly diffrently, each index is
// the index into that specific class of data instead. This allows storage to be accessed correctly
void CVariableSymbolTable::TableComplete()
{
	mTableComplete=true;

	TArrayIterator< CAssociativeEntryT<CVariableSymbol> * >	iter(GetArray());		// NB: using GetArray() to avoid gcc4 compile error if mArray is used directly : mr/C++_Objects/CAssociativeArrayT.h:124: error: 'TArray<CAssociativeEntryT<CVariableSymbol>*> CAssociativeArrayT<CVariableSymbol>::mArray' is protected
	CAssociativeEntryT<CVariableSymbol> 						*entry;
	ArrayIndexT									intIndex=0;
	ArrayIndexT									strIndex=0;
	ArrayIndexT									floatIndex=0;
	ArrayIndexT									intAIndex=0;
	ArrayIndexT									strAIndex=0;
	ArrayIndexT									floatAIndex=0;

	while (iter.Next(entry))
	{
		if (IsGuest(entry->GetData()))
			continue;
			
		switch (entry->GetData()->GetType())
		{
			case CVariableSymbol::kInteger:
				entry->GetData()->AssignIndex(intIndex++);
				break;

			case CVariableSymbol::kString:
				entry->GetData()->AssignIndex(strIndex++);
				break;

			case CVariableSymbol::kFloat:
				entry->GetData()->AssignIndex(floatIndex++);
				break;

			case CVariableSymbol::kIntegerArray:
				entry->GetData()->AssignIndex(intAIndex++);
				break;

			case CVariableSymbol::kStringArray:
				entry->GetData()->AssignIndex(strAIndex++);
				break;

			case CVariableSymbol::kFloatArray:
				entry->GetData()->AssignIndex(floatAIndex++);
				break;
				
			case CVariableSymbol::kUndeclaredPrimitive:
			case CVariableSymbol::kUndeclaredArray:
				break;

			default:
				SignalPStr_("\pUnknown variable class in CVariableSymbolTable::TableComplete()");
				Throw_(-1);
				break;
		}
	}
}

