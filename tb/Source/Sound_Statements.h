// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Sound_Statements.h
// © John Treece-Birch 2000
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
#include		"CArithExpression.h"
#include		"UExpressions.h"
#include		"UKaboom.h"
#include		"CGeneralExpression.h"

class CBeepStatement : public CStatement
{
	protected:
		StArithExpression	mBeeps;
	
		void				ExecuteSelf(
								CProgram			&ioState);

	public:
							CBeepStatement(
								CArithExpression	*inBeeps)
							{
								mBeeps=inBeeps;
							}
};

class CUnloadSoundsStatement : public CStatement
{
	protected:
		void				ExecuteSelf(
								CProgram			&ioState);
	public:
							CUnloadSoundsStatement() {}
};
	

class CPlaySoundStatement : public CStatement
{
	protected:
		StArithExpression	mSound,mBalance,mVolume,mPriority;
	
		void				ExecuteSelf(
								CProgram			&ioState);

	public:
							CPlaySoundStatement(
								CGeneralExpression		*inExpression)
							{
								int index=0;
								
								// we have a list of maybe arith exps. We copy each expression into the appropiate
								// data member depending on it's index in the list
								for (CGeneralExpression *exp=inExpression; exp; exp=exp->TailData())
								{
									switch (index++)
									{
										case 0: mSound=exp->ReleaseArithExp(); break;
										case 1: mBalance=exp->ReleaseArithExp(); break;
										case 2: mVolume=exp->ReleaseArithExp(); break;
										case 3: mPriority=exp->ReleaseArithExp(); break;
									}
								}
							}
};

class CLoopSoundStatement : public CStatement
{
	protected:
		StArithExpression	mSound,mBalance,mVolume;
	
		void				ExecuteSelf(
								CProgram			&ioState);

	public:
							CLoopSoundStatement(
								CGeneralExpression		*inExpression)
							{
								int index=0;
								
								// we have a list of maybe arith exps. We copy each expression into the appropiate
								// data member depending on it's index in the list
								for (CGeneralExpression *exp=inExpression; exp; exp=exp->TailData())
								{
									switch (index++)
									{
										case 0: mSound=exp->ReleaseArithExp(); break;
										case 1: mBalance=exp->ReleaseArithExp(); break;
										case 2: mVolume=exp->ReleaseArithExp(); break;
									}
								}
							}
};

class CSoundResultFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger			EvaluateInt(
								CProgram			&ioState);

	public:
							CSoundResultFunction(
								const STextRange		&inRange) :
								CFixedLocArithExpression(inRange)
							{
							}
};

class CStopSoundStatement : public CStatement
{
	protected:
		StArithExpression	mSound;
	
		void /*e*/			ExecuteSelf(
								CProgram			&ioState);

	public:
							CStopSoundStatement(
								CArithExpression	*inSound)
							{
								mSound=inSound;
							}
};

class CStopChannelStatement : public CStatement
{
	protected:
		StArithExpression	mChannel;
	
		void /*e*/			ExecuteSelf(
								CProgram			&ioState);

	public:
							CStopChannelStatement(
								CArithExpression	*inChannel)
							{
								mChannel=inChannel;
							}
};

class CLoadSoundsStatement : public CStatement
{
	protected:
		StArithExpression	mResID;
		StStrExpression		mResName;
	
		void /*e*/			ExecuteSelf(
								CProgram			&ioState);

	public:
							CLoadSoundsStatement(
								CGeneralExpression	*inExp)
							{
								if (inExp->GetArithExp())
									mResID=inExp->ReleaseArithExp();
								else
									mResName=inExp->ReleaseStrExp();
							}
};

class CSoundPlayingFunction : public CFixedLocArithExpression
{
	private:
		StArithExpression		mSound;

	public:
								CSoundPlayingFunction(
									CArithExpression	*inExp,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange)
								{
									mSound=inExp;
								}
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CChannelPlayingFunction : public CFixedLocArithExpression
{
	private:
		StArithExpression		mChannel;

	public:
								CChannelPlayingFunction(
									CArithExpression	*inExp,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange)
								{
									mChannel=inExp;
								}
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CMoveSoundStatement : public CStatement
{
	protected:
		StArithExpression	mSound,mBalance,mVolume;
	
		void /*e*/			ExecuteSelf(
								CProgram			&ioState);

	public:
							CMoveSoundStatement(
								CGeneralExpression		*inExpression)
							{
								mSound=inExpression->ReleaseArithExp();
	
								inExpression=inExpression->TailData();
								mBalance=inExpression->ReleaseArithExp();
								
								inExpression=inExpression->TailData();
								mVolume=inExpression->ReleaseArithExp();
							}
};

class CMoveChannelStatement : public CStatement
{
	protected:
		StArithExpression	mChannel,mBalance,mVolume;
	
		void /*e*/			ExecuteSelf(
								CProgram			&ioState);

	public:
							CMoveChannelStatement(
								CGeneralExpression		*inExpression)
							{
								mChannel=inExpression->ReleaseArithExp();
	
								inExpression=inExpression->TailData();
								mBalance=inExpression->ReleaseArithExp();
								
								inExpression=inExpression->TailData();
								mVolume=inExpression->ReleaseArithExp();
							}
};

class CSoundVolumeFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CSoundVolumeFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CSetSoundVolumeStatement : public CStatement
{
	protected:
		StArithExpression	mValue;
	
		void /*e*/			ExecuteSelf(
								CProgram			&ioState);

	public:
							CSetSoundVolumeStatement(
								CGeneralExpression		*inExpression)
							{
								mValue=inExpression->ReleaseArithExp();
							}
};