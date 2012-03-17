// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CForStatement.h
// © Mark Tully 1999
// 26/12/99
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

#include				"TNTBasic_Types.h"
#include				"CStatementBlock.h"
#include				"UExpressions.h"

class					CIntegerStorage;
class					CVariableSymbolTable;
class					CArithExpression;
class					CIntegerSymbol;

class CForStatement : public CStatementBlock
{
	protected:
		CIntegerStorage				*mIndex;
		StArithExpression			mStart,mEnd,mStep;
		class CForTestStatement		*mForTestStatement;

	public:
		/*e*/						CForStatement(
										CIntegerStorage		*inIndex,
										CArithExpression	*inStart,
										CArithExpression	*inEnd,
										CArithExpression	*inStep);
		
		void						SetSubStatements(
										CVariableSymbolTable &inScope,
										CStatement	*inSubStatements);
		CIntegerStorage				*GetIndexStorage()		{ return mIndex; }
		virtual void /*e*/			ExecuteSelf(
										CProgram		&ioState);
};

class CForTestStatement : public CStatement
{
	protected:
		CArithExpression			*mEndRange;				// end range is "owned" by the parent CForStatement & shouldn't be deleted by this object
		CIntegerStorage				*mIndex;
		CIntegerSymbol				*mSymbol;
	
	public:
									CForTestStatement(
										CVariableSymbolTable	&ioScope,
										CIntegerStorage		*inSymbol,
										CArithExpression	*inEndValue);
		
		void						FixStartValue(
										CProgram			&ioState,
										TTBInteger			inValue);
										
		virtual void /*e*/			ExecuteSelf(
										CProgram		&ioState);
};

class CForNextStatement : public CStatement
{
	protected:
		CArithExpression			*mStep;					// step is "owned" by the parent CForStatement & shouldn't be deleted
		CStatement					*mTest;
		CIntegerStorage				*mIndex;
		
	public:
									CForNextStatement(
										CIntegerStorage		*inIndex,
										CArithExpression	*inStep,
										CStatement			*inTest) : mStep(inStep), mTest(inTest), mIndex(inIndex) {}
		
		virtual void /*e*/			ExecuteSelf(
										CProgram		&ioState);
};