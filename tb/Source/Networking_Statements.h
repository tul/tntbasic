// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Networking_Statements.h
// © John Treece-Birch 2000
// 19/3/00
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

class CVariableSymbol;

class CInitNetworkingStatement : public CStatement
{
	protected:
	
		void				ExecuteSelf(
								CProgram			&ioState);
};

class CSetNetworkingNormalStatement : public CStatement
{
	protected:
		void				ExecuteSelf(
								CProgram			&ioState);
};

class CSetNetworkingRegisteredStatement : public CStatement
{
	protected:
		void				ExecuteSelf(
								CProgram			&ioState);
};

class CHostNetworkFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mMaxPlayers;
		StStrExpression			mGameName,mPlayerName;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CHostNetworkFunction(
									CGeneralExpression		*inExp,
									const STextRange		&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mMaxPlayers=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mGameName=inExp->ReleaseStrExp();
									
									inExp=inExp->TailData();
									mPlayerName=inExp->ReleaseStrExp();
								}
};

class CJoinNetworkFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression		mIPAddress;
	
		TTBInteger			EvaluateInt(
								CProgram			&ioState);

	public:
							CJoinNetworkFunction(
								CGeneralExpression		*inExp,
								const STextRange		&inPos) :
								CFixedLocArithExpression(inPos)
							{
								if (inExp)
								{
									mIPAddress=inExp->ReleaseStrExp();
								}
							}
};

class CUpdateNetDataStatement : public CStatement
{
	protected:
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSetNetDataStatement : public CStatement
{
	protected:
		CVariableSymbol			*mSymbol;
		CGeneralExpression		*mExpression;
		CArithExpression		*mNum;
		CStrExpression			*mString;
		
		virtual void			ExecuteSelf(
									CProgram			&ioState);
									
	public:		
								CSetNetDataStatement(
									CVariableSymbol		*inSymbol,
									CArithExpression	*inValue,
									CGeneralExpression	*inExpression=NULL)
								{
									mSymbol=inSymbol;
									mExpression=inExpression;
									mNum=inValue;
									mString=NULL;
								}
								
								CSetNetDataStatement(
									CVariableSymbol		*inSymbol,
									CStrExpression		*inValue,
									CGeneralExpression	*inExpression=NULL)
								{
									mSymbol=inSymbol;
									mExpression=inExpression;
									mNum=NULL;
									mString=inValue;
								}
};

class CMoreNetEventsFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CMoreNetEventsFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CGetNetEventTypeFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetNetEventTypeFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CGetNetEventContentFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetNetEventContentFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CCountNetPlayersFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CCountNetPlayersFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CGetNetIDFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetNetIDFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CGetNetPlayerNameFunction : public CFixedLocStrExpression
{
	protected:
		StArithExpression		mID;
	
		CCString				&Evaluate(
									CProgram			&ioState);
									
	public:
								CGetNetPlayerNameFunction(
									const STextRange	&inRange,
									CArithExpression	*inID) :
									mID(inID),
									CFixedLocStrExpression(inRange)
									{}
};

class CLeaveNetGameStatement : public CStatement
{
	public:
		virtual void			ExecuteSelf(CProgram &inState);
};

class CNetGameTerminatedFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CNetGameTerminatedFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CNetPlayerLeftFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CNetPlayerLeftFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CNetPlayerDisconnectedFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CNetPlayerDisconnectedFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CNetConnectionLostFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CNetConnectionLostFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};