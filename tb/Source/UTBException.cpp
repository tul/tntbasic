// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UTBException.cpp
// © Mark Tully 2000
// 14/4/00
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
	Use this class for throwing exceptions. Later it may proove useful to have centralised error dispatching (think error
	reporting, logging etc...)
*/

#include	"UTBException.h"
#include	"CCString.h"
#include	"Procedural_Statements.h"
#include	"BisonTokenLocation.h"
#include	"Root.h"

void TypeToAEDesc(
	CVariableSymbol::ESymbolType	inType,
	StAEDescriptor					&outDesc);

SInt32 UTBException::sExpectedException=noErr;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AddRangeToContext
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// adds the text range specified to the aecontext
void UTBException::AddRangeToContext(
	const STextRange	&inRange,
	AERecord			&outContext)
{
	Try_
	{
		StAEDescriptor		start(inRange.startOffset),end(inRange.endOffset),line(inRange.line);
		
		UAEDesc::AddKeyDesc(&outContext,keyTBSourceRangeStart,start);
		UAEDesc::AddKeyDesc(&outContext,keyTBSourceRangeEnd,end);
		UAEDesc::AddKeyDesc(&outContext,keyTBSourceLineNum,line);
	}
	Catch_(err)
	{
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowWithRange
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// adds the text range specified to the aecontext and then throws the error
void UTBException::ThrowWithRange(
	const STextRange	&inRange,
	SInt32				inErrCode)
{
	AERecord			context={typeNull,0};

	AddRangeToContext(inRange,context);

	ThrowAEErr_(inErrCode,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowCantDefineVarWithProcName				/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowCantDefineVarWithProcName(
	const STextRange	&inRange,
	const char			*inStr)
{
	StAEDescriptor		varName(typeText,inStr,std::strlen(inStr));
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,varName);
	AddRangeToContext(inRange,context);
	
	ThrowAEErr_(kTBErr_CantDefineVarWithProcName,context);	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowNotAnArray								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called when attempting to dimension a primitive type instead of an array or using a primitive type as an array.
void UTBException::ThrowNotAnArray(
	CVariableSymbol		*inSymbol,
	bool				inDimensioning,
	const STextRange	&inRange)
{
	StAEDescriptor		varName(typeText,inSymbol->GetString(),inSymbol->GetString().GetLength());
	StAEDescriptor		wasType;
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,varName);

	TypeToAEDesc(inSymbol->GetType(),wasType);

	UAEDesc::AddKeyDesc(&context,keyTBWasType,wasType);
	AddRangeToContext(inRange,context);
	
	ThrowAEErr_(inDimensioning ? kTBErr_CantDimensionNonArray : kTBErr_CantIndexNonArray,context);	
}
	
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowUndeclaredVariable									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adds the name of the undeclared variable
void UTBException::ThrowUndeclaredVariable(
	const STextRange	&inRange,
	const char			*inStr)
{
	StAEDescriptor		varName(typeText,inStr,std::strlen(inStr));
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,varName);
	AddRangeToContext(inRange,context);
	
	ThrowAEErr_(kTBErr_UndeclaredVariable,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ TypeToAEDesc												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Support routine for converting a type to a string
void TypeToAEDesc(
	CVariableSymbol::ESymbolType	inType,
	StAEDescriptor					&outDesc)
{
	switch (inType)
	{
		case CVariableSymbol::kInteger:
			outDesc.Assign("\pinteger");
			break;

		case CVariableSymbol::kFloat:
			outDesc.Assign("\pfloat");
			break;

		case CVariableSymbol::kString:
			outDesc.Assign("\pstring");
			break;

		case CVariableSymbol::kIntegerArray:
			outDesc.Assign("\pinteger[]");
			break;

		case CVariableSymbol::kFloatArray:
			outDesc.Assign("\pfloat[]");
			break;

		case CVariableSymbol::kStringArray:
			outDesc.Assign("\pstring[]");
			break;

		case CVariableSymbol::kUndeclaredPrimitive:
			outDesc.Assign("\pundeclared variable");
			break;
			
		case CVariableSymbol::kUndeclaredArray:
			outDesc.Assign("\pundeclared array");
			break;

		default:
			SignalPStr_("\pUnknown type in TypeToAEDesc");
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowBadForLoopIndex										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowBadForLoopIndex(
	const STextRange			&inRange)
{
	AERecord			context={typeNull,0};

	AddRangeToContext(inRange,context);
	
	ThrowAEErr_(kTBErr_ForIndexMustBeInteger,context);	
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowVariableRedeclared									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowVariableRedeclared(
	const STextRange			&inRange,
	CVariableSymbol				*inSymbol,
	CVariableSymbol::ESymbolType inNewType)
{
	StAEDescriptor		varName(typeText,inSymbol->GetString(),inSymbol->GetString().GetLength());
	StAEDescriptor		wasType,newType;
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,varName);

	TypeToAEDesc(inSymbol->GetType(),wasType);
	TypeToAEDesc(inNewType,newType);

	UAEDesc::AddKeyDesc(&context,keyTBWasType,wasType);
	UAEDesc::AddKeyDesc(&context,keyTBNewType,newType);
	AddRangeToContext(inRange,context);
	
	ThrowAEErr_(kTBErr_VariableTypeRedeclared,context);	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowExpectedArrayIndex									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called when the lexer finds an array identifier missing the [ after it
void UTBException::ThrowExpectedArrayIndex(
	const STextRange			&inRange,
	CVariableSymbol				*inSymbol)
{
	StAEDescriptor		varName(typeText,inSymbol->GetString(),inSymbol->GetString().GetLength());
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,varName);
	AddRangeToContext(inRange,context);
	
	ThrowAEErr_(kTBErr_ExpectedArrayIndex,context);	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowProcNotDefined										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adds the proc name to the error message
void UTBException::ThrowProcNotDefined(
	const STextRange	&inRange,
	const CCString		&inProcName)					
{
	StAEDescriptor		procName(typeText,inProcName.mString,inProcName.GetLength());
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBProcName,procName);
	AddRangeToContext(inRange,context);
	
	ThrowAEErr_(kTBErr_ProcNotDefined,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowNotAProcedure										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowNotAProcedure(
	const STextRange	&inRange,
	const CCString		&inProcName)
{
	StAEDescriptor		procName(typeText,inProcName.mString,inProcName.GetLength());
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBProcName,procName);
	AddRangeToContext(inRange,context);
	
	ThrowAEErr_(kTBErr_NotAProcedure,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowProcParameterTypeMismatch							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowProcParameterTypeMismatch(
	CCallProc			*inBadCall)
{
	BadCall(inBadCall,kTBErr_ProcParamTypeMismatch);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowProcParameterMiscount								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowProcParameterMiscount(
	CCallProc			*inBadCall)
{
	BadCall(inBadCall,kTBErr_ProcParamMiscount);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BadCall													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Throws a bad procedure call linker exception. Puts in the source line num of the call and the name of the procedure
// being called
void UTBException::BadCall(
	CCallProc			*inBadCall,
	SInt32				inErrCode)
{
	CCString			&procName=inBadCall->GetProcName();
	StAEDescriptor		procName2(typeText,procName.mString,procName.GetLength());
	StAEDescriptor		lineNum((SInt32)inBadCall->GetSourceLineNo());
	AERecord			context={typeNull,0};

	UAEDesc::AddKeyDesc(&context,keyTBProcName,procName2);
	UAEDesc::AddKeyDesc(&context,keyTBSourceLineNum,lineNum);

	throw CTBLinkerException(inErrCode,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowCanvasNotOpen										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowCanvasNotOpen(
	TTBInteger		inCanvasIndex)
{
	StAEDescriptor		canvasIndex(inCanvasIndex);
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBCanvasIndex,canvasIndex);
	
	ThrowAEErr_(kTBErr_CanvasNotOpen,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowCantFindSuitableDisplay								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowCantFindSuitableDisplay(
	TTBInteger		inXRez,
	TTBInteger		inYRez)
{
	StAEDescriptor		x(inXRez),y(inYRez);
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBXPos,x);
	UAEDesc::AddKeyDesc(&context,keyTBYPos,y);
	
	ThrowAEErr_(kTBErr_CantFindSuitableDisplay,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowUndefinedLabel										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// throws an undefined label error with the name and position of the label reference.
void UTBException::ThrowUndefinedLabel(
	const STextRange		&inLabelRange,
	const char				*inLabelName)
{
	StAEDescriptor		labName(typeText,inLabelName,std::strlen(inLabelName));
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBLabelName,labName);
	AddRangeToContext(inLabelRange,context);

	ThrowAEErr_(kTBErr_UndefinedLabel,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowBadGlobalMoanRegId										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowBadGlobalMoanRegId(
	char				*inStr)
{
	StAEDescriptor		regId(typeText,inStr,std::strlen(inStr));
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,regId);
	ThrowAEErr_(kTBErr_BadMoanGlobalRegId,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowBadLocalMoanRegId										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowBadLocalMoanRegId(
	char				*inStr)
{
	StAEDescriptor		regId(typeText,inStr,std::strlen(inStr));
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,regId);
	ThrowAEErr_(kTBErr_BadMoanLocalRegId,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowBadGlobalMoanRegId										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowBadGlobalMoanRegId(
	TTBInteger		inId)
{
	StAEDescriptor		regId(inId);
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,regId);
	ThrowAEErr_(kTBErr_BadMoanGlobalRegId,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowBadLocalMoanRegId										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowBadLocalMoanRegId(
	TTBInteger		inId)
{
	StAEDescriptor		regId(inId);
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,regId);
	ThrowAEErr_(kTBErr_BadMoanLocalRegId,context);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ThrowUnknownFileFormat										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UTBException::ThrowUnknownFileFormat(
	const char *inFile)
{
	StAEDescriptor		regId(typeText,inFile,std::strlen(inFile));
	AERecord			context={typeNull,0};
	
	UAEDesc::AddKeyDesc(&context,keyTBVarName,regId);
	ThrowAEErr_(kTBErr_UnknownFileType,context);
}

/*e*/
void UTBException::ThrowEditorMissingParameter(
    const char		*inMissingParamName)
{
    // TODO pass parameters and provide descriptive message
    Throw_(kTBErr_EditorMissingRequiredParameter);
}

/*e*/
void UTBException::ThrowEditorParamWrongType(
    const char		*inParamName,
    const char		*inExpectedType,
    const char		*inReceivedType)
{
    // TODO pass parameters and provide descriptive message
    Throw_(kTBErr_EditorParameterTypeError);
}

/*e*/
void UTBException::ThrowEditorInvalidProjectId(
    UInt32			inProjectId)
{
    // TODO pass parameters and provide descriptive message
    Throw_(kTBErr_EditorInvalidProjectId);
}

/*e*/
void UTBException::ThrowEditorJsonParseFail()
{
    // TODO pass parameters and provide descriptive message
    Throw_(kTBErr_EditorJsonParseFail);
}

/*e*/
void UTBException::ThrowEditorUnknownCmd(
    const char		*inCmd)
{
    // TODO pass parameters and provide descriptive message
    Throw_(kTBErr_EditorUnknownCmd);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LogThrow
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// logs an exception to an output file
void LogThrow(
	const char	*inFunc,
	const char	*inFileName,
	long		inLineNo,
	const PP_PowerPlant::LException &inException)
{
	SInt32		errcode=ErrCode_(inException);
	SInt32		expected=UTBException::GetAndClearExpectedException();
	
	if (errcode && errcode!=expected)
	{
		Str255		errstr;
		CCString	logstr("** EXCEPTION ");
		ErrStr_(inException,errstr);
		logstr+=inFileName;
		logstr+=":";
		logstr+=inFunc;
		logstr+="():";
		logstr.Append(inLineNo);
		logstr+=" code=";
		logstr.Append(errcode);
		logstr+=" msg=";
		logstr+=errstr;
		ECHO(logstr);
	}
}
