// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CArrayStoreT.h
// © Mark Tully 2000
// 5/2/00
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
	See Arrays.doc for more

	The array store represents a multi-dimensional array for the data type RAWTYPE.
	The array must be dimensioned using Dimension before it can be accessed. Access is done by passing a linked list of
	indexes which are evaluated into a unique item index which is accessed from the array.
*/

#pragma once

class		CArithExpressionNode;
class		CProgram;
class		CGeneralExpression;

#include	"CArrayStorage.h"

// This constructs a symbol representing an array of type RAWTYPE
// RAWTYPE is the primative type of data, eg TTBInteger, or CCString*
template <class RAWTYPE>
class CArrayStoreT : public CArrayStorage
{
	protected:
		ArrayIndexT				*mDimensions;
		ArrayIndexT				mArity;
		TArray<RAWTYPE>			mArray;
		
	public:
								CArrayStoreT();
		virtual					~CArrayStoreT();

		void /*e*/				Dimension(
									CProgram		&inProgram,
									ArrayIndexT		inArity,
									ArrayIndexT		*inDimensions);
		
		TArray<RAWTYPE>			&GetRawData()			{ return mArray; }
		
		RAWTYPE /*e*/			&Access(
									CProgram				&inState,
									CGeneralExpression		*inIndex);

		ArrayIndexT /*e*/		CalcFlatIndex(
									CProgram				&inState,
									CGeneralExpression		*inIndex);
		
		ArrayIndexT				GetSize()				{ return mArray.GetCount(); }
		
		RAWTYPE*				GetData(
									CProgram				&inState,
									CGeneralExpression		*inIndex)
								{
									return GetData(CalcFlatIndex(inState,inIndex));
								}
								
		RAWTYPE*				GetData(
									ArrayIndexT		inIndex)
								{
									return (RAWTYPE*)(*mArray.GetItemsHandle()+(inIndex-1)*sizeof(RAWTYPE));
								}
		
		// returns the number of parameters required to index into the array. >=1 if dimensioned ==0 otherwise
		ArrayIndexT				GetArity()							{ return mArity; }
		// returns the dimension of the array, inDimension from 0..GetArity-1 inclusive.
		ArrayIndexT				GetDimension(
									ArrayIndexT		inDimension)	{ Assert_(inDimension>=0 && inDimension<GetArity()); return mDimensions[inDimension]; }
		
		// Override this to resize the array and fill it with default values
		// Array may already be dim and may need growing/shrinking
		virtual void /*e*/		AllocateStorage(
									CProgram		&ioState,
									UInt32			inNumElements)	= 0;
};

#include			"CArithExpression.h"
#include			"CGeneralExpression.h"
#include			"TNTBasic_Types.h"
#include			"UTBException.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Pass in the name of the array
template <class RAWTYPE>
CArrayStoreT<RAWTYPE>::CArrayStoreT() :
	mDimensions(0),
	mArity(0)
{
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Fress the mDimension
template <class RAWTYPE>
CArrayStoreT<RAWTYPE>::~CArrayStoreT()
{		
	delete [] mDimensions;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CalcFlatIndex														/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the index into the master array for a (possibly) multi-dimensional index.
// The result is 1 based, eg 1... as is standard for a PowerPlant array.
// the expression list will yield when evaluated a list of indexes which select 1 item
// from the array. eg [1] or [1,3] for a 2D array etc.
// Each subIndex is 0 based ie 0...(length-1)
// This is transformed into a single index into the flatted array by the following
// equation:
// FlatIndex(A,B....C)	 =	A*ProductOfAllDimensionAfterThisOne *
//							B*ProductOfAllDimensionAfterThisOne ...
//							+ C
// So for a [5,3,2] Array index [A,B,C] would yield A*3*2 * B*2 + C
// Throws if the array has not been dimenesioned or if the index is out of range
template <class RAWTYPE>
ArrayIndexT CArrayStoreT<RAWTYPE>::CalcFlatIndex(
	CProgram				&inState,
	CGeneralExpression		*inIndex)
{																		
	// Have we been dimensioned?
	if (mArity==0)
		UTBException::ThrowArrayNotDimensioned();
	
	ArrayIndexT				subIndex=0;
	ArrayIndexT				dimProduct=mArray.GetCount();	// Maximum index possible, also the product of all dimension
															// lengths.
	ArrayIndexT				flatIndex=1;					// 1 is the first index in a PowerPlant array
	
	while (inIndex)
	{
		if (subIndex>(mArity-1))
		{
			// Too many sub indexes for an array of this arity
			UTBException::ThrowArrayIndexWrongArity();
		}

		TTBInteger			subIndexValue=inIndex->GetArithExp()->EvaluateInt(inState);
		
		if ((subIndexValue>=mDimensions[subIndex]) || (subIndexValue<0))
			UTBException::ThrowArrayIndexOutOfBounds();
			
		dimProduct/=mDimensions[subIndex];
		flatIndex+=subIndexValue*dimProduct;
		
		subIndex++;
				
		inIndex=inIndex->TailData();
	}
	
	if (subIndex!=mArity)
	{
		// Too few sub indexes for an array of this arity
		UTBException::ThrowArrayIndexWrongArity();
	}
	
	// Quick sanity check
	Assert_(flatIndex>=1 && flatIndex<=mArray.GetCount());
	
	return flatIndex;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Access														/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Selects an element from the array for read or write. Note it's unlocked memory this reference.
template <class RAWTYPE>
RAWTYPE &CArrayStoreT<RAWTYPE>::Access(
	CProgram				&inState,
	CGeneralExpression		*inIndex)
{	
	ArrayIndexT		flatIndex=CalcFlatIndex(inState,inIndex);

	return mArray[flatIndex];
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Dimension														/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the dimensions of the array. Takes ownership of inDimensions, even if this routine fails.
template <class RAWTYPE>
void CArrayStoreT<RAWTYPE>::Dimension(
	CProgram		&inState,
	ArrayIndexT		inArity,
	ArrayIndexT		*inDimensions)
{
#ifdef DONT_ALLOW_REDIM		
	if (mArity)
	{
		// already dimensioned		
		delete [] inDimensions;
		UTBException::ThrowArrayAlreadyDimensioned();
	}
#endif

	delete [] mDimensions;	// in case already dimmmed

	mDimensions=inDimensions;
	mArity=inArity;
	
	// Parameter check
	UInt32				totalSize=1;
	
	if (!mArity)		UTBException::ThrowNullArrayDimension();

	for (ArrayIndexT arity=0; arity<mArity; arity++)
	{
		if (!inDimensions[arity])
			UTBException::ThrowNullArrayDimension();
		
		totalSize*=inDimensions[arity];
	}
	
	// Allocate the array
	AllocateStorage(inState,totalSize);
}
