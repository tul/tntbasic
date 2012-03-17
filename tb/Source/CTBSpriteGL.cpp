// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CTBSpriteGL.cpp
// © John Treece-Birch 2002
// 21/2/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

#include	"CTBSpriteGL.h"
#include	"CViewport.h"
#include	"FP.h"
#ifdef __MWERKS__
#include	<agl.h>
#else
#include	<AGL/agl.h>
#endif

const TTBFloat		kPi=3.141592654;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Render
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CTBSpriteGL::Render(
	int 		inScreenWidth,
	int			inScreenHeight,
	Rect		&inBounds,
	float		inOffsetX,
	float		inOffsetY)
{
	if (Valid())
	{						
		float		glScreenWidth=inScreenWidth/2.0f;
		float		glScreenHeight=inScreenHeight/2.0f;
	
		float		top,left,bottom,right,temp;
		float		width=FRectWidth(inBounds);
		float		height=FRectHeight(inBounds);
		
		CalculateRenderCoords(top,left,bottom,right);
		
		if (mCropped)
		{
			int		leftScissor=Greater(inBounds.left,mCropRect.left+inBounds.left-inOffsetX);
			int		topScissor=Greater(inBounds.top,mCropRect.top+inBounds.top-inOffsetY);
			int		rightScissor=Lesser(inBounds.right,mCropRect.right+inBounds.left-inOffsetX);
			int		bottomScissor=Lesser(inBounds.bottom,mCropRect.bottom+inBounds.top-inOffsetY);
			
			glScissor(leftScissor,inScreenHeight-bottomScissor,rightScissor-leftScissor,bottomScissor-topScissor);
		}
		else
			glScissor(inBounds.left,inScreenHeight-inBounds.bottom,width,height);
	
		top=-top;
		bottom=-bottom;
	
		// Handle flipping
		if (GetXFlip())
		{
			temp=left;
			left=right;
			right=temp;
		}
		
		if (GetYFlip())
		{
			temp=top;
			top=bottom;
			bottom=temp;
		}
		
		glPushMatrix();
		glTranslatef((GetX()+inBounds.left-inOffsetX)/glScreenWidth-1.0f,(inScreenHeight-(GetY()+inBounds.top-inOffsetY))/glScreenHeight-1.0f,0.0f);
		glScalef((float)2.0f/(float)inScreenWidth,(float)2.0f/(float)inScreenHeight,1.0f);
		
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			
		if (GetAngle())
			glRotatef(GetAngle(),0,0,1.0);
		
		glScalef(GetXScale(),GetYScale(),0.0f);
		
		if (IsColoured())
		{
			glColor4f(GetRed(),GetGreen(),GetBlue(),GetOpacity());
			glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND);
		}
		else
		{
			glColor4f(1.0f,1.0f,1.0f,GetOpacity());
			glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		}
		
		glBindTexture(GL_TEXTURE_2D,GetTextureNumber());
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,GetHeightPercent()); glVertex3f(left,bottom,0.0f);
			
			glTexCoord2f(0.0f,0.0f); glVertex3f(left,top,0.0f);
			
			glTexCoord2f(GetWidthPercent(),0.0f); glVertex3f(right,top,0.0f);
			
			glTexCoord2f(GetWidthPercent(),GetHeightPercent()); glVertex3f(right,bottom,0.0f);
		glEnd();
		glPopMatrix();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Sprite Collision
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool CTBSpriteGL::SpriteCollision(
	CTBSprite		*inSprite)
{
	if (Valid())
	{
		CTBSpriteGL		*otherSprite=(CTBSpriteGL*)inSprite;
		
		if (otherSprite->Valid())
		{
			Rect			rectA,rectB;

			// Calculate the sprite rects
			CalculateCollisionRect(rectA);
			otherSprite->CalculateCollisionRect(rectB);
		
			// If the rects do not touch then return
			// NB: if rectA.top==rectB.bottom then the rects are not overlapping, same for left==right
			if ((rectA.top>=rectB.bottom) || (rectA.bottom<=rectB.top) || (rectA.left>=rectB.right) || (rectA.right<=rectB.left))
				return false;

			return true;
		}
	}
	
	return false;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Calculate Collision Rect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CTBSpriteGL::CalculateCollisionRect(
	Rect	&outRect)
{
	if (mAngle==0)
	{
		if (mYFlipped)
			outRect.top=GetY()-mSpriteImage->GetHeight()+mSpriteImage->GetYOffset()*GetYScale();
		else
			outRect.top=GetY()-mSpriteImage->GetYOffset()*GetYScale();
		
		if (mXFlipped)
			outRect.left=GetX()-mSpriteImage->GetWidth()+mSpriteImage->GetXOffset()*GetXScale();
		else
			outRect.left=GetX()-mSpriteImage->GetXOffset()*GetXScale();
			
		outRect.bottom=outRect.top+mSpriteImage->GetHeight()*GetYScale();
		outRect.right=outRect.left+mSpriteImage->GetWidth()*GetXScale();
	}
	else
	{
		float	cosValue=::cos(-mAngle*kPi/180),sinValue=::sin(-mAngle*kPi/180);
		float	a,b,c,d;
		
		if (mXFlipped)
		{
			a=(mSpriteImage->GetWidth()-mSpriteImage->GetXOffset())*GetXScale();
			b=mSpriteImage->GetXOffset()*GetXScale();
		}
		else
		{
			b=(mSpriteImage->GetWidth()-mSpriteImage->GetXOffset())*GetXScale();
			a=mSpriteImage->GetXOffset()*GetXScale();
		}
		
		if (mYFlipped)
		{
			c=(mSpriteImage->GetHeight()-mSpriteImage->GetYOffset())*GetYScale();
			d=mSpriteImage->GetYOffset()*GetYScale();
		}
		else
		{
			d=(mSpriteImage->GetHeight()-mSpriteImage->GetYOffset())*GetYScale();
			c=mSpriteImage->GetYOffset()*GetYScale();
		}
		
		// Calculate left
		outRect.left=-a*cosValue-c*sinValue;
		outRect.left=Lesser(outRect.left,-a*cosValue-d*sinValue);
		outRect.left=Lesser(outRect.left,b*cosValue+c*sinValue);
		outRect.left=Lesser(outRect.left,b*cosValue-d*sinValue);
		
		// Calculate top
		outRect.top=-c*cosValue+b*sinValue;
		outRect.top=Lesser(outRect.top,-c*cosValue-a*sinValue);
		outRect.top=Lesser(outRect.top,d*cosValue-a*sinValue);
		outRect.top=Lesser(outRect.top,d*cosValue+b*sinValue);
		
		// Calculate right
		outRect.right=-a*cosValue-c*sinValue;
		outRect.right=Greater(outRect.right,-a*cosValue-d*sinValue);
		outRect.right=Greater(outRect.right,b*cosValue+c*sinValue);
		outRect.right=Greater(outRect.right,b*cosValue-d*sinValue);
		
		// Calculate bottom
		outRect.bottom=-c*cosValue+b*sinValue;
		outRect.bottom=Greater(outRect.bottom,-c*cosValue-a*sinValue);
		outRect.bottom=Greater(outRect.bottom,d*cosValue-a*sinValue);
		outRect.bottom=Greater(outRect.bottom,d*cosValue+b*sinValue);
		
		outRect.top+=GetY();
		outRect.left+=GetX();
		outRect.bottom+=GetY();
		outRect.right+=GetX();
	}
	
	if (mViewport)
	{
		SInt16	xOffset=mViewport->GetLeft()-mViewport->GetXOffset();
		SInt16	yOffset=mViewport->GetTop()-mViewport->GetYOffset();
		
		outRect.top+=yOffset;
		outRect.left+=xOffset;
		outRect.bottom+=yOffset;
		outRect.right+=xOffset;
	}
}
