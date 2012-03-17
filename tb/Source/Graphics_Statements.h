// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Graphics_Statements.h
// © Mark Tully 2000
// 1/1/00
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
#include		"Blast Layer Manager.h"
#include		"UExpressions.h"
#include		"CGeneralExpression.h"
#include		"CArithExpression.h"

class CSetVBLSyncStatement : public CStatement
{
	protected:
		bool					mState;
	
	public:
								CSetVBLSyncStatement(
									bool		inNewState) : mState(inNewState) {}
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CSetFrameRateStatement : public CStatement
{
	protected:
		StArithExpression		mFrameRate;
	
	public:
								CSetFrameRateStatement(
									CArithExpression	*inRate) : mFrameRate(inRate) {}
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};
	
class CSyncScreenStatement : public CStatement
{
	public:
		void /*e*/				ExecuteSelf(
									CProgram			&ioState);
};

class CGraphicsModeStatement : public CStatement
{
	protected:
		StArithExpression		mWidth,mHeight;
		bool					mGoFullScreen;
		
	public:
								CGraphicsModeStatement(
									CGeneralExpression	*inExpression,
									bool				inGoFullScreen) :
									mGoFullScreen(inGoFullScreen)
								{								
									mWidth=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mHeight=inExpression->ReleaseArithExp();
								}
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CGraphicsModeHardwareStatement : public CStatement
{
	protected:
		StArithExpression		mWidth,mHeight;
		bool					mGoFullScreen;
		
	public:
								CGraphicsModeHardwareStatement(
									CGeneralExpression	*inExpression,
									bool				inGoFullScreen) :
									mGoFullScreen(inGoFullScreen)
								{								
									mWidth=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mHeight=inExpression->ReleaseArithExp();
								}
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CTextModeStatement : public CStatement
{
	public:
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CSetDefaultSpriteBankStatement : public CStatement
{
	protected:
		StArithExpression		mResID;
		StStrExpression			mResName;
		
	public:
								CSetDefaultSpriteBankStatement(
									CGeneralExpression	*inExp)
								{
									if (inExp->GetArithExp())
										mResID=inExp->ReleaseArithExp();
									else
										mResName=inExp->ReleaseStrExp();
								}
									
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class COpenCanvasStatement : public CStatement
{
	protected:
		StArithExpression			mCanvasIndex,mWidth,mHeight;
	
	public:
									COpenCanvasStatement(
										CGeneralExpression	*inExp)
									{
										mCanvasIndex=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mWidth=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mHeight=inExp->ReleaseArithExp();
									}
										
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CCloseCanvasStatement : public CStatement
{
	protected:
		StArithExpression			mIndex;
	
	public:
									CCloseCanvasStatement(
										CGeneralExpression	*inExp)
									{
										mIndex=inExp ? inExp->ReleaseArithExp() : 0L;
									}
										
		virtual void /*e*/			ExecuteSelf(
										CProgram			&ioState);										
};

class CTargetCanvasStatement : public CStatement
{
	protected:
		StArithExpression			mIndex;
	
	public:
									CTargetCanvasStatement(
										CArithExpression	*inCanvas) :
										mIndex(inCanvas)
										{}
		virtual void /*e*/			ExecuteSelf(
										CProgram			&ioState);										
};

class CConstColourFunction : public CFixedLocArithExpression
{
	protected:
		int						mColourToken;
	
	public:
								CConstColourFunction(
									int					inToken,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mColourToken(inToken)
									{}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};
