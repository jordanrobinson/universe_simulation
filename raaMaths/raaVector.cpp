#include "StdAfx.h"

#include <math.h>

#include "raaMaths.h"
#include "raaVector.h"

void vecAdd( float *pV0, float *pV1, float *pVRes )
{
	if(pV0 && pV1 && pVRes)
	{
		pVRes[0]=pV0[0]+pV1[0];
		pVRes[1]=pV0[1]+pV1[1];
		pVRes[2]=pV0[2]+pV1[2];
	}
}

void vecInitPVec( float *pV )
{
	if(pV)
	{
		pV[0]=pV[1]=pV[2]=0.0f;
		pV[3]=1.0f;
	}
}

void vecInitDVec( float *pV )
{
	if(pV) pV[0]=pV[1]=pV[2]=pV[3]=0.0f;
}

void vecInit( float *pV )
{
	if(pV) pV[0]=pV[1]=pV[2]=0.0f;
}

void vecScalarProduct( float *pV0, float fS, float *pVRes )
{
	if(pV0 && pVRes)
	{
		pVRes[0]=pV0[0]*fS;
		pVRes[1]=pV0[1]*fS;
		pVRes[2]=pV0[2]*fS;
	}
}

void vecSub( float *pV0, float *pV1, float *pVRes )
{
	if(pV0 && pV1 && pVRes)
	{
		pVRes[0]=pV0[0]-pV1[0];
		pVRes[1]=pV0[1]-pV1[1];
		pVRes[2]=pV0[2]-pV1[2];
	}
}

void vecVectorProduct( float *pV0, float *pV1, float *pVRes )
{
	if(pV0 && pV1 && pVRes)
	{
		pVRes[0]=pV0[0]*pV1[0];
		pVRes[1]=pV0[1]*pV1[1];
		pVRes[2]=pV0[2]*pV1[2];
	}
}

float vecDotProduct( float *pV0, float *pV1 )
{
	if(pV0 && pV1) return pV0[0]*pV1[0]+pV0[1]*pV1[1]+pV0[2]*pV1[2];
	return 0.0f;
}

void vecCrossProduct( float *pV0, float *pV1, float *pVRes )
{
	if(pV0 && pV1 && pVRes)
	{
		float v[3];

		v[0]=pV0[1]*pV1[2]-pV0[2]*pV1[1];
		v[1]=pV0[2]*pV1[0]-pV0[0]*pV1[2];
		v[2]=pV0[0]*pV1[1]-pV0[1]*pV1[0];

		pVRes[0]=v[0];
		pVRes[1]=v[1];
		pVRes[2]=v[2];
	}
}

float vecLength( float *pV )
{
	if(pV) return sqrt(pV[0]*pV[0]+pV[1]*pV[1]+pV[2]*pV[2]);
	return 0.0f;
}

float vecNormalise( float *pV0, float *pVRes )
{
	if(pV0 && pVRes)
	{
		float f=sqrt(pV0[0]*pV0[0]+pV0[1]*pV0[1]+pV0[2]*pV0[2]);
		
		pVRes[0]=pV0[0]/f;
		pVRes[1]=pV0[1]/f;
		pVRes[2]=pV0[2]/f;

		return f;
	}
	return 0.0f;
}

void vecProject( float *pVStart, float *pVDir, float fLen, float *pVRes )
{
	if(pVStart && pVDir && pVRes)
	{
		pVRes[0]=pVStart[0]+pVDir[0]*fLen;
		pVRes[1]=pVStart[1]+pVDir[1]*fLen;
		pVRes[2]=pVStart[2]+pVDir[2]*fLen;
	}
}

void vecSet( float fX, float fY, float fZ, float *pV )
{
	if(pV)
	{
		pV[0]=fX;
		pV[1]=fY;
		pV[2]=fZ;
	}
}

void vecCopy( float *pIn, float *pOut )
{
	if(pIn && pOut) for(int i=0;i<4;i++) pOut[i]=pIn[i];
}

float vecDistance( float *pV1, float *pV2 )
{
	float fLen=0.0f;

	if(pV1 && pV2)
	{
		float av[4];

		vecSub(pV2, pV1, av);
		fLen=vecLength(av);
	}

	return fLen;
}

void vecSlerp( float *pVStart, float *pVEnd, float fSlerp, float *pV )
{
	if(pVStart && pVEnd && pV)
	{
		pV[0]=pVStart[0]+(((pVEnd[0]-pVStart[0]))*fSlerp);
		pV[1]=pVStart[1]+(((pVEnd[1]-pVStart[1]))*fSlerp);
		pV[2]=pVStart[2]+(((pVEnd[2]-pVStart[2]))*fSlerp);

		pV[3]=pVStart[3]*pVEnd[3];
	}
}

void vecInitCVec( float *pV )
{
	if(pV)
	{
		pV[0]=randFloat();
		pV[1]=randFloat();
		pV[2]=randFloat();
		pV[3]=1.0f;
	}
}
