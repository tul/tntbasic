// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// UBLEncodedImageFX16.h
// © Mark Tully 2000
// 21/3/00
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

#include		"UBLEncodedImage16.h"
#include		"DecodeImage16.h"

extern			BlastColour16		gSpriteColour;

class UBLEncodedImageFX16
{
	public:
		BL_MAKEDECODERPROTOTYPES16(DecodeRedImage);
		BL_MAKEDECODERPROTOTYPES16(DecodeGreenImage);
		
		BL_MAKEDECODERPROTOTYPES16(Decode6Image);
		BL_MAKEDECODERPROTOTYPES16(Decode12Image);
		BL_MAKEDECODERPROTOTYPES16(Decode25Image);
		BL_MAKEDECODERPROTOTYPES16(Decode50Image);
		BL_MAKEDECODERPROTOTYPES16(Decode75Image);
		BL_MAKEDECODERPROTOTYPES16(Decode87Image);
		BL_MAKEDECODERPROTOTYPES16(Decode93Image);

		BL_MAKEDECODERPROTOTYPES16(Decode63Image);
		BL_MAKEDECODERPROTOTYPES16(Decode38Image);

		BL_MAKEDECODERPROTOTYPES16(Decode6Shadow);
		BL_MAKEDECODERPROTOTYPES16(Decode12Shadow);
		BL_MAKEDECODERPROTOTYPES16(Decode25Shadow);
		BL_MAKEDECODERPROTOTYPES16(Decode50Shadow);
		BL_MAKEDECODERPROTOTYPES16(Decode75Shadow);
		BL_MAKEDECODERPROTOTYPES16(Decode87Shadow);
		BL_MAKEDECODERPROTOTYPES16(Decode93Shadow);
		
		BL_MAKEDECODERPROTOTYPES16(DecodeColour);
		BL_MAKEDECODERPROTOTYPES16(DecodeColour93);
		BL_MAKEDECODERPROTOTYPES16(DecodeColour87);
		BL_MAKEDECODERPROTOTYPES16(DecodeColour75);
		BL_MAKEDECODERPROTOTYPES16(DecodeColour63);
		BL_MAKEDECODERPROTOTYPES16(DecodeColour50);
		BL_MAKEDECODERPROTOTYPES16(DecodeColour38);
		BL_MAKEDECODERPROTOTYPES16(DecodeColour25);
		BL_MAKEDECODERPROTOTYPES16(DecodeColour12);
		BL_MAKEDECODERPROTOTYPES16(DecodeColour6);
};