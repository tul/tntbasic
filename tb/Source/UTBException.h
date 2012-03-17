// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UTBException.h
// © Mark Tully 1999
// 22/12/99
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

#include			"TNT_Debugging.h"
#include			"TNTBasic_Errors.h"
#include			"TNTBasic_Types.h"

#include			"CVariableSymbol.h"

class				CStatement;
class				CCString;
class				CCallProc;
class				STextRange;

class CTBRuntimeException : public CAEDescException
{
	protected:
		CStatement					*mExceptionStatement;
	
	public:
									CTBRuntimeException(
										CStatement	*inStatement,
										SInt32		inErrorCode) :
										mExceptionStatement(inStatement),
										CAEDescException(inErrorCode)
										{}
									CTBRuntimeException(
										CStatement				*inStatement,
										const CAEDescException	&inException) :
										mExceptionStatement(inStatement),
										CAEDescException(inException)
										{}
									
		CStatement					*GetStatement() const	{ return mExceptionStatement; }
};

class CTBLinkerException : public CAEDescException
{
	public:
									CTBLinkerException(
										SInt32			inErrorCode,
										AERecord		&inContextRecord) :
										CAEDescException(inErrorCode,inContextRecord,true)
										{}
};

static const AEKeyword				keyTBWasType=		'Typ1';					// used in redeclaring type errors
static const AEKeyword				keyTBNewType=		'Typ2';
static const AEKeyword				keyTBVarName=		'VaNm';					// A variables name
static const AEKeyword				keyTBLabelName=		'LbNm';					// A label's name
static const AEKeyword				keyTBProcName=		'PrNm';					// A procedures name
static const AEKeyword				keyTBSourceLineNum=	'SrLn';					// source code line index
static const AEKeyword				keyTBCanvasIndex=	'CvIx';					// The index of a canvas
static const AEKeyword				keyTBSourceRangeStart='SrSt';
static const AEKeyword				keyTBSourceRangeEnd='SrEn';
static const AEKeyword				keyTBXPos=			'X   ';
static const AEKeyword				keyTBYPos=			'Y   ';
static const AEKeyword				keyTBMoanChannel=	'Chan';
static const AEKeyword				keyTBMoanProg=		'MPrg';
static const AEKeyword				keyTBMoanProgError=	'MErr';

class UTBException
{
	protected:
		static SInt32				sExpectedException;
		
		static void	/*e*/			BadCall(
										CCallProc			*inBadCall,
										SInt32				inErrCode);

	public:
		static void					ExpectException(
										SInt32				inExpected)		{ sExpectedException=inExpected; }
		static SInt32				GetAndClearExpectedException()			{ SInt32 a=sExpectedException; sExpectedException=noErr; return a; }
										
		static void					AddRangeToContext(
										const STextRange	&inRange,
										AERecord			&outContext);

		static void /*e*/			ThrowWithRange(
										const STextRange	&inRange,
										SInt32				inErrCode);

		static void /*e*/			ThrowIfNoParserMemory(
										void				*inMemory)			{ ThrowIfMemFull_(inMemory); }

		static void	/*e*/			ThrowBadForLoopIndex(
										const STextRange			&inRange);
										
		static void /*e*/			ThrowForLoopIdentifierMismatch(
										const STextRange &inRange)				{ ThrowWithRange(inRange,kTBErr_ForLoopIdentifierMismatch); }

		static void /*e*/			ThrowCantFindSuitableDisplay(
										TTBInteger			inXRez,
										TTBInteger			inYRez);

		static void /*e*/			ThrowDuplicateLabel()						{ Throw_(kTBErr_DuplicateLabel); }

		static void /*e*/			ThrowUndefinedLabel(
										const STextRange		&inLabelRange,
										const char				*inLabelName);
		
		static void /*e*/			ThrowReadTypeMismatch()						{ Throw_(kTBErr_ReadTypeMismatch); }
		
		static void /*e*/			ThrowNotInGraphicsMode()					{ Throw_(kTBErr_NotInGraphicsMode); }
		
		static void /*e*/			ThrowOutOfData()							{ Throw_(kTBErr_ReadOutOfData); }
		
		static void /*e*/			ThrowBadFrameRate()							{ Throw_(kTBErr_BadFrameRate); }
		
		static void /*e*/			ThrowArrayNotDimensioned()					{ Throw_(kTBErr_ArrayNotDimensioned); }

		static void /*e*/			ThrowArrayIndexOutOfBounds()				{ Throw_(kTBErr_ArrayIndexOutOfBounds); }

		static void /*e*/			ThrowArrayIndexWrongArity()					{ Throw_(kTBErr_ArrayIndexWrongArity); }

		static void /*e*/			ThrowArrayAlreadyDimensioned()				{ Throw_(kTBErr_ArrayAlreadyDimensioned); }

		static void /*e*/			ThrowNullArrayDimension()					{ Throw_(kTBErr_NullArrayDimension); }
		
		static void /*e*/			ThrowBadFontSize()							{ Throw_(kTBErr_BadFontSize); }
		
		static void /*e*/			ThrowProcNotDefined(
										const STextRange	&inRange,
										const CCString		&inProcName);
		static void /*e*/			ThrowNotAProcedure(
										const STextRange	&inRange,
										const CCString		&inProcName);										
		
		static void /*e*/			ThrowStackOverflow()						{ Throw_(kTBErr_StackOverflow); }
		
		static void /*e*/			ThrowProcParameterTypeMismatch(
										CCallProc			*inBadCall);

		static void /*e*/			ThrowProcParameterMiscount(
										CCallProc			*inBadCall);
		
		static void /*e*/			ThrowProgramAlreadyRunning()				{ Throw_(kTBErr_ProgramAlreadyRunning); }
		
		static void /*e*/			ThrowCreatorCodeNotDefined()				{ Throw_(kTBErr_CreatorCodeNotDefined); }


		// Unused currently?	
		static void /*e*/			ThrowBadSharedVariable()					{ Throw_(kTBErr_BadSharedVariable); }
		
		
		
		static void /*e*/			ThrowBadParameterVariable()					{ Throw_(kTBErr_BadParameterVariable); }
		
		static void /*e*/			ThrowCantUseGlobalAsParameter(
										const STextRange &inRange)				{ ThrowWithRange(inRange,kTBErr_CantUseGlobalAsParam); }
		
		static void /*e*/			ThrowBadGlobalVariable()					{ Throw_(kTBErr_BadGlobalVariable); }
		
		static void /*e*/			ThrowGlobalsMustBeBeforeProcs(const STextRange &inRange)				{ ThrowWithRange(inRange,kTBErr_GlobalsMustBeBeforeProcs); }
		
		static void /*e*/			ThrowCantCloseMainCanvas()					{ Throw_(kTBErr_CantCloseMainCanvas); }

		static void /*e*/			ThrowCantResizeMainCanvas()					{ Throw_(kTBErr_CantResizeMainCanvas); }

		static void /*e*/			ThrowCanvasNotOpen(
										TTBInteger		inCanvasIndex);

		static void /*e*/			ThrowBadCanvasIndex()						{ Throw_(kTBErr_BadCanvasIndex); }

		static void /*e*/			ThrowBadCanvasDimensions()					{ Throw_(kTBErr_BadCanvasDimensions); }
		
		static void /*e*/			ThrowMatrixMustBe2D()						{ Throw_(kTBErr_MatrixMustBe2D); }
		
		static void /*e*/			ThrowTNTMapNotLoaded()						{ Throw_(kTBErr_TNTMapNotLoaded); }
				
		static void /*e*/			ThrowTNTMapNotFound()						{ Throw_(kTBErr_TNTMapNotFound); }

		static void /*e*/			ThrowResTypeWrongLength()					{ Throw_(kTBErr_ResTypeWrongLength); }
		
		static void /*e*/			ThrowTooManyCoOrds()						{ Throw_(kTBErr_ThrowTooManyCoOrds); }
		
		static void /*e*/			ThrowTooManyBreaks()						{ Throw_(kTBErr_TooManyBreaks); }
		
		static void /*e*/			ThrowUndeclaredVariable(
										const STextRange	&inRange,
										const char			*inStr);
		
		static void /*e*/			ThrowVariableRedeclared(
										const STextRange			&inRange,
										CVariableSymbol				*inSymbol,
										CVariableSymbol::ESymbolType inNewType);
										
		static void /*e*/			ThrowNotAnArray(
										CVariableSymbol		*inSymbol,
										bool				inDimensioning,
										const STextRange	&inRange);
		
		static void /*e*/			ThrowCantDefineVarWithProcName(
										const STextRange	&inRange,
										const char		*inStr);

		static void /*e*/			ThrowUnexpectedCharacter(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_UnexpectedCharacter); }
		
		static void /*e*/			ThrowExpectedArrayIndex(
										const STextRange	&inRange,
										CVariableSymbol		*inSymbol);
		static void /*e*/			ThrowExpectedCommaOrSemi(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_ExpectedCommaOrSemi); }
		static void /*e*/			ThrowExpectedComma(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_ExpectedComma); }
		static void /*e*/			ThrowExpectedSemiColon(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_ExpectedSemiColon); }
		static void /*e*/			ThrowExpectedTo(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_ExpectedTo); }
		static void /*e*/			ThrowExpectedNumExp(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_ExpectedNumExp); }
		static void /*e*/			ThrowExpectedStrExp(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_ExpectedStrExp); }

		static void /*e*/			ThrowUnexpectedComma(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_UnexpectedComma); }
		static void /*e*/			ThrowUnexpectedSemiColon(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_UnexpectedSemiColon); }
		static void /*e*/			ThrowUnexpectedTo(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_UnexpectedTo); }
		static void /*e*/			ThrowUnexpectedParameter(
										const STextRange	&inRange)			{ ThrowWithRange(inRange,kTBErr_UnexpectedParameter); }
										
		static void /*e*/			ThrowCouldNotLoadInput()					{ Throw_(kTBErr_CouldNotLoadInput); }

		static void /*e*/			ThrowNoInputElementsDefined()				{ Throw_(kTBErr_NoInputElementsDefined); }
		
		static void /*e*/			ThrowNegativeSpriteIndex()					{ Throw_(kTBErr_NegativeSpriteIndex); }
		
		static void /*e*/			ThrowExpectedNumInAss()						{ Throw_(kTBErr_ExpectedNumericValueInAss); }

		static void /*e*/			ThrowExpectedStrInAss()						{ Throw_(kTBErr_ExpectedStrValueInAss); }

		static void /*e*/			ThrowViewportCanvasZero()					{ Throw_(kTBErr_ViewportCanvasZero); }
		
		static void /*e*/			ThrowViewportOutOfCanvas()					{ Throw_(kTBErr_ViewportOutOfCanvas); }

		static void /*e*/			ThrowViewportLargerThanCanvas()				{ Throw_(kTBErr_ViewportLargerThanCanvas); }
		
		static void /*e*/			ThrowViewportOffScreen()					{ Throw_(kTBErr_ViewportOffScreen); }
		
		static void /*e*/			ThrowViewportNotOpen()						{ Throw_(kTBErr_ViewportNotOpen); }
		
		static void /*e*/			ThrowInvalidProgInfo()						{ Throw_(kTBErr_InvalidProgInfo); }
		
		static void /*e*/			ThrowResourceIndexOutOfRange()				{ Throw_(kTBErr_ResourceIndexOutOfRange); }
		
		static void /*e*/			ThrowFileIsNotLoaded()						{ Throw_(kTBErr_FileIsNotLoaded); }
		
		static void /*e*/			ThrowCouldNotCreateFile()					{ Throw_(kTBErr_CouldNotCreateFile); }
		
		static void /*e*/			ThrowCouldNotOpenFile()						{ Throw_(kTBErr_CouldNotOpenFile); }
		
		static void /*e*/			ThrowCouldNotCreateFolder()					{ Throw_(kTBErr_CouldNotCreateFolder); }

		static void /*e*/			ThrowUnknownFileFormat(
										const char *inFile);
		
		static void /*e*/			ThrowCouldNotDeleteFile()					{ Throw_(kTBErr_CouldNotDeleteFile); }
		
		static void /*e*/			ThrowUnexpectedEndOfFile()					{ Throw_(kTBErr_UnexpectedEndOfFile); }
		
		static void /*e*/			ThrowWrongFileType()						{ Throw_(kTBErr_WrongFileType); }
		
		static void /*e*/			ThrowFileTypeCouldNotBeFound()				{ Throw_(kTBErr_FileTypeCouldNotBeFound); }
		
		static void /*e*/			ThrowFileTypeWrongLength()					{ Throw_(kTBErr_FileTypeWrongLength); }

		static void /*e*/			ThrowMustBeRegistered()						{ Throw_(kTBErr_MustBeRegistered); }
		
		static void /*e*/			ThrowCouldNotInitialiseInput()				{ Throw_(kTBErr_CouldNotInitialiseInput); }
		
		static void /*e*/			ThrowCouldNotPollInput()					{ Throw_(kTBErr_CouldNotPollInput); }
		
		static void /*e*/			ThrowCouldNotConfigureInput()				{ Throw_(kTBErr_CouldNotConfigureInput); }
		
		static void /*e*/			ThrowInvalidDataType()						{ Throw_(kTBErr_InvalidDataType); }
		
		static void /*e*/			ThrowNetworkingNotInitialised()				{ Throw_(kTBErr_NetworkingNotInitialised); }
		
		static void /*e*/			ThrowNetworkGameAlreadyRunning()			{ Throw_(kTBErr_NetworkGameAlreadyRunning); }
		
		static void /*e*/			ThrowBadGlobalMoanRegId(
										char			*inStr);
		static void /*e*/			ThrowBadLocalMoanRegId(
										char			*inStr);
		static void /*e*/			ThrowBadGlobalMoanRegId(
										TTBInteger		inId);
		static void /*e*/			ThrowBadLocalMoanRegId(
										TTBInteger		inId);

};

// use to disable exception logging if a given exception occurs
class StExpectException
{
	public:
					StExpectException(
						SInt32		inErrCode)
					{
						UTBException::ExpectException(inErrCode);
					}
					~StExpectException()
					{
						UTBException::ExpectException(noErr);
					}
};
