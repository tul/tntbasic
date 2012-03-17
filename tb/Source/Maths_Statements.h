// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Maths_Statements.h
// © John Treece-Birch 2000
// 22/5/00
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
#include		"UExpressions.h"
#include		"CGeneralExpression.h"

class CAbsFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CAbsFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CModFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mFirst,mSecond;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CModFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mFirst=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mSecond=inExp->ReleaseArithExp();
									
									mIsFloat=mFirst->IsFloat() || mSecond->IsFloat();
								}
};

class CSquareRootFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CSquareRootFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CSinFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CSinFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CCosFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CCosFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CTanFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CTanFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CInvSinFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CInvSinFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CInvCosFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CInvCosFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CInvTanFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CInvTanFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CRadSinFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CRadSinFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CRadCosFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CRadCosFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CRadTanFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CRadTanFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CInvRadSinFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CInvRadSinFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CInvRadCosFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CInvRadCosFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CInvRadTanFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CInvRadTanFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CRadToDegFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CRadToDegFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CDegToRadFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CDegToRadFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CCeilFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CCeilFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CRoundFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CRoundFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CFloorFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CFloorFunction(
									CArithExpression		*inValue,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true),
									mValue(inValue)
									{}
};

class CRandomFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mLower,mHigher;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CRandomFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mLower=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mHigher=inExp->ReleaseArithExp();
								}									
};

class CSetRandomSeedStatement : public CStatement
{
	protected:
		StArithExpression	mValue;
	
		void				ExecuteSelf(
								CProgram			&ioState);

	public:
							CSetRandomSeedStatement(
								CArithExpression		*inValue);
};

class CMaxFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValueA,mValueB;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CMaxFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true)
								{
									mValueA=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mValueB=inExp->ReleaseArithExp();
								}
};

class CMinFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValueA,mValueB;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CMinFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true)
								{
									mValueA=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mValueB=inExp->ReleaseArithExp();
								}
};

class CWrapFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue,mMinValue,mMaxValue;
	
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CWrapFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos,true)
								{
									mValue=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mMinValue=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mMaxValue=inExp->ReleaseArithExp();
								}
};

class CBitAndFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValueA,mValueB;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CBitAndFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mValueA=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mValueB=inExp->ReleaseArithExp();
								}
};

class CBitOrFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValueA,mValueB;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CBitOrFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mValueA=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mValueB=inExp->ReleaseArithExp();
								}
};

class CBitXorFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValueA,mValueB;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CBitXorFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mValueA=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mValueB=inExp->ReleaseArithExp();
								}
};

class CBitSetFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue,mBit;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CBitSetFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mValue=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mBit=inExp->ReleaseArithExp();
								}
};

class CBitShiftFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue,mShift;
		
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
	
	public:
								CBitShiftFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mValue=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									mShift=inExp->ReleaseArithExp();
								}
};

class CSetBitFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mValue,mBit,mBitValue;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CSetBitFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mValue=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mBit=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mBitValue=inExp->ReleaseArithExp();
								}
};

class CAngleDifferenceFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mFirst,mSecond;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CAngleDifferenceFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mFirst=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mSecond=inExp->ReleaseArithExp();
									
									mIsFloat=mFirst->IsFloat() || mSecond->IsFloat();
								}
};

class CCalculateAngleFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mStartX,mStartY,mDestX,mDestY;

		TTBFloat				EvaluateFloat(
									CProgram			&ioState);

	public:
								CCalculateAngleFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mStartX=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mStartY=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mDestX=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mDestY=inExp->ReleaseArithExp();
									
									mIsFloat=true;
								}
};