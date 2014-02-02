#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <iostream>
#include <soil/soil.h>
#include <mmsystem.h>

#include "raaCamera/raaCamera.h"
#include "raaUtilities/raaUtilities.h"
#include "raaMaths/raaMaths.h"
#include "raaMaths/raaVector.h"
#include "raaMaths/raaMatrix.h"
#include "jrLinkedList.h"

//options
#define PAUSE 1
#define PLAY 2
#define RESET 3
#define LONG_TRAILS 4
#define SHORT_TRAILS 5
#define TRAILS_OFF 6
#define TRAILS_ON 7
#define INCREASE_FPS 8
#define DECREASE_FPS 9
#define SAVE 10
#define LOAD 11
#define INCREASE_POPCAP 12
#define DECREASE_POPCAP 13
#define TEXTURES_ON 14
#define TEXTURES_OFF 15
#define PARTICLES_ON 16
#define PARTICLES_OFF 17
#define INCREASE_VELOCITY 18
#define DECREASE_VELOCITY 19
#define REVERSE_VELOCITY 20

//following planet options
#define INCREASE_SIZE 21
#define DECREASE_SIZE 22
#define REMOVE_VELOCITY 23
#define CHANGE_COLOUR 24
#define STOP_FOLLOWING 25

//object types
#define SPHERE 100
#define CUBE 101
#define CONE 102
#define TEAPOT 103
#define VERY_LARGE_SPHERE 104
#define COMET 105

//special planets
#define SPECIAL 199
#define SUN 200
#define EARTH 201
#define MOON 202
#define MARS 203
#define DEATH_STAR 204

raaCameraInput g_Input;
raaCamera g_Camera;
jrPlanet* g_pFollowing = 0;

unsigned long g_ulGrid=0;

int g_iFrame = 0;
int g_iTime = 0;
int g_iTimebase = 0;
int g_iMaintainPlanetNumber = 10;
int g_iHistoryVariableLength = iHistoryMax / 3;
const int g_uiMaxFPS = 60;
const int g_iTextureNumber = 5;

unsigned int g_uiNextTick = 0;

float g_fFPSMultiplier = 1.0f;
float g_fVelocityMultiplier = 1.0f;
float g_af_SphereSpecular[]={1.0f, 1.0f, 1.0f, 1.0f}; //the specular colour of the spheres
float g_f_SphereShininess=5.0f; //the shininess of the spheres
float g_fDamping = 0.999f;
float g_fGravity = 0.98f;

bool g_bExplore = false;
bool g_bFly = false;
bool g_bMaintainPlanets = false;
bool g_bPlaying = true;
bool g_bTrailsOn = true;
bool g_bPicking = false;
bool g_bTexturesOn = true;
bool g_bParticlesOn = true;

char fpsString[50];
const char * g_pcTextureNames[]={"textures/sunmap.jpg", "textures/earthmap.jpg",
	"textures/moonmap.jpg", "textures/marsmap.jpg", "textures/deathstarmap.jpg"};

GLuint g_uiTexture[g_iTextureNumber];
GLuint g_uiBuffer[512];

void keyboard(unsigned char c, int iXPos, int iYPos);
void keyboardUp(unsigned char c, int iXPos, int iYPos);
void sKeyboard(int iC, int iXPos, int iYPos);
void sKeyboardUp(int iC, int iXPos, int iYPos);

void mouse(int iKey, int iEvent, int iXPos, int iYPos);
void mouseMotion();
void createMenu();
void rightClickMenu(int iCommand);

void display();
void idle();
void reshape(int iWidth, int iHeight);
void myInit();

void applyTransitions(jrPlanet *pFirstPlanet);
bool detectCollision(jrPlanet *pFirstPlanet, jrPlanet *pSecondPlanet, float afDistance[3]);
int getNumberOfPlanets();
void maintainPlanetNumber();
void reset();

void drawFPS();

void picking(int iXPos, int iYPos);
void finishPicking();
void resetFollow();

bool loadTextures();
void drawTexturedSphere(jrPlanet *pPlanet);
void applyParticles(jrPlanet *pPlanet);

void saveFile();
void loadFile();

void processList();
void drawPlanet(jrPlanet* planet);
void drawTrail(jrPlanet* planet);
void resetTrails();


//this is where the actual drawing happens
void display() {

	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	camApply(g_Camera);

	drawFPS();

	processList();

	glFlush();

	if (!g_bPicking) {
		glutSwapBuffers();
	}
	else {
		finishPicking();
	}
}

void idle()
{

	g_iFrame++;
	g_iTime = glutGet(GLUT_ELAPSED_TIME);

	if (g_iTime - g_iTimebase > 1000) {
		sprintf_s(fpsString, "FPS:%4.2f\n Planets: %i Time: %i", g_iFrame * 1000.0 / (g_iTime - g_iTimebase), getNumberOfPlanets(), g_iTime / 1000);
		g_iTimebase = g_iTime;
		g_iFrame = 0;
	}

	if (g_bPlaying) {
		for(jrPlanet *pPlanet=g_pHead; pPlanet; pPlanet=pPlanet->m_pNext) {
			applyTransitions(pPlanet);
		}
	}

	if (g_bMaintainPlanets) {
		maintainPlanetNumber();
	}

	g_uiNextTick += 1000 / (g_uiMaxFPS * g_fFPSMultiplier);
	int iSleepTime = g_uiNextTick - glutGet(GLUT_ELAPSED_TIME);
	if (iSleepTime < -1000) {
		g_uiNextTick = glutGet(GLUT_ELAPSED_TIME); //we don't want the simulation getting faster, even if we lose time
	}

	if (iSleepTime >= 0) {
		Sleep(iSleepTime);
	}

	mouseMotion();
	glutPostRedisplay();
}

void applyTransitions(jrPlanet *pPlanet) {
	if (pPlanet) {
		float afForce[4];
		vecInitDVec(afForce);

		for (jrPlanet *pOtherPlanet = g_pHead; pOtherPlanet; pOtherPlanet = pOtherPlanet->m_pNext) {

			if (pPlanet != pOtherPlanet && pOtherPlanet)  {
				float afDistance[3];
				vecSub(pPlanet->afPosition, pOtherPlanet->afPosition, afDistance); //get distance

				float fForce = g_fGravity * (pPlanet->fMass * pOtherPlanet->fMass) / (vecLength(afDistance));

				float afNormaliseInput[4];
				vecSub(pOtherPlanet->afPosition, pPlanet->afPosition, afNormaliseInput);
				float afNormaliseOutput[4];
				vecInit(afNormaliseOutput);

				vecNormalise(afNormaliseInput, afNormaliseOutput);

				afForce[0] = (afForce[0] + afNormaliseOutput[0]);
				afForce[1] = (afForce[1] + afNormaliseOutput[1]);
				afForce[2] = (afForce[2] + afNormaliseOutput[2]);

				if (pPlanet->fMass > pOtherPlanet->fMass) {
					if (detectCollision(pPlanet, pOtherPlanet, afDistance)) {
						break;
					}
				}
			}
		}

		// calc accV -> afForce/mass
		pPlanet->afAcceleration[0] = afForce[0] / pPlanet->fMass;
		pPlanet->afAcceleration[1] = afForce[1] / pPlanet->fMass;
		pPlanet->afAcceleration[2] = afForce[2] / pPlanet->fMass;

		// calc new pos
		if (!pPlanet->m_bFixed) {
			pPlanet->afPosition[0] = pPlanet->afPosition[0] + (pPlanet->afVelocity[0] * g_fVelocityMultiplier);
			pPlanet->afPosition[1] = pPlanet->afPosition[1] + (pPlanet->afVelocity[1] * g_fVelocityMultiplier);
			pPlanet->afPosition[2] = pPlanet->afPosition[2] + (pPlanet->afVelocity[2] * g_fVelocityMultiplier);
		}
		// calc new vel
		pPlanet->afVelocity[0] = pPlanet->afVelocity[0] + pPlanet->afAcceleration[0];
		pPlanet->afVelocity[1] = pPlanet->afVelocity[1] + pPlanet->afAcceleration[1];
		pPlanet->afVelocity[2] = pPlanet->afVelocity[2] + pPlanet->afAcceleration[2];

		// apply drag
		pPlanet->afAcceleration[0] = pPlanet->afAcceleration[0] * g_fDamping;
		pPlanet->afAcceleration[1] = pPlanet->afAcceleration[1] * g_fDamping;
		pPlanet->afAcceleration[2] = pPlanet->afAcceleration[2] * g_fDamping;

		// end

		if (pPlanet->iHistoryCount < g_iHistoryVariableLength - 4) {
			pPlanet->iHistoryCount = pPlanet->iHistoryCount + 3;
		}
		else {
			pPlanet->iHistoryCount = 0;
		}

		pPlanet->afPositionHistory[pPlanet->iHistoryCount] = pPlanet->afPosition[0];
		pPlanet->afPositionHistory[pPlanet->iHistoryCount + 1] = pPlanet->afPosition[1];
		pPlanet->afPositionHistory[pPlanet->iHistoryCount + 2] = pPlanet->afPosition[2];

		if (pPlanet->fRotationAngle > 359.0f) {
			pPlanet->fRotationAngle = 0.0f;
		}
		else {
			pPlanet->fRotationAngle = pPlanet->fRotationAngle += 1.0f;
		}
	}

}

bool detectCollision(jrPlanet *pFirstPlanet, jrPlanet *pSecondPlanet, float afDistance[3]) {

	float afSimplifiedDistance[3];
	vecInit(afSimplifiedDistance); //we can simplify the distance to not include negative values

	for (int i = 0; i < 3; i++) {
		if (afDistance[i] < 0.0f) {
			afSimplifiedDistance[i]= afDistance[i] * -1;
		}
		else {
			afSimplifiedDistance[i] = afDistance[i];
		}
	}

	bool bCollideX = afSimplifiedDistance[0] < pFirstPlanet->fSize;
	bool bCollideY = afSimplifiedDistance[1] < pFirstPlanet->fSize;
	bool bCollideZ = afSimplifiedDistance[2] < pFirstPlanet->fSize;

	if (bCollideX && bCollideY && bCollideZ) {
		pFirstPlanet->fSize = pFirstPlanet->fSize + (pSecondPlanet->fSize / 2);
		pFirstPlanet->fMass = pFirstPlanet->fMass + (pSecondPlanet->fMass / 2);

		if (pSecondPlanet == g_pFollowing) {
			resetFollow();
		}
		deleteElement(pSecondPlanet);
		pSecondPlanet = 0;
		return true;
	}
	return false;
}

void reshape(int iWidth, int iHeight)
{
	glViewport(0, 0, iWidth, iHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0f, ((float)iWidth)/((float)iHeight), 1000.1f, 1000000.0f);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void keyboard(unsigned char c, int iXPos, int iYPos)
{
	switch(c)
	{
	case 'w':
		camInputTravel(g_Input, tri_pos);
		break;
	case 's':
		camInputTravel(g_Input, tri_neg);
		break;
	case 'a':
		pushTail(seedPlanetValues(createNewElement()));
		break;
	case 'd': {
		jrPlanet* pDel = popTail();

		if (pDel == g_pFollowing) {
			resetFollow();
		}

		destroy(pDel);
		break; }
	case 'q':
		g_bMaintainPlanets = !g_bMaintainPlanets;
		break;

	}
}

void keyboardUp(unsigned char c, int iXPos, int iYPos)
{
	switch(c)
	{

	case 'w':
	case 's':
		camInputTravel(g_Input, tri_null);
		break;
	case 'f':
		camInputFly(g_Input, !g_Input.m_bFly);
		break;
	}
}

void sKeyboard(int iC, int iXPos, int iYPos)
{
	switch(iC)
	{
	case GLUT_KEY_UP:
		camInputTravel(g_Input, tri_pos);
		break;
	case GLUT_KEY_DOWN:
		camInputTravel(g_Input, tri_neg);
		break;
	}
}

void sKeyboardUp(int iC, int iXPos, int iYPos)
{
	switch(iC)
	{
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		camInputTravel(g_Input, tri_null);
		break;
	}
}

void rightClickMenu(int iCommand) {


	switch (iCommand) { //change to constants

	case PAUSE:
		g_bPlaying = false;
		break;
	case PLAY:
		g_bPlaying = true;
		break;
	case RESET:
		reset();
		break;
	case LONG_TRAILS: {
		resetTrails();
		g_iHistoryVariableLength = iHistoryMax;
		break; }
	case SHORT_TRAILS: {
		resetTrails();
		g_iHistoryVariableLength = g_iHistoryVariableLength / 3;

		if (g_iHistoryVariableLength < 300) {
			g_iHistoryVariableLength = 300; //with alpha, any shorter and they are basically off
		}

		break; }
	case TRAILS_OFF:
		g_bTrailsOn = false;
		break;
	case TRAILS_ON:
		g_bTrailsOn = true;
		break;
	case INCREASE_FPS:
		g_fFPSMultiplier += 0.2f;
		break;
	case DECREASE_FPS: {
		g_fFPSMultiplier -= 0.2f;

		if (g_fFPSMultiplier < 0.2f) { //stops it from having 0 fps, which currently would cause a crash
			g_fFPSMultiplier = 0.2f;
		}
		break; }
	case SAVE:
		saveFile();
		break;
	case LOAD:
		loadFile();
		break;
	case INCREASE_POPCAP:
		g_iMaintainPlanetNumber = g_iMaintainPlanetNumber * 2;
		break;
	case DECREASE_POPCAP:
		g_iMaintainPlanetNumber = g_iMaintainPlanetNumber / 2;
		break;
	case TEXTURES_ON:
		g_bTexturesOn = true;
		break;
	case TEXTURES_OFF:
		g_bTexturesOn = false;
		break;
	case PARTICLES_ON:
		g_bParticlesOn = true;
		break;
	case PARTICLES_OFF:
		g_bParticlesOn = false;
		break;
	case INCREASE_VELOCITY: 
		g_fVelocityMultiplier *= 2.0f;
		break;
	case DECREASE_VELOCITY:
		g_fVelocityMultiplier /= 2.0f;
		break; 
	case REVERSE_VELOCITY:
		g_fVelocityMultiplier *= -1.0f;
		break;
	case INCREASE_SIZE: {
		if (g_pFollowing) {
			g_pFollowing->fMass *= 2.0f;
			g_pFollowing->fSize *= 2.0f;
			g_pFollowing->iSlices += 5;
			g_pFollowing->iSegments += 5;
		}
		break; }
	case DECREASE_SIZE: {
		if (g_pFollowing) {
			g_pFollowing->fMass /= 2.0f;
			g_pFollowing->fSize /= 2.0f;
			g_pFollowing->iSlices = 20;
			g_pFollowing->iSegments = 20;
		}
		break; }
	case REMOVE_VELOCITY: {
		if (g_pFollowing) {
			vecInitDVec(g_pFollowing->afVelocity);
			vecInitDVec(g_pFollowing->afAcceleration);
		}
		break; }
	case CHANGE_COLOUR: {
		if (g_pFollowing) {
			g_pFollowing->afDiffuse[0] = randFloat(0.3f, 0.8f);
			g_pFollowing->afDiffuse[1] = randFloat(0.3f, 0.8f);
			g_pFollowing->afDiffuse[2] = randFloat(0.3f, 0.8f);
			g_pFollowing->afAmbient[0] = g_pFollowing->afDiffuse[0] / 2.0f;
			g_pFollowing->afAmbient[1] = g_pFollowing->afDiffuse[1] / 2.0f;
			g_pFollowing->afAmbient[2] = g_pFollowing->afDiffuse[2] / 2.0f;
		}
		break; }
	case STOP_FOLLOWING: 
		resetFollow();
		break;
	case SPHERE:
		pushTail(seedPlanetValues(createNewElement()));
		break;
	case CUBE:
		pushTail(seedCubeValues(createNewElement()));
		break;
	case CONE:
		pushTail(seedConeValues(createNewElement()));
		break;
	case TEAPOT:
		pushTail(seedTeapotValues(createNewElement()));
		break;
	case VERY_LARGE_SPHERE: {
		jrPlanet* pPlanet = seedPlanetValues(createNewElement()); 
		pPlanet->fSize = randFloat(50.0f, 100.0f);
		pPlanet->fMass = pPlanet->fSize * 20;
		pPlanet->iSegments = 30;
		pPlanet->iSlices = 30;
		pushTail(pPlanet);
		break; }
	case COMET: {
		jrPlanet* pPlanet = seedPlanetValues(createNewElement()); 
		pPlanet->fSize = pPlanet->fSize / 2;
		pPlanet->fMass = pPlanet->fSize * 20;
		pPlanet->m_bParticles = true;
		pushTail(pPlanet);
		break; }
	case SUN: {
		jrPlanet* pSun = initSun(createNewElement());
		pSun->afPosition[0] = randFloat(-3000.0f, 3000.0f);
		pSun->afPosition[1] = randFloat(-3000.0f, 3000.0f);
		pSun->afPosition[2] = randFloat(-3000.0f, 3000.0f);
		pushTail(pSun);
		break; }
	case EARTH: {
		jrPlanet* pPlanet = seedPlanetValues(createNewElement());
		pPlanet->fMass *= 1.5f;
		pPlanet->fSize *= 1.5f;

		pPlanet->iSegments = 20;
		pPlanet->iSlices = 20;

		pPlanet->afDiffuse[0] = 0.2f;
		pPlanet->afDiffuse[1] = 0.4f;
		pPlanet->afDiffuse[2] = 0.8f;

		pPlanet->afAmbient[0] = pPlanet->afDiffuse[0] / 2.0f;
		pPlanet->afAmbient[1] = pPlanet->afDiffuse[0] / 2.0f;
		pPlanet->afAmbient[2] = pPlanet->afDiffuse[0] / 2.0f;

		pPlanet->iType = EARTH;
		pushTail(pPlanet);
		break; }
	case MOON: {
		jrPlanet* pPlanet = seedPlanetValues(createNewElement());
		pPlanet->fMass *= 1.5f;
		pPlanet->fSize *= 1.5f;

		pPlanet->iSegments = 20;
		pPlanet->iSlices = 20;

		pPlanet->afDiffuse[0] = 0.8f;
		pPlanet->afDiffuse[1] = 0.8f;
		pPlanet->afDiffuse[2] = 0.8f;

		pPlanet->afAmbient[0] = pPlanet->afDiffuse[0] / 2.0f;
		pPlanet->afAmbient[1] = pPlanet->afDiffuse[0] / 2.0f;
		pPlanet->afAmbient[2] = pPlanet->afDiffuse[0] / 2.0f;

		pPlanet->iType = MOON;
		pushTail(pPlanet);
		break; }
	case MARS: {
		jrPlanet* pPlanet = seedPlanetValues(createNewElement());
		pPlanet->fMass *= 1.5f;
		pPlanet->fSize *= 1.5f;

		pPlanet->iSegments = 20;
		pPlanet->iSlices = 20;

		pPlanet->afDiffuse[0] = 0.8f;
		pPlanet->afDiffuse[1] = 0.3f;
		pPlanet->afDiffuse[2] = 0.3f;

		pPlanet->afAmbient[0] = pPlanet->afDiffuse[0] / 2.0f;
		pPlanet->afAmbient[1] = pPlanet->afDiffuse[0] / 2.0f;
		pPlanet->afAmbient[2] = pPlanet->afDiffuse[0] / 2.0f;

		pPlanet->iType = MARS;
		pushTail(pPlanet);
		break; }
	case DEATH_STAR: {
		jrPlanet* pPlanet = seedPlanetValues(createNewElement());
		pPlanet->fMass *= 1.5f;
		pPlanet->fSize *= 1.5f;

		pPlanet->iSegments = 20;
		pPlanet->iSlices = 20;

		pPlanet->afDiffuse[0] = 0.2f;
		pPlanet->afDiffuse[1] = 0.2f;
		pPlanet->afDiffuse[2] = 0.2f;

		pPlanet->afAmbient[0] = pPlanet->afDiffuse[0] / 2.0f;
		pPlanet->afAmbient[1] = pPlanet->afDiffuse[0] / 2.0f;
		pPlanet->afAmbient[2] = pPlanet->afDiffuse[0] / 2.0f;

		pPlanet->iType = DEATH_STAR;
		pushTail(pPlanet);
		break; }
	}
}

void mouse(int iKey, int iEvent, int iXPos, int iYPos)
{
	if (iKey==GLUT_LEFT_BUTTON && iEvent==GLUT_DOWN)	{
		camInputMouse(g_Input, true);
		camInputSetMouseStart(g_Input, iXPos, iYPos);
		picking(iXPos, iYPos);
		g_bPicking = true;
	}
	else if (iKey==GLUT_LEFT_BUTTON && iEvent==GLUT_UP) {
		camInputMouse(g_Input, false);
	}

}

void picking(int iXPos, int iYPos) {
	float fRatio;
	GLint viewport[4];

	for (int i = 0; i < 512; i++) {
		g_uiBuffer[i] = 0;
	}

	glSelectBuffer(512, g_uiBuffer);
	glGetIntegerv(GL_VIEWPORT, viewport);

	glRenderMode(GL_SELECT);
	glInitNames();
	g_bPicking = true;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	int delX = 16;
	int delY = 16;

	gluPickMatrix(iXPos, viewport[3] - iYPos, delX, delY, viewport);
	fRatio = ((float) viewport[2] / (float) viewport[3]);
	gluPerspective(30.0f, fRatio, 0.1f, 50000.0f);
	glMatrixMode(GL_MODELVIEW);
}

void finishPicking() {
	GLint hits;

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	hits = glRenderMode(GL_RENDER);

	bool bFoundHit = false;
	for (int i = 0; i < 512; i++) {
		if (g_uiBuffer[i] == 1) { //found something that has one name
			i++; //skip the depth
			i++;
			i++;

			resetFollow(); //reset the following to stop us getting into weird situations

			g_pFollowing = (jrPlanet*) g_uiBuffer[i];
			bFoundHit = true;
			if (g_pFollowing->m_bFixed) { //if it's fixed, there's no point following
				bFoundHit = false; //so we didn't really find anything
			}

			if (bFoundHit) { //we want the closest thing to the user, so break out to save time
				break;
			}
		}
	}

	if (!bFoundHit && g_pFollowing != 0) { //we are following something and we didn't find a new object to follow
		resetFollow();
	}

	g_bPicking = false;
}

void resetFollow() {
	g_pFollowing = 0;
	camInputFly(g_Input, false);
	camInit(g_Camera);
	camInputInit(g_Input);
	camInputExplore(g_Input, true);
}

void resetTrails() {


	for(jrPlanet *pPlanet=g_pHead; pPlanet; pPlanet=pPlanet->m_pNext) {
		pPlanet->iHistoryCount = 0;

		for (int i = 0; i < iHistoryMax; i++) {
			pPlanet->afPositionHistory[i] = pPlanet->afPosition[0];
			i++;
			pPlanet->afPositionHistory[i] = pPlanet->afPosition[1];
			i++;
			pPlanet->afPositionHistory[i] = pPlanet->afPosition[2];
		}
	}
}

void motion(int iXPos, int iYPos)
{
	if(g_Input.m_bMouse)
	{
		camInputSetMouseLast(g_Input, iXPos, iYPos);
	}
}

void mouseMotion()
{
	camProcessInput(g_Input, g_Camera);
	glutPostRedisplay();
}

void myInit()
{
	initMaths();
	camInit(g_Camera);
	camInputInit(g_Input);
	camInputExplore(g_Input, true);

	glClearColor(0.01f, 0.01f, 0.01f, 0.0f); //sets the colour values to use when clearing the colours, essentially sets the canvas colour
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0); //sets the colour of the light (0 == white)
	glEnable(GL_LIGHTING); //allows GL to make lighting calculations
	glEnableClientState(GL_VERTEX_ARRAY);

	pushTail(initSun(createNewElement())); //init sun

	loadTextures();
}

void followPlanet(jrPlanet *pPlanet) {
	camSet(pPlanet->afPosition[0], pPlanet->afPosition[1], pPlanet->afPosition[2] + 6000.0f, g_Camera);
}

void drawPlanet(jrPlanet *pPlanet) {

	glPushMatrix(); //begin drawing
	glPushName((unsigned int) pPlanet);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (pPlanet->iType < SUN || !g_bTexturesOn) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pPlanet->afAmbient); //set the materials of the planet
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pPlanet->afDiffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, g_af_SphereSpecular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, g_f_SphereShininess);
	}

	glTranslatef(pPlanet->afPosition[0], pPlanet->afPosition[1], pPlanet->afPosition[2]); //move the position so we can draw at the correct point
	glRotatef(pPlanet->fRotationAngle, pPlanet->afPosition[0] - pPlanet->fSize, pPlanet->afPosition[1] - pPlanet->fSize, pPlanet->afPosition[2] - pPlanet->fSize);

	int iType = pPlanet->iType;

	if (!g_bTexturesOn && pPlanet->iType >= SUN) {
		iType = SPHERE;
	}
	else if (g_bTexturesOn && pPlanet->iType >= SUN) {
		iType = SPECIAL;
	}


	switch (iType) { //change to constants

	case SPHERE:
		glutSolidSphere(pPlanet->fSize, pPlanet->iSlices, pPlanet->iSegments); //draw the planet
		break;
	case CUBE:
		glutSolidCube(pPlanet->fSize * 2.0f);
		break;
	case CONE:
		glutSolidCone(pPlanet->fSize, pPlanet->fSize * 2.0f, pPlanet->iSlices, pPlanet->iSegments);
		break;
	case TEAPOT:
		glutSolidTeapot(pPlanet->fSize);
		break;
	case SPECIAL:
		drawTexturedSphere(pPlanet);
		break;
	}

	glPopName();
	glPopMatrix(); //end drawing

	if (g_bTrailsOn && !pPlanet->m_bFixed) {
		drawTrail(pPlanet);
	}

	glPopAttrib();

}

void drawTrail(jrPlanet *pPlanet) {

	glDisable(GL_LIGHTING); //lines can't be lit
	glEnable(GL_BLEND); //set up alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
	glEnableClientState(GL_COLOR_ARRAY);

	//because position history loops round, we need to feed LINE_STRIP a consistently ascending or descending array

	//init the array to actually draw
	float afDrawable[iHistoryMax] = {};

	//get the point just after the last recorded position
	int firstStart = pPlanet->iHistoryCount + 3;
	int firstCount = g_iHistoryVariableLength; //up till the end of the history

	//get the start of the array
	int secondStart = 0;
	int secondCount = pPlanet->iHistoryCount; //up to the last recorded position

	//create a new array from those points in the original array
	int iCurrentDrawablePoint = 0;
	for (int j = firstStart; j < firstCount; j++) {
		afDrawable[iCurrentDrawablePoint] = pPlanet->afPositionHistory[j];
		iCurrentDrawablePoint++;
	}

	for (int j = secondStart; j < secondCount; j++) {
		afDrawable[iCurrentDrawablePoint] = pPlanet->afPositionHistory[j];
		iCurrentDrawablePoint++;
	}

	//and then pass it to be drawn
	glVertexPointer(3, GL_FLOAT, 0, afDrawable);

	//we also calculate the alpha so that we can have a fading trail
	float afColour[(iHistoryMax + (iHistoryMax / 3))];

	float iAlpha = 0.0f;

	for (int i = 0; i < (iHistoryMax + (iHistoryMax / 3)); i++) {
		afColour[i] = pPlanet->afDiffuse[0];
		afColour[++i] = pPlanet->afDiffuse[1];
		afColour[++i] = pPlanet->afDiffuse[2];
		if (iAlpha > 0.7f) {
			afColour[++i] = 0.7f;
		}
		else {
			iAlpha += 0.001f;
			afColour[++i] = iAlpha;
		}
	}

	glColorPointer(4, GL_FLOAT, 0, afColour);

	//if there are particle effects for the planet, then we draw those instead of a trail
	if (g_bParticlesOn && pPlanet->m_bParticles) {

		float afPoints[iHistoryMax];

		for (int i = 0; i < iCurrentDrawablePoint; i++) {
			afPoints[i] = afDrawable[i] - randFloat(pPlanet->fSize/2* -1, pPlanet->fSize/2 + 10.0f);
			afPoints[++i] = afDrawable[i] + randFloat(pPlanet->fSize/2* -1, pPlanet->fSize/2 + 10.0f);
			afPoints[++i] = afDrawable[i] + randFloat(pPlanet->fSize/2 * -1, pPlanet->fSize/2 + 10.0f);
		}

		glVertexPointer(3, GL_FLOAT, 0, afPoints);

		glDrawArrays(GL_POINTS, 0, iCurrentDrawablePoint / 3);
	}
	else {
		glDrawArrays(GL_LINE_STRIP, 0, iCurrentDrawablePoint / 3);
	}

	glPopClientAttrib();
}

void drawTexturedSphere(jrPlanet *pPlanet) {

	GLUquadricObj* quadro = gluNewQuadric();							
	gluQuadricNormals(quadro, GLU_SMOOTH);		
	gluQuadricTexture(quadro, GL_TRUE);			
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();

	switch (pPlanet->iType) {

	case SUN: {
		glBindTexture(GL_TEXTURE_2D, g_uiTexture[0]);
		float af_SunEmissive[]={1.0f, 0.1f, 0.1f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, af_SunEmissive);
		break; }
	case EARTH:
		glBindTexture(GL_TEXTURE_2D, g_uiTexture[1]);
		break;
	case MOON:
		glBindTexture(GL_TEXTURE_2D, g_uiTexture[2]);
		break;
	case MARS:
		glBindTexture(GL_TEXTURE_2D, g_uiTexture[3]);
		break;
	case DEATH_STAR:
		glBindTexture(GL_TEXTURE_2D, g_uiTexture[4]);
		break;
	}

	gluSphere(quadro, pPlanet->fSize, pPlanet->iSlices, pPlanet->iSegments);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);

}

void drawFPS() {

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	glRasterPos2f(-0.98, 0.95f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	for (int i = 0; i < 50; i++) {
		if (fpsString[i] != '\0') {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, fpsString[i]);
		}
	}

	if (g_pFollowing != 0) {
		char detailString[50] = {};

		glRasterPos2f(-0.97, 0.91f);
		sprintf_s(detailString, "Selected Planet Details\0");

		for (int i = 0; i < 50; i++) {
			if (detailString[i] != '\0') {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, detailString[i]);
			}
		}

		glRasterPos2f(-0.97, 0.87f);
		sprintf_s(detailString, "Size: %f Mass: %f\0", g_pFollowing->fSize, g_pFollowing->fMass, g_pFollowing->fRotationAngle);
		for (int i = 0; i < 50; i++) {
			if (detailString[i] != '\0') {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, detailString[i]);
			}
		}


		glRasterPos2f(-0.97, 0.83f);
		sprintf_s(detailString, "Position: %2.4f %2.4f %2.4f\0", g_pFollowing->afPosition[0], g_pFollowing->afPosition[1], g_pFollowing->afPosition[2]);
		for (int i = 0; i < 50; i++) {
			if (detailString[i] != '\0') {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, detailString[i]);
			}
		}


		glRasterPos2f(-0.97, 0.79f);
		sprintf_s(detailString, "Acceleration: %2.4f %2.4f %2.4f\0", g_pFollowing->afAcceleration[0], g_pFollowing->afAcceleration[1], g_pFollowing->afAcceleration[2]);

		for (int i = 0; i < 50; i++) {
			if (detailString[i] != '\0') {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, detailString[i]);
			}
		}


		glRasterPos2f(-0.97, 0.75f);
		sprintf_s(detailString, "Velocity: %2.4f %2.4f %2.4f\0", g_pFollowing->afVelocity[0], g_pFollowing->afVelocity[1], g_pFollowing->afVelocity[2]);
		for (int i = 0; i < 50; i++) {
			if (detailString[i] != '\0') {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, detailString[i]);
			}
		}
	}

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

int getNumberOfPlanets() {
	int ret = 0;
	for(jrPlanet *pPlanet=g_pHead; pPlanet; pPlanet=pPlanet->m_pNext) {
		ret++;
	}
	return ret;
}

void maintainPlanetNumber() {
	if (getNumberOfPlanets() < g_iMaintainPlanetNumber) {
		pushTail(seedPlanetValues(createNewElement()));
	}
}

bool loadTextures() {

	for (int i = 0; i < g_iTextureNumber; i++) {
		g_uiTexture[i] = SOIL_load_OGL_texture(g_pcTextureNames[i], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		if (g_uiTexture[i] == 0) {
			return false;
		}
	}
	return true;
}

void processList() {

	for(jrPlanet *pPlanet=g_pHead; pPlanet; pPlanet=pPlanet->m_pNext) {
		drawPlanet(pPlanet);
	}

	if (g_pFollowing != 0) {
		camInputFly(g_Input, true);
		followPlanet(g_pFollowing);
	}
}

void reset() {
	resetFollow();
	while (g_pHead != 0) {
		destroy(popTail());
	}
	pushTail(initSun(createNewElement())); //init sun
}

void createMenu() {

	int iFileSubMenu = glutCreateMenu(rightClickMenu);
	glutAddMenuEntry("Save", SAVE);
	glutAddMenuEntry("Load", LOAD);

	int iTrailSubMenu = glutCreateMenu(rightClickMenu);
	glutAddMenuEntry("Long Trails", LONG_TRAILS);
	glutAddMenuEntry("Short Trails", SHORT_TRAILS);
	glutAddMenuEntry("Trails Off", TRAILS_OFF);
	glutAddMenuEntry("Trails On", TRAILS_ON);

	int iAddSubMenu = glutCreateMenu(rightClickMenu);
	glutAddMenuEntry("Sphere Planet", SPHERE);
	glutAddMenuEntry("Cube Planet", CUBE);
	glutAddMenuEntry("Conical Planet", CONE);
	glutAddMenuEntry("Teapot Planet", TEAPOT);
	glutAddMenuEntry("Very Large Planet", VERY_LARGE_SPHERE);
	glutAddMenuEntry("Comet", COMET);
	glutAddMenuEntry("Sun", SUN);

	int iSpecialPlanetSubSubMenu = glutCreateMenu(rightClickMenu);
	glutAddMenuEntry("Earth", EARTH);
	glutAddMenuEntry("Moon", MOON);
	glutAddMenuEntry("Mars", MARS);
	glutAddMenuEntry("Death Star", DEATH_STAR);

	int iPopulationSubMenu = glutCreateMenu(rightClickMenu);
	glutAddMenuEntry("Increase Managed Population", INCREASE_POPCAP);
	glutAddMenuEntry("Decrese Managed Population", DECREASE_POPCAP);

	int iTexturesSubMenu = glutCreateMenu(rightClickMenu);
	glutAddMenuEntry("Textures On", TEXTURES_ON);
	glutAddMenuEntry("Textures Off", TEXTURES_OFF);

	int iParticlesSubMenu = glutCreateMenu(rightClickMenu);
	glutAddMenuEntry("Particles On", PARTICLES_ON);
	glutAddMenuEntry("Particles Off", PARTICLES_OFF);

	int iSimulationMenu = glutCreateMenu(rightClickMenu);
	glutAddMenuEntry("Pause", PAUSE);
	glutAddMenuEntry("Play", PLAY);
	glutAddMenuEntry("Reset", RESET);
	glutAddMenuEntry("Increase Speed", INCREASE_VELOCITY);
	glutAddMenuEntry("Decrease Speed", DECREASE_VELOCITY);
	glutAddMenuEntry("Reverse Mode", REVERSE_VELOCITY);
	glutAddMenuEntry("Increase FPS", INCREASE_FPS);
	glutAddMenuEntry("Decrease FPS", DECREASE_FPS);

	int iFollowingMenu = glutCreateMenu(rightClickMenu);
	glutAddMenuEntry("Increase Size + Mass", INCREASE_SIZE);
	glutAddMenuEntry("Decrease Size + Mass", DECREASE_SIZE);
	glutAddMenuEntry("Remove All Velocity", REMOVE_VELOCITY);
	glutAddMenuEntry("Change Colour", CHANGE_COLOUR);
	glutAddMenuEntry("Stop Following", STOP_FOLLOWING);

	int menu;

	menu = glutCreateMenu(rightClickMenu);

	glutAddSubMenu("File", iFileSubMenu);
	glutAddSubMenu("Simulation", iSimulationMenu);
	glutAddSubMenu("Trails", iTrailSubMenu);
	glutAddSubMenu("Population", iPopulationSubMenu);
	glutAddSubMenu("Add New", iAddSubMenu);
	glutAddSubMenu("Textures", iTexturesSubMenu);
	glutAddSubMenu("Particles", iParticlesSubMenu);
	glutAddSubMenu("Following Planet", iFollowingMenu);

	glutSetMenu(iAddSubMenu);
	glutAddSubMenu("Special Planets", iSpecialPlanetSubSubMenu);

	glutSetMenu(menu);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void saveFile() {
	FILE *pOutFile;

	pOutFile = fopen("savefile.sav\0", "w");  // using absolute path name of file
	if (pOutFile != NULL) { 
		for(jrPlanet *pPlanet=g_pHead; pPlanet; pPlanet=pPlanet->m_pNext) {
			fprintf(pOutFile, "%f,", pPlanet->fSize);
			fprintf(pOutFile, "%f,", pPlanet->fMass);
			fprintf(pOutFile, "%f,", pPlanet->fRotationAngle);
			fprintf(pOutFile, "%f,%f,%f,%f,", pPlanet->afPosition[0], pPlanet->afPosition[1], pPlanet->afPosition[2], pPlanet->afPosition[3]);
			//position history not saved
			fprintf(pOutFile, "%f,%f,%f,%f,", pPlanet->afAmbient[0], pPlanet->afAmbient[1], pPlanet->afAmbient[2], pPlanet->afAmbient[3]);
			fprintf(pOutFile, "%f,%f,%f,%f,", pPlanet->afDiffuse[0], pPlanet->afDiffuse[1], pPlanet->afDiffuse[2], pPlanet->afDiffuse[3]);
			fprintf(pOutFile, "%f,%f,%f,%f,", pPlanet->afVelocity[0], pPlanet->afVelocity[1], pPlanet->afVelocity[2], pPlanet->afVelocity[3]);
			fprintf(pOutFile, "%f,%f,%f,%f,", pPlanet->afAcceleration[0], pPlanet->afAcceleration[1], pPlanet->afAcceleration[2], pPlanet->afAcceleration[3]);
			fprintf(pOutFile, "%f,%f,%f,%f,", pPlanet->afForce[0], pPlanet->afForce[1], pPlanet->afForce[2], pPlanet->afForce[3]);
			fprintf(pOutFile, "%i,", pPlanet->iSegments);
			fprintf(pOutFile, "%i,", pPlanet->iSlices);
			//history count not saved
			fprintf(pOutFile, "%i,", pPlanet->iType);
			fprintf(pOutFile, "%i,", pPlanet->m_bParticles);
			fprintf(pOutFile, "%i\n", pPlanet->m_bFixed);
		}
	}
	else {
		printf("Trouble writing to file.");
	}
	fclose(pOutFile);
}

void loadFile() {

	g_bMaintainPlanets = false; //create a blank canvas
	reset();
	popTail();

	char sFileName[512];
	strcpy(sFileName, "savefile.sav\0");
	printf("Loading %s\n", sFileName);

	FILE *pFile = fopen(sFileName, "r");
	if (pFile) {
		while(!feof(pFile)) {
			char sFileContents[32768];
			fscanf(pFile, " %s ", sFileContents);

			char *cToken = strtok(sFileContents, ",");

			while(cToken != '\0') {

				jrPlanet* pPlanet = createNewElement();

				pPlanet->fSize = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->fMass = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->fRotationAngle = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->afPosition[0] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afPosition[1] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afPosition[2] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afPosition[3] = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->iHistoryCount = 0;

				pPlanet->afAmbient[0] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afAmbient[1] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afAmbient[2] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afAmbient[3] = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->afDiffuse[0] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afDiffuse[1] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afDiffuse[2] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afDiffuse[3] = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->afVelocity[0] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afVelocity[1] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afVelocity[2] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afVelocity[3] = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->afAcceleration[0] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afAcceleration[1] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afAcceleration[2] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afAcceleration[3] = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->afForce[0] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afForce[1] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afForce[2] = atof(cToken);
				cToken = strtok(0, ",");
				pPlanet->afForce[3] = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->iSegments = atoi(cToken);
				cToken = strtok(0, ",");

				pPlanet->iSlices = atoi(cToken);
				cToken = strtok(0, ",");

				for (int i = 0; i < iHistoryMax; i++) {
					pPlanet->afPositionHistory[i] = pPlanet->afPosition[0];
					i++;
					pPlanet->afPositionHistory[i] = pPlanet->afPosition[1];
					i++;
					pPlanet->afPositionHistory[i] = pPlanet->afPosition[2];
				}

				pPlanet->iType = atof(cToken);
				cToken = strtok(0, ",");

				pPlanet->m_bParticles = !!atoi(cToken);
				cToken = strtok(0, ",");

				pPlanet->m_bFixed = !!atoi(cToken);
				cToken = strtok(0, ",");

				pushTail(pPlanet);
			}
		}

		fclose(pFile);
	}

}

int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //inits display mode with a depth buffer, double buffering and that shows colour
	glutInitWindowPosition(0,0); //set the window x/y pixels position from top left
	glutInitWindowSize(1000,800);  //set the window size
	glutCreateWindow("Universe Simulation"); //create the window with the title given

	myInit();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(sKeyboard);
	glutSpecialUpFunc(sKeyboardUp);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	createMenu();

	glutMainLoop(); //starts the glut event loop

	return 0;
}
