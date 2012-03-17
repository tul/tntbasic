// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Data_Statements.h
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

#include			"CStatement.h"
#include			"UExpressions.h"
#include			"CLinkableObject.h"

class				CStrStorage;
class				CIntegerStorage;
class				CArithExpression;
class				CStrExpression;
class				CArithExpressionNode;
class				CIntegerArraySymbol;
class				CStrArraySymbol;
class				CFloatStorage;
class				CFloatArraySymbol;
class				CArraySymbol;
class				CLabelTable;
class				CGeneralExpression;

class CRestoreStatement : public CStatement, public CLinkableObject
{
	protected:
		CCString				mLabelName;
		CLabelTable				*mLabelTable;
		CGeneralExpression		*mData;
		STextRange				mLabelRange;			// we need this so we can report an error if link fails
		
		void /*e*/				ExecuteSelf(
									CProgram		&ioState);
	
	public:
								CRestoreStatement(
									const char	*inStr,
									CProgram	&inProg,
									const STextRange	&inLabelRange);
		
		void /*e*/				Link(
									CProgram	&inProg);
};

class CArrayDimEntry : public CSimpleListNodeT<CArrayDimEntry>
{
	protected:
		CIntegerArraySymbol			*mIntArray;
		CFloatArraySymbol			*mFloatArray;
		CStrArraySymbol				*mStrArray;
		StGenExpression				mDimensions;
		
	public:
									CArrayDimEntry(
										CArraySymbol			*inArraySymbol,
										CGeneralExpression		*inDimensions);

		virtual 					~CArrayDimEntry();
		
		void /*e*/					Dimension(
										CProgram			&ioState);
};

class CDimStatement : public CStatement
{
	protected:
		CArrayDimEntry				*mArrayDims;
	
	public:
									CDimStatement(
										CArrayDimEntry	*inDimEntry) :
										mArrayDims(inDimEntry)
										{}
		virtual						~CDimStatement() { delete mArrayDims; }	

		virtual void /*e*/			ExecuteSelf(
										CProgram		&ioState);
};

class CIntAssignmentStatement : public CStatement
{
	protected:
		CIntegerStorage				*mSymbol;
		CArithExpression			*mValue;

		virtual void /*e*/			ExecuteSelf(
										CProgram	&ioState);
		
	public:
									CIntAssignmentStatement(
										CIntegerStorage		*inSymbol,
										CArithExpression	*inValue) : mSymbol(inSymbol), mValue(inValue) {}		
		virtual						~CIntAssignmentStatement();

};

class CFloatAssignmentStatement : public CStatement
{
	protected:
		CFloatStorage				*mSymbol;
		StArithExpression			mValue;

		virtual void /*e*/			ExecuteSelf(
										CProgram	&ioState);
		
	public:
									CFloatAssignmentStatement(
										CFloatStorage		*inSymbol,
										CArithExpression	*inValue) : mSymbol(inSymbol), mValue(inValue) {}		
};

class CStrAssignmentStatement : public CStatement
{
	protected:
		CStrStorage					*mStorage;
		CStrExpression				*mValue;

		virtual void /*e*/			ExecuteSelf(
										CProgram	&ioState);
		
	public:
									CStrAssignmentStatement(
										CStrStorage			*inSymbol,
										CStrExpression		*inValue) : mStorage(inSymbol), mValue(inValue) {}
		virtual						~CStrAssignmentStatement();
};

class CPrimitiveStorage;

class CReadStatement : public CStatement
{
	protected:
		CPrimitiveStorage			*mStore;
	
	public:
									CReadStatement(
										CPrimitiveStorage	*inStore) : mStore(inStore) {}
		
		virtual void /*e*/			ExecuteSelf(
										CProgram			&ioState);
};