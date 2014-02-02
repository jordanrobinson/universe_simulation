#pragma once

#pragma comment(lib,"raaMaths")

void vecInitPVec(float *pV);
void vecInitDVec(float *pV);
void vecInitCVec(float *pV);
void vecInit(float *pV);
void vecAdd(float *pV0, float *pV1, float *pVRes);
void vecSub(float *pV0, float *pV1, float *pVRes);
void vecScalarProduct(float *pV0, float fS, float *pVRes);
void vecVectorProduct(float *pV0, float *pV1, float *pVRes);
float vecDotProduct(float *pV0, float *pV1);
void vecCrossProduct(float *pV0, float *pV1, float *pVRes);
float vecLength(float *pV);
float vecDistance(float *pV1, float *pV2);
float vecNormalise(float *pV0, float *pVRes);
void vecProject(float *pVStart, float *pVDir, float fLen, float *pVRes);
void vecSet(float fX, float fY, float fZ, float *pV);
void vecCopy(float *pIn, float *pOut);
void vecSlerp(float *pVStart, float *pVEnd, float fSlerp, float *pV);