// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// File_Statements.h
// © John Treece-Birch 2001
// 4/5/01
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2001, Mark Tully and John Treece-Birch
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

#include	"CStatement.h"
#include	"CGeneralExpression.h"
#include	"CArithExpression.h"
#include	"CStrExpression.h"

class CCreateFileFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression			mPath,mType,mDataType;
	
	public:
								CCreateFileFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mPath=inExpression->ReleaseStrExp();
									inExpression=inExpression->TailData();
									
									mType=inExpression->ReleaseStrExp();
									inExpression=inExpression->TailData();
									
									if (inExpression)
										mDataType=inExpression->ReleaseStrExp();
								}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};
class CFileOpenFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mReadWrite;
		StStrExpression			mPath,mType,mDataType;
	
	public:
								CFileOpenFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mPath=inExpression->ReleaseStrExp();
									inExpression=inExpression->TailData();
									
									mReadWrite=inExpression->ReleaseArithExp();
									inExpression=inExpression->TailData();
									
									mType=inExpression->ReleaseStrExp();
									inExpression=inExpression->TailData();
									
									if (inExpression)
										mDataType=inExpression->ReleaseStrExp();
								}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CAskCreateFileFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression			mType,mDataType;
		
	public:
								CAskCreateFileFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mType=inExpression->ReleaseStrExp();
									inExpression=inExpression->TailData();
									
									if (inExpression)
										mDataType=inExpression->ReleaseStrExp();
								}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CAskFileOpenFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression			mType,mDataType;
		StArithExpression		mReadWrite;
	
	public:
								CAskFileOpenFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mReadWrite=inExpression->ReleaseArithExp();
									inExpression=inExpression->TailData();
									
									mType=inExpression->ReleaseStrExp();
									inExpression=inExpression->TailData();
									
									if (inExpression)
										mDataType=inExpression->ReleaseStrExp();
								}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CFileCloseStatement : public CStatement
{
	protected:
		StArithExpression		mFile;
	
	public:
								CFileCloseStatement(
									CArithExpression	*inExpression)
								{
									mFile=inExpression;
								}
		
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CDeleteFileStatement : public CStatement
{
	protected:
		StStrExpression			mFile;
	
	public:
								CDeleteFileStatement(
									CGeneralExpression	*inExpression) 
								{
									mFile=inExpression->ReleaseStrExp();
								}
		
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CNewFolderStatement : public CStatement
{
	protected:
		StStrExpression			mFolder;
	
	public:
								CNewFolderStatement(
									CGeneralExpression	*inExpression)
								{
									mFolder=inExpression->ReleaseStrExp();
								}
		
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CFileWriteNumberStatement : public CStatement
{
	protected:
		StArithExpression		mFile,mData;
	
	public:
								CFileWriteNumberStatement(
									CGeneralExpression	*inExpression)
								{
									mFile=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mData=inExpression->ReleaseArithExp();
								}
		
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CFileWriteStringStatement : public CStatement
{
	protected:
		StArithExpression		mFile;
		StStrExpression			mData;
	
	public:
								CFileWriteStringStatement(
									CGeneralExpression	*inExpression)
								{
									mFile=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mData=inExpression->ReleaseStrExp();
								}
		
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CFileReadStringFunction : public CFixedLocStrExpression
{
	protected:
		StArithExpression		mFile;
			
	public:
								CFileReadStringFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inRange) :
									CFixedLocStrExpression(inRange)
								{
									mFile=inExpression->ReleaseArithExp();
								}
									
		CCString				&Evaluate(
									CProgram			&ioState);
};

class CFileReadIntFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mFile;
			
	public:
								CFileReadIntFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange)
								{
									mFile=inExpression->ReleaseArithExp();
								}
									
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CFileReadFloatFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mFile;
			
	public:
								CFileReadFloatFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange,true)
								{
									mFile=inExpression->ReleaseArithExp();
								}
									
		virtual TTBFloat		EvaluateFloat(
									CProgram			&ioState);
};

class CFileLengthFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mFile;
			
	public:
								CFileLengthFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange)
								{
									mFile=inExpression->ReleaseArithExp();
								}
									
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CFilePosFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mFile;
			
	public:
								CFilePosFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange)
								{
									mFile=inExpression->ReleaseArithExp();
								}
									
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CSetFilePosStatement : public CStatement
{
	protected:
		StArithExpression		mFile,mPos;
	
	public:
								CSetFilePosStatement(
									CGeneralExpression	*inExpression)
								{
									mFile=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mPos=inExpression->ReleaseArithExp();
								}
		
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CFileExistsFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression			mPath;
	
	public:
								CFileExistsFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange)
								{
									mPath=inExpression->ReleaseStrExp();
								}
									
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CFileTypeFunction : public CFixedLocStrExpression
{
	protected:
		StStrExpression			mPath;
			
	public:
								CFileTypeFunction(
									CGeneralExpression	*inExpression,
									const STextRange	&inRange) :
									CFixedLocStrExpression(inRange)
								{
									mPath=inExpression->ReleaseStrExp();
								}
									
		CCString				&Evaluate(
									CProgram			&ioState);
};

class CSetDirectoryUserPreferencesStatement : public CStatement
{	
	public:
		virtual void		ExecuteSelf(
									CProgram			&ioState);
};

class CSetDirectoryTemporaryStatement : public CStatement
{	
	public:
		virtual void		ExecuteSelf(
									CProgram			&ioState);
};

class CSetDirectorySystemPreferencesStatement : public CStatement
{	
	public:
		virtual void		ExecuteSelf(
									CProgram			&ioState);
};

class CSetDirectoryGameStatement : public CStatement
{	
	public:
		virtual void		ExecuteSelf(
									CProgram			&ioState);
};