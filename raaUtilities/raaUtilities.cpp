#include "StdAfx.h"

#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/GLUT.h>

#include "raaUtilities.h"

float g_afSphereAmbient[]={0.1f, 0.3f, 0.1f, 1.0f}; //the ambient colour of the sphere
float g_afSphereDiffuse[]={0.1f, 0.8f, 0.1f, 1.0f}; //the diffuse colour of the spere
float g_afSphereSpecular[]={1.0f, 1.0f, 1.0f, 1.0f}; //the specular colour of the sphere
float g_fSphereShininess=5.0f; //the shininess of the sphere


void gridInit(unsigned long &ulGrid, float *pafColour, int iMin, int iMax, float fCellSize)
{
	float fMin=(float)iMin;
	float fMax=(float)iMax;

	if(!ulGrid) ulGrid=glGenLists(1);

	glNewList(ulGrid, GL_COMPILE);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);

	glColor4fv(pafColour);

	glBegin(GL_LINES);
	for(int i=iMin;i<=iMax;i++)
	{
		glVertex3f(((float)i)*fCellSize, 0.0f, fMin*fCellSize);	
		glVertex3f(((float)i)*fCellSize, 0.0f, fMax*fCellSize);	
		glVertex3f(fMin*fCellSize, 0.0f, ((float)i)*fCellSize);	
		glVertex3f(fMax*fCellSize, 0.0f, ((float)i)*fCellSize);	
	}
	glEnd();

	glPopAttrib();

	glEndList();
}

void gridDraw( unsigned long &ulGrid )
{
	glCallList(ulGrid);
}

void drawSphere( float fRadius, int iSlices, int iSegments )
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, g_afSphereAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_afSphereDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, g_afSphereSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, g_fSphereShininess);

	glutSolidSphere(fRadius, iSlices, iSegments);
	glPopAttrib();
	glPopMatrix();
}
