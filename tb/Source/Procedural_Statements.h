// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Procedural_Statements.h
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

#include		"CStatement.h"
#include		"CCString.h"
#include		"CLinkableObject.h"
#include		"UExpressions.h"
#include		"BisonTokenLocation.h"
#include		"CStrExpression.h"

class CVariableNode : public CSimpleListNodeT<CVariableNode>
{
	public:
		CVariableSymbol		*mSymbol;
		CStatement			*mInitStatement;

							CVariableNode(
								CVariableSymbol		*inSymbol) :
								CSimpleListNodeT<CVariableNode>(this),
								mSymbol(inSymbol),
								mInitStatement(0)
								{}
		
		virtual				~CVariableNode();
};

class CArithProcResult : public CFixedLocArithExpression
{
	public:
							CArithProcResult(
								const STextRange &inRange,
								bool		inGetFloat) :
								CFixedLocArithExpression(inRange,inGetFloat)
								{}
		TTBInteger /*e*/	EvaluateInt(
								CProgram	&ioProgram);
		TTBFloat /*e*/		EvaluateFloat(
								CProgram	&ioProgram);
};

class CStrProcResult : public CFixedLocStrExpression
{
	public:
							CStrProcResult(
								const STextRange &inRange) :
								CFixedLocStrExpression(inRange)
								{}
		CCString /*e*/		&Evaluate(
								CProgram			&inState);
};

class			CProcedure;

class CCallProc : public CStatement, public CLinkableObject
{
	protected:
		CCString			mProcName;
		CProcedure			*mResolvedProc;
		StGenExpression		mParameters;
		SBisonTokenLoc		mParamListTerminatorLoc;

		virtual void /*e*/	ExecuteSelf(
								CProgram	&ioState);
	
	public:
		/*e*/				CCallProc(
								CProgram				*inProgram,
								char					*inProcName,
								CGeneralExpression		*inExp,
								const SBisonTokenLoc	&inParamListTerminator);
		
		CCString			&GetProcName()		{ return mProcName; }
		
		virtual void /*e*/	Link(
								CProgram	&inState);								
};

class						CArithExpression;
class						CStrExpression;

class CEndProcStatement : public CStatement
{
	protected:
		StArithExpression	mArith;
		StStrExpression		mStr;
	
		virtual void /*e*/	ExecuteSelf(
								CProgram	&ioState);
	
	public:
							CEndProcStatement(
								CArithExpression	*inArith,
								CStrExpression		*inStr) :
								mArith(inArith),
								mStr(inStr)
								{}
};