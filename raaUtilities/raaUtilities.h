#pragma once
#pragma comment(lib,"raaUtilities")

// grid
void gridInit(unsigned long &ulGrid, float *pafColour, int iMin=-50, int iMax=50, float fCellSize=1.0f);
void gridDraw(unsigned long &ulGrid);
void drawSphere(float fRadius, int iSlices, int iSegments);