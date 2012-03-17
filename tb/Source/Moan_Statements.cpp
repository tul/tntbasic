// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Moan_Statements.cpp
// © Mark Tully 2002
// 17/2/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

/*
	Moan Statements

	Provides high level moan functions such as "move sprite" and "animate sprite", and more
	low level functions such as moan channel manipulation.
*/

#include "Moan_Statements.h"
#include "CTBSprite.h"
#include "CProgMoanChannel.h"
#include "CGraphicsContext16.h"
#include "basic.tab.h"

#define	DISSASSEMBLE_MOAN	0			// set to 1 to print moan dissassembly

#if DISSASSEMBLE_MOAN
#include	"CVMCodeChunk.h"
#endif

char RegStrToLocalRegId(
	CCString		&inStr);
char RegStrToGlobalRegId(
	CCString		&inStr);
char RegIndexToLocalRegId(
	TTBInteger		inIndex);
char RegIndexToGlobalRegId(
	TTBInteger		inIndex);


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CChanAttributesFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CChanAttributesFunction::EvaluateInt(
	CProgram			&ioState)
{
	CProgMoanChannel	*ch=ioState.CheckGraphicsMode()->mMoanManager.GetChannel(ioState,mChan(ioState));

	switch (mAttribute)
	{
		case CHANMOVE:
			return ch->IsMoving();
			break;
			
		case CHANANIM:
			return ch->IsAnimating();
			break;
			
		case CHANPLAY:
			return ch->ChannelPlaying();
			break;
			
		case CHANPAUSED:
			if (ioState.CheckGraphicsMode()->GetMasterMoanPause())
				return true;
			else
				return ch->IsChannelPaused();
			break;
			
		default:
			Throw_(-1);
			return 0;
			break;
	}	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CResetMoanChannelStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CResetMoanChannelStatement::ExecuteSelf(
	CProgram		&ioState)
{
	if (!mChannelId.IsNull())
		delete ioState.CheckGraphicsMode()->mMoanManager.GetChannelNoCreate(mChannelId(ioState));
	else
	{
		CListIndexerT<CProgMoanChannel>		indexer(&ioState.CheckGraphicsMode()->mMoanManager.GetChannelsList());
		
		while (CProgMoanChannel *c=indexer.GetNextData())
			delete c;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetMoanAttributesStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CSetMoanAttributesStatement::ExecuteSelf(
	CProgram		&ioState)
{
	switch (mAttribute)
	{
		case AUTOMOANON:
			ioState.CheckGraphicsMode()->SetAutoMoan(true);
			break;
		case AUTOMOANOFF:
			ioState.CheckGraphicsMode()->SetAutoMoan(false);
			break;
		default:
			Throw_(-1);
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CStepMoanStatement::ExecuteSelf										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CStepMoanStatement::ExecuteSelf(
	CProgram		&ioState)
{
	ioState.CheckGraphicsMode()->StepMoan();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CMoanAttributesFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CMoanAttributesFunction::EvaluateInt(
	CProgram			&ioState)
{
	switch (mAttribute)
	{
		case AUTOMOANING:
			return ioState.CheckGraphicsMode()->IsMoanAuto();
			break;
			
		default:
			Throw_(-1);
			return 0;
			break;
	}	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetMoanProgramStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// takes channelId,progstr
CSetMoanProgramStatement::CSetMoanProgramStatement(
	CGeneralExpression	*inExp)
{
	mChannelId=inExp->ReleaseArithExp();
	inExp=inExp->TailData();
	mProgram=inExp->ReleaseStrExp();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetMoanProgramStatement::ExecuteSelf				/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Installs a program on a moan channel
void CSetMoanProgramStatement::ExecuteSelf(
	CProgram		&ioState)
{
	CProgMoanChannel	*ch=ioState.CheckGraphicsMode()->mMoanManager.GetChannel(ioState,mChannelId(ioState));
	
	ch->SetProgram(mProgram(ioState));
	
#if DISSASSEMBLE_MOAN		// print dissassembly of moan prog
	CVMCodeChunk		*code=ch->GetCode();
	
	if (code)
	{
		code->Dissassemble(std::cout,ch);
	}
#endif
	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CRotateSpriteStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Starts the internal sprite moan channel for the specified sprite with a program to rotate it
// xdelta,ydelta,at speed. If speed is negative, it is taken as the number of steps to do the rotate in
CRotateSpriteStatement::CRotateSpriteStatement(
	CGeneralExpression	*inExpression)
{
	mSprite=inExpression->ReleaseArithExp();
									
	inExpression=inExpression->TailData();
	if (!inExpression->IsNullExp())
		mRotateDelta=inExpression->ReleaseArithExp();

	inExpression=inExpression->TailData();
	mSpeed=inExpression->ReleaseArithExp();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CRotateSpriteStatement::ExecuteSelf						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CRotateSpriteStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CTBSprite			*sprite=0;
	CBLMoanChannel		*chan=0;

	sprite=ioState.GetSpriteManager().GetSprite(mSprite->EvaluateInt(ioState));
	chan=&sprite->mLocalMoanChannel;
	
	float				speed=mSpeed->EvaluateFloat(ioState);

	if (!mRotateDelta.IsNull())
	{
		float				rDelta=mRotateDelta->EvaluateFloat(ioState);
			
		// are we going anywhere?	
		if (rDelta && speed)
		{
			if (speed>0)
				chan->InstallRotate(rDelta,speed,false);
			else
				chan->InstallRotateBySteps(rDelta,-speed);
		}
		else
			chan->ResetRotate();
	}
	else
	{
		if (speed)
			chan->InstallRotate(0.0f,speed,true);
		else
			chan->ResetRotate();
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CScaleSpriteStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Starts the internal sprite moan channel for the specified sprite with a program to scale it
// xdelta,ydelta,at speed. If speed is negative, it is taken as the number of steps to do the moment in
CScaleSpriteStatement::CScaleSpriteStatement(
	CGeneralExpression	*inExpression)
{
	mSprite=inExpression->ReleaseArithExp();
									
	inExpression=inExpression->TailData();
	mXDelta=inExpression->ReleaseArithExp();

	inExpression=inExpression->TailData();
	mYDelta=inExpression->ReleaseArithExp();

	inExpression=inExpression->TailData();
	mSpeed=inExpression->ReleaseArithExp();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CScaleSpriteStatement::ExecuteSelf						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CScaleSpriteStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CTBSprite			*sprite=0;
	CBLMoanChannel		*chan=0;

	sprite=ioState.GetSpriteManager().GetSprite(mSprite->EvaluateInt(ioState));
	chan=&sprite->mLocalMoanChannel;

	float				xDelta=mXDelta->EvaluateFloat(ioState),yDelta=mYDelta->EvaluateFloat(ioState);
	float				speed=mSpeed->EvaluateFloat(ioState);
		
	// are we going anywhere?	
	if ((xDelta || yDelta) && speed)
	{
		if (speed>0)
			chan->InstallScale(xDelta,yDelta,speed);
		else
			chan->InstallScaleBySteps(xDelta,yDelta,-speed);
	}
	else
		chan->ResetScale();
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CMoveSpriteStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Starts the internal sprite moan channel for the specified sprite with a program to move it
// xdelta,ydelta,at speed. If speed is negative, it is taken as the number of steps to do the moment in
// eg speed = 3 -> 3 pix/frame speed
//    speed = -3 -> 3 steps until destination
CMoveSpriteStatement::CMoveSpriteStatement(
	bool				inSprite,
	CGeneralExpression	*inExpression) :
	mIsSprite(inSprite)
{
	mSprite=inExpression->ReleaseArithExp();
									
	inExpression=inExpression->TailData();
	mXDelta=inExpression->ReleaseArithExp();

	inExpression=inExpression->TailData();
	mYDelta=inExpression->ReleaseArithExp();

	inExpression=inExpression->TailData();
	mSpeed=inExpression->ReleaseArithExp();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CMoveSpriteStatement::ExecuteSelf						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CMoveSpriteStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CTBSprite			*sprite=0;
	CViewport			*vp=0;
	CBLMoanChannel		*chan=0;
	
	if (mIsSprite)
	{
		sprite=ioState.GetSpriteManager().GetSprite(mSprite->EvaluateInt(ioState));
		chan=&sprite->mLocalMoanChannel;
	}
	else
	{
		vp=ioState.CheckGraphicsMode()->GetViewport(mSprite->EvaluateInt(ioState));
		chan=&vp->mLocalMoanChannel;
	}
	
	TTBFloat			xDelta=mXDelta->EvaluateFloat(ioState),yDelta=mYDelta->EvaluateFloat(ioState),speed=mSpeed->EvaluateFloat(ioState);
		
	// are we going anywhere?	
	if ((xDelta || yDelta) && speed)
	{
		if (speed>0)
			chan->InstallMovement(xDelta,yDelta,speed);
		else
			chan->InstallMovementBySteps(xDelta,yDelta,-speed);
	}
	else
		chan->ResetMovement();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CAnimSpriteStatement::Constructor						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Performs the type check
CAnimSpriteStatement::CAnimSpriteStatement(
	CGeneralExpression		*inExp,
	const STextRange		&inListTerminatingLoc) :
	mAnimFrames(0),
	mExpressionsList(0)
{
	// Type check:
	// a,a,AtA,AtA,AtA...
	
	CGeneralExpression	*exp=inExp;

	// no params?	
	if (!exp)
		UTBException::ThrowExpectedNumExp(inListTerminatingLoc);
	
	// number followed by a comma?
	STextRange			range;
	
	exp->GetExpLoc(range);
	
	if (!exp->GetArithExp())
		UTBException::ThrowExpectedNumExp(range);		
	range=exp->GetSepLoc();
	if (exp->GetSeperator()!=CGeneralExpression::kComma)
		UTBException::ThrowExpectedComma(range);
		
	mSprite=exp->ReleaseArithExp();
	exp=exp->TailData();
	
	// get repeat count, again a number, followed either by a comma or nothing	
	if (!exp)
		UTBException::ThrowExpectedNumExp(range);

	exp->GetExpLoc(range);
	
	if (!exp->GetArithExp())
		UTBException::ThrowExpectedNumExp(range);		
	range=exp->GetSepLoc();
	if (exp->GetSeperator()!=CGeneralExpression::kComma && (exp->TailData() && !exp->TailData()->IsNullExp()))
		UTBException::ThrowExpectedComma(range);
		
	mRepeatCount=exp->ReleaseArithExp();
	exp=exp->TailData();
	
	// now read the frame,delay to ... list
	CGeneralExpression *startOfExps=exp;
		
	while (exp && !exp->IsNullExp())
	{
		// frame
		#define SafeGetSepLoc(e)		(e->GetSeperator()==CGeneralExpression::kUnspecified ? inListTerminatingLoc : e->GetSepLoc())

		exp->GetExpLoc(range);
		if (!exp->GetArithExp())
			UTBException::ThrowExpectedNumExp(range);
	
		// frame , 
		
		range=SafeGetSepLoc(exp);
		if (exp->GetSeperator()!=CGeneralExpression::kComma)
			UTBException::ThrowExpectedComma(range);

		// frame , delay
		exp=exp->TailData();
		
		if (!exp || !exp->GetArithExp())
			UTBException::ThrowExpectedNumExp(range);		
		
		exp->GetExpLoc(range);
		if (!exp->GetArithExp())
			UTBException::ThrowExpectedNumExp(range);

		mAnimFrames++;
		
		// test for to
		range=SafeGetSepLoc(exp);
		if (exp->GetSeperator()!=CGeneralExpression::kTo && (exp->TailData() && !exp->TailData()->IsNullExp()))
			UTBException::ThrowExpectedTo(range);

		exp=exp->TailData();
	}
	
	// All passed, now release all the arith exps into our custody, storing them in a handle
	if (mAnimFrames)
	{
		mExpressionsList=::NewHandle(sizeof(CArithExpression*)*2*mAnimFrames);
		ThrowIfMemFull_(mExpressionsList);
		
		for (TTBInteger i=0; i<(2*mAnimFrames); i++)
		{
			((CArithExpression**)(*mExpressionsList))[i]=startOfExps->ReleaseArithExp();
			startOfExps=startOfExps->TailData();
		}
	}
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CAnimSpriteStatement::Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Deletes the arith expressions list for anim frames
CAnimSpriteStatement::~CAnimSpriteStatement()
{
	// run through list and delete them all
	for (TTBInteger i=0; i<(mAnimFrames*2); i++)
		delete ((CArithExpression**)(*mExpressionsList))[i];
	::DisposeHandle(mExpressionsList);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CAnimSpriteStatement::ExecuteSelf						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Evaluates the anim frames list into a big array and then runs installs it into the sprites local channel
void CAnimSpriteStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CTBSprite			*sprite=ioState.GetSpriteManager().GetSprite(mSprite->EvaluateInt(ioState));
	
	// right we have the sprite, now get the anim handle to fill out
	SBLMoanFrame		**animHandle=sprite->mLocalMoanChannel.PrepareToInstallAnim(mAnimFrames);
	TTBInteger			frame,delay;

	// fill the handle out with our data by evaluating the expressions
	for (TTBInteger i=0,o=0; i< 2*mAnimFrames; i+=2,o++)
	{
		frame=((CArithExpression**)*mExpressionsList)[i]->EvaluateInt(ioState);
		delay=((CArithExpression**)*mExpressionsList)[i+1]->EvaluateInt(ioState);
		(*animHandle)[o].image=frame;
		(*animHandle)[o].delay=delay;
	}
		
	// done
	sprite->mLocalMoanChannel.AnimInstalled(mRepeatCount(ioState));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CEditChanObsStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adds either a viewport or a sprite to a moan channel
CEditChanObsStatement::CEditChanObsStatement(
	CGeneralExpression	*inExp,
	bool				inIsSprite,
	bool				inAddIt) :
	mIsSprite(inIsSprite),
	mAddIt(inAddIt)	
{
	mOb=inExp->ReleaseArithExp();
	inExp=inExp->TailData();
	if (inExp)
		mChannelId=inExp->ReleaseArithExp();		// channel id can be null for remove sprite/viewport commands
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CEditChanObsStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adds either a viewport or a sprite to a moan channel
void CEditChanObsStatement::ExecuteSelf(
	CProgram		&ioState)
{
	TTBInteger			s=mOb(ioState);
	CProgMoanChannel	*chan=mChannelId.IsNull() ? 0 : ioState.CheckGraphicsMode()->mMoanManager.GetChannel(ioState,mChannelId(ioState));
	
	if (mIsSprite)
	{
		if (mAddIt)
			chan->AddSpriteToChannel(ioState.CheckGraphicsMode()->mSpriteManager.GetSprite(s));
		else
		{
			if (chan)
				chan->RemoveSpriteFromChannel(ioState.CheckGraphicsMode()->mSpriteManager.GetSprite(s));
			else
			{
				// remove from all channels (high complexity op - may need optimising)
				CListIndexerT<CProgMoanChannel>		indexer(&ioState.CheckGraphicsMode()->mMoanManager.GetChannelsList());
				CTBSprite							*sprite=ioState.CheckGraphicsMode()->mSpriteManager.GetSprite(s);
				
				while (chan=indexer.GetNextData())
					chan->RemoveSpriteFromChannel(sprite);
			}
		}
	}
	else
	{
		if (mAddIt)
			chan->AddViewportToChannel(ioState.CheckGraphicsMode()->GetViewport(s));
		else
		{
			if (chan)
				chan->RemoveViewportFromChannel(ioState.CheckGraphicsMode()->GetViewport(s));
			else
			{
				// remove from all channels (high complexity op - may need optimising)
				CListIndexerT<CProgMoanChannel>		indexer(&ioState.CheckGraphicsMode()->mMoanManager.GetChannelsList());
				CViewport							*v=ioState.CheckGraphicsMode()->GetViewport(s);
				
				while (chan=indexer.GetNextData())
					chan->RemoveViewportFromChannel(v);
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetChanAttributeStatement::ExecuteSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CSetChanAttributeStatement::ExecuteSelf(
	CProgram		&ioState)
{
	CProgMoanChannel	*chan=mChannelId.IsNull() ? 0 : ioState.CheckGraphicsMode()->mMoanManager.GetChannel(ioState,mChannelId(ioState));
	
	switch (mAttribute)
	{
		case PAUSECHANNEL:
		case UNPAUSECHANNEL:
			if (chan)
				chan->SetChannelPause(mAttribute==PAUSECHANNEL);
			else
				ioState.CheckGraphicsMode()->SetMasterMoanPause(mAttribute==PAUSECHANNEL);
			break;
			
		default:
			SignalString_("Unknown moan channel property");
			Throw_(-1);
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetChanAttributeStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CSetChanAttributeStatement::CSetChanAttributeStatement(
	CArithExpression	*inChannelId,
	int					inTokenId,
	int					inValue) :
	mAttribute(inTokenId),
	mValue(inValue),
	mChannelId(inChannelId)
{
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetChannelRegStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// nb clone of get channel reg function
CSetChannelRegStatement::CSetChannelRegStatement(
	EType				inParamForm,
	CGeneralExpression	*inParams) :
	mType(inParamForm)
{
	switch (inParamForm)
	{
		case kLocalStr:
			mChannelId=inParams->ReleaseArithExp();
			inParams=inParams->TailData();
			mRegIdStr=inParams->ReleaseStrExp();			
			inParams=inParams->TailData();
			mValue=inParams->ReleaseArithExp();
			break;
			
		case kGlobalStr:
			mRegIdStr=inParams->ReleaseStrExp();			
			inParams=inParams->TailData();
			mValue=inParams->ReleaseArithExp();
			break;
			
		case kLocalNum:
			mChannelId=inParams->ReleaseArithExp();
			inParams=inParams->TailData();
			mRegId=inParams->ReleaseArithExp();			
			inParams=inParams->TailData();
			mValue=inParams->ReleaseArithExp();
			break;
			
		case kGlobalNum:		
			mRegId=inParams->ReleaseArithExp();			
			inParams=inParams->TailData();
			mValue=inParams->ReleaseArithExp();
			break;
			
		default:
			Throw_(-1);
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RegIndexToGlobalRegId													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// takes indexes 0-25
char RegIndexToGlobalRegId(
	TTBInteger		inIndex)
{
	if (inIndex>=0 && inIndex<=25)
		return inIndex+'A';
	UTBException::ThrowBadGlobalMoanRegId(inIndex);
	return 0;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RegStrToGlobalRegId													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// takes strs of the form "Rx" where x is A-Z and converts it to a reg id
char RegStrToGlobalRegId(
	CCString		&inStr)
{
	if (inStr.GetLength()!=2 || inStr.mString[0]!='R')
		UTBException::ThrowBadGlobalMoanRegId(inStr);

	if (inStr.mString[1]>='A' && inStr.mString[1]<='Z')
		return inStr.mString[1];
	else
		UTBException::ThrowBadGlobalMoanRegId(inStr);		
		
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RegIndexToLocalRegId													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// takes indexes 0-9 and 10-12
char RegIndexToLocalRegId(
	TTBInteger		inIndex)
{
	if (inIndex>=0 && inIndex<=9)
		return inIndex+'0';
	switch (inIndex)
	{
		case 10:
			return 'x';
			break;
		case 11:
			return 'y';
			break;
		case 12:
			return 'i';
			break;
	}
	UTBException::ThrowBadLocalMoanRegId(inIndex);
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ RegStrToLocalRegId													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// takes strs of the form "Rx" where x is 0-9, or takes X Y I and converts it to a reg id
char RegStrToLocalRegId(
	CCString		&inStr)
{
	if (inStr.GetLength()==2)
	{
		if (inStr.mString[0]!='R')
			UTBException::ThrowBadLocalMoanRegId(inStr);
		if (inStr.mString[1]>='0' && inStr.mString[1]<='9')
			return inStr.mString[1];
		UTBException::ThrowBadLocalMoanRegId(inStr);
	}
	else if (inStr.GetLength()==1)
	{
		if (inStr.mString[0]=='X' || inStr.mString[0]=='Y' || inStr.mString[0]=='I')
			return ToLower(inStr.mString[0]);
	}
	UTBException::ThrowBadLocalMoanRegId(inStr);
	return 0;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetChannelRegStatement::ExecuteSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// nb clone of get channel reg function
void CSetChannelRegStatement::ExecuteSelf(
	CProgram		&ioState)
{
	CProgMoanChannel	*ch=0;
	char				regId;
	
	if (!mChannelId.IsNull())
		ch=ioState.CheckGraphicsMode()->mMoanManager.GetChannel(ioState,mChannelId(ioState));
		
	switch (mType)
	{
		case kGlobalStr:
		case kLocalStr:
			{
				CCString		&str(mRegIdStr(ioState));				
				if (mType==kLocalStr)
					regId=RegStrToLocalRegId(str);
				else
					regId=RegStrToGlobalRegId(str);
			}
			break;
						
		case kLocalNum:
			regId=RegIndexToLocalRegId(mRegId(ioState));
			break;
			
		case kGlobalNum:
			regId=RegIndexToGlobalRegId(mRegId(ioState));
			break;
	}
	
	if (ch)
		ch->SetReg(regId,mValue(ioState));
	else
		CProgMoanChannel::SetSharedReg(regId,mValue(ioState));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetChannelRegFunction::EvaluateInt									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// nb clone of set channel reg statement
TTBInteger CGetChannelRegFunction::EvaluateInt(
	CProgram		&ioState)
{
	CProgMoanChannel	*ch=0;
	char				regId;
	
	if (!mChannelId.IsNull())
		ch=ioState.CheckGraphicsMode()->mMoanManager.GetChannel(ioState,mChannelId(ioState));
		
	switch (mType)
	{
		case kGlobalStr:
		case kLocalStr:
			{
				CCString		&str(mRegIdStr(ioState));				
				if (mType==kLocalStr)
					regId=RegStrToLocalRegId(str);
				else
					regId=RegStrToGlobalRegId(str);
			}
			break;
						
		case kLocalNum:
			regId=RegIndexToLocalRegId(mRegId(ioState));
			break;
			
		case kGlobalNum:
			regId=RegIndexToGlobalRegId(mRegId(ioState));
			break;
	}
	
	if (ch)
		return ch->GetReg(regId);
	else
		return CProgMoanChannel::GetSharedReg(regId);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetChannelRegFunction::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// NB Close of set statement constructor
CGetChannelRegFunction::CGetChannelRegFunction(
	EType				inParamForm,
	CGeneralExpression	*inParams,
	const STextRange	&inRange) :
	mType(inParamForm),
	CFixedLocArithExpression(inRange)
{
	switch (inParamForm)
	{
		case kLocalStr:
			mChannelId=inParams->ReleaseArithExp();
			inParams=inParams->TailData();
			mRegIdStr=inParams->ReleaseStrExp();			
			break;
			
		case kGlobalStr:
			mRegIdStr=inParams->ReleaseStrExp();			
			break;
			
		case kLocalNum:
			mChannelId=inParams->ReleaseArithExp();
			inParams=inParams->TailData();
			mRegId=inParams->ReleaseArithExp();			
			break;
			
		case kGlobalNum:		
			mRegId=inParams->ReleaseArithExp();			
			break;
			
		default:
			Throw_(-1);
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CRestartMoanChannelStatement::EvaluateInt									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CRestartMoanChannelStatement::ExecuteSelf(
	CProgram		&ioState)
{
	if (!mChannelId.IsNull())
		ioState.CheckGraphicsMode()->mMoanManager.GetChannel(ioState,mChannelId(ioState))->RestartProgram();
	else
	{
		CListIndexerT<CProgMoanChannel>		indexer(&ioState.CheckGraphicsMode()->mMoanManager.GetChannelsList());
		
		while (CProgMoanChannel *c=indexer.GetNextData())
			c->RestartProgram();
	}	
}
