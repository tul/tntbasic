// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UMoanCompiler.h
// Mark Tully
// 15/2/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

#pragma once

class CVMCodeChunk;
class CVMVirtualMachine;

union STokenValue
{
	SInt32		i;
	float		f;
	char		c;
};

class UMoanCompiler
{
	protected:
		typedef  UInt32		TTokenType;
		enum									// value
		{
			kEOF=			(1 << 0),			// undef						Cannot be masked
			kNOP=			(1 << 1),			// undef						Cannot be masked
			kError=			(1 << 2),			// undef						Cannot be masked
			kInteger=		(1 << 3),			// parsed int
			kOperator=		(1 << 4),			// Operator literal ()+-*/^
			kWhiteSpace=	(1 << 5),			// undef
			kMoanCommand=	(1 << 6),			// Letter of command
			kPunctuation=	(1 << 7),			// punctuation encountered
			kRegister=		(1 << 8),			// reg letter 0-9 local, A-Z global, xyi special
			kLabel=			(1 << 9),			// label letter
			kForceLabel=	(1 << 10),			// force all captials to be interpreted as labels, even if not followed by a : Needed for jmps (Warning: Hides all other functions if used)
			kFloat=			(1 << 11)			// parsed float
		};

		static char			*CompileExpressionPairs(
								char				*inString,
								CVMCodeChunk		*outCode);
		static char			*Compile1Expression(
								char				*inString,
								CVMCodeChunk		*outCode);
		static char			*PullToken(
								char				*ioString,
								bool				&inExpectTerm,
								TTokenType			inMask,
								TTokenType			&outType,
								STokenValue			&outValue);
								
		static char			*sSourceProgram;
		static SInt32		sChannelId;

		static void			ThrowCompilerError(
								const char		*inString,
								OSErr			inErr);
				
	public:
		static void			CompileMoanProgram(
								char				*inMoanProgram,
								CVMCodeChunk		*outCode,
								SInt32				inChannelId);

		static void			InstallMoanFunctions(
								CVMVirtualMachine	*inVM);
};