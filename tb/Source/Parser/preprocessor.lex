%{				// %{ indicates C code %}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Proc.lex
// © John Treece-Birch 2000
// 25/6/00
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

// This is a lexer finding procedures in TNT Basic

// Section 1 - Declarations and definitions section

#include	<stdlib.h>
#include	<iostream>
#include	"preprocessor.tab.h"
#include	"UCString.h"

#define FLEX_SUPPORT_NAMESPACE      TBPreprocessor
#define FLEX_SUPPORT_SCAN_BUFFER    yy_scan_buffer
#define FLEX_SUPPORT_CURRENT_BUFFER YY_CURRENT_BUFFER
#include	"CFlexBufferSupport.cpp"
#undef FLEX_SUPPORT_SCAN_BUFFER
#undef FLEX_SUPPORT_CURRENT_BUFFER
#undef FLEX_SUPPORT_NAMESPACE

int preprocessorlex();
extern "C" int preprocessorwrap();

int preprocessorwrap()
{
	return 1;
}

int	gProcLine;

%}

digit				[0-9]
whitespace			[ \t]
newl				[\n]
letter				[a-zA-Z_]
identifier			{letter}({letter}|{digit})*
strchar				[^"\n]|\\\"
str					{strchar}*
strend				[\"\n]
rem					("'"|"Rem ").*

%%

{rem}				{ }
{newl}				{ gProcLine++; return ENDLINE; }
"\""{str}{strend}	{ }
"Procedure"			{ return PROCEDURE; }
"End Proc"			{ return ENDPROC; }
{identifier}		{ preprocessorlval.string=UCString::Duplicate(yytext); return IDENTIFIER; }
.					{ }

%%
