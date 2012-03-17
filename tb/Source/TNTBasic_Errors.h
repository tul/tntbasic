// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// TNTBasic_Errors.h
// © Mark Tully 1999
// 21/12/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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

enum
{
	kTBErr_BaseErrorCode				= 16000,

	kTBErr_UnrecognisedBinaryOp			= 0 + kTBErr_BaseErrorCode,			// Binary operator is unrecognised
	kTBErr_ArithmeticOverflow			= 1 + kTBErr_BaseErrorCode,
	kTBErr_ForLoopIdentifierMismatch	= 2 + kTBErr_BaseErrorCode,			// The identifiers in the "FOR" statement and "NEXT" statement don't match
	kTBErr_DuplicateLabel				= 3 + kTBErr_BaseErrorCode,			// A label is defined in two places
	kTBErr_UndefinedLabel				= 4 + kTBErr_BaseErrorCode,			// A label is not defined for a goto/gosub call
	kTBErr_ReadTypeMismatch				= 5 + kTBErr_BaseErrorCode,			// A read statment has been made to assign a variable the wrong type of data
	kTBErr_ReadOutOfData				= 6 + kTBErr_BaseErrorCode,			// Read is out of data
	kTBErr_NotInGraphicsMode			= 7 + kTBErr_BaseErrorCode,			// You must be in graphics mode to do that
	kTBErr_BadFrameRate					= 8 + kTBErr_BaseErrorCode,			// Frame rate is invalid, either zero or too high
	kTBErr_ArrayNotDimensioned			= 9 + kTBErr_BaseErrorCode,			// Array must be dimensioned before use
	kTBErr_ArrayIndexOutOfBounds		=10 + kTBErr_BaseErrorCode,			// Index into array is out of bounds
	kTBErr_ArrayIndexWrongArity			=11 + kTBErr_BaseErrorCode,			// The index used to access the array is the wrong arity. Eg [2,2] is wrong arity for an array Dim Array[5]
	kTBErr_ArrayAlreadyDimensioned		=12 + kTBErr_BaseErrorCode,			// Can't Dim an array twice
	kTBErr_NullArrayDimension			=13 + kTBErr_BaseErrorCode,			// One of the parameters to Dim was null or there were no parameters
	kTBErr_ProcNotDefined				=14 + kTBErr_BaseErrorCode,			// A procedure call cannot be resolved to a procedure
	kTBErr_StackOverflow				=15 + kTBErr_BaseErrorCode,			// Not enough memory for a new stack frame
	kTBErr_ProcParamTypeMismatch		=16 + kTBErr_BaseErrorCode,			// Procedure parameter type mismatch
	kTBErr_ProcParamMiscount			=17 + kTBErr_BaseErrorCode,			// Procedure call has too many/few parameters for the definition
	kTBErr_ProgramAlreadyRunning		=18 + kTBErr_BaseErrorCode,			// A program is already running and we can't run another
	kTBErr_CreatorCodeNotDefined		=19 + kTBErr_BaseErrorCode,			// The creator code for the program has not yet been defined
	kTBErr_TooManyBreaks				=20 + kTBErr_BaseErrorCode,			// the "break" statement attempted to break out of more loops than was available
	kTBErr_BadSharedVariable			=21 + kTBErr_BaseErrorCode,			// Array members cannot be shared. Eg can share gMap[] but not gMap[1,1]
	kTBErr_CantShareParam				=22 + kTBErr_BaseErrorCode,			// A procedures parameter cannot be shared immediately after it's recieved.
	kTBErr_CantUseGlobalAsParam			=23 + kTBErr_BaseErrorCode,			// Attempt to receive a passed parameter in a variable which has the same name as one which is declared as GLOBAL
	kTBErr_BadParameterVariable			=24 + kTBErr_BaseErrorCode,			// Attempt to receive a parameter in a variable which you're not allowed to eg an array member. The only way to receive in a array member is if it's global and we can't receive in globals so this holds together
	kTBErr_BadGlobalVariable			=25 + kTBErr_BaseErrorCode,			// Can't declare non symbols as global. Eg GLOBAL gMap[1,1] is invalid
	kTBErr_GlobalsMustBeBeforeProcs		=26 + kTBErr_BaseErrorCode,			// Globals must be declared before procedures are created
	kTBErr_CantCloseMainCanvas			=27 + kTBErr_BaseErrorCode,			// Can't change the main canvas
	kTBErr_CantResizeMainCanvas			=28 + kTBErr_BaseErrorCode,			//  "
	kTBErr_CanvasNotOpen				=29 + kTBErr_BaseErrorCode,			// Canvas has not been opened
	kTBErr_BadCanvasIndex				=30 + kTBErr_BaseErrorCode,			// Canvas index negative
	kTBErr_BadCanvasDimensions			=31 + kTBErr_BaseErrorCode,			// Attempt to open canvas with neg width/height
	kTBErr_MatrixMustBe2D				=32 + kTBErr_BaseErrorCode,			// Attempt to do canvas FX with an array that aint 2D
	kTBErr_BadFontSize					=33 + kTBErr_BaseErrorCode,			// fontsize <= 0
	kTBErr_TNTMapNotLoaded				=34 + kTBErr_BaseErrorCode,			// Called Set Current Map and passed an invalid or unloaded map
//	kTBErr_TooManyTNTMapsLoaded			=35 + kTBErr_BaseErrorCode,			// Attempt to load too many maps into memory at once
	kTBErr_TNTMapNotFound				=36 + kTBErr_BaseErrorCode,			// Attempt to load a map which does not exist
	kTBErr_ResTypeWrongLength			=37 + kTBErr_BaseErrorCode,			// The specified res type was not four bytes long
	kTBErr_ThrowTooManyCoOrds			=38 + kTBErr_BaseErrorCode,			// There were too many coords passed to this function
	kTBErr_UndeclaredVariable			=39 + kTBErr_BaseErrorCode,			// A variable was used before it was declared
	kTBErr_VariableTypeRedeclared		=40 + kTBErr_BaseErrorCode,			// A variable was redeclared when it already existed with a different type
	kTBErr_CantDimensionNonArray		=41 + kTBErr_BaseErrorCode,			// Type error - Only arrays can be dimensioned
	kTBErr_CantIndexNonArray			=42 + kTBErr_BaseErrorCode,			// Type error - Attempt to index a primitive type
	kTBErr_CantDefineVarWithProcName	=43 + kTBErr_BaseErrorCode,			// Cant declare a variable with the same name as a procedure
	kTBErr_CouldNotLoadInput			=44 + kTBErr_BaseErrorCode,			// Couldn't load the input file.
	kTBErr_ExpectedCommaOrSemi			=45 + kTBErr_BaseErrorCode,			// To was used to seperate items in a place where ; or , was expected
	kTBErr_NegativeSpriteIndex			=46 + kTBErr_BaseErrorCode,			// A negative sprite index was used.
	kTBErr_ExpectedNumericValueInAss	=47 + kTBErr_BaseErrorCode,			// Error generated by numstorage = str
	kTBErr_ExpectedStrValueInAss		=48 + kTBErr_BaseErrorCode,			// Error generated by strstorage = num
	kTBErr_ExpectedComma				=49 + kTBErr_BaseErrorCode,			// , seperator expected but something else found
	kTBErr_ExpectedSemiColon			=50 + kTBErr_BaseErrorCode,			// ; seperator expected but something else found
	kTBErr_ExpectedTo					=51 + kTBErr_BaseErrorCode,			// to seperator expected but something else found
	kTBErr_ExpectedNumExp				=52 + kTBErr_BaseErrorCode,			// expected numeric expression but got something else
	kTBErr_ExpectedStrExp				=53 + kTBErr_BaseErrorCode,			// expected str expression but got something else
	kTBErr_UnexpectedParameter			=54 + kTBErr_BaseErrorCode,			// too many parameters in param list
	kTBErr_UnexpectedComma				=55 + kTBErr_BaseErrorCode,			// too many seperators in param list
	kTBErr_UnexpectedSemiColon			=56 + kTBErr_BaseErrorCode,			// too many seperators in param list
	kTBErr_UnexpectedTo					=57 + kTBErr_BaseErrorCode,			// too many seperators in param list
	kTBErr_IllegalExp					=58 + kTBErr_BaseErrorCode,			// mangled expression eg if+while-4 or something
	kTBErr_NotAProcedure				=59 + kTBErr_BaseErrorCode,			// tried to call a variable, int x : x(5) etc.
	kTBErr_MangledProcedureCall			=60 + kTBErr_BaseErrorCode,			// tried to call a procedure wrong eg wend()
	kTBErr_SyntaxError					=61 + kTBErr_BaseErrorCode,			// Error was unidentified
	kTBErr_MissingEndIf					=62 + kTBErr_BaseErrorCode,			// end if missing from if
	kTBErr_MissingWend					=63 + kTBErr_BaseErrorCode,			// wend missing from while
	kTBErr_MissingNext					=64 + kTBErr_BaseErrorCode,			// next missing from for
	kTBErr_MissingUntilForever			=65 + kTBErr_BaseErrorCode,			// until or forever missing repeat
	kTBErr_ResourceIndexOutOfRange		=66 + kTBErr_BaseErrorCode,			// resource index out of range
	kTBErr_CantPutProcsInProcs			=67 + kTBErr_BaseErrorCode,			// attempt to declare a proc inside a proc
	kTBErr_IllegalProcedureName			=68 + kTBErr_BaseErrorCode,			// mangled procedure name in def
	kTBErr_SharedMustBeUsedInAProcedure =69 + kTBErr_BaseErrorCode,			// shared was used outside of a proc
	kTBErr_AttemptToShareDeclaredVar	=70 + kTBErr_BaseErrorCode,			// they attempted to share a var which was already declared in the local scope. This causes probs for TNT basic as it can't move vars between scopes after they've been declared
	kTBErr_UnmatchedSyntaxError			=71 + kTBErr_BaseErrorCode,			// aim to elimiate all of these
	kTBErr_ExpectedATypeName			=72 + kTBErr_BaseErrorCode,			// missed out a type name in a shared, global or procedure table
	kTBErr_NotAVariable					=73 + kTBErr_BaseErrorCode,			// illegal token when variable expected
	kTBErr_ExpectedOpenBracket			=74 + kTBErr_BaseErrorCode,			// expected open bracket
	kTBErr_ViewportCanvasZero			=75 + kTBErr_BaseErrorCode,			// viewport specified leading to canvas 0
	kTBErr_ViewportOutOfCanvas			=76 + kTBErr_BaseErrorCode,			// viewport rect out of canvas rect
	kTBErr_ViewportOffScreen			=77 + kTBErr_BaseErrorCode,			// viewport displayed off screen
	kTBErr_ViewportNotOpen				=78 + kTBErr_BaseErrorCode,			// viewport is not open
	kTBErr_ForIndexMustBeInteger		=79 + kTBErr_BaseErrorCode,			// For index must be an int rather than a str etc..
	kTBErr_InvalidProgInfo				=80 + kTBErr_BaseErrorCode,			// The program info could not be loaded
	kTBErr_ExpectedArrayIndex			=81 + kTBErr_BaseErrorCode,			// The lexer encountered a symbol declared as an array used with it's [
	kTBErr_UnexpectedCharacter			=82 + kTBErr_BaseErrorCode,			// The lexer encountered an unexpected character
	kTBErr_FileIsNotLoaded				=83 + kTBErr_BaseErrorCode,			// That file is not loaded
	kTBErr_CouldNotCreateFile			=84 + kTBErr_BaseErrorCode,			// Could not create file
	kTBErr_CouldNotOpenFile				=85 + kTBErr_BaseErrorCode,			// Could not open file
	kTBErr_CouldNotCreateFolder			=86 + kTBErr_BaseErrorCode,			// Could not create folder
	kTBErr_CouldNotDeleteFile			=87 + kTBErr_BaseErrorCode,			// Could not delete file
	kTBErr_UnexpectedEndOfFile			=88 + kTBErr_BaseErrorCode,			// Unexpected end of file
	kTBErr_WrongFileType				=89 + kTBErr_BaseErrorCode,			// Wrong File Type
	kTBErr_FileTypeCouldNotBeFound		=90 + kTBErr_BaseErrorCode,			// File Type Could Not Be found
	kTBErr_FileTypeWrongLength			=91 + kTBErr_BaseErrorCode,			// File Type Wrong Length
	kTBErr_MustBeRegistered				=92 + kTBErr_BaseErrorCode,			// Function only permitted if tb is registered
	kTBErr_CantFindSuitableDisplay		=93 + kTBErr_BaseErrorCode,			// can't find a suitable display for the requested rez
	kTBErr_CouldNotInitialiseInput		=94 + kTBErr_BaseErrorCode,			// Could not initialise the input
	kTBErr_CouldNotPollInput			=95 + kTBErr_BaseErrorCode,			// Could not poll input
	kTBErr_CouldNotConfigureInput		=96 + kTBErr_BaseErrorCode,			// Could not configure the input
	kTBErr_ViewportLargerThanCanvas		=97 + kTBErr_BaseErrorCode,			// An attempt to open a view port that is larger than the src canvas - yields undefined areas

	// moan compile errors (these are run time errors)
	kTBErr_ExpectedANext				=98 + kTBErr_BaseErrorCode,			// No next for a for
	kTBErr_ExpectedATo					=99 + kTBErr_BaseErrorCode,			// Expected a to in a for..next decl
	kTBErr_UnexpectedOpenBracket		=100 + kTBErr_BaseErrorCode,
	kTBErr_MaxNestedForLoopsExceeded	=101 + kTBErr_BaseErrorCode,
	kTBErr_MiscErrorInInputString		=102 + kTBErr_BaseErrorCode,		// An expected char in a moan program
	kTBErr_ExpectedARegister			=103 + kTBErr_BaseErrorCode,
	kTBErr_ExpectedAnEquals				=104 + kTBErr_BaseErrorCode,		// Expected an assignment op for a for..next def
	kTBErr_UnexpectedNext				=105 + kTBErr_BaseErrorCode,		// A next without a matching for
	kTBErr_ExpectedAJump				=106 + kTBErr_BaseErrorCode,		// Expected a jump
	kTBErr_ExpectedALabel				=107 + kTBErr_BaseErrorCode,
	kTBErr_UnknownMoanFunction			=108 + kTBErr_BaseErrorCode,		// Probably a misuse of capitalisation
	kTBErr_UnexpectedOp					=109 + kTBErr_BaseErrorCode,		// An op which is out of place
	kTBErr_MalformedExpressionPair		=110 + kTBErr_BaseErrorCode,		// When defining anim lists, bad pair of expressions (e.g. missing close bracket or something)
	kTBErr_ExpectedACloseBracket		=111 + kTBErr_BaseErrorCode,		// Expected a close bracket to match a previous (
	kTBErr_BadRegisterId				=112 + kTBErr_BaseErrorCode,		// Bad register id	

	kTBErr_BadMoanLocalRegId			=113 + kTBErr_BaseErrorCode,		// tb get/set moan channel command - bad reg
	kTBErr_BadMoanGlobalRegId			=114 + kTBErr_BaseErrorCode,		// tb get/set moan channel command - bad reg	
	
	// moan vm runtime errors
	kTBErr_BadJumpAddress				=115 + kTBErr_BaseErrorCode,		// branch to address had no address set, code gen error
	kTBErr_UndefinedMoanLabel			=116 + kTBErr_BaseErrorCode,		// label being jumped to is undefined
	
	kTBErr_ExpectedAnEndProc			=117 + kTBErr_BaseErrorCode,		// missing end proc statement
	
	kTBErr_UnknownFileType				=118 + kTBErr_BaseErrorCode,		// file type was unknown (not 'Data' or 'Text' as of writing)

	// moan again
	kTBErr_ExpectedAValue				=119 + kTBErr_BaseErrorCode,		// expected a value but found something else
	
	kTBErr_NoInputElementsDefined		=120 + kTBErr_BaseErrorCode,			// no input elements defined - messes up isp
	
	// data folder run time errors
	kTBErr_InvalidDataType				=121 + kTBErr_BaseErrorCode,		// the specified data type was not recognised
	
	kTBErr_NetworkingNotInitialised		=122 + kTBErr_BaseErrorCode,
	kTBErr_NetworkGameAlreadyRunning	=123 + kTBErr_BaseErrorCode,
	kTBErr_UnrecognisedResFileFormat	=124 + kTBErr_BaseErrorCode,
};
