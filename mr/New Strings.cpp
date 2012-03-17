// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// New Strings.cpp
// Collection of routines for operating on strings
// Mark Tully
// 9/7/95
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1995, Mark Tully and John Treece-Birch
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

#include	<Math.h>
#include	"Marks Routines.h"
#include	<cctype>

short StrLen(StringPtr theString)
{
	short	len=0;
	
	while((theString[len]) && (len<255))
		len++;

	return len;
}

// Takes a pascal string and pads it out to the right len by inserting padchar before it
void PadPString(StringPtr iString,short len,char padChar)
{
	short	pad=len-iString[0];

	if (pad>0) // padding required?
	{
		BlockMoveData(&iString[1],&iString[pad+1],iString[0]);
		FillMem((Ptr)&iString[1],(Ptr)&iString[pad+1],padChar);
		iString[0]=len;
	}
}

// same as above except to the end
void PostPadPString(StringPtr iString,short len,char padChar)
{
	if (len>255)
		len=255;
	if (len>iString[0])	
	{
		FillMem((Ptr)&iString[iString[0]+1],(Ptr)&iString[len+1],padChar);
		iString[0]=len;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CmpPStrNoCaseDifference
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// compares 2 strings using case insensitive p string comparison
// returns 0 for equal
// <0 if str1 alphabetically < str2
// >0 if str1 alphabetically > str2
SInt32 CmpPStrNoCaseDifference(
	ConstStringPtr inA,
	ConstStringPtr inB)
{
	unsigned char		testLen=Lesser(inA[0],inB[0]);
	UInt16				index;
	
	for (index=1; index<=testLen; index++)
	{
		char	a=std::tolower(inA[index]),b=std::tolower(inB[index]);			// nb: std::tolower can handle diametric marks and see Ž and ƒ as equal
		if (a!=b)
			return a-b;
	}
	
	return inA[0]-inB[0];
}

// case sensitive compare
Boolean CmpPStr(ConstStringPtr sourceA, ConstStringPtr sourceB)
{	
	if (sourceA[0]!=sourceB[0]) // if not the same length
		return false;	

	return CmpBuffer((const char*)sourceA+1,(const char*)sourceB+1,sourceA[0],true); // case senstive
}

// same as above 'cept little letter -> big letter
Boolean CmpPStrNoCase(ConstStringPtr sourceA, ConstStringPtr sourceB)
{
	if (sourceA[0]!=sourceB[0]) // if not the same length
		return false;	

	return CmpBuffer((const char*)sourceA+1,(const char*)sourceB+1,sourceA[0],false); // case insenstive
}

// Makes a pascal string out of a block of letters up to a stop char
// Copys from start which is of length buffLen upto but not including stopChar
void CopyPBuffer(Ptr start,StringPtr dest,char stopChar,short buffLen)
{
	short	len=1;

	while(start[len-1]!=stopChar && len<=buffLen && len<256)
	{
		dest[len]=start[len-1]; // Read one char
		len++;	
	}

	dest[0]=len-1; // set the length byte
}

// Copy C String
void CopyStr(StringPtr source, StringPtr dest)
{
	short	sourceLen=StrLen(source)+1; // +1 to get the null
	BlockMoveData(source,dest,(Size)sourceLen);
}

void CopyCAsPStr(StringPtr source, StringPtr dest)
{
	short	sourceLen=StrLen(source);

	BlockMoveData(source,&dest[1],sourceLen);
	dest[0]=sourceLen;
}

void CopyPAsCStr(const StringPtr source, StringPtr dest)
{
	BlockMoveData(&source[1],dest,source[0]);
	dest[source[0]]=0;
}

void CopyPStr(const unsigned char *source,unsigned char *dest)
{
	BlockMoveData(source,dest,source[0]+1);
}

// dps is the max number of decimal places
void DoubleToString(double theNum,StringPtr theString,short dps)
{
	Str32	tempString;
	double	power=1;
	short	counter;
	
	for (counter=0; counter<dps; counter++)
		power*=10;
	
	NumToString((long)theNum,theString);
	ConcatPStr(theString,"\p.");
	theNum-=(long)theNum;
	/*std:: Sometimes this is needed sometimes not - what's going on??? */
//#ifdef _MSL_USING_NAMESPACE	// this might work...
//	using std::fabs;
//	theNum=_CSTD::fabs(theNum);
//#else
	theNum=/*std::*/fabs(theNum);		// Must be positive
//#endif
	theNum*=power;
	
	// roundtol rounds to the nearest whole integer
	ConcatPStringNum(theString,(long)::lround(theNum));
	//NumToString((theNum-(long)theNum)*power,tempString);
	//ConcatPStr(theString,tempString);
}

// Counts the number of words in a pascal string
short CountWords(StringPtr theString)
{
	short		wordCount=0,counter;
	Boolean		inWord=false;

	// for each letter
	for (counter=1; counter<=theString[0]; counter++)
	{
		if (theString[counter]==' ' || theString[counter]=='\r')
			inWord=false;
		else
		{
			if (!inWord)
			{
				// just entered a word, bump the count
				wordCount++;
				inWord=true;
			}
		}
	}
	
	return wordCount;
}

// high level find and replace routine
// parameters
// startBuff		: address of the memory to operate on
// buffLength		: length of the memory to operate on. Receives the new length of the buffer
// maxBuffSize		: limit on the buffer size
// replaceMe		: ptr to a sample of the stuff to replace
// replaceMeSize	: size of above;
// withMe			: ptr to the stuff to replace the replace me with
// withmeSize		: size of the above
// caseSensitive	: used for text replacments

// returns
// num of replaces done. Returns -1 if ran out of memory. As many replaces as possible are carried
// out if memory runs out. Half replaces are never done.
short FindAndReplace(Ptr startBuff,Size *buffLength,Size maxBuffSize,Ptr replaceMe,
							Size replaceMeSize,Ptr withMe,Size withMeSize,Boolean caseSensitive)
{
	short		foundPos=0,lastPos;
	short		numReplaces=0;
	Size		lenToMove;
	
	while(*buffLength-foundPos>0)
	{
		// first find the chars in question
		lastPos=foundPos;
		foundPos+=SearchForChars(startBuff+foundPos,replaceMe,*buffLength-foundPos,replaceMeSize,caseSensitive);
		
		// did we find any occurances? (did it return -1)
		if (foundPos==lastPos-1)
			return numReplaces;	// no
			
		// check that we have enough space to replace the found bit with the replace bit
		if (*buffLength-replaceMeSize+withMeSize>maxBuffSize)
			return -1;	// lack of space
			
		// move the data to the right of the replaceMe section either left or right respectively
		// how much to move?
		lenToMove=replaceMeSize-withMeSize;
		if (lenToMove!=0)
		{
			// if data to right of replace string the move it right amount of chars
			if (foundPos+replaceMeSize<*buffLength) // if there is any data
				BlockMoveData(startBuff+foundPos+replaceMeSize,startBuff+foundPos+withMeSize,*buffLength-foundPos-replaceMeSize+1);	
		}
		
		// Now copy the replacement data in
		BlockMoveData(withMe,startBuff+foundPos,withMeSize);
		
		// and alter the size of the buffer
		*buffLength-=lenToMove;

		// and up the counter
		numReplaces++;
		
		// and start the next search from the end of where we are now
		foundPos+=withMeSize;
	}
	
	return numReplaces;
}

// pascal string version of the above.
// string must be a full sized pascal string (ie have 256 bytes in total inc length byte).
short FindAndReplacePStr(StringPtr string,StringPtr replaceMe,StringPtr withMe,
																			Boolean caseSensitive)
{
	short		numReplaces;
	Size		stringLen=string[0];

	// do the replace
	numReplaces=FindAndReplace((Ptr)&string[1],&stringLen,255,(Ptr)&replaceMe[1],replaceMe[0],(Ptr)&withMe[1],
																		withMe[0],caseSensitive);

	// and stuff the new length byte back in
	string[0]=stringLen;
	
	return numReplaces;
}

// Takes pascal strings
short ReplaceWord(StringPtr string,StringPtr replace,StringPtr with,Boolean caseSensitive)
{
	short	pos;
	short	len;

	// check that the new one will fit into 255 bytes
	if (string[0]-replace[0]+with[0]>255)
		return -1;

	pos=SearchForChars((Ptr)&string[1],(Ptr)&replace[1],string[0],replace[0],caseSensitive);
	if (pos==-1)
		return -1;
	pos++; // must do this as we missed of the first byte when calling SearchForChars	
	
	len=replace[0]-with[0];
	if (len!=0) // need to shift data on the right or the inserted word
	{
		// if data to right of replace string the move it right amount of chars
		if (pos+replace[0]<=string[0]) // if there is any data
			BlockMoveData(&string[pos+replace[0]],&string[pos+with[0]],string[0]-pos-replace[0]+1);	
	}
	
	BlockMoveData(&with[1],&string[pos],with[0]);
	string[0]=string[0]-replace[0]+with[0];

	return pos;
}

Boolean IsVowel(char theChar)
{
	if (theChar=='a' || theChar=='e' || theChar=='i' || theChar=='o' || theChar=='u' ||
	theChar=='A' || theChar=='E' || theChar=='I' || theChar=='O' || theChar=='U')
		return true;
	return false;
}

// Removes a chunk of memory between 2 ptrs and shifts the data to the right left to replace it
void RemoveChunk(Ptr a,short pos1, short pos2, short bufferLen)
{
	BlockMoveData(&a[pos2],&a[pos1],bufferLen-pos2);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊSearchForChars
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Searches for a particular string of charactors in another string of characters
// Returns the index of the first char of the found string or -1 for none
// Feed back the returned value +1 to get the next occurance of a string
SInt32 SearchForChars(
	Ptr		inSource,
	Ptr		inFind,
	SInt32	inSourceLen,
	SInt32	inFindLen,
	Boolean inCaseSensitive)
{	
	if (!inSourceLen || !inFindLen)
		return -1;

	SInt32 offset=-1;
	SInt32 temp=0;

	do
	{
		offset++;
					
		temp=SearchMemForChar(&inSource[offset],inFind[0],inSourceLen-offset,inCaseSensitive);
		if (temp==-1)
			return -1;
		offset+=temp;
		
		// check string will fit
		if ((offset+inFindLen)>inSourceLen)
			return -1;

	} while(!CmpBuffer((const char*)inFind,(const char*)&inSource[offset],inFindLen,inCaseSensitive));

	return offset;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊSearchMemForChar
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns off set releative to ptr searchMe
SInt32 SearchMemForChar(
	Ptr		inSearchMe,
	char	inForMe,
	SInt32	inBuffLen,
	Boolean inCaseSensitive)
{
	SInt32	count;
	char	toggled=ToggleCase(inForMe);

	for (count=0; count<inBuffLen; count++)
	{
		if (inSearchMe[count]==inForMe)
			return count;
		else if (!inCaseSensitive)
		{ // test for other case
			if (inSearchMe[count]==toggled)
				return count;
		}
	}
	
	return -1;
}

// Extracts a number which follows some text (preFix) in the string. ie if it said page 20. The
// refix is "\ppage " and the return is 20
void ExtractNumber(StringPtr theString,StringPtr preFix,long *theNum)
{
	short 	thePos,count;
	Str15	tempString;
	
	thePos=SearchForChars((Ptr)&theString[1],(Ptr)&preFix[1],theString[0],preFix[0],false);
	thePos+=preFix[0]+1; // Now pointing to the number
	count=1;
	
	// Copy the ASCII number into a p string
	while((theString[thePos]<='9' && theString[thePos]>='0') || theString[thePos]=='.')
		tempString[count++]=theString[thePos++];
	
	tempString[0]=count-1;
	StringToNum(tempString,theNum);
}

// Toggle the case of a charactor ignores numbers and others
char ToggleCase(char theChar)
{
	// if it's a cap
	if (theChar>='A' && theChar<='Z')
		theChar+=32;
	else if (theChar>='a' && theChar<='z')
		theChar-=32;

	return theChar;
}

char LowerCase(char theChar)
{
	if (theChar>='A' && theChar<='Z')
		theChar+=32;
	
	return theChar;
}

char UpperCase(char theChar)
{
	if (theChar>='a' && theChar<='z')
		theChar-=32;
	
	return theChar;
}

Boolean BetterGetFNum(StringPtr theString,short *theNum)
{
	GetFNum(theString,theNum);
	if (*theNum==0)
	{
		Str255		tempString;
		
		GetFontName(0,tempString);
		if (!CmpPStrNoCase(tempString,theString))
		{
			// can't get font
			return false;
		}
	}

	return true;
}

// Compares the data held at the addresses a and b and returns true if they are the same for len
// charactors
Boolean CmpBuffer(const char *a,const char *b,Size len,Boolean caseSensitive)
{
	short	count;	

	for (count=0; count<len; count++)
	{
		if (a[count]!=b[count])
		{			
			if (!caseSensitive) // test other case
			{
				if (a[count]!=ToggleCase(b[count]))
					return false;
			}
			else
				return false;
		}
	}
	return true;
}

// Compares two c strings
Boolean CmpString(StringPtr a,StringPtr b)
{
	short		counter=0;

	do
	{
		if (a[counter]!=b[counter])
			return false;
		if (!a[counter++])
			return true;
	} while (true);

	return true;
}

// compares two c strings with no case match
Boolean CmpStringNoCase(StringPtr a,StringPtr b)
{
	if (StrLen(a)!=StrLen(b))
		return false;
	
	return CmpBuffer((const char*)a,(const char*)b,StrLen(a),false);
}

// Appends one C string onto another
// front gets back attached to it
void ConcatStr(StringPtr front, StringPtr back)
{
	short	frontLen=StrLen(front);
	short	backLen=StrLen(back)+1; // Get the NULL too
	if ((backLen+frontLen)>256) // problem, we'll overflow the string!
	{
		backLen=256-frontLen;
		BlockMoveData(back,front+frontLen,(Size)backLen);
		front[255]=0; // terminate the string
	}
	else
		BlockMoveData(back,front+frontLen,(Size)backLen);
}

// same as above but ends the string with É (with dots) if it's too long (option-;)
void ConcatStrWD(StringPtr front,StringPtr back)
{
	short	frontLen=StrLen(front);
	short	backLen=StrLen(back)+1; // Get the NULL too
	if ((backLen+frontLen)>256) // problem, we'll overflow the string!
	{
		backLen=256-frontLen;
		BlockMoveData(back,front+frontLen,(Size)backLen);
		front[254]='É'; // insert before null
		front[255]=0; // not forgetting to insert the null
	}
	else
		BlockMoveData(back,front+frontLen,(Size)backLen);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ConcatPStr
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Appends one Pascal string onto another
// front gets back attached to it
// if back + front > 255 then the result is truncated.
void ConcatPStr(
	StringPtr 		ioFront,
	ConstStringPtr	inBack)
{
	SInt16		sizeToCopy=inBack[0];
	
	if ((ioFront[0]+inBack[0])>255)
		sizeToCopy=255-ioFront[0];
		
	BlockMoveData(inBack+1,ioFront+ioFront[0]+1,sizeToCopy);
	ioFront[0]+=sizeToCopy;
}

// adds a number to a pstring
void ConcatPStringNum(StringPtr string,long num)
{
	Str255	temp;
	
	NumToString(num,temp);
	ConcatPStr(string,temp);
}

// Creates a C String from a number
void Num2Str(long num,StringPtr str)
{
	NumToString(num,str);
	PStrToCStr(str);
}

// creates a num from a C string
void CStr2Num(StringPtr theString,long *num)
{
	Str255		temp;
	CopyCAsPString(theString,temp);
	StringToNum(temp,num);
}

// Put in a pascal string and get a NULL terminated C string out
void PStrToCStr(StringPtr pString)
{
	short	len=0;
	len=(Size)pString[0]; // Get length byte
	BlockMoveData(&pString[1],&pString[0],len); // Shift the data to the left
	pString[len]=0;
}

// Put in a C string and get a pascal out
void CStrToPStr(StringPtr cString)
{
	short	len;
	len=StrLen(cString);
	if (len>255)
		len=255; // max	
	BlockMoveData(&cString[0],&cString[1],len);
	cString[0]=len;
}

// 31/3/97	: ie not out of it's face like most of this file which was written when I was learning
// converts a number into a hex digit
// this is to replace the one in memory.c which seems a bit out of it's face. minLength is min length of
// the string you want. It can be up to 8
void NumToHexString(long number,StringPtr string,unsigned char minLength)
{
	unsigned char	*bytes=(unsigned char*)&number;
	Str15			temp;
	unsigned char	outCount,leadingZeros=0,outputLength,otherCounter=1;
	
	// to convert to hex you split a number into 8 bit chunks and then convert each into hex then
	// stick them together.
	CharToHexPair(bytes[0],&temp[0]);
	CharToHexPair(bytes[1],&temp[2]);
	CharToHexPair(bytes[2],&temp[4]);
	CharToHexPair(bytes[3],&temp[6]);
	
	// figure out how many digits of value we have in there by stripping leading zeros
	while (temp[leadingZeros++]=='0')
		;
	leadingZeros--;
	
	// now we know how long to make the output string, either minlength or a longer length if the
	// new string is bigger than minlength
	outputLength=Greater(8-leadingZeros,minLength);
	
	for (outCount=8-outputLength; outCount<8; outCount++)
		string[otherCounter++]=temp[outCount];
	
	string[0]=outputLength;
}

// changes the given char to 2 ASCII hex digits
void CharToHexPair(unsigned char in,StringPtr out)
{
	unsigned char upperNibble,lowerNibble;
	
	upperNibble=in>>4;
	lowerNibble=in&0x0F;
	
	// convert upper nibble
	if (upperNibble>9)
		out[0]=upperNibble+'A'-10;
	else
		out[0]=upperNibble+'0';
		
	// convert lower nibble
	if (lowerNibble>9)
		out[1]=lowerNibble+'A'-10;
	else
		out[1]=lowerNibble+'0';
}

// Like the standard StringToNum but can handle strings up to the full value of an unsigned long
unsigned long BetterStringToNumber(StringPtr string,unsigned long *number,Boolean *wasNegative)
{
	unsigned long		num=0;
	unsigned long		power=1;
	StringPtr			charPtr=string+string[0];
	
	*wasNegative=false;
	
	while (charPtr!=string)
	{
		if (charPtr==(string+1))
		{
			// first char, check for neg/plus
			if (charPtr[0]=='+')
			{
				charPtr--;
				continue;
			}
			else if (charPtr[0]=='-')
			{
				charPtr--;
				*wasNegative=true;
				continue;
			}
		}
	
		num+=(charPtr[0]-'0')*power;
		power*=10;
		charPtr--;
	}
	
	if (wasNegative)
		num=-num;
		
	*number=num;
	return num;
}

// This number to string proc can handle negatives and also numbers up to and including the max you can fit in an unsigned
// long.
void BetterNumberToString(unsigned long number,Boolean numberIsSigned,StringPtr string)
{
	if (numberIsSigned)
		NumToString((signed long)number,string);
	else
	{
		unsigned long		divisor=1000000000;
		Boolean				started=false;
	
		string[0]=0;
	
		while (divisor)
		{
			if (number/divisor || started)
			{
				started=true;
				string[++string[0]]=(number/divisor)+'0';
				number-=(number/divisor)*divisor;
			}
			
			if (divisor==1)
				divisor=0;
			else
				divisor/=10;
		}
	
		if (!string[0])
			string[++string[0]]='0';
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PPCompareString
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// A string comparison routine with the same sig as the PowerPlant string compare. It makes a good replacedment if you
// aren't going to mess with international text much. It's a lot faster than the tool box.
// Case sensitive compare
// Returns -1 if inLeft<inRight
// Returns +1 if inLeft>inRight
// Returns 0 if equal
SInt16 PPCompareString(
	const void*		inLeft,
	const void*		inRight,
	UInt8			inLeftLength,
	UInt8			inRightLength)
{
	if (inLeftLength<inRightLength)
		return -1;
	else if (inRightLength<inLeftLength)
		return +1;
	else
	{
		// Same length, examine chars
		for (UInt8 index=0; index<inLeftLength; index++)
		{
			if (((unsigned char*)inLeft)[index]!=((unsigned char*)inRight)[index])
			{
				// Not equal, which way do we swing?
				if (((unsigned char*)inLeft)[index]<((unsigned char*)inRight)[index])
					return -1;
				else
					return +1;
			}
		}
	}
	
	return 0;	// equal
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ BCDToString
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// converts a BCD (binary coded decimal) to a cstr.
// BCDs are used for apple version numbers, like library or os revisions. They are hex
// numbers of the form 0x0751 (7.5.1), 0x0850 (8.5) or 0x1015 (10.1.5)
char *BCDToString(
	UInt32	inBCD,
	char	outStr[10])
{
	char	*front=outStr;
	
	if (inBCD&0xF000)
		*outStr++=((inBCD&0xF000)>>12)+'0';
	*outStr++=((inBCD&0x0F00)>>8)+'0';
	*outStr++='.';
	*outStr++=((inBCD&0x00F0)>>4)+'0';
	if (inBCD&0x000F)
	{
		*outStr++='.';
		*outStr++=(inBCD&0x000F)+'0';
	}
	*outStr++=0;	// null terminate
	
	return front;
}
