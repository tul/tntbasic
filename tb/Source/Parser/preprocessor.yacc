%{
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Proc.yacc
// © Mark Tully 2000
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

// This is a parser for extracting procedures in TNT Basic

#include	<cstdlib>
#define		free(x) std::free(x)		// The bison generated code needs this to compile in cw

#include	"TNT_Debugging.h"
#include	"CProcNode.h"

// yylex must be declared as external here to allow linker to correctly link up the lexer and parser
extern int preprocessorlex();
extern int gProcLine;

int preprocessorparse();
int yyerror(char *s);

int yyerror(char *s)
{
	//SignalPStr_("\pPreprocessor parse error");

	return 1;
}

CProcNode		*gProceduresList=0L;

%}

%union
{
	char		*string;
};

%token PROCEDURE IDENTIFIER ENDLINE ENDPROC

%type <string>	IDENTIFIER

%start prog

%%	

prog:			/*null*/ |
				prog thing {}

thing:			ENDLINE {} | procedure {} | endProc {} | IDENTIFIER { delete [] $1; }

muchCrap 	:	/*null*/ |
				muchCrap IDENTIFIER { delete $2; }

endProc		:	ENDPROC muchCrap ENDLINE
				{
					if (gProceduresList)
					{					
						CProcNode		*last=gProceduresList;
						
						while (!last->IsEnd())
							last=last->TailData();
					
						if (last->mEndProcLine==-1)		// last proc exist?
							last->mEndProcLine=gProcLine;
					}
				}
				
procedure	:	PROCEDURE IDENTIFIER muchCrap ENDLINE		{ gProceduresList=ConsData(gProceduresList,new CProcNode($2,gProcLine)); delete [] $2; } |
				// this case is used when there is no endline at the end of the input
				PROCEDURE IDENTIFIER muchCrap error			{ gProceduresList=ConsData(gProceduresList,new CProcNode($2,gProcLine+1)); delete [] $2; }

%%
