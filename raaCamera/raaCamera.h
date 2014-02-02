#pragma once
#pragma comment(lib,"raaCamera")

enum tribool
{
	tri_null,
	tri_pos,
	tri_neg
};

typedef struct _raaCameraInput
{
	bool m_bExplore;
	bool m_bFly;
	bool m_bMouse;
	int m_aiMouseStart[2];
	int m_aiMouseLast[2];
	tribool m_tbKeyTravel;
	tribool m_tbKeyPanHori;
	tribool m_tbKeyPanVert;
	float m_fAngularSensitivity;
	float m_fLinearSensitivity;
} raaCameraInput;


typedef struct _raaCamera
{
	float m_fVP[4];
	float m_fVT[4];
	float m_fVR[4];
	float m_fVU[4];
	float m_fVD[4];
	float m_fNear;
	float m_fFar;
} raaCamera;

// input mode functions
void camInputInit(raaCameraInput &rInput);
void camInputExplore(raaCameraInput &rInput, bool bState);
void camInputFly(raaCameraInput &rInput, bool bState);
void camInputMouse(raaCameraInput &rInput, bool bState);
void camInputTravel(raaCameraInput &rInput, tribool tbState);
void camInputPanHori(raaCameraInput &rInput, tribool tbState);
void camInputPanVert(raaCameraInput &rInput, tribool tbState);
void camInputSetMouseStart(raaCameraInput &rInput, int iX, int iY);
void camInputSetMouseLast(raaCameraInput &rInput, int iX, int iY);

void camProcessInput(raaCameraInput &rInput, raaCamera &rCam);

// basic functions
void camInit(raaCamera &rCam);
void camReset(raaCamera &rCam);
void camCopy(raaCamera &rIn, raaCamera &rOut);
void camApply(raaCamera &rCam);
void camObjMat(raaCamera &rCam, float *pafMat);

// explore mode functions
void camExploreRotate(raaCamera &rCam, float fUp, float fRight);
void camExploreTravel(raaCamera &rCam, float fDisp);
void camExploreUpdateTarget(raaCamera &rCam, float *pfTarget);

// fly mode functions
void camFlyRotate(raaCamera &rCam, float fUp, float fRight);
void camFlyTravel(raaCamera &rCam, float fForward, float fHori, float fVert);

// slerp
void camSLERP(raaCamera &rCamStart, raaCamera &rCamEnd, float fSlerp, raaCamera &rCam);

// internal use
void camResolveUp(raaCamera &rCam);

//jr functions
void camSet(float fPositionX, float fPositionY, float fPositionZ, raaCamera &rCam);