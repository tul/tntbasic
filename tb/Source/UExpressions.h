// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UExpressions.h
// © Mark Tully 2000
// 13/4/00
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
	Utility functions for stack based expression deleters
	
	Statements store a CArithExpression which is evaluated at execution time to yield a value. This CArithExpression must be
	deleted on Statement destruction. StArithExpression deletes it automatically. Also it can evaluate it by calling
	myStArithExpression(ioState);
	
	Ditto for StStrExpression
*/

#pragma once

#include	"TNTBasic_Types.h"

class CArithExpression;
class CProgram;
class CStrExpression;
class CCString;
class CGeneralExpression;

class StGenExpression
{
	protected:
		CGeneralExpression		*mExp;
	
	public:
								StGenExpression(
									CGeneralExpression *inExp=0):
									mExp(inExp)
									{}
								~StGenExpression();

		StGenExpression			&operator=(
									CGeneralExpression *inExp);

		bool					IsNull()				{ return mExp==0; }
		
		void					Release()				{ mExp=0; }
				
		CGeneralExpression		*Get()					{ return mExp; }
		CGeneralExpression		&operator*() const		{ return *mExp; }
		CGeneralExpression		*operator -> () const	{ return mExp; }
								operator CGeneralExpression*() { return mExp; }
};

class StStrExpression
{
	protected:
		CStrExpression			*mExp;
	
	public:
								StStrExpression(
									CStrExpression		*inExp=0) :
									mExp(inExp)
									{}

								~StStrExpression();
								
		StStrExpression			&operator=(
									CStrExpression *inExp);
		
		bool					IsNull()				{ return mExp==0; }
		
		CCString				&operator()(
									CProgram	&ioState);

		CStrExpression			*Release()				{ CStrExpression *exp=mExp; mExp=0; return exp; }
		CStrExpression			*Get()					{ return mExp; }
		CStrExpression			&operator*() const		{ return *mExp; }
		CStrExpression			*operator -> () const	{ return mExp; }
};

class StArithExpression
{
	protected:
		CArithExpression		*mExp;

	public:
								StArithExpression(
									CArithExpression	*inExp=0) :
									mExp(inExp)
									{
									}
								~StArithExpression();

		StArithExpression		&operator=(
									CArithExpression *inExp);

		bool					IsNull()				{ return mExp==0; }
		
		TTBInteger				operator()(
									CProgram	&ioState);
		
		CArithExpression		*Release()				{ CArithExpression *exp=mExp; mExp=0; return exp; }
		CArithExpression		*Get()					{ return mExp; }
		CArithExpression		&operator*() const		{ return *mExp; }
		CArithExpression		*operator -> () const	{ return mExp; }
};