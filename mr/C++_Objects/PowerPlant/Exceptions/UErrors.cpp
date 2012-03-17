// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UErrors.cpp
// Mark Tully
// 2/8/99
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

/*
	The UErrors class is a static utility class for obtaining error text for a given error code. It is expandable by the
	addition of "error brokers". These are held in a list and are asked in a series until one of them understands how to
	build the error string.
	
	The SubInKeys function replaces sub strings in the error message of the form Æxxxx where xxxx is the key to sub in from
	the AERecord.
*/

#include		<UExtractFromAEDesc.h>
#include		<LString.h>
#include		<UAppleEventsMgr.h>
#include		<AEObjects.h>

#include		"UErrors.h"
#include		"CLinkedListT.h"
#include		"CErrorBroker.h"

CLinkedListT<CErrorBroker>		UErrors::sErrorBrokers;
const StringPtr					UErrors::kKeywordPreamble="\pÆ"; // option-j

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ExtractKeyAsText										Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Extracts the given keyword from the AERecord and coerces it to text. If it's an object specifier then it coerces it
// using the current scripting component (it doesn't do a get). Returns false if the keyword wasn't to be found on the
// record.
bool UErrors::ExtractKeyAsText(
	const AERecord		*inRecord,
	AEKeyword			inAEKeyWord,
	Str255				outString)
{
	if (inRecord && inRecord->descriptorType!=typeNull)
	{
		OSErr				err;
		StAEDescriptor		descData;
		
		err=::AEGetKeyDesc(inRecord,inAEKeyWord,typeWildCard,descData);
		
		if (err==errAEDescNotFound)
			return false;
		ThrowIfOSErr_(err);

		if (descData.mDesc.descriptorType==typeObjectSpecifier)
		{
			LString::CopyPStr("\pCoercion from object specifiers not yet implemented",outString);
		}
		else
		{
			// This line attempts to coerce the descData from the type it was supplied in to the desired type
			// Very likely to throw a coercion error
			UExtractFromAEDesc::ThePString(descData,outString);
		}

		return true;
	}
	
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SubInKeys												Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Takes a string and subs in all the keywords using the AERecord supplied. Returns false if any of the keywords are missing.
bool UErrors::SubInKeys(
	const AERecord		*inRecord,
	Str255				ioString)
{
	LStringRef			string(sizeof(Str255),ioString);
	UInt8				pos=1;
	
	// Scan the string for the keyword preamble then extract the keyword, then do a replace
	while ((pos=string.Find(kKeywordPreamble,pos))!=0)
	{
		// Extract the keyword
		LStr255			keywordStr=string(pos+1,sizeof(AEKeyword));
		
		// Get the text for the replacement op from the AERecord
		LStr255			replacementText;
		
		if (!ExtractKeyAsText(inRecord,keywordStr,replacementText))
			return false;		// Missing keyword
		
		// Perform the replacement
		string.Replace(pos,sizeof(AEKeyword)+UErrors::kKeywordPreamble[0],replacementText);
		
		// Skip past the keyword and preamble
		pos+=replacementText[0];
	}	

	return true;
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetErrorMessage										Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Retrieves the error message for a given error code, subbing in the keys from the aerecord. If no broker is found then the
// string is left untouched. Passing 0L or typeNull descs is fine.
bool UErrors::GetErrorMessage(
	OSErr				inErr,
	const AERecord		*inRecord,
	Str255				outString)
{
	CListIndexerT<CErrorBroker>		indexer(&sErrorBrokers);

	while (CErrorBroker *broker=indexer.GetNextData())
	{
		if (broker->GetErrorMessage(inErr,inRecord,outString))
			return true;
	}

	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LinkErrorBroker										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Adds an error broker to the list of error brokers. The brokers are searched in the reverse order that they were inserted
// so newer brokers take presidence over existing ones.
void UErrors::LinkErrorBroker(
	CListElementT<CErrorBroker>	&inLink)
{
	sErrorBrokers.LinkElement(&inLink);
}
