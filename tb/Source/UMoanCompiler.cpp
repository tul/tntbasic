// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UMoanCompiler.cpp
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

/*
	Compiles a moan string into TB VM code
	
	It's a simple language that is designed to be compiled in real time quickly and easily
	It's almost easy enough to be real time interpreted, but the expressions require use of
	a stack so you may as well compile it to eliminate that need.
*/

#include "UMoanCompiler.h"
#include "CVMCodeChunk.h"
#include "UTBException.h"

// statics for use when throwing errors
char		*UMoanCompiler::sSourceProgram=0;
SInt32		UMoanCompiler::sChannelId=-1;


#define		kUnaryMinus		'_'

// these are indexes into the functions table, these will be dynamic in TNT Basic, but for moan they're fixed
// MOAN specific functions
static const SInt32	kFun_Move=0;			// moves a sprite, data: x,y,speed
static const SInt32	kFun_Anim=1;			// animates a sprite, data: #loops, #datums, (image,delay)+
static const SInt32	kFun_Scale=2;			// scales a sprite, data: xdelta,ydelta,speed
static const SInt32	kFun_Rotate=3;			// rotate a sprite, data: rdelta,speed,forever

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Precedence
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Inline func to evaluate the precedence of an operator
inline UInt8 Precedence(
	char	inOperator);
inline UInt8 Precedence(
	char	inOperator)
{
	switch (inOperator)
	{
		case '=':
		case '>':
		case '<':
		case '³':
		case '²':
		case '­':
			return 1;
			break;
	
		case '+':
		case '-':
			return 2;
			break;
			
		case '*':
		case '/':
			return 3;
			break;

		// NB:	Not sure about the relative precendences of the unary - and the ^ operator
		//		What should -2^2 evaluate to? (-2)^2 or -(2^2) ???
		//		I choose -(2^2)
		case kUnaryMinus:
			return 4;
			break;
		
		case '^':
			return 5;
			break;			
			
		default:
			return 255;		// err...
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AppendArithOp
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Inline func to add an arith op to a code chunk
inline void AppendArithOp(
	CVMCodeChunk	*ioCode,
	char			inArithOp);
inline void AppendArithOp(
	CVMCodeChunk	*ioCode,
	char			inArithOp)
{
	if (ioCode)
	{
		SVMInstruction	ins(kIns_Nop, SInt32(0));
							
		switch (inArithOp)
		{
			case '+':
				ins.op=kIns_Add;
				break;

			case '-':
				ins.op=kIns_Sub;
				break;

			case '*':
				ins.op=kIns_Mul;
				break;

			case '/':
				ins.op=kIns_Div;
				break;

			case kUnaryMinus:
				ins.op=kIns_Neg;
				break;
				
			case '^':
				ins.op=kIns_Power;
				break;
				
			case '>':
				ins.op=kIns_GT;
				break;
				
			case '<':
				ins.op=kIns_LT;
				break;
			
			case '=':
				ins.op=kIns_EQ;
				break;	
		
			case '­':
				ins.op=kIns_NEQ;
				break;
				
			case '²':
				ins.op=kIns_LTE;
				break;
				
			case '³':
				ins.op=kIns_GTE;
				break;
				
			default:
				std::cout << "Internal moan error: Unknown arith op, cannot generate VM code for " << inArithOp << "\n";
				Throw_(-1);
				break;
		}
		
		ioCode->AppendInstruction(ins);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CompileMoanProgram
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// a moan program is a string of statements separated by ; or spaces
// The program spec is in Moan.doc
// The channelId is for reference when reporting errors only.
void UMoanCompiler::CompileMoanProgram(
	char			*inMoanProgram,
	CVMCodeChunk	*outCode,
	SInt32			inChannelId)
{
	bool				doneLexing=false,expectingTerm=true;
	char				*str=inMoanProgram;
	TTokenType			type;
	STokenValue			value;
	static const SInt32	kMaxNestedForLoops=15;
	
	struct SForHeader
	{
		SInt32		loopStart;			// where the next should branch to
		SInt32		exitTestLoc;		// address of test that exits the for loop (at loop head)
		char		inductionReg;		// reg that should be increased by the next statement
	};
	SForHeader		forStack[kMaxNestedForLoops];
	SInt32			forStackP=0;
	#define			PUSH_FOR_HEADER(f)		do { if (forStackP>=kMaxNestedForLoops) \
													ThrowCompilerError(str,kTBErr_MaxNestedForLoopsExceeded);\
												else \
													forStack[forStackP++]=f;\
											} while (false)
	#define			FOR_STACK_EMPTY			(forStackP==0)
	#define			POP_FOR_HEADER(f)		f=forStack[--forStackP]	
	
	sChannelId=inChannelId;
	sSourceProgram=inMoanProgram;
	
	while (!doneLexing)
	{
		str=PullToken(str,expectingTerm,kOperator|kWhiteSpace|kMoanCommand|kLabel,type,value);
		
		switch (type)
		{
			case kError:
				//std::cout << "Error in input stream!\n";
				ThrowCompilerError(str,kTBErr_MiscErrorInInputString);
				doneLexing=true;
				break;
			
			case kNOP:	
			case kWhiteSpace:
				break;
				
			case kLabel:
				outCode->AppendLabel(value.c);
				break;
				
			case kMoanCommand:
				switch (value.c)
				{
					case 'M':
						// produce code for 3 expressions
						str=Compile1Expression(str,outCode);
						str=Compile1Expression(str,outCode);
						str=Compile1Expression(str,outCode);
						
						// now create a move command
						{
							SVMInstruction	ins(kIns_FuncCall,kFun_Move);
							outCode->AppendInstruction(ins);
						}
						break;
						
					case 'A':
						// read the loop count variable
						str=Compile1Expression(str,outCode);
						// now keep reading (image,delay) pairs until we're done
						str=CompileExpressionPairs(str,outCode);
						
						// now create an anim command
						{
							SVMInstruction	ins(kIns_FuncCall,kFun_Anim);
							outCode->AppendInstruction(ins);
						}
						break;
						
					case 'S':
						// read the 3 expressions
						str=Compile1Expression(str,outCode);
						str=Compile1Expression(str,outCode);
						str=Compile1Expression(str,outCode);
						
						// now create a scale command
						{
							SVMInstruction	ins(kIns_FuncCall,kFun_Scale);
							outCode->AppendInstruction(ins);
						}
						break;
						
					case 'R':
						// read the 3 expressions
						str=Compile1Expression(str,outCode);
						str=Compile1Expression(str,outCode);
						str=Compile1Expression(str,outCode);
						
						// now create a rotate command
						{
							SVMInstruction	ins(kIns_FuncCall,kFun_Rotate);
							outCode->AppendInstruction(ins);
						}
						break;
					
					case 'P':	// pause command, emit a nop which will cause a run slice to break at that point
						{
							SVMInstruction	ins(kIns_Nop,SInt32(0));
							outCode->AppendInstruction(ins);
						}
						break;
					
					case 'F':
					case 'L':
						{
							STokenValue		reg,value2;
						
							// read register
							do
							{
								str=PullToken(str,expectingTerm,kRegister|kWhiteSpace,type,reg);
							} while (type==kWhiteSpace || type==kNOP);
							
							if (type!=kRegister)
							{
								//std::cout << "Error, register expected!\n";
								ThrowCompilerError(str,kTBErr_ExpectedARegister);
								doneLexing=true;
								break;
							}
							
							// read equals
							do
							{
								str=PullToken(str,expectingTerm,kOperator|kWhiteSpace,type,value2);
							} while (type==kWhiteSpace || type==kNOP);
							
							if (value2.c!='=')
							{
								//std::cout << "Error, = expected!\n";
								ThrowCompilerError(str,kTBErr_ExpectedAnEquals);
								doneLexing=true;
								break;
							}
							
							// read expression
							str=Compile1Expression(str,outCode);
							
							// create an assign register command
							{
								SVMInstruction	ins(kIns_SaveRegister, reg.c);
								outCode->AppendInstruction(ins);
							}
						
							if (value.c=='L')	// let command finishes here, for continues
								break;
							
							// for command
							// have read	for reg=exp
							// now read		to exp and put the exp on the stack
							SForHeader		f;
							
							f.inductionReg=reg.c;
							f.loopStart=outCode->GetCodeLen()+1;	// this is where we'll branch back to (1 based)

							// we've got the assignment, now generate the test instructions to see if the for
							// loop has completed

							// load reg							
							{
								SVMInstruction	ins(kIns_LoadRegister, reg.c);
								outCode->AppendInstruction(ins);
							}
							
							// read 'To exp'
							do
							{
								str=PullToken(str,expectingTerm,kMoanCommand|kWhiteSpace,type,value2);
							} while (type==kWhiteSpace || type==kNOP);
							
							if (value2.c!='T')
							{
								//std::cout << "Error, T expected!\n";
								ThrowCompilerError(str,kTBErr_ExpectedATo);
							}
							
							// read exp
							str=Compile1Expression(str,outCode);
							
							
							// generate compare and branch to exit the for loop if the test fails
							{
								SVMInstruction	ins(kIns_GTE, SInt32(0));
								outCode->AppendInstruction(ins);
								
								// branch to immediate address to get out of loop if reg is >=
								// end of 'to' range
								ins.op=kIns_BNZI;
								ins.data.i=0xFFFFFFFF;		// we'll come back and patch this up later when we know where the 'next' is
								outCode->AppendInstruction(ins);
							}

							f.exitTestLoc=outCode->GetCodeLen(); // 1 based
							PUSH_FOR_HEADER(f);
						}
						break;
					
					case 'N':	// next
						{
							SForHeader		f;
							if (FOR_STACK_EMPTY)
								//std::cout << "Next without matching for!\n";
								ThrowCompilerError(str,kTBErr_UnexpectedNext);
							else
							{
								POP_FOR_HEADER(f);
								
								// generate an instructions to increase the induction var by 1
								{
									SVMInstruction	ins(kIns_LoadRegister,f.inductionReg);

									outCode->AppendInstruction(ins);
									ins.op=kIns_LoadI;
									ins.data.i=1;
									outCode->AppendInstruction(ins);
									ins.op=kIns_Add;
									ins.data.i=NULL;
									outCode->AppendInstruction(ins);
									ins.op=kIns_SaveRegister;
									ins.data.i=f.inductionReg;
									outCode->AppendInstruction(ins);
								}

								// generate a loop instruction to go back to the for test
								{
									SVMInstruction	ins(kIns_BRAI,f.loopStart);
									outCode->AppendInstruction(ins);
								}
								
								// patch up the for test exit instruction so that it can jump out if to is completed
								Assert_((*outCode)[f.exitTestLoc].op==kIns_BNZI);
								(*outCode)[f.exitTestLoc].data.i=outCode->GetCodeLen()+1; // next instruction
							}
						}
						break;	
						
					case 'I':	// if
						{
							// read expression
							str=Compile1Expression(str,outCode);
							
							// read jump statement
							do
							{
								str=PullToken(str,expectingTerm,kMoanCommand|kWhiteSpace,type,value);
							} while (type==kWhiteSpace || type==kNOP);
							
							if (value.c!='J')
							{
								//std::cout << "Error, J expected!\n";
								ThrowCompilerError(str,kTBErr_ExpectedAJump);
							}
							
							// read label to jump to
							do
							{
								str=PullToken(str,expectingTerm,kForceLabel|kWhiteSpace,type,value);
							} while (type==kWhiteSpace || type==kNOP);							
							
							if (type!=kLabel)
							{
								//std::cout << "Error, label expected\n";
								ThrowCompilerError(str,kTBErr_ExpectedALabel);
							}
							else
							{
								// create the conditional jump command
								SVMInstruction	ins(kIns_BNZ,value.c);
								outCode->AppendInstruction(ins);
							}
						}
						break;
						
					case 'J':	// jump
						{
							// read label to jump to
							do
							{
								str=PullToken(str,expectingTerm,kForceLabel|kWhiteSpace,type,value);
							} while (type==kWhiteSpace || type==kNOP);							
							
							if (type!=kLabel)
							{
								//std::cout << "Error, label expected\n";
								ThrowCompilerError(str,kTBErr_ExpectedALabel);
							}
							else
							{
								// create the conditional jump command
								SVMInstruction	ins(kIns_Jump,value.c);
								outCode->AppendInstruction(ins);
							}
						}
						break;
						
					default:
						//std::cout << "Unknown MOAN function, probably a misuse of capitalisation\n";
						ThrowCompilerError(str,kTBErr_UnknownMoanFunction);
						break;
				}
				break;
				
			default:
				doneLexing=true;
				break;
		}
	}

	if (outCode)
	{
		SVMInstruction	ins(kIns_Halt,SInt32(0));
		outCode->AppendInstruction(ins);
	}
	
	if (!FOR_STACK_EMPTY)
		ThrowCompilerError(str,kTBErr_ExpectedANext);

	// it's not massively important that these not be left hanging, as they're only used from
	// inside this function and this function always sets them before use.
	sChannelId=-1;
	sSourceProgram=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CompileExpressionPairs
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This routine scans a string for pairs of expressions in brackets, e.g. (a,b),(a,b),(a,b)
char *UMoanCompiler::CompileExpressionPairs(
	char			*inString,
	CVMCodeChunk	*outCode)
{
	// pull token, we want a bracket and nothing else
	bool			doneLexing=false;
	SInt32			pairs=0;		// how many pairs we processed
	TTokenType		type;
	STokenValue		value;
	bool			expectingTerm=true;
	
	enum EState
	{
		kBeforeExpression1,
		kAfterExpression2,
		kBetweenPairs
	} state;
	
	state=kBeforeExpression1;
	
	while (!doneLexing)
	{
		// depending on whether we're expecting an ( or a ) it determines whether we want
		// the tokeniser to cry if it gets a term or not
		if (state==kBeforeExpression1)
			expectingTerm=true;
		else
			expectingTerm=false;
			
		inString=PullToken(inString,expectingTerm,kPunctuation|kWhiteSpace|kOperator,type,value);
	
		switch (type)
		{
			case kError:
				//std::cout << "Error in input stream!\n";
				ThrowCompilerError(inString,kTBErr_MiscErrorInInputString);
				doneLexing=true;
				break;

			case kOperator:
				if (value.c=='(')
				{
					if (state!=kBeforeExpression1)
					{
						//std::cout << "Error in input stream, unexpected (\n";
						ThrowCompilerError(inString,kTBErr_UnexpectedOpenBracket);
						doneLexing=true;
					}
					else
					{
						// signals the start of an expression pair
						inString=Compile1Expression(inString,outCode);
						inString=Compile1Expression(inString,outCode);
						++pairs;
						state=kAfterExpression2;
					}
				}
				else if (value.c==')' && state==kAfterExpression2)
				{
					state=kBetweenPairs;
				}
				else
				{
					// error
					//std::cout << "Error in input stream, unexpected operator " << ((char)value) << "\n";
					ThrowCompilerError(inString,kTBErr_UnexpectedOp);
				}
				break;
				
			case kPunctuation:
				if (value.c==',' && state==kBetweenPairs)
					state=kBeforeExpression1;
				else
					//std::cout << "Error in input stream, unexpected character " << ((char)value) << "\n";
					ThrowCompilerError(inString,kTBErr_MiscErrorInInputString);
				break;
				
			case kEOF:
				doneLexing=true;
				if ((state!=kBetweenPairs) && (!pairs && state!=kBeforeExpression1))
					//std::cout << "Error in input stream, bad expression pair\n";
					ThrowCompilerError(inString,kTBErr_MalformedExpressionPair);
				break;
			
			case kNOP:
			case kWhiteSpace:
				break;
			
			default:
				//std::cout << "Unknown token in input stream\n";
				ThrowCompilerError(inString,kTBErr_MiscErrorInInputString);
				break;
		}
	}
	
	// push the number of pairs processed so the function knows how many are there
	{
		SVMInstruction	ins(kIns_LoadI, pairs);
		outCode->AppendInstruction(ins);
	}
	
	return inString;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Compile1Expression
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// compiles part of a string, breaking at eof or anything that isn't part of an expression
char *UMoanCompiler::Compile1Expression(
	char			*inString,
	CVMCodeChunk	*outCode)
{
	bool			doneLexing=false;
	TTokenType		type;
	STokenValue		value;
	char			opStack[100];
	UInt8			opStackIndex=0;
	bool			expectingTerm=true;

	// Lex the string
	while (!doneLexing)
	{
		inString=PullToken(inString,expectingTerm,kOperator|kWhiteSpace|kInteger|kFloat|kRegister|kPunctuation,type,value);
		
		switch (type)
		{
			case kError:
//				std::cout << "Error in input stream!\n";
				ThrowCompilerError(inString,kTBErr_MiscErrorInInputString);
				doneLexing=true;
				break;
				
			case kRegister:
				if (outCode)
				{
					SVMInstruction	ins(kIns_LoadRegister, value.c);
					outCode->AppendInstruction(ins);
				}
				break;
				
			case kInteger:
//				std::cout << "Number: " << value << "\n";
				if (outCode)
				{
					SVMInstruction	ins(kIns_LoadI, value.i);
					outCode->AppendInstruction(ins);
				}
				break;
				
			case kFloat:
				if (outCode)
				{
					SVMInstruction	ins(kIns_LoadF, value.f);
					outCode->AppendInstruction(ins);
				}
				break;
								
			case kOperator:
//				std::cout << "Operator: " << (char)value << "\n";
				if (value.c==')')		// dump until last bracket
				{
					while (opStackIndex && opStack[opStackIndex-1]!='(')
					{						
						AppendArithOp(outCode,opStack[--opStackIndex]);
					}
					
					if (opStack[opStackIndex-1]!='(')		// missing open bracket, that's OK, it just means end of expression, assume the open bracket is at a higher level
					{
						//std::cout << "Error, missing '('\n";
						// possibly slightly dodgy trick to put the ) back in the string
						--inString;
						doneLexing=true;
					}
					else
						--opStackIndex;	// dump (				
				}
				else if (opStackIndex==0 || value.c=='(' || Precedence(value.c) > Precedence(opStack[opStackIndex-1]) || (value.c==opStack[opStackIndex-1] && value.c==kUnaryMinus))			// note, right assoc ops should be pushed if == to prec of prev op also, only right assoc op is unary -
					opStack[opStackIndex++]=value.c;
				else
				{
					// dump ops until precendence exceeds (don't dump ( )
					while (opStackIndex && (opStack[opStackIndex-1]!='(' && Precedence(value.c) <= Precedence(opStack[opStackIndex-1])))
					{
						AppendArithOp(outCode,opStack[--opStackIndex]);
					}
					opStack[opStackIndex++]=value.c;
				}
				break;
								
			case kPunctuation:		// punc means end of expression, swallow it
			case kEOF:
				if (expectingTerm)
					ThrowCompilerError(inString,kTBErr_ExpectedAValue);

				doneLexing=true;
				while (opStackIndex)
				{
					AppendArithOp(outCode,opStack[opStackIndex-1]);

					if (opStack[--opStackIndex]=='(')
						//std::cout << "Unmatched '(' bracket\n";
						ThrowCompilerError(inString,kTBErr_ExpectedACloseBracket);
				}
				break;
			
			case kWhiteSpace:
			case kNOP:
				break;
				
			default:
				//std::cout << "Unknown token\n";
				ThrowCompilerError(inString,kTBErr_MiscErrorInInputString);
				break;
		}
	}
	
	return inString;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsDigit
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
inline bool IsDigit(
	char	inChar);
inline bool IsDigit(
	char	inChar)
{
	return (inChar >= '0' && inChar <= '9');
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsWhiteSpace
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
inline bool IsWhiteSpace(
	char	inChar);
inline bool	IsWhiteSpace(
	char	inChar)
{
	return inChar==' ' || inChar=='\t' || (inChar >='a' && inChar <= 'z') || inChar==';';	// semi colon is another seperator
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsArithOperator
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
inline bool IsArithOperator(
	char	inChar);
inline bool IsArithOperator(
	char	inChar)
{
	switch (inChar)
	{
		case '+':
		case '-':
		case '/':
		case '*':
		case '(':
		case ')':
		case '^':
		case '=':
		case '>':
		case '<':
//		case '­':	// these don't occur in input strings
//		case '²':
//		case '³':
			return true;
			break;
	}
	
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ParseOp
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// allows two char arith ops such as >= <= and <>
inline char *ParseOp(
	char	*inStr,
	SInt32	&outValue)
{
	switch (*inStr)
	{
		case '+':
		case '-':
		case '/':
		case '*':
		case '(':
		case ')':
		case '^':
		case '=':
			outValue=*inStr;
			return inStr+1;
			break;
			
		case '>':
			if (inStr[1]=='=')
			{
				outValue='³';
				return inStr+2;
			}
			outValue=*inStr;
			return inStr+1;
			break;
			
		case '<':
			if (inStr[1]=='=')
			{
				outValue='²';
				return inStr+2;
			}
			else if (inStr[1]=='>')
			{
				outValue='­';
				return inStr+2;
			}
			outValue=*inStr;
			return inStr+1;
			break;
	}
	
	return inStr;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PullToken
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Lexes one token from the string, returns the new offset into the string
// An expression is made up of alternating operators and terms, except for the case of brackets, which are seen as operators but can occur either when a term is
// expected (for an '(') or when an operator is expected (in the case for a  ')')
// If one knows whether to expect a term or an operator, one knows whether to interpret + and - as a unary or binary opeator
char *UMoanCompiler::PullToken(
	char			*ioString,
	bool			&ioExpectTerm,
	TTokenType		inMask,
	TTokenType		&outType,
	STokenValue		&outValue)
{

	if (*ioString==0)	// eof
	{
		outType=kEOF;
		return ioString;
	}
	
	// do a label check before we begin interpretting labels as other things
	if (ioString[0]>='A' && ioString[0]<='Z')	// label
	{
		// is this letter followed by a :?
		char		*str=ioString+1;	// skip letter
		while (IsWhiteSpace(*str))		// skip white space
			str++;
		
		if (*str==':')
		{		
			if (inMask & kLabel)
			{
				outType=kLabel;
				outValue.c=ioString[0];
				return str+1;
			}
			else
			{
				outType=kEOF;
				return ioString;
			}
		}
		else	// it's a capital letter, are we forcing to labels in which case interpret it as a label anyway (this is used for jump commands)
		{
			if (inMask & kForceLabel)
			{
				outType=kLabel;
				outValue.c=ioString[0];
				return ioString+1;
			}
		}
	}
	
	switch (ioString[0])
	{
		case 0:	// eof
			outType=kEOF;
			return ioString;
			break;
			
		case ',':
			if (inMask & kPunctuation)
			{
				outType=kPunctuation;
				outValue.c=ioString[0];
				return ioString+1;
			}
			else
			{
				outType=kEOF;
				return ioString;
			}
			break;
			
		case 'X':
		case 'Y':
		case 'I':	// special regs
			if ((inMask & kRegister) && ioExpectTerm)
			{
				outType=kRegister;
				ioExpectTerm=false;			// expect an op next
				outValue.c=ioString[0]+32;	// lower case letter for special regs
				return ioString+1;
			}
			else
			{
				if (ioString[0]=='I')		// bit of overloading, here...
				{
					if (inMask & kMoanCommand)	// return an if statement
					{
						outType=kMoanCommand;
						outValue.c=ioString[0];
						return ioString+1;
					}
				}
				
				outType=kEOF;
				return ioString;
			}
			break;
			
		case 'R':
			if ((inMask & kRegister) && ioExpectTerm)		// register or rotate command
			{
				outType=kRegister;
				
				char	reg=*(ioString+1);
				
				if ((reg>='0' && reg<='9') || (reg>='A' && reg<='Z'))
				{				
					ioString+=2;
					outValue.c=reg;
					ioExpectTerm=false;		// expect an op next
				}
				else
				{
					//std::cout << "Error in input stream, bad register id\n";
					ThrowCompilerError(ioString,kTBErr_BadRegisterId);
					outType=kError;
				}
			}
			else if (inMask & kMoanCommand)		// rotate
			{
				outType=kMoanCommand;
				outValue.c=ioString[0];
				return ioString+1;
			}
			else
			{
				outType=kEOF;
				return ioString;
			}
			break;
							
		case 'M':
		case 'A':
		case 'S':	// scale statement
		case 'L':	// let statement
		case 'J':	// jump
		case 'F':
		case 'T':	// to
		case 'N':	// next
		case 'P':	// pause
			if (inMask & kMoanCommand)
			{
				outType=kMoanCommand;
				outValue.c=ioString[0];
				return ioString+1;
			}
			else
			{
				outType=kEOF;
				return ioString;
			}
			break;
												
		default:
			if (IsWhiteSpace(ioString[0]))		// white space includes lower case letters
			{
				if (inMask & kWhiteSpace)
				{			
					char	*temp=ioString+1;
					
					while (IsWhiteSpace(*temp))
						temp++;
					
					outType=kWhiteSpace;
					return temp;
				}
				else
				{
					outType=kEOF;
					return ioString;
				}
			}		
			else if (IsDigit(ioString[0])) 			// Number
			{
				if (inMask & (kInteger|kFloat))
				{
					char		*end=ioString;
					char		temp;
					bool		seenDP=false;
					
					do
					{
						++end;
						
						if ((inMask & kFloat) && *end=='.' && !seenDP)
						{
							seenDP=true;
							end++;
						}
						
					} while (IsDigit(*end));
					
					temp=*end;
					*end=0;
					if (seenDP)
					{
						outValue.f=std::atof(ioString);
						outType=kFloat;
					}
					else
					{
						outValue.i=std::atoi(ioString);
						outType=kInteger;
					}
					*end=temp;
					ioExpectTerm=false;		// expect an operator next
					return end;
				}
				else
				{
					outType=kEOF;
					return ioString;
				}
			}
			else if (IsArithOperator(ioString[0]))
			{
				char	*outStr=ioString+1;
			
				if (inMask & kOperator)
				{
					outType=kOperator;
					
					SInt32	op;
					outStr=ParseOp(ioString,op);
					outValue.c=op;

					// if we got an operator when we were expecting a term then it's either unary +/-, an open bracket or an error
					if (ioExpectTerm==true)
					{
						switch (outValue.c)
						{
							case '-':
								outValue.c=kUnaryMinus;
								break;

							case '+':
								outType=kNOP;
								break;	// ignore these

							case '(':	// open bracket
								break;
								
							default:	// some other op, must be an error
								outType=kError;
								break;
						}
					}

					if (ioString[0]!=')')		// don't expect a term after a close bracket, these are followed by operators					
						ioExpectTerm=true;
					return outStr;
				}
				else
				{
					outType=kEOF;
					return ioString;
				}
			}
			else
			{
				outType=kError;
				return ioString+std::strlen(ioString);		// skip to eof
			}
			break;
	}
	
	return ioString;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowCompilerError
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Gets the moan channel id and program string, then wraps it in a tb error string
// Note that these are runtime errors, not compile time errors, as the moan progs are
// compiled at runtime
void UMoanCompiler::ThrowCompilerError(
	const char		*inString,
	OSErr			inErr)
{
	if (sChannelId==-1 || sSourceProgram==0)		// channel or string not set
	{
		SignalString_("Internal moan compiler error! Please report to bugs@tntbasic.com.");
		Throw_(-1);
	}
	
	StAEDescriptor		str(typeText,sSourceProgram,std::strlen(sSourceProgram)),chan(sChannelId),errLoc((SInt32)(inString-sSourceProgram));
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBMoanChannel,chan);
	UAEDesc::AddKeyDesc(&context,keyTBMoanProg,str);
	UAEDesc::AddKeyDesc(&context,keyTBMoanProgError,errLoc);
	
	sChannelId=-1;
	sSourceProgram=0;
	
	ThrowAEErr_(inErr,context);
}