/***************************************************************************************
 Autodesk(R) Open Reality(R) Samples
 
 (C) 2009 Autodesk, Inc. and/or its licensors
 All rights reserved.
 
 AUTODESK SOFTWARE LICENSE AGREEMENT
 Autodesk, Inc. licenses this Software to you only upon the condition that 
 you accept all of the terms contained in the Software License Agreement ("Agreement") 
 that is embedded in or that is delivered with this Software. By selecting 
 the "I ACCEPT" button at the end of the Agreement or by copying, installing, 
 uploading, accessing or using all or any portion of the Software you agree 
 to enter into the Agreement. A contract is then formed between Autodesk and 
 either you personally, if you acquire the Software for yourself, or the company 
 or other legal entity for which you are acquiring the software.
 
 AUTODESK, INC., MAKES NO WARRANTY, EITHER EXPRESS OR IMPLIED, INCLUDING BUT 
 NOT LIMITED TO ANY IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR 
 PURPOSE REGARDING THESE MATERIALS, AND MAKES SUCH MATERIALS AVAILABLE SOLELY ON AN 
 "AS-IS" BASIS.
 
 IN NO EVENT SHALL AUTODESK, INC., BE LIABLE TO ANYONE FOR SPECIAL, COLLATERAL, 
 INCIDENTAL, OR CONSEQUENTIAL DAMAGES IN CONNECTION WITH OR ARISING OUT OF PURCHASE 
 OR USE OF THESE MATERIALS. THE SOLE AND EXCLUSIVE LIABILITY TO AUTODESK, INC., 
 REGARDLESS OF THE FORM OF ACTION, SHALL NOT EXCEED THE PURCHASE PRICE OF THE 
 MATERIALS DESCRIBED HEREIN.
 
 Autodesk, Inc., reserves the right to revise and improve its products as it sees fit.
 
 Autodesk and Open Reality are registered trademarks or trademarks of Autodesk, Inc., 
 in the U.S.A. and/or other countries. All other brand names, product names, or 
 trademarks belong to their respective holders. 
 
 GOVERNMENT USE
 Use, duplication, or disclosure by the U.S. Government is subject to restrictions as 
 set forth in FAR 12.212 (Commercial Computer Software-Restricted Rights) and 
 DFAR 227.7202 (Rights in Technical Data and Computer Software), as applicable. 
 Manufacturer is Autodesk, Inc., 10 Duke Street, Montreal, Quebec, Canada, H3C 2L7.
***************************************************************************************/

/**	\file	ortoolview3d_view.cxx
*/

#include <gl\glut.h>

//--- Class declaration
#include "ortoolview3d_view.h"


FBClassImplementation( ORView3D );

/************************************************
 *	Constructor
 ************************************************/
ORView3D::ORView3D() :
	mRender(NULL)
{
	mRender = new FBRenderer(0);

	FBViewingOptions* lVO = mRender->GetViewingOptions();
	lVO->ShowTimeCode() = false;
	lVO->ShowSafeArea() = false;
	lVO->ShowCameraLabel() = false;

	mRender->ShowStats = false;

	mOffset = 0;


}

void ORView3D::SetCamera(FBCamera *c)
{
	mRender->CurrentCamera = NULL;
	mCamera = c;
	mRender->UseCameraSwitcher = false;
	mRender->CurrentCamera = c;
}


/************************************************
 *	Constructor
 ************************************************/
void ORView3D::FBDestroy()
{
	delete mRender;
}

/************************************************
 *	Refresh callback
 ************************************************/
void ORView3D::Refresh(bool pNow)
{
	FBView::Refresh(pNow);
}

/************************************************
 *	Expose callback
 ************************************************/
void ORView3D::ViewExpose()
{
	if (mRender->RenderBegin(0, 0, Region.Position.X[1] - Region.Position.X[0], Region.Position.Y[1] - Region.Position.Y[0]))
	{

		// Here I can do my rendering
		SetViewport(0, 0, Region.Width, Region.Height);

		// This call will destroy the frame buffer
		mRender->PreRender();

		mRender->Render();

		/*glMatrixMode(GL_PROJECTION);
		float m[16] = { 1, 0, 0, mOffset,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };

		glLoadMatrixf(m);


		glMatrixMode(GL_MODELVIEW);

		glBegin(GL_LINES);
		glColor3f(1.f, 1.f, 1.f);
		glLineWidth(2.5f);
		glVertex3f(0.f, 5.f, 0.f);
		glVertex3f(-50.f, 5.f, 0.f);
		glVertex3f(50.f, 5.f, 0.f);
		glVertex3f(0.f, 5.f, 0.f);

		glVertex3f(5.f, 0.f, 0.f);
		glVertex3f(5.f, -50.f, 0.f);
		glVertex3f(5.f, 50.f, 0.f);
		glVertex3f(5.f, 0.f, 0.f);

		glVertex3f(5.f, 0.f, 0.f);
		glVertex3f(5.f, 0.f, -60.f);
		glVertex3f(5.f, 0.f, 60.f);
		glVertex3f(5.f, 0.f, 0.f);

		glEnd();*/

		//glClearColor(0,0,0,0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		mRender->RenderEnd();
	}
}


void ORView3D::ViewInput(int pMouseX, int pMouseY, FBInputType pAction, int pButtonKey, int pModifier)
{
	//mRender->MouseInput(pMouseX, pMouseY, pAction, pButtonKey, (FBInputModifier)pModifier);

	if (pAction == FBInputType::kFBKeyPress)
	{
		mRender->CurrentCamera = mCamera;
	}
/*	if (pAction == FBInputType::kFBKeyRelease)
	{
		mRender->KeyboardInput((FBDeviceKeyboardKey) pButtonKey, false);
	}
	if (pAction == FBInputType::kFBButtonPress)
	{
	}*/
}
