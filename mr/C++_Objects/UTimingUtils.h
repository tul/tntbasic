// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UTimingUtils.h
// © Mark Tully 2000
// 2/1/00
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

#include		"CTimeMgrTask.h"

/*
	A class for timing periods to an accuracy of 1ms. Use StartTask() and StopTask() to err start and stop it and Reset to
	well; reset it.
	GetTime get's the time.
*/
class CMsTimer : public CTimeMgrTask
{
	protected:
		volatile UInt32		mMs;
	
		virtual void		Tick()				{ mMs++; }
	
	public:
							CMsTimer() :
								CTimeMgrTask(1), mMs(0) {}
		virtual				~CMsTimer()			{ StopTask(); }
							
		UInt32				GetTime()			{ return mMs; }
		void				Reset()				{ mMs=0; }
};


class CRateTimer : public CTimeMgrTask
{
	protected:
		static const UInt16	kSampleSize=3;			// number of halfseconds to time for

		volatile UInt32		mRates[kSampleSize];
		volatile UInt16		mUsedRates;				// number of entries used in the above array
		volatile UInt16		mCurrentRate;			// the currently active entry in the above array
		volatile UInt32		mPulseCount;			// number of pulses this sample
		
		virtual void		Tick();
		
	public:
							CRateTimer();
		virtual				~CRateTimer()			{ StopTask(); }									
		
		virtual void		Pulse();
		virtual void		Reset()					{ mUsedRates=0; }
		
		virtual void		StopTask();
		
		virtual double		CalcRate();
};