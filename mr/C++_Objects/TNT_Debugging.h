// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// TNT_Debugging.h
// © Mark Tully and TNT Software 1999
// 16/7/99
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
	The ultimate debugging header, include this to get exceptions macros Try_ Catch_ etc which use LException variants on the
	mac and SInt32 on other platforms.
*/

#pragma once

#ifndef _H_UException
#include		<UException.h>
#endif

/*
	Define Catch_ ourselves so that it catches LException class throws if we're using them.
	This is done as PowerPlant has changed it's exception object type much to the annoyance of the entire planet.
	If you have the wrong catch block you miss exceptions and die. Fortunately I've always used Catch_ macro so by
	cunningly adjusting it I can catch the correct type of exception.
	Unfortunately my code presumes it's a UInt32 when comparing error codes. To fix that change statements like
		Catch_(err)
		{
			if (err==paramErr)
				.
	To
		Catch_(err)
		{
			if (ErrCode_(err)==paramErr)
				.
	And it will work proper.
	
	The PowerPlant define PP_Obsolete_ThrowExceptionCode can be set to true in your prefix header. Obviously it needs to
	be before this file in the debug chain for that to work.
	
	See TNT Tech Note 2 for a further discussion.
*/

#define COMPILE_TIME_ASSERT(expr)       char UNIQUE_NAME[expr?1:-1]
#define UNIQUE_NAME                     MAKE_NAME(__LINE__)
#define MAKE_NAME(line)                 MAKE_NAME2(line)
#define MAKE_NAME2(line)                constraint_ ## line

#if TARGET_OS_MAC==1
	#define	ThrowIfMemFull_(ptr)															\
		do {																				\
			if ((ptr) == 0) { ThrowIfMemError_(); Throw_(memFullErr); }					\
		} while (false)

	#ifndef Try_		
		#define Try_			try
	#endif
	#ifndef Catch_
		#define Catch_(err)		catch(const PP_PowerPlant::LException &err)
	#endif

#else
	#define readErr		-19								/*I/O System Errors*/
	#define writErr		-20								/*I/O System Errors*/
	#define memFullErr	-108
	#define nilPointer	27728				// FOUR_CHAR_CODE('nilp')
	
#endif

#if PP_Obsolete_ThrowExceptionCode==1
	#define Catch_(err)			catch(PP_PowerPlant::ExceptionCode err)
	#define ErrCode_(err)		err

	inline bool ErrStr_(
		const LException		&inErr,
		Str255					outStr)
	{
		outStr[0]=0;
		return false;
	}
	
#else
	#if TARGET_OS_MAC==1
		#include					"CAEDescException.h"
		#include					"UErrors.h"
		#include					<LString.h>
	#endif

	#include						"TNT_Errors.h"

	#define AssertThrow_(test)	do {										\
							        if (!(test))							\
							        {										\
							        	SignalStringLiteral_(#test);		\
							    		Throw_(kTNTErr_AssertionFailed);	\
							    	}										\
							    } while (false)

	#define AssertThrowStr_(test,msg)	\
								do {										\
							        if (!(test))							\
							        {										\
							        	SignalStringLiteral_(#test);		\
							    		ThrowStr_(kTNTErr_AssertionFailed,msg);	\
							    	}										\
							    } while (false)


	#if TARGET_OS_MAC==1
		#define ThrowStr_(err,pstr)		  	PP_PowerPlant::LException::Throw(err,pstr)
		#define ThrowAEErr_(err,aeRecord)	CAEDescException::Throw(err,aeRecord,true)
		#define Catch_(err)					catch(const PP_PowerPlant::LException &err)
		#define ErrCode_(err)				(err).GetErrorCode()
	#endif	// other platforms are define in UException.h in MR:CrossPlatform:(NonMac)

	#define Debug_(str)			do { SignalString_(str); Throw_(kTNTErr_InternalError); } while (false)

	// need rtti on for ErrStr_ to work
	#if TARGET_OS_MAC==1
		#if __GNUC__
			#define RTTI_ENABLED	1
		#else
			#define RTTI_ENABLED	__option(RTTI)
		#endif
		#if RTTI_ENABLED
		inline bool ErrStr_(
			const LException		&inErr,
			Str255					outStr)
		{
			outStr[0]=0;
			const CAEDescException	*ae=dynamic_cast<const CAEDescException*>(&inErr);
			
			// It's an ae exception - call it's specialised message fetcher.
			if (ae && ae->CreateErrorString(outStr))
				return true;
			else
			{
				LString::CopyPStr(inErr.GetErrorString(),outStr);
				
				// No string direct from exception, generate one from error code
				if (outStr[0]==0)
				{
					Str15	temp;
					::NumToString(ErrCode_(inErr),temp);
					LString::CopyPStr("\pAn error of type ",outStr);
					LString::AppendPStr(outStr,temp);
					LString::AppendPStr(outStr,"\p occurred.");

					Try_
					{
						return UErrors::GetErrorMessage(inErr.GetErrorCode(),0L,outStr);
					}
					Catch_(err)
					{
						return false;
					}
				}
				else
					return true;
			}
		}
		#else
			#define ErrStr_ #pragma warning You must have RTTI enabled to use ErrStr_ with new style exceptions
		#endif
	#endif
	
#endif
