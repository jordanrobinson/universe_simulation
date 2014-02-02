#include "StdAfx.h"
#include <math.h>

#include "raaMatrix.h"

// 0   1     2   3
// 4   5     6   7
// 8   9    10 11
// 12 13 14 15


void matMakeIdentity( float *pM )
{
	if(pM)
	{
		pM[0]=pM[5]=pM[10]=pM[15]=1.0f;
		pM[1]=pM[2]=pM[3]=pM[4]=pM[6]=pM[7]=pM[8]=pM[9]=pM[11]=pM[12]=pM[13]=pM[14]=0.0f;
	}
}

void matMakeTranslation( float fX, float fY, float fZ, float *pM )
{
	if(pM)
	{
		pM[0]=pM[5]=pM[10]=pM[15]=1.0f;
		pM[1]=pM[2]=pM[3]=pM[4]=pM[6]=pM[7]=pM[8]=pM[9]=pM[11]=0.0f;
		pM[12]=fX;
		pM[13]=fY;
		pM[14]=fZ;
	}
}

void matMakeScale( float fX, float fY, float fZ, float *pM )
{
	if(pM)
	{
		pM[0]=fX;
		pM[5]=fY;
		pM[10]=fZ;
		pM[15]=1.0f;
		pM[1]=pM[2]=pM[3]=pM[4]=pM[6]=pM[7]=pM[8]=pM[9]=pM[11]=pM[12]=pM[13]=pM[14]=0.0f;
	}
}

void matMakeXAxisRot( float fRadians, float *pM )
{
	if(pM)
	{
		float c=cos(fRadians);
		float s=sin(fRadians);

		pM[0]=pM[15]=1.0f;
		pM[5]=pM[10]=c;
		pM[6]=s;
		pM[9]=-s;
		pM[1]=pM[2]=pM[3]=pM[4]=pM[7]=pM[8]=pM[11]=pM[12]=pM[13]=pM[14]=0.0f;
	}
}

void matMakeYAxisRot( float fRadians, float *pM )
{
	if(pM)
	{
		float c=cos(fRadians);
		float s=sin(fRadians);

		pM[0]=pM[10]=c;
		pM[5]=pM[15]=1.0f;
		pM[1]=pM[3]=pM[4]=pM[6]=pM[7]=pM[9]=pM[11]=pM[12]=pM[13]=pM[14]=0.0f;
		pM[2]=s;
		pM[8]=-s;
	}
}

void matMakeZAxisRot( float fRadians, float *pM )
{
	if(pM)
	{
		float c=cos(fRadians);
		float s=sin(fRadians);

		pM[0]=pM[5]=c;
		pM[10]=pM[15]=1.0f;
		pM[1]=s;
		pM[4]=-s;
		pM[2]=pM[3]=pM[6]=pM[7]=pM[8]=pM[9]=pM[11]=pM[12]=pM[13]=pM[14]=0.0f;
	}
}

void matVecMult( float *pV, float *pM, float *pVRes )
{
	if(pV && pM && pVRes)
	{
		float v[4];

		v[0]=pV[0]*pM[0]+pV[1]*pM[4]+pV[2]*pM[8]+pV[3]*pM[12];
		v[1]=pV[0]*pM[1]+pV[1]*pM[5]+pV[2]*pM[9]+pV[3]*pM[13];
		v[2]=pV[0]*pM[2]+pV[1]*pM[6]+pV[2]*pM[10]+pV[3]*pM[14];
		v[3]=pV[3];

		pVRes[0]=v[0];
		pVRes[1]=v[1];
		pVRes[2]=v[2];
		pVRes[3]=v[3];
	}
}

void matMatMult( float *pM0, float *pM1, float *pMRes )
{
	if(pM0 && pM1 && pMRes)
	{
		float m[16];

		m[0]=pM0[0]*pM1[0]+pM0[1]*pM1[4]+pM0[2]*pM1[8]+pM0[3]*pM1[12];
		m[1]=pM0[0]*pM1[1]+pM0[1]*pM1[5]+pM0[2]*pM1[9]+pM0[3]*pM1[13];
		m[2]=pM0[0]*pM1[2]+pM0[1]*pM1[6]+pM0[2]*pM1[10]+pM0[3]*pM1[14];
		m[3]=pM0[0]*pM1[3]+pM0[1]*pM1[7]+pM0[2]*pM1[11]+pM0[3]*pM1[15];

		m[4]=pM0[4]*pM1[0]+pM0[5]*pM1[4]+pM0[6]*pM1[8]+pM0[7]*pM1[12];
		m[5]=pM0[4]*pM1[1]+pM0[5]*pM1[5]+pM0[6]*pM1[9]+pM0[7]*pM1[13];
		m[6]=pM0[4]*pM1[2]+pM0[5]*pM1[6]+pM0[6]*pM1[10]+pM0[7]*pM1[14];
		m[7]=pM0[4]*pM1[3]+pM0[5]*pM1[7]+pM0[6]*pM1[11]+pM0[7]*pM1[15];

		m[8]=pM0[8]*pM1[0]+pM0[9]*pM1[4]+pM0[10]*pM1[8]+pM0[11]*pM1[12];
		m[9]=pM0[8]*pM1[1]+pM0[9]*pM1[5]+pM0[10]*pM1[9]+pM0[11]*pM1[13];
		m[10]=pM0[8]*pM1[2]+pM0[9]*pM1[6]+pM0[10]*pM1[10]+pM0[11]*pM1[14];
		m[11]=pM0[8]*pM1[3]+pM0[9]*pM1[7]+pM0[10]*pM1[11]+pM0[11]*pM1[15];

		m[12]=pM0[12]*pM1[0]+pM0[13]*pM1[4]+pM0[14]*pM1[8]+pM0[15]*pM1[12];
		m[13]=pM0[12]*pM1[1]+pM0[13]*pM1[5]+pM0[14]*pM1[9]+pM0[15]*pM1[13];
		m[14]=pM0[12]*pM1[2]+pM0[13]*pM1[6]+pM0[14]*pM1[10]+pM0[15]*pM1[14];
		m[15]=pM0[12]*pM1[3]+pM0[13]*pM1[7]+pM0[14]*pM1[11]+pM0[15]*pM1[15];

		pMRes[0]=m[0];
		pMRes[1]=m[1];
		pMRes[2]=m[2];
		pMRes[3]=m[3];
		pMRes[4]=m[4];
		pMRes[5]=m[5];
		pMRes[6]=m[6];
		pMRes[7]=m[7];
		pMRes[8]=m[8];
		pMRes[9]=m[9];
		pMRes[10]=m[10];
		pMRes[11]=m[11];
		pMRes[12]=m[12];
		pMRes[13]=m[13];
		pMRes[14]=m[14];
		pMRes[15]=m[15];
	}
}

void matTranslate( float fX, float fY, float fZ, float *pM )
{
	float m[16];
	matMakeTranslation(fX, fY, fZ, m);
	matMatMult(m, pM, pM);
}

void matScale( float fX, float fY, float fZ, float *pM )
{
	float m[16];
	matMakeScale(fX, fY, fZ, m);
	matMatMult(m, pM, pM);
}

void matXAxisRot( float fRadians, float *pM )
{
	float m[16];
	matMakeXAxisRot(fRadians, m);
	matMatMult(m, pM, pM);
}

void matYAxisRot( float fRadians, float *pM )
{
	float m[16];
	matMakeYAxisRot(fRadians, m);
	matMatMult(m, pM, pM);
}

void matZAxisRot( float fRadians, float *pM )
{
	float m[16];
	matMakeZAxisRot(fRadians, m);
	matMatMult(m, pM, pM);
}