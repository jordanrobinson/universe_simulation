#include "StdAfx.h"

#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include "raaVector.h"
#include "raaMatrix.h"

#include "raaCamera.h"

void camInit( raaCamera &rCam )
{
	camReset(rCam);
}

void camReset( raaCamera &rCam )
{
	vecInitPVec(rCam.m_fVP);
	vecInitPVec(rCam.m_fVT);
	vecInitDVec(rCam.m_fVR);
	vecInitDVec(rCam.m_fVD);
	vecInitDVec(rCam.m_fVU);
	vecSet(0.0f, 0.0f, 9000.0f, rCam.m_fVP);
	vecSet(0.0f, 0.0f, 1.0f, rCam.m_fVD);
	vecSet(1.0f, 0.0f, 0.0f, rCam.m_fVR);
	vecSet(0.0f, 1.0f, 0.0f, rCam.m_fVU);
	vecSet(0.0f, 0.0f, 0.0f, rCam.m_fVT);
	rCam.m_fNear=0.2f;
	rCam.m_fFar=3000000.0f;
}

void camSet(float fPositionX, float fPositionY, float fPositionZ, raaCamera &rCam) {
	vecSet(fPositionX, fPositionY, fPositionZ, rCam.m_fVP);
}

void camCopy(raaCamera &rIn, raaCamera &rOut)
{
	vecCopy(rIn.m_fVP, rOut.m_fVP);
	vecCopy(rIn.m_fVT, rOut.m_fVT);
	vecCopy(rIn.m_fVR, rOut.m_fVR);
	vecCopy(rIn.m_fVU, rOut.m_fVU);
	vecCopy(rIn.m_fVD, rOut.m_fVD);

	rOut.m_fFar=rIn.m_fFar;
	rOut.m_fNear=rIn.m_fNear;
}

void camApply( raaCamera &rCam )
{
	gluLookAt(rCam.m_fVP[0], rCam.m_fVP[1], rCam.m_fVP[2], rCam.m_fVT[0], rCam.m_fVT[1], rCam.m_fVT[2], rCam.m_fVU[0], rCam.m_fVU[1], rCam.m_fVU[2]); 
}

void camObjMat( raaCamera &rCam, float *pafMat )
{
	float mR[16], mT[16];

	matMakeIdentity(mR);
	matMakeIdentity(mT);

	mR[0]=rCam.m_fVR[0];
	mR[1]=rCam.m_fVR[1];
	mR[2]=rCam.m_fVR[2];
	mR[4]=rCam.m_fVU[0];
	mR[5]=rCam.m_fVU[1];
	mR[6]=rCam.m_fVU[2];
	mR[8]=-rCam.m_fVD[0];
	mR[9]=-rCam.m_fVD[1];
	mR[10]=-rCam.m_fVD[2];
	mT[12]=rCam.m_fVP[0];
	mT[13]=rCam.m_fVP[1];
	mT[14]=rCam.m_fVP[2];

	matMatMult(mR, mT, pafMat);
}

void camExploreRotate( raaCamera &rCam, float fUp, float fRight )
{
	float af[]={0.0f, 0.0f, 0.0f, 0.0f};
	float fLen;

	vecSub(rCam.m_fVT, rCam.m_fVP, af);
	fLen=vecLength(af);

	vecProject(rCam.m_fVP, rCam.m_fVR, fRight, rCam.m_fVP);
	vecSub(rCam.m_fVT, rCam.m_fVP, rCam.m_fVD);
	vecNormalise(rCam.m_fVD, rCam.m_fVD);
	vecCrossProduct(rCam.m_fVD, rCam.m_fVU, rCam.m_fVR);
	vecNormalise(rCam.m_fVR, rCam.m_fVR);

	vecProject(rCam.m_fVP, rCam.m_fVU, fUp, rCam.m_fVP);
	vecSub(rCam.m_fVT, rCam.m_fVP, rCam.m_fVD);
	vecNormalise(rCam.m_fVD, rCam.m_fVD);
	vecCrossProduct(rCam.m_fVR, rCam.m_fVD, rCam.m_fVU);
	vecNormalise(rCam.m_fVU, rCam.m_fVU);
	vecProject(rCam.m_fVT, rCam.m_fVD, -fLen, rCam.m_fVP);

	camResolveUp(rCam);
}

void camExploreTravel( raaCamera &rCam, float fDisp)
{
	float fLen;
	float aVec[4];

	vecInitDVec(aVec);
	vecSub(rCam.m_fVP, rCam.m_fVT, aVec);
	fLen=vecNormalise(aVec, aVec)+fDisp;
	if(fLen<rCam.m_fNear) fLen=rCam.m_fNear;
	if(fLen>rCam.m_fFar) fLen=rCam.m_fFar;
	vecProject(rCam.m_fVT, aVec, fLen, rCam.m_fVP);
}


void camExploreUpdateTarget( raaCamera &rCam, float *pfTarget )
{
	if(pfTarget)
	{
		float fLen=vecDistance(rCam.m_fVP, rCam.m_fVT);

		vecCopy(pfTarget, rCam.m_fVT);

		vecProject(rCam.m_fVT, rCam.m_fVD, -fLen, rCam.m_fVP);
	}
}

void camFlyRotate( raaCamera &rCam, float fUp, float fRight )
{
	float af[]={0.0f, 0.0f, 0.0f, 0.0f};

	vecProject(af, rCam.m_fVR, fRight, af);
	vecAdd(rCam.m_fVD, af,rCam.m_fVD);
	vecNormalise(rCam.m_fVD, rCam.m_fVD);
	vecProject(rCam.m_fVP, rCam.m_fVD, 1.0f, rCam.m_fVT);
	vecCrossProduct(rCam.m_fVD, rCam.m_fVU, rCam.m_fVR);
	vecNormalise(rCam.m_fVR, rCam.m_fVR);
	vecProject(af, rCam.m_fVU, fUp, af);
	vecAdd(rCam.m_fVD, af, rCam.m_fVD);
	vecNormalise(rCam.m_fVD, rCam.m_fVD);
	vecProject(rCam.m_fVP, rCam.m_fVD, 1.0f, rCam.m_fVT);
	vecCrossProduct(rCam.m_fVR, rCam.m_fVD, rCam.m_fVU);
	vecNormalise(rCam.m_fVU, rCam.m_fVU);

	camResolveUp(rCam);
}

void camFlyTravel( raaCamera &rCam, float fForward, float fHori, float fVert )
{
	float fLen=vecDistance(rCam.m_fVP, rCam.m_fVT);

	vecProject(rCam.m_fVP, rCam.m_fVD, fForward, rCam.m_fVP);
	vecProject(rCam.m_fVP, rCam.m_fVR, fHori, rCam.m_fVP);
	vecProject(rCam.m_fVP, rCam.m_fVU, fVert, rCam.m_fVP);

	vecProject(rCam.m_fVP, rCam.m_fVD, fLen, rCam.m_fVT);
}

void camSLERP( raaCamera &rCamStart, raaCamera &rCamEnd, float fSlerp, raaCamera &rCam )
{
	vecSlerp(rCamStart.m_fVD, rCamEnd.m_fVD, fSlerp, rCam.m_fVD);
	vecNormalise(rCam.m_fVD, rCam.m_fVD);

	vecSlerp(rCamStart.m_fVU, rCamEnd.m_fVU, fSlerp, rCam.m_fVU);
	vecNormalise(rCam.m_fVU, rCam.m_fVU);

	vecSlerp(rCamStart.m_fVR, rCamEnd.m_fVR, fSlerp, rCam.m_fVR);
	vecNormalise(rCam.m_fVR, rCam.m_fVR);

	vecSlerp(rCamStart.m_fVP, rCamEnd.m_fVP, fSlerp, rCam.m_fVP);
	vecSlerp(rCamStart.m_fVT, rCamEnd.m_fVT, fSlerp, rCam.m_fVT);
}

void camResolveUp( raaCamera &rCam )
{
	float av[]={0.0f, 1.0f, 0.0f, 0.0f};

	vecCrossProduct(rCam.m_fVD, av, rCam.m_fVR);
	vecNormalise(rCam.m_fVR, rCam.m_fVR);

	vecCrossProduct(rCam.m_fVR, rCam.m_fVD, rCam.m_fVU);
	vecNormalise(rCam.m_fVU, rCam.m_fVU);
}

void camInputInit( raaCameraInput &rInput )
{
	rInput.m_bExplore=false;
	rInput.m_bFly=false;
	rInput.m_bMouse=false;
	rInput.m_aiMouseStart[0]=0;
	rInput.m_aiMouseStart[1]=0;
	rInput.m_aiMouseLast[0]=0;
	rInput.m_aiMouseLast[1]=0;
	rInput.m_tbKeyTravel=tri_null;
	rInput.m_tbKeyPanHori=tri_null;
	rInput.m_tbKeyPanVert=tri_null;
	rInput.m_fAngularSensitivity=2.0f;
	rInput.m_fLinearSensitivity=50.0f;
}

void camInputExplore( raaCameraInput &rInput, bool bState )
{
	rInput.m_bExplore=bState;
}

void camInputFly( raaCameraInput &rInput, bool bState )
{
	rInput.m_bFly=bState;
}

void camInputMouse( raaCameraInput &rInput, bool bState )
{
	rInput.m_bMouse=bState;
}

void camInputTravel( raaCameraInput &rInput, tribool tbState )
{
	rInput.m_tbKeyTravel=tbState;
}

void camInputPanHori( raaCameraInput &rInput, tribool tbState )
{
	rInput.m_tbKeyPanHori=tbState;
}

void camInputPanVert( raaCameraInput &rInput, tribool tbState )
{
	rInput.m_tbKeyPanVert=tbState;
}

void camInputSetMouseStart( raaCameraInput &rInput, int iX, int iY )
{
	rInput.m_aiMouseLast[0]=rInput.m_aiMouseStart[0]=iX;
	rInput.m_aiMouseLast[1]=rInput.m_aiMouseStart[1]=iY;
}

void camInputSetMouseLast( raaCameraInput &rInput, int iX, int iY )
{
	rInput.m_aiMouseLast[0]=iX;
	rInput.m_aiMouseLast[1]=iY;
}

void camProcessInput( raaCameraInput &rInput, raaCamera &rCam )
{
	float fDeltaX=0.0f;
	float fDeltaY=0.0f;

	float fTravel=(rInput.m_tbKeyTravel==tri_null) ? 0.0f : (rInput.m_tbKeyTravel==tri_pos) ? 1.0f : -1.0f;
	float fHori=(rInput.m_tbKeyPanHori==tri_null) ? 0.0f : (rInput.m_tbKeyPanHori==tri_pos) ? 1.0f : -1.0f;
	float fVert=(rInput.m_tbKeyPanVert==tri_null) ? 0.0f : (rInput.m_tbKeyPanVert==tri_pos) ? 1.0f : -1.0f;

	if(rInput.m_bMouse)
	{
		fDeltaX=((float)(rInput.m_aiMouseLast[0]-rInput.m_aiMouseStart[0]))*rInput.m_fAngularSensitivity;
		fDeltaY=((float)(rInput.m_aiMouseLast[1]-rInput.m_aiMouseStart[1]))*rInput.m_fAngularSensitivity;
	}

	if(rInput.m_bFly)
	{
		camFlyRotate(rCam, fDeltaY, fDeltaX);
		camFlyTravel(rCam, fTravel*rInput.m_fLinearSensitivity, fHori*rInput.m_fLinearSensitivity, fVert*rInput.m_fLinearSensitivity);
	}
	else if(rInput.m_bExplore)
	{
		camExploreRotate(rCam, fDeltaY, fDeltaX);
		camExploreTravel(rCam, -fTravel*rInput.m_fLinearSensitivity);			
	}
}
