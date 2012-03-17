// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLMask16.h
// © Mark Tully 2000
// 7/1/00
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

#include		"BLAST_Types.h"
#include		"BLAST.h"

class			CBLCanvas;

class CBLMask16
{
	public:
		virtual				~CBLMask16()	{}
	
		// Quantified transparancy levels. These represent levels which can be achieved via bit shifting
		// BEWARE: That changing the enumerated values of these will change the encoding format as it uses these constants
		// directly in the flattened image representation
		typedef UInt8 			TTranspLevel;
		enum
		{
			kTransp100,		// not-there (black)
			kTransp93,
			kTransp87,
			kTransp75,
			kTransp50,
			kTransp25,
			kTransp12,
			kTransp6,
			kTransp0		// totally opaque (white)
		};
	
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//		¥ ApplyMask												/*e*/
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		// Takes a colour and returns a (possibly) modified colour and a transparency level by using the mask.
		// inX and inY are offsets into the mask
		virtual CBLMask16::TTranspLevel /*e*/	ApplyMask(
								TBLColour16			&ioColour,
								SInt16				inX,
								SInt16				inY) = 0;
};

class CBLColourMask16 : public CBLMask16
{
	protected:
		TBLColour16						mMaskColour;
	
	public:
										CBLColourMask16(
											TBLColour16		inCol) :
											mMaskColour(inCol)
											{}
										CBLColourMask16(
											const RGBColour		&inCol)
										{
											mMaskColour=RGBColourToBlastColour16(&inCol);
										}

		TBLColour16						GetMaskColour()		{ return mMaskColour; }
		void							SetMaskColour(
											TBLColour16	inCol) { mMaskColour=inCol; }

		virtual CBLMask16::TTranspLevel /*e*/	ApplyMask(
													TBLColour16		&ioColour,
													SInt16			inX,
													SInt16			inY);
};

class CBLGreyScaleMask16 : public CBLMask16
{
	protected:
		Rect							mMaskRect;
		CBLCanvas						*mMaskCanvas;
		bool							mOwnsCanvas;

		static unsigned char			sLevelsTable[];
		static unsigned char			sRangeTable[];

	public:		
										CBLGreyScaleMask16(
											CBLCanvas		*inCanvas,
											const Rect		&inRect,
											bool			inAdoptCanvas=true) :
											mMaskRect(inRect),
											mMaskCanvas(inCanvas),
											mOwnsCanvas(inAdoptCanvas)
										{}
		virtual							~CBLGreyScaleMask16();
		
		CBLCanvas						*GetMaskCanvas()	{ return mMaskCanvas; }

		static CBLMask16::TTranspLevel	Quantify(
											TBLColour16		inColour);
		static TBLColour16				InvQuantify(
											CBLMask16::TTranspLevel	inLev);		
		
		virtual CBLMask16::TTranspLevel /*e*/	ApplyMask(
													TBLColour16		&ioColour,
													SInt16			inX,
													SInt16			inY);
};