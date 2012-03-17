// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Music_Statements.h
// © Mark Tully 2000
// 11/4/00
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
#include		"CGeneralExpression.h"

class			CProgram;

class CLoadMusicStatement : public CStatement
{
	protected:
		StArithExpression			mResID;
		StStrExpression				mResName;

		virtual void /*e*/			ExecuteSelf(
										CProgram	&ioState);	
	public:
									CLoadMusicStatement(
										CGeneralExpression		*inParams)
									{
										if (inParams->GetArithExp())
											mResID=inParams->ReleaseArithExp();
										else
											mResName=inParams->ReleaseStrExp();
									}
};

class CStopMusicStatement : public CStatement
{
	protected:
		virtual void /*e*/			ExecuteSelf(
										CProgram	&ioState);	
};

class CPlayMusicStatement : public CStatement
{
	protected:
		virtual void /*e*/			ExecuteSelf(
										CProgram	&ioState);	
};

class CGetMusicLength : public CFixedLocArithExpression
{
	protected:
		TTBInteger /*e*/			EvaluateInt(
										CProgram	&ioState);	

	public:
									CGetMusicLength(
										const STextRange	&inPos) :
										CFixedLocArithExpression(inPos)
										{}
};

class CGetMusicPlayPosition : public CFixedLocArithExpression
{
	protected:
		TTBInteger /*e*/			EvaluateInt(
										CProgram	&ioState);	

	public:
									CGetMusicPlayPosition(
										const STextRange	&inPos) :
										CFixedLocArithExpression(inPos)
										{}
};

class CSetMusicPlayPosition : public CStatement
{
	protected:
		CArithExpression			*mExp;
		
		virtual void /*e*/			ExecuteSelf(
										CProgram	&ioState);	
	
	public:
									CSetMusicPlayPosition(
										CArithExpression	*inExp) : mExp(inExp) {}
		virtual						~CSetMusicPlayPosition();
};

class CMusicVolumeFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger /*e*/			EvaluateInt(
										CProgram	&ioState);	

	public:
									CMusicVolumeFunction(
										const STextRange	&inPos) :
										CFixedLocArithExpression(inPos)
										{}
};

class CSetMusicVolumeStatement : public CStatement
{
	protected:
		StArithExpression			mExp;
		
		virtual void /*e*/			ExecuteSelf(
										CProgram	&ioState);	
	
	public:
									CSetMusicVolumeStatement(
										CArithExpression	*inExp) : mExp(inExp) {}
};

class CSetMusicLoopStatement : public CStatement
{
	protected:
		bool						mOn;
		
		virtual void /*e*/			ExecuteSelf(
										CProgram	&ioState);	
	
	public:
									CSetMusicLoopStatement(
										bool		inOn) : mOn(inOn) {}
};

class CMusicPlayingFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger /*e*/			EvaluateInt(
										CProgram	&ioState);	

	public:
									CMusicPlayingFunction(
										const STextRange	&inPos) :
										CFixedLocArithExpression(inPos)
										{}
};
