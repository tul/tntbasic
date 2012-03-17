// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Procedural_Statements.cpp
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

#include	"TNT_Debugging.h"
#include	"Procedural_Statements.h"
#include	"CStackFrame.h"
#include	"CProgram.h"
#include	"CProcedure.h"
#include	"CArithExpression.h"
#include	"CStrExpression.h"
#include	"UFunctionMgr.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊCStrProcResult::Evaluate										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CCString &CStrProcResult::Evaluate(
	CProgram			&inState)
{
	return inState.CurrentStackFrame()->GetResultStr();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊCArithProcResult::EvaluateInt									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CArithProcResult::EvaluateInt(
	CProgram	&ioProgram)
{
	return ioProgram.CurrentStackFrame()->GetResultInt();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊCArithProcResult::EvaluateFloat								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBFloat CArithProcResult::EvaluateFloat(
	CProgram	&ioProgram)
{
	return ioProgram.CurrentStackFrame()->GetResultFloat();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊCCallProc::Constructor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CCallProc::CCallProc(
	CProgram				*inProgram,
	char					*inProcName,
	CGeneralExpression		*inExp,
	const SBisonTokenLoc	&inParamListTerminator) :
	mProcName(inProcName),
	mParameters(inExp),
	CLinkableObject(*inProgram),
	mResolvedProc(0),
	mParamListTerminatorLoc(inParamListTerminator)
{
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊCCallProc::ExecuteSelf										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Create a new stack frame back to the current pc and then redirect pc into the procedure
void CCallProc::ExecuteSelf(
	CProgram	&ioState)
{
	AssertThrow_(mResolvedProc);

	CStackFrame	*frame;
	
	Try_
	{
		frame=new CStackFrame(CStackFrame::kProcedure,ioState.GetPC(),ioState.GetDC(),mResolvedProc->mIdentifierScope);
		ThrowIfMemFull_(frame);
	}
	Catch_(err)
	{
		if (ErrCode_(err)==memFullErr)
			UTBException::ThrowStackOverflow();
		throw;
	}
	
	// Evaluate the parameters and copy the results into the new stack frame
	CCString			*tempStr;
	TTBInteger			tempInt;
	TTBFloat			tempFloat;
	CVariableNode		*storage=mResolvedProc->GetParameters();
	CGeneralExpression	*exp=mParameters;
		
	while (storage)
	{
		// Evaluate under old stack frame
		if (exp->GetArithExp())
		{
			if (exp->GetArithExp()->IsFloat())
			{
				tempFloat=exp->GetArithExp()->EvaluateFloat(ioState);
				tempInt=tempFloat;
			}
			else
			{
				tempInt=exp->GetArithExp()->EvaluateInt(ioState);
				tempFloat=tempInt;
			}
		}
		else
			tempStr=&exp->GetStrExp()->Evaluate(ioState);
		
		// Push new stack frame and store results
		ioState.PushStackFrame(frame);

		// type checking was done at link time, all we need to do is copy the values...
		switch (storage->mSymbol->GetType())
		{
			case CVariableSymbol::kInteger:
				static_cast<CIntegerSymbol*>(storage->mSymbol)->SetValue(ioState,tempInt);
				break;

			case CVariableSymbol::kFloat:
				static_cast<CFloatSymbol*>(storage->mSymbol)->SetValue(ioState,tempFloat);
				break;
			
			case CVariableSymbol::kString:
				static_cast<CStrSymbol*>(storage->mSymbol)->Access(ioState).Assign(tempStr->mString);
				break;
		}

		// Pop it for the next parameter evaluation
		ioState.PopStackFrame();
	
		exp=exp->TailData();
		storage=storage->TailData();
	}
	
	ioState.PushStackFrame(frame);
	
	ioState.SetPC(mResolvedProc->GetCode());
	ioState.SetDC(mResolvedProc->mIdentifierScope.DataStatements());
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊCCallProc::Link												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Look up the procedure name in the programs procedure list and check the parameters are of the correct type
void CCallProc::Link(
	CProgram	&inState)
{
	Try_
	{
		mResolvedProc=inState.ResolveProc(mProcName);
		AssertThrow_(mResolvedProc);
		
		// Create a simple type check parameter template
		CCString		typeTemplate;
		
		for (CVariableNode *storage=mResolvedProc->GetParameters(); storage; storage=storage->TailData())
		{
			switch (storage->mSymbol->GetType())
			{
				case CVariableSymbol::kInteger:
				case CVariableSymbol::kFloat:
					if (!typeTemplate.IsNull())
						typeTemplate+=",a";
					else
						typeTemplate+="a";
					break;
					
				case CVariableSymbol::kString:
					if (!typeTemplate.IsNull())
						typeTemplate+=",s";
					else
						typeTemplate+="s";
					break;
					
				default:
					Throw_(-1);	// somethings gone wrong here...
					break;
			}
		}
	
		UFunctionMgr::SimpleTypeCheck(typeTemplate,mParameters,mParamListTerminatorLoc);
	}
	Catch_(err)
	{
		inState.LogError(err);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊCEndProcStatement::ExecuteSelf								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Redirects the program flow to the return address of the last stack frame and then deletes that frame, propagating it's
// result values back.
void CEndProcStatement::ExecuteSelf(
	CProgram	&ioState)
{
	// Current frame is used to evaluate the return values, these are then stored in the next frame in the stack
	CStackFrame	*currentFrame=ioState.CurrentStackFrame();
	CStackFrame	*newFrame=currentFrame->GetNextElement() ? currentFrame->GetNextElement()->GetData() : 0 ;
	
	ThrowIfNil_(newFrame);
	
	newFrame->SetReturnValues(mArith.IsNull() ? 0 : mArith(ioState),mArith.IsNull() ? 0.0 : mArith->EvaluateFloat(ioState),mStr.IsNull() ? "" : mStr->Evaluate(ioState).mString);
	
	ioState.SetPC(currentFrame->GetReturnAddress());
	ioState.SetDC(currentFrame->GetReturnDataAddress());
	delete currentFrame;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊCVariableNode::Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CVariableNode::~CVariableNode()
{
	delete mTail;
	delete mInitStatement;
}
