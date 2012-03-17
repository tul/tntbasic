// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Preprocessor.cpp
// © Mark Tully and TNT Software 2000
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

#include	"Preprocessor.h"
#include	"CStdInHandleStream.h"
#include	"CProcNode.h"
#include    "Utility Objects.h"
#include    "CFlexBuffer.h"

#define FLEX_SUPPORT_NAMESPACE TBPreprocessor
#include	"CFlexBufferSupport.h"
#undef FLEX_SUPPORT_NAMESPACE

int preprocessorparse();
void preprocessorrestart( std::FILE *input_file );

extern CProcNode *gProceduresList;
extern int gProcLine;

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//		* ExtractProcedures                                         /*e*/
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Returns a list of procedure names from inHandle
// inHandle should be terminated with a double null
CProcNode *ExtractProcedures(
	Handle		inHandle)
{
    // inHandle should already be terminated with 2 null chars
    Size        sz=::GetHandleSize(inHandle);
    AssertThrow_((*inHandle)[sz-2]==0 && (*inHandle)[sz-1]==0);

	gProceduresList=0;
	gProcLine=0;	// one based line numbering - start from zero as procedure isn't created until first endline is encountered

    {
        UHandleLocker	locked(inHandle,true);
        CFlexBuffer		buff(*inHandle,sz,&TBPreprocessor::AllocAndSwitchToBuffer,&TBPreprocessor::DeallocBuffer,&TBPreprocessor::GetCurrentLexOffset);

        preprocessorparse();
    }
	
	CProcNode		*prevNode=0,*currentNode=gProceduresList;
	
	while (currentNode)
	{
		if (prevNode && prevNode->mEndProcLine==-1)		// prev node had no termination for the end proc, set it to the line before the current procs def
			prevNode->mEndProcLine=currentNode->mLineNum-1;
		
		prevNode=currentNode;
		currentNode=currentNode->TailData();
	}
	
	// if the last item in the procedures list hasn't had it's end proc filled out then it means that either there was no
	// end proc or the end proc was on the last line of the code file
	if (prevNode && prevNode->mEndProcLine==-1)	
		prevNode->mEndProcLine=gProcLine+1;		// add one cos the missing cr always means line number is 1 too high

	return gProceduresList;
}
