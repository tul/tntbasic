// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CCString.h
// © Mark Tully 1999
// 23/12/99
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

// This is a non standard string routine which appears to be standard on unix so I define it here for compat
#if TARGET_OS_MAC==1
	namespace std
	{
	int strcasecmp(
		const char *inStr1,
		const char *inStr2);
	};
#endif

class CCString
{
	public:
		typedef unsigned short	TCharIndex;
		static const TCharIndex	kStrNotFound=0xFFFF;		// return value for FindString

	protected:
		TCharIndex				mMaxLength;

	public:
		char					*mString;

		/*e*/					CCString(
									const CCString		&inSource);
		/*e*/					CCString(
									const char			*inString="",
									TCharIndex			inDefaultMaxLength=255);
								~CCString();
		
		bool					IsNull() const 										{ return mString[0]==0; }
		
		TCharIndex				GetLength() const;

		void					ToLower();
		void					ToUpper();
		
		TCharIndex				GetMaxLength() const								{ return mMaxLength; }
		void /*e*/				SetMaxLength(
									TCharIndex			inMaxLength,
									bool				inShrinkIfNeeded=false);
		
		void /*e*/				Append(
									const char			*inString);
		void /*e*/				Append(
									const char			*inString,
									TCharIndex			inIndex);
		void /*e*/				Append(
									const unsigned char	*inString);
		void /*e*/				Append(
									signed long			inInt);
		
		void /*e*/				Assign(
									const char			*inString);
		void /*e*/				Assign(
									const char			*inString,
									TCharIndex			inIndex);
		void /*e*/				Assign(
									const unsigned char	*inPString);
		void /*e*/				Assign(
									signed long			inInt);
		#if TARGET_API_MAC_CARBON
		void /*e*/				Assign(
									const AEDesc		&inDesc);
		#endif

		CCString				&operator+=(
									char		inChar)								{ Append(&inChar,1); return *this; }
		CCString				&operator+=(
									const char	*inString)							{ Append(inString); return *this; }	
		CCString				&operator+=(
									const unsigned char	*inString)					{ Append(inString); return *this; }	

		// case sens compare by default
		bool					Compare(
									const char	*inString) const;
		bool					operator==(
									const char	*inString) const;
		bool					operator==(
									const CCString &inString) const					{ return Compare(inString.mString); }
		signed long				CompareNoCase(
									const char	*inString) const;
								
		CCString				&operator=(
									const CCString &inString)						{ Assign(inString.mString); return *this; }
		CCString				&operator=(
									const char	*inString)							{ Assign(inString); return *this; }	
		CCString				&operator=(
									const unsigned char	*inString)					{ Assign(inString); return *this; }	
								operator char*() const								{ return mString; }
								
		bool					EndsWith(
									const char	*inString);
		
		void					RemoveChars(
									TCharIndex	inStart,
									TCharIndex	inEnd);
		void					InsertChars(
									const char	*inString,
									TCharIndex	inStringLength,
									TCharIndex	inPosition);
		void					InsertChars(
									const char	*inString,
									TCharIndex	inPosition);
		
		void					Remove(
									const char	*inString);
									
		TCharIndex				FindString(
									const char	*inString);
};
