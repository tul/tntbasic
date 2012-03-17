// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Button_Statements.h
// © John Treece-Birch 2000
// 12/7/00
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

#include	"CStatement.h"
#include	"CArithExpression.h"
#include	"UExpressions.h"
#include	"CGeneralExpression.h"
#include	"UButtonManager.h"

class CPollButtonClickFunction : public CFixedLocArithExpression
{
	public:
								CPollButtonClickFunction(
									const STextRange	&inRange) : CFixedLocArithExpression(inRange)
								{}
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CWaitButtonClickHeadStatement : public CStatement
{
	public:
								CWaitButtonClickHeadStatement();
	
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CWaitButtonClickTailStatement : public CStatement
{
	public:
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CGetButtonClickFunction : public CFixedLocArithExpression
{
	public:
		static TTBInteger		sButton;
	
								CGetButtonClickFunction(
									const STextRange	&inRange) : CFixedLocArithExpression(inRange)
								{}
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState)
								{ return sButton; }
};

class CButtonTracker
{
	public:
		static TTBInteger		sTrackThis,sHiliteThis;
		static bool				sWasPressed;
	
		static void				GetButtonClick(
									CProgram		&ioState);
};

class CNewButtonStatement : public CStatement
{
	protected:
		StArithExpression		mIndex,mX,mY,mImage,mOverImage,mDownImage,mBank;

	public:
								CNewButtonStatement(
									CGeneralExpression	*inExp)
								{
									if (inExp->Length()==7)
									{
										mIndex=inExp->ReleaseArithExp();
									
										inExp=inExp->TailData();
										mX=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mY=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mImage=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mOverImage=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mDownImage=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mBank=inExp->ReleaseArithExp();
									}
									else
									{
										mIndex=inExp->ReleaseArithExp();
									
										inExp=inExp->TailData();
										mX=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mY=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mImage=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mOverImage=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mDownImage=inExp->ReleaseArithExp();
									}
								}
	
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CClearButtonsStatement : public CStatement
{	
	public:
	
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};