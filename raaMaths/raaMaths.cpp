#include "StdAfx.h"

#include <stdlib.h>
#include <time.h>

#include "raaMaths.h"

float degToRad( float f )
{
	return (f/180.0f)*cs_fPi;
}

float redToDeg( float f )
{
	return (f/cs_fPi)*180.0f;
}

void initMaths()
{
	if(!s_bMathsInit)
	{
		srand(time(0));
		s_bMathsInit=true;
	}
}

float randFloat( float fMin/*=1.0f*/, float fMax/*=1.0f*/ )
{
//	initMaths();

	float fRand=(float)rand();

	fRand/=(float)RAND_MAX;
	fRand*=fMax-fMin;
	fRand+=fMin;

	return fRand;
}
