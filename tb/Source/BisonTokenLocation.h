// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// BisonTokenLocation.h
// © Mark Tully 2000
// 29/6/00
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

/*
	Defines for the location stack feature of bison. Keeps track of the text offsets of each token
*/

#pragma once

// This is a parser for the TNT basic language
#define		YYLSP_NEEDED			// state that we require a location stack specifying the locations of tokens within the
									// source code
struct SBisonTokenLoc				// this is the structure which contains token locations
{
  int first_line, last_line;
  int first_column, last_column;
  char *text;
};

struct STextRange
{
	SInt32		line;					// line number
	SInt32		startOffset,endOffset;	// offsets into the text
	
				STextRange(
					const SBisonTokenLoc &inLoc) :
					line(inLoc.first_line),
					startOffset(inLoc.first_column),
					endOffset(inLoc.last_column)
					{}
				STextRange(
					const STextRange &inStart,
					const STextRange &inEnd) :
					line(inStart.line),
					startOffset(inStart.startOffset),
					endOffset(inEnd.endOffset)
					{}
				STextRange(
					const SBisonTokenLoc &inStart,
					const SBisonTokenLoc &inEnd) :
					line(inStart.first_line),
					startOffset(inStart.first_column),
					endOffset(inEnd.last_column)
					{}
				STextRange() :
					startOffset(0),
					endOffset(0),
					line(0)
					{}
};

#define LogException_(c) \
	do \
	{ \
		try \
		{ \
			c; \
		} \
		catch(const PP_PowerPlant::LException &err) \
		{ \
			CProgram::GetParsingProgram()->LogError(err); \
		} \
	} while (false)
