// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Viewport_Statements.h
// © John Treece-Birch 2000
// 20/7/00
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

class CCreateViewportStatement : public CStatement
{
	private:
		StArithExpression		mCanvasID,mLeft,mTop,mRight,mBottom;
		StArithExpression		mLayer;		// layer if using tmaps
		bool					mIsTMAPViewport;

	public:
								CCreateViewportStatement(
									CGeneralExpression	*inExp,
									bool				inTMAP) :
									mIsTMAPViewport(inTMAP)
								{
									mCanvasID=inExp->ReleaseArithExp();

									if (inTMAP)		// in tmap, extract the layer id also
									{
										inExp=inExp->TailData();
										mLayer=inExp->ReleaseArithExp();
									}
									
									inExp=inExp->TailData();
									mLeft=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mTop=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mRight=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mBottom=inExp->ReleaseArithExp();
								}
	
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CRemoveViewportStatement : public CStatement
{
	private:
		StArithExpression		mCanvasID;

	public:
								CRemoveViewportStatement(
									CArithExpression	*inExp)
								{
									mCanvasID=inExp;
								}
	
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CViewportOffsetStatement : public CStatement
{
	private:
		StArithExpression		mCanvasID,mX,mY;

	public:
								CViewportOffsetStatement(
									CGeneralExpression	*inExp)
								{
									mCanvasID=inExp->ReleaseArithExp();
								
									inExp=inExp->TailData();
									mX=inExp->ReleaseArithExp();
								
									inExp=inExp->TailData();
									mY=inExp->ReleaseArithExp();
								}
	
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CGetViewportAttributeFunction : public CFixedLocArithExpression
{
	private:
		StArithExpression		mCanvas;
		int						mAttribute;

	public:
								CGetViewportAttributeFunction(
									CArithExpression	*inExp,
									const STextRange	&inRange,
									int					inAtt) :
									CFixedLocArithExpression(inRange),
									mAttribute(inAtt)
								{
									mCanvas=inExp;
								}
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CGetViewportYOffsetFunction : public CFixedLocArithExpression
{
	private:
		StArithExpression		mCanvas;

	public:
								CGetViewportYOffsetFunction(
									CArithExpression	*inExp,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange)
								{
									mCanvas=inExp;
								}
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CGetViewportWidthFunction : public CFixedLocArithExpression
{
	private:
		StArithExpression		mCanvas;

	public:
								CGetViewportWidthFunction(
									CArithExpression	*inExp,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange)
								{
									mCanvas=inExp;
								}
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CGetViewportHeightFunction : public CFixedLocArithExpression
{
	private:
		StArithExpression		mCanvas;

	public:
								CGetViewportHeightFunction(
									CArithExpression	*inExp,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange)
								{
									mCanvas=inExp;
								}
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};