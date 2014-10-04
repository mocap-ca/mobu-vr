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



/**	\file	ortoolview3d_tool.cxx
*/

//--- Class declaration
#include "ortoolview3d_tool.h"
#include <fbsdk/fbsystem.h>

//--- Registration defines
#define ORTOOLVIEW3D__CLASS	ORTOOLVIEW3D__CLASSNAME
#define ORTOOLVIEW3D__LABEL	"View3D"
#define ORTOOLVIEW3D__DESC	"OR - 3D Viewer Tool"

//--- Implementation and registration
FBToolImplementation(	ORTOOLVIEW3D__CLASS	);
FBRegisterTool		(	ORTOOLVIEW3D__CLASS,
						ORTOOLVIEW3D__LABEL,
						ORTOOLVIEW3D__DESC,
						FB_DEFAULT_SDK_ICON	);	// Icon filename (default=Open Reality icon)

/************************************************
 *	Constructor.
 ************************************************/


bool ORToolView3D::FBCreate()
{


	/* No effect 
	Border.Style = kFBNoBorder;
	Border.Spacing = 0;
	Border.Width = 0;
	Border.Caption = "Blah 1";
	Caption = "Blash";
	*/

	// Init oculus

	mLensSeperation = 0.0635f;
	mHScreenSize = 0.149759993f;
	mInterpupillaryDistance = 0.064f;
	mOffset = 0.f;
	StartSize[0] = 1280;
	StartSize[1] = 800;


#ifdef OCULUSSUPPORT
	// Get oculus device info
	OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
	OVR::Ptr<OVR::DeviceManager> pManager;
	OVR::Ptr<OVR::HMDDevice> pHMD;
	pManager = *OVR::DeviceManager::Create();
	pHMD = *pManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
	OVR::HMDInfo hmd;


	if (pHMD && pHMD->GetDeviceInfo(&hmd))
	{
		//std::string MonitorName(hmd.DisplayDeviceName);
		/*float EyeDistance = hmd.InterpupillaryDistance;
		float DistortionK[4];
		DistortionK[0] = hmd.DistortionK[0];
		DistortionK[1] = hmd.DistortionK[1];
		DistortionK[2] = hmd.DistortionK[2];
		DistortionK[3] = hmd.DistortionK[3];*/
		StartSize[0] = hmd.HResolution;
		StartSize[1] = hmd.VResolution;

		StartPos[0] = hmd.DesktopX;
		StartPos[1] = hmd.DesktopY;

		mLensSeperation = hmd.LensSeparationDistance;
		mHScreenSize = hmd.HScreenSize;
		mInterpupillaryDistance = hmd.InterpupillaryDistance;

		mOffset =  (mHScreenSize * 0.25f - mLensSeperation * 0.5f) / mHScreenSize * hmd.HResolution;
	}
#endif


	// Manage UI
	UICreate	();
	UIConfigure	();
	UIReset		();

	// Add tool callbacks
	OnShow.Add	( this, (FBCallback) &ORToolView3D::EventToolShow		);
	OnIdle.Add	( this, (FBCallback) &ORToolView3D::EventToolIdle		);
	OnResize.Add( this, (FBCallback) &ORToolView3D::EventToolResize		);
	OnPaint.Add	( this, (FBCallback) &ORToolView3D::EventToolPaint		);
	OnInput.Add	( this, (FBCallback) &ORToolView3D::EventToolInput		);


	return true;
}


void ORToolView3D::SetCamera()
{
	FBSystem fb;

	mCamera = NULL;


	FBPropertyListCamera cams = fb.Scene->Cameras;
	int ncams = cams.GetCount();
	for (int i = 0; i < ncams; i++)
	{
		FBComponent *cam = cams.GetAt(i);
		if (cam == NULL) continue;
		if (cam->Name == "ORCamera")
		{
			if (cam->Is(FBCameraStereo::TypeInfo))
			{
				mCamera = dynamic_cast<FBCameraStereo*>(cam);
				mViewL.SetCamera(mCamera->LeftCamera);
				mViewR.SetCamera(mCamera->RightCamera);
				mViewL.mOffset = mLensSeperation;
				mViewR.mOffset = -mLensSeperation;
				return;
			}
		}
	}

	mCamera = new FBCameraStereo("ORCamera");
	mCamera->InteraxialSeparation = this->mInterpupillaryDistance * 100;
	mCamera->InteractiveMode = true;
	mCamera->Stereo = FBCameraStereoType::kFBCameraStereoParallel;
	//mCamera->ViewShowName = false;
	mCamera->ViewShowAxis = false;
	mCamera->ViewShowGrid = true;
	mCamera->ViewShowTimeCode = false;

	mViewL.mOffset = mOffset;
	mViewR.mOffset = -mOffset;
	mViewL.SetCamera(mCamera->LeftCamera);
	mViewR.SetCamera(mCamera->RightCamera);

}

/************************************************
 *	Create, configure & reset UI.
 ************************************************/
void ORToolView3D::UICreate()
{

	SetCamera();

	// Configure layout
	AddRegion( "View3DL", "View3DL",
										0,	kFBAttachLeft,	"",	1.0,
										0,	kFBAttachTop,   "",	1.0,
										0,  kFBAttachWidth,	"", 0.45,
										0,  kFBAttachBottom,"",1.0 );
	AddRegion( "View3DR", "View3DR",
										0, kFBAttachWidth, "", 0.5,
										0, kFBAttachTop,   "", 1.0,
										0, kFBAttachWidth,"",  0.5,
										0, kFBAttachBottom,"", 1.0 );

	MoveRegion("View3DL", mOffset, 0);
	MoveRegion("View3DR", -mOffset, 0);

	// Assign regions
	SetView( "View3DL", mViewL);
	SetView( "View3DR", mViewR);

	/* no effect
	SetBorder("View3DL", FBBorderStyle::kFBNoBorder, false, false, 0, 0, 0, 0);
	SetBorder("View3DR", FBBorderStyle::kFBNoBorder, false, false, 0, 0, 0, 0);
	*/


}
void ORToolView3D::UIConfigure()
{
}
void ORToolView3D::UIReset()
{
}

/************************************************
 *	Destruction function.
 ************************************************/
void ORToolView3D::FBDestroy()
{
	// Remove tool callbacks
	OnShow.Remove	( this, (FBCallback) &ORToolView3D::EventToolShow	);
	OnIdle.Remove	( this, (FBCallback) &ORToolView3D::EventToolIdle	);
	OnPaint.Remove	( this, (FBCallback) &ORToolView3D::EventToolPaint	);
	OnInput.Remove	( this, (FBCallback) &ORToolView3D::EventToolInput	);
	OnResize.Remove	( this, (FBCallback) &ORToolView3D::EventToolResize	);

#ifdef OCULUSSUPPORT
	OVR::System::Destroy();
#endif
	// Free user allocated memory
}

/************************************************
 *	UI Idle callback.
 ************************************************/
void ORToolView3D::EventToolIdle( HISender pSender, HKEvent pEvent )
{
	RefreshView();
}

void ORToolView3D::RefreshView()
{
	mViewL.Refresh(true);
	mViewR.Refresh(true);
}

/************************************************
 *	Handle tool activation (selection/unselection).
 ************************************************/
void ORToolView3D::EventToolShow( HISender pSender, HKEvent pEvent )
{
	FBEventShow lEvent( pEvent );

	if( lEvent.Shown )
	{
		// Reset the UI here.
		SetCamera();
	}
	else
	{
		mViewL.SetCamera(NULL);
		mViewR.SetCamera(NULL);
		mCamera = NULL;
	}
}

/************************************************
 *	Paint callback for tool (on expose).
 ************************************************/
void ORToolView3D::EventToolPaint( HISender pSender, HKEvent pEvent )
{
}

/************************************************
 *	Tool resize callback.
 ************************************************/
void ORToolView3D::EventToolResize( HISender pSender, HKEvent pEvent )
{
}

/************************************************
 *	Handle input into the tool.
 ************************************************/
void ORToolView3D::EventToolInput( HISender pSender, HKEvent pEvent )
{
}

/************************************************
 *	FBX Storage.
 ************************************************/
bool ORToolView3D::FbxStore	( FBFbxObject* pFbxObject, kFbxObjectStore pStoreWhat )
{
	pFbxObject->FieldWriteBegin( "ORToolView3DSection" );
	{
	}
	pFbxObject->FieldWriteEnd();
	return true;
}

/************************************************
 *	FBX Retrieval.
 ************************************************/
bool ORToolView3D::FbxRetrieve( FBFbxObject* pFbxObject, kFbxObjectStore pStoreWhat )
{
	pFbxObject->FieldReadBegin( "ORToolView3DSection" );
	{
	}
	pFbxObject->FieldReadEnd();
	return true;
}

