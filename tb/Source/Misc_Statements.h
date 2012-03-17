// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Misc_Statements.h
// © John Treece-Birch 2000
// 21/3/00
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
#include		"CArithExpression.h"
#include		"CStrExpression.h"
#include		"UExpressions.h"

class CEndProgramStatement : public CStatement
{
	protected:
		void				ExecuteSelf(
								CProgram			&ioState);
};

class CGetTimerFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger			EvaluateInt(
								CProgram			&ioState);

	public:
							
							CGetTimerFunction(
								const STextRange		&inPos) :
								CFixedLocArithExpression(inPos)
								{}
};

class CDelayHeadStatement : public CStatement
{
	protected:
		StArithExpression	mValue;
	
		void				ExecuteSelf(
								CProgram			&ioState);

	public:
							CDelayHeadStatement(
								CArithExpression		*inValue);
};

class CDelayTailStatement : public CStatement
{
	protected:
		void				ExecuteSelf(
								CProgram			&ioState);

	public:
		static UInt32		sEndDelayTime;
};

class CDisableBreakStatement : public CStatement
{
	protected:
		void				ExecuteSelf(
								CProgram			&ioState);
};

class CCountResourcesFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression		mString;
	
		TTBInteger			EvaluateInt(
								CProgram			&ioState);
								
	public:
							CCountResourcesFunction(
								CStrExpression			*inExp,
								const STextRange		&inPos) :
								CFixedLocArithExpression(inPos),
								mString(inExp)
								{}
};

class CNthResourceIDFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression		mString;
		StArithExpression	mID;
	
		TTBInteger			EvaluateInt(
								CProgram			&ioState);
								
	public:
							CNthResourceIDFunction(
								CGeneralExpression		*inExp,
								const STextRange		&inPos) :
								CFixedLocArithExpression(inPos)
							{
								mString=inExp->ReleaseStrExp();
								
								inExp=inExp->TailData();
								mID=inExp->ReleaseArithExp();
							}
};

class CResourceExistsFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression		mString,mName;
		StArithExpression	mID;
	
		TTBInteger			EvaluateInt(
								CProgram			&ioState);
								
	public:
							CResourceExistsFunction(
								CGeneralExpression		*inExp,
								const STextRange		&inPos) :
								CFixedLocArithExpression(inPos)
							{
								mString=inExp->ReleaseStrExp();
								
								inExp=inExp->TailData();
								if (inExp->GetArithExp())
									mID=inExp->ReleaseArithExp();
								else
									mName=inExp->ReleaseStrExp();
							}
};

class CGetResourceNameFunction : public CFixedLocStrExpression
{
	protected:
		StStrExpression		mResType;
		StArithExpression	mID;
	
		CCString			&Evaluate(
								CProgram			&ioState);
								
	public:
							CGetResourceNameFunction(
								const STextRange	&inRange,
								CGeneralExpression	*inExp) :
								CFixedLocStrExpression(inRange)
							{
								mResType=inExp->ReleaseStrExp();
								
								inExp=inExp->TailData();
								mID=inExp->ReleaseArithExp();
							}
};

class CStringLengthFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression		mString;
	
		TTBInteger			EvaluateInt(
								CProgram			&ioState);
								
	public:
							CStringLengthFunction(
								const STextRange	&inRange,
								CGeneralExpression	*inExp) :
								CFixedLocArithExpression(inRange)
							{
								mString=inExp->ReleaseStrExp();
							}
};

