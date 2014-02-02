#pragma once
#pragma comment(lib,"raaMaths")

typedef float raaMatrix4[16];
typedef float raaVector4[4];

const static float cs_fPi=3.14159265f;

static bool s_bMathsInit=false;

void initMaths();

float degToRad(float f);
float redToDeg(float f);
float randFloat(float fMin=0.0f, float fMax=1.0f);