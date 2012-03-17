// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Image_Statements.h
// © Mark Tully 2001
// 15/3/01
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2001, Mark Tully and John Treece-Birch
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
#include	"CGeneralExpression.h"
#include	"CArithExpression.h"

class CBLMask16;
class CBLCanvas;

class CPasteImageStatement : public CStatement
{
	protected:
		StArithExpression		mImage,mX,mY,mBank;
	
	public:
								CPasteImageStatement(
									CGeneralExpression	*inExpression)
								{
									if (inExpression->Length()==4)
									{
										mX=inExpression->ReleaseArithExp();
									
										inExpression=inExpression->TailData();
										mY=inExpression->ReleaseArithExp();
									
										inExpression=inExpression->TailData();
										mImage=inExpression->ReleaseArithExp();
								
										inExpression=inExpression->TailData();
										mBank=inExpression->ReleaseArithExp();
									}
									else
									{
										mX=inExpression->ReleaseArithExp();
									
										inExpression=inExpression->TailData();
										mY=inExpression->ReleaseArithExp();
									
										inExpression=inExpression->TailData();
										mImage=inExpression->ReleaseArithExp();
									}
								}
									
		virtual					~CPasteImageStatement()
									{};
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CCountImagesFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mBank;
	
	public:
								CCountImagesFunction(
									CArithExpression	*inBank,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mBank(inBank)
									{}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CSetImagePropStatement : public CStatement
{
	protected:
		StArithExpression		mBank,mImage,mNewX,mNewY;
		int						mProp;
	
	public:
								CSetImagePropStatement(
									int					inProp,
									CGeneralExpression	*inExpression);

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CGetImagePropFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mBank;
		StArithExpression		mImage;
		int						mProp;
	
	public:
								CGetImagePropFunction(
									int					inProp,
									CArithExpression	*inBank,
									CArithExpression	*inImage,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mBank(inBank),
									mImage(inImage),
									mProp(inProp)
									{}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CCopyImageStatement : public CStatement
{
	protected:
		bool					mUseGSMask;
		StArithExpression		mImage,mX,mY,mX2,mY2,mBank,mColour,mMX,mMY,mShrinkToContents,mMCanvas;
	
	public:
		// Can't determine from the num params itself whether we're supposed to be using an alpha mask or not so we need
		// a param
								CCopyImageStatement(
									CGeneralExpression	*inExpression,
									bool				inUseAlpha) :
									mUseGSMask(inUseAlpha)
								{
									if (inUseAlpha)
									{
										mX=inExpression->ReleaseArithExp();
									
										inExpression=inExpression->TailData();
										mY=inExpression->ReleaseArithExp();
									
										inExpression=inExpression->TailData();
										mX2=inExpression->ReleaseArithExp();
								
										inExpression=inExpression->TailData();
										mY2=inExpression->ReleaseArithExp();

										inExpression=inExpression->TailData();
										mMCanvas=inExpression->ReleaseArithExp();

										inExpression=inExpression->TailData();
										mMX=inExpression->ReleaseArithExp();

										inExpression=inExpression->TailData();
										mMY=inExpression->ReleaseArithExp();

										inExpression=inExpression->TailData();
										mShrinkToContents=inExpression->ReleaseArithExp();
										
										inExpression=inExpression->TailData();
										mImage=inExpression->ReleaseArithExp();

										inExpression=inExpression->TailData();
										if (inExpression)
											mBank=inExpression->ReleaseArithExp();
									}
									else
									{
										mX=inExpression->ReleaseArithExp();
									
										inExpression=inExpression->TailData();
										mY=inExpression->ReleaseArithExp();
									
										inExpression=inExpression->TailData();
										mX2=inExpression->ReleaseArithExp();
								
										inExpression=inExpression->TailData();
										mY2=inExpression->ReleaseArithExp();

										inExpression=inExpression->TailData();
										mColour=inExpression->ReleaseArithExp();

										inExpression=inExpression->TailData();
										mShrinkToContents=inExpression->ReleaseArithExp();
										
										inExpression=inExpression->TailData();
										mImage=inExpression->ReleaseArithExp();

										inExpression=inExpression->TailData();
										if (inExpression)
											mBank=inExpression->ReleaseArithExp();
									}
								}
									
		virtual CBLMask16		*GetMask(
									CProgram	&ioState,
									CBLCanvas	&inCanvas,
									TTBInteger	inW,
									TTBInteger	inH);
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CSetImageCollisionStatement : public CStatement
{
	protected:
		StArithExpression		mTolerance,mType,mImage,mBank;
	
	public:
								CSetImageCollisionStatement(
									CGeneralExpression *inExpression)
								{
									mTolerance=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mType=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mImage=inExpression->ReleaseArithExp();

									inExpression=inExpression->TailData();
									if (inExpression)
										mBank=inExpression->ReleaseArithExp();
								}
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CDeleteImageStatement : public CStatement
{
	protected:
		StArithExpression		mImage,mBank;
	
	public:
								CDeleteImageStatement(
									CGeneralExpression *inExpression)
								{
									mImage=inExpression->ReleaseArithExp();

									inExpression=inExpression->TailData();
									if (inExpression)
										mBank=inExpression->ReleaseArithExp();
								}
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CLoadImagesStatement : public CStatement
{
	protected:
		StArithExpression		mResID;
		StStrExpression			mResName;
	
	public:
								CLoadImagesStatement(
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

class CUnloadImagesStatement : public CStatement
{
	protected:
		StArithExpression		mResID;
		StStrExpression			mResName;
	
	public:
								CUnloadImagesStatement(
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
