// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CFPrefsHelpers.cpp
// © Mark Tully and TNT Software 2002 -- All Rights Reserved
// 30/4/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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
	Code to help with using CFPreferences
	
	Return true if ok (set ok, found prop)
	Return false if error (couldn't find/set)
	
	Don't forget to flush the prefs with CFPrefsFlush()
	after you've set them all
*/

#include "TNT_Debugging.h"
#include "CFPrefsHelpers.h"

#include <CFPropertyList.h>
#include <CFPreferences.h>
#include <CFNumber.h>
#include <UEnvironment.h>
#include <UProcess.h>

#define		kAllocatorDefault	NULL

static CFStringRef		s9AppId=NULL;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsFlush
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsFlush()
{
	return ::CFPreferencesAppSynchronize(CFPrefsGetCurAppId());
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsGetCurAppId
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CFStringRef CFPrefsGetCurAppId()
{
	// on pre os x, kCFPreferencesCurrentApplication doesn't seem to work, so return a
	// hardcoded version instead
	if (!UEnvironment::IsRunningOSX())
	{
		if (s9AppId)
			return s9AppId;
		else
		{
			Try_
			{
				SProcessInfo	info;
				UProcess::GetMyInfo(info);

				s9AppId=::CFStringCreateWithPascalString(kCFAllocatorDefault,info.name,smDefault);				
			}
			Catch_(err)
			{
				s9AppId=::CFStringCreateWithPascalString(kCFAllocatorDefault,"\ptnt",smDefault);
			}
			
			return s9AppId;
		}
	}
	else
		return kCFPreferencesCurrentApplication;			
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsGetString
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsGetString(
	CFStringRef	inKey,
	char		*outValue,
	Size		inBuffSize)
{
	outValue[0]=0;
	
	CFPropertyListRef		prop=::CFPreferencesCopyAppValue(inKey,CFPrefsGetCurAppId());
	bool					result=false;

	if (prop && ::CFGetTypeID(prop)==::CFStringGetTypeID())
		result=::CFStringGetCString((CFStringRef)prop,outValue,inBuffSize,smDefault);
	
	if (prop)
		::CFRelease(prop);
	
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsSetString
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsSetString(
	CFStringRef	inKey,
	const char *inValue)
{
	CFStringRef		value=CFStringCreateWithCString(kCFAllocatorDefault,inValue,smDefault);
	
	if (!value)
		return false;
	
	CFPreferencesSetAppValue(inKey,value,CFPrefsGetCurAppId());
	
	CFRelease(value);
	
	return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsSetHandle
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsSetHandle(
	CFStringRef			inKey,
	Handle				inHandle)
{
	CFDataRef	data;
	char		null=0;
	bool		result=false;
	
	if (inHandle)
	{
		::HLockHi(inHandle);
		data=CFDataCreateWithBytesNoCopy(kCFAllocatorDefault,(UInt8*)*inHandle,::GetHandleSize(inHandle),kCFAllocatorNull);
	}
	else
		data=CFDataCreateWithBytesNoCopy(kCFAllocatorDefault,(UInt8*)&null,sizeof(char),kCFAllocatorNull);
		
	if (data)
	{
		CFPreferencesSetAppValue(inKey,data,CFPrefsGetCurAppId());
	
		CFRelease(data);
		result=true;
	}
	
	if (inHandle)
		::HUnlock(inHandle);
		
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsGetHandle
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsGetHandle(
	CFStringRef		inKey,
	Handle			&outHandle)
{
	outHandle=NULL;
	
	CFPropertyListRef		prop=::CFPreferencesCopyAppValue(inKey,CFPrefsGetCurAppId());
	bool					result=false;

	if (prop && ::CFGetTypeID(prop)==::CFDataGetTypeID())
	{
		CFIndex				size=CFDataGetLength((CFDataRef)prop);
		
		outHandle=::NewHandle(size);
		
		if (outHandle)
		{
			CFRange			range={0,size};
			::HLock(outHandle);		
			CFDataGetBytes((CFDataRef)prop,range,(UInt8*)*outHandle);
			::HUnlock(outHandle);
			result=true;
		}
	}

	if (prop)
		::CFRelease(prop);
	
	return result;

}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsGetBool
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsGetBool(
	CFStringRef	inKey,
	bool		&outValue)
{
	Boolean		out=outValue;
	bool		result=CFPrefsGetBool(inKey,out);
	outValue=out;
	return result;
}

bool CFPrefsGetBool(
	CFStringRef	inKey,
	Boolean		&outValue)
{
	outValue=false;
	
	CFPropertyListRef		prop=::CFPreferencesCopyAppValue(inKey,CFPrefsGetCurAppId());
	bool					result=false;

	if (prop && ::CFGetTypeID(prop)==::CFBooleanGetTypeID())
	{
		outValue=::CFBooleanGetValue((CFBooleanRef)prop);
		result=true;
	}

	if (prop)
		::CFRelease(prop);
	
	return result;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsSetBool
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CFPrefsSetBool(
	CFStringRef	inKey,
	Boolean		inValue)
{
	if (inValue)
		CFPreferencesSetAppValue(inKey,kCFBooleanTrue,CFPrefsGetCurAppId());
	else
		CFPreferencesSetAppValue(inKey,kCFBooleanFalse,CFPrefsGetCurAppId());
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsGetInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsGetInt(
	CFStringRef	inKey,
	SInt32		&outValue)
{
	outValue=0;
	
	CFPropertyListRef		prop=::CFPreferencesCopyAppValue(inKey,CFPrefsGetCurAppId());
	bool					result=false;

	if (prop && ::CFGetTypeID(prop)==::CFNumberGetTypeID())
		result=::CFNumberGetValue((CFNumberRef)prop,kCFNumberSInt32Type,&outValue);

	if (prop)
		::CFRelease(prop);
	
	return result;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsSetInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsSetInt(
	CFStringRef	inKey,
	SInt32		inValue)
{
	CFNumberRef		value=CFNumberCreate(kCFAllocatorDefault,kCFNumberSInt32Type,&inValue);
	
	if (!value)
		return false;
	
	CFPreferencesSetAppValue(inKey,value,CFPrefsGetCurAppId());
	
	CFRelease(value);
	
	return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsGetColour
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsGetColour(
	CFStringRef	inKey,
	RGBColour	&outColour)
{
	SInt32		cols[3];
	SInt32		count;
	
	count=CFPrefsGetIntArray(inKey,cols,3);
	
	if (count==3)
	{
		outColour.red=cols[0];
		outColour.green=cols[1];
		outColour.blue=cols[2];
		return true;
	}
	else
		outColour.red=outColour.green=outColour.blue=0;
		
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsSetColour
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsSetColour(
	CFStringRef		inKey,
	const RGBColour	&inColour)
{
	SInt32		cols[3];
	
	cols[0]=inColour.red;
	cols[1]=inColour.green;
	cols[2]=inColour.blue;
	
	return CFPrefsSetIntArray(inKey,cols,3);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsSetIntArray
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CFPrefsSetIntArray(
	CFStringRef		inKey,
	const SInt32	*inArray,
	SInt32			inArrayCount)
{
	CFNumberRef		*nums=new CFNumberRef[inArrayCount];
	if (!nums)
		return false;
	
	for (SInt32 count=0; count<inArrayCount; count++)
		nums[count]=CFNumberCreate(kCFAllocatorDefault,kCFNumberSInt32Type,&inArray[count]);
	
	CFArrayRef	arr = CFArrayCreate(
							kAllocatorDefault, 
							(const void**)nums,
							inArrayCount,
							&kCFTypeArrayCallBacks );
	
	while (inArrayCount)
		::CFRelease(nums[--inArrayCount]);

	delete [] nums;

	if (!arr)
		return false;
	
	CFPreferencesSetAppValue(inKey,arr,CFPrefsGetCurAppId());
	
	CFRelease(arr);
	
	return true;	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFPrefsGetIntArray
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// returns element count
SInt32 CFPrefsGetIntArray(
	CFStringRef		inKey,
	SInt32			*outArray,
	SInt32			inMaxEle)
{
	CFPropertyListRef		prop=::CFPreferencesCopyAppValue(inKey,CFPrefsGetCurAppId());
	SInt32					result=0;

	if (prop && ::CFGetTypeID(prop)==::CFArrayGetTypeID())
	{
		CFIndex		count=CFArrayGetCount((CFArrayRef)prop);
		CFRange		range={0,Lesser(inMaxEle,count) };
		
		CFNumberRef		*nums=new CFNumberRef[range.length];
		if (nums)
		{	
			for (SInt32 count=0; count<range.length; count++)
			{
				CFNumberRef		num=(CFNumberRef)::CFArrayGetValueAtIndex((CFArrayRef)prop,count);		// num owned by array - don't release
				
				if (num && ::CFGetTypeID(num)==::CFNumberGetTypeID())
				{
					if (!::CFNumberGetValue((CFNumberRef)num,kCFNumberSInt32Type,&outArray[count]))
						outArray[count]=0;
				}
				else
					outArray[count]=0;
			}

			result=range.length;
			
			delete [] nums;
		}
		else
			result=0;
	}
	
	if (prop)
		::CFRelease(prop);
	
	return result;
}