// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CSymbolTableT.h
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

/*
	A symbol table looks up classes by a string. When being entered, if they're not there then they are created with null
	values. This is a templated class to allow it to store entries of any form, labels, integers, reals, strings etc...
	The template parameter should be a class which is a sub class of CSymbolEntryT<T>.
*/

#pragma once

#include			"TNT_Debugging.h"
#include			"CAssociativeArrayT.h"

template <class T>
class CSymbolTableT;

template <class T>
class CSymbolEntryT : public CAssociativeEntryT< T >  // <-- Must be first super class so that a static cast from CAssArray to CSymbol works
{
	protected:
		ArrayIndexT			mSymbolTableIndex;
		CSymbolTableT<T>	*mParentTable;

	public:
							CSymbolEntryT(
								T			*inData,
								const char	*inStr) : CAssociativeEntryT<T>(inData,inStr), mSymbolTableIndex(0), mParentTable(0) {}
		// add virtual destructor to reserve right for symbols to have virtual functions
		// if we don't do this, then if a subclass adds a vfun, it will break the CSymbolTableT::DeleteAllEntries func, which will delete
		// from a CSymbolEntryT ptr. if a subclass has vfuncs, that ptr will != the subclass's ptr, and so deletion will be using the wrong ptr
		// (typically 4 bytes off)
		virtual				~CSymbolEntryT() {}
	
		CSymbolTableT<T>	*GetParentTable()						{ return mParentTable; }
		void				SetParentTable(
								CSymbolTableT<T> *inNewParent) 		{ mParentTable=inNewParent; }
	
		ArrayIndexT			GetIndex()								{ return mSymbolTableIndex; }
		void				AssignIndex(
								ArrayIndexT	inIndex) 				{ mSymbolTableIndex=inIndex; }
};

template <class T>
class CSymbolTableT : protected CAssociativeArrayT< T >
{
	private:
		typedef CAssociativeArrayT<T>			inherited;
		
		using inherited::mArray;									// this is necessary to get this code to compile in gcc 4
																	// gcc 4 uses a different name lookup model which is more standards compliant. essentially, you can't
																	// refer to members of an inherited template class without qualifying it or declaring that you're 'using' it
																	// search gcc docs for name lookup for more info
	
	protected:
		bool			mTableComplete;
		
		const TArray< CAssociativeEntryT<T> * >		&GetArray() const { return mArray; }		// this is only necessary as using mArray directly give a tntbasic.com.sf/mr/C++_Objects/CAssociativeArrayT.h:124: error: 'TArray<CAssociativeEntryT<CVariableSymbol>*> CAssociativeArrayT<CVariableSymbol>::mArray' is protected' error in dervied classes - that doesn't make sense to me and compiles fine in cw

	public:
						CSymbolTableT() : mTableComplete(false) {}
		virtual			~CSymbolTableT();

		virtual void	DeleteAllEntries();

		virtual T /*e*/	*Exists(
							const char		*inName);
		
		virtual T /*e*/	*Enter(
							const char		*inName);

		virtual bool	IsGuest(
							CSymbolEntryT<T>	*inSymbol);
		
		virtual void /*e*/	EnterGuest(
								CSymbolEntryT<T>	*inGuest);

		virtual void	TableComplete();
		
		ArrayIndexT		CountEntries()		{ return mArray.GetCount(); }
		
		T				*GetNthEntry(
							ArrayIndexT	inIndex);

		void			Release(
							CAssociativeEntryT<T>	*inSymbol);

//T *CreateReference(
//	T	*inOriginal)
		
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Delete all entries
template <class T>
CSymbolTableT<T>::~CSymbolTableT()
{
	DeleteAllEntries();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Release
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Releases a symbol from the table without deleting it. Set's it parent table to nil if it was owned by this table
template <class T>
void CSymbolTableT<T>::Release(
	CAssociativeEntryT<T>	*inEntry)
{
	CSymbolEntryT<T>	*entry2=(CSymbolEntryT<T>*)inEntry;		// can't get the dynamic cast line to compile for some reason...
	
	if (entry2->GetParentTable()==this)
		entry2->SetParentTable(0);
	
	CAssociativeArrayT<T>::Release(inEntry);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DeleteAllEntries
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Overridden to not delete the guests
template <class T>
void CSymbolTableT<T>::DeleteAllEntries()
{
	TArrayIterator<CAssociativeEntryT<T>*>			iter(mArray);
	CAssociativeEntryT<T>	*entry;
	
	while (iter.Next(entry))
	{
//		CSymbolEntryT<T>	*entry2=dynamic_cast<CSymbolEntryT<T>*>(entry);
		CSymbolEntryT<T>	*entry2=(CSymbolEntryT<T>*)entry;		// can't get the dynamic cast line to compile for some reason...
		if (!IsGuest(entry2))
			delete entry2;
	}
		
	mArray.RemoveItemsAt(mArray.GetCount(),1);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EnterGuest														/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// A guest symbol is owned by another table but is placed in the table to be referenced. use this for inheritance.
template <class T>
void CSymbolTableT<T>::EnterGuest(
	CSymbolEntryT<T>	*inEntry)
{
	Assert_(mTableComplete==false);

	// it's it's there then don't add it again
	if (mArray.FetchIndexOf(inEntry)==LArray::index_Bad)
		Store(inEntry);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsGuest
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns true if the symbol is a guest symbol, ie not owned by this table
template <class T>
bool CSymbolTableT<T>::IsGuest(
	CSymbolEntryT<T>	*inSymbol)
{
	return inSymbol->GetParentTable()!=this;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ TableComplete
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This fixes all the indexes of the symbols in the table which allows the entries to index uniquely a value table.
template <class T>
void CSymbolTableT<T>::TableComplete()
{
	mTableComplete=true;
	
	TArrayIterator< CAssociativeEntryT<T> * >	iter(mArray);
	CAssociativeEntryT<T> 						*entry;
	ArrayIndexT									index=0;
	
	while (iter.Next(entry))
		entry->GetData()->AssignIndex(index++);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Exists										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the symbol if it exists. Doesn't create it if not.
template <class T>
T  *CSymbolTableT<T>::Exists(
	const char		*inName)
{
	CCString		tempString(inName);

	tempString.ToLower();

	// fetch it
	return inherited::FetchData(tempString);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Enter											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Enters a symbol with the given name. If it already exists it just returns it, hence this function is also a look up func
template <class T>
T *CSymbolTableT<T>::Enter(
	const char		*inName)
{
	Assert_(mTableComplete==false);
	
	CCString		tempString(inName);

	tempString.ToLower();

	// fetch it
	T				*entry=inherited::FetchData(tempString);

	if (!entry)
	{
		// not there, create it
		entry=new T(tempString);
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
//		¥ GetNthEntry
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns nil if index is bad. Index is zero based.
template <class T>
T *CSymbolTableT<T>::GetNthEntry(
	ArrayIndexT	inIndex)
{
	if (inIndex>mArray.GetCount())
		return 0;
	else
		return mArray[inIndex+1]->GetData();
}
