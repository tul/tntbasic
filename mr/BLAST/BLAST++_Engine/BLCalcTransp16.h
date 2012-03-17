// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// BLCalcTransp16.h
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

/*
	Macros for calculating transparencies between 16 bit colours using fast shifts
*/

// These macros are for darkening a colour
// They place a lot of trust in the optimising power of the compiler allowing us to write readable statements and avoid
// having to hand code every expression							// xRRR RRGG GGGB BBBB
#define		BLDarkenCol616(x)				(((x)>>4) & 0x0421)	// 0000 0100 0010 0001 				=		0x0421
#define		BLDarkenCol1216(x)				(((x)>>3) & 0x0C63)	// 0000 1100 0110 0011				=		0x0C63
#define		BLDarkenCol2516(x)				(((x)>>2) & 0x1CE7)	// 0001 1100 1110 0111				=		0x1CE7
#define		BLDarkenCol5016(x)				(((x)>>1) & 0x3DEF)	// 0011 1101 1110 1111				=		0x3DEF

#define		BLDarkenCol3816(x)				(BLDarkenCol2516(x)+BLDarkenCol1216(x))
#define		BLDarkenCol6316(x)				(BLDarkenCol5016(x)+BLDarkenCol1216(x))
#define		BLDarkenCol7516(x)				(BLDarkenCol5016(x)+BLDarkenCol2516(x))
#define		BLDarkenCol8716(x)				((x)-BLDarkenCol1216(x))
#define		BLDarkenCol9316(x)				((x)-BLDarkenCol616(x))

// These macros do a blend between two colours
// The percentage is the percentage transparency of the first colour
#define		BLCalcTransp616(x,y)			(BLDarkenCol9316(x) + BLDarkenCol616(y))
#define		BLCalcTransp1216(x,y)			(BLDarkenCol8716(x) + BLDarkenCol1216(y))
#define		BLCalcTransp2516(x,y)			(BLDarkenCol7516(x) + BLDarkenCol2516(y))
#define		BLCalcTransp3816(x,y)			(BLDarkenCol6316(x) + BLDarkenCol3816(y))
#define		BLCalcTransp5016(x,y)			(BLDarkenCol5016(x) + BLDarkenCol5016(y))
#define		BLCalcTransp6316(x,y)			BLCalcTransp3816(y,x)
#define		BLCalcTransp7516(x,y)			BLCalcTransp2516(y,x)
#define		BLCalcTransp8716(x,y)			BLCalcTransp1216(y,x)
#define		BLCalcTransp9316(x,y)			BLCalcTransp616(y,x)