#pragma once

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


const int iHistoryMax = (900 * 3); //has to be * 3 so that there can be room for x, y and z
struct jrPlanet {
	jrPlanet *m_pNext;
	jrPlanet *m_pLast;
	float fSize;
	float fMass;
	float fRotationAngle;
	float afPosition[4];
	float afPositionHistory[iHistoryMax];
	float afAmbient[4];
	float afDiffuse[4];
	float afVelocity[4];
	float afAcceleration[4];
	float afForce[4];
	int iSegments;
	int iSlices;
	int iHistoryCount;
	int iType; //sphere, cube etc
	bool m_bFixed;
	bool m_bParticles;
};

void printList();
bool pushHead(jrPlanet* pPlanet);
bool pushTail(jrPlanet* pPlanet);
bool destroy(jrPlanet* pPlanet);
jrPlanet* popHead();
jrPlanet* popTail();

jrPlanet* createNewElement();
jrPlanet* deleteElement(jrPlanet *pPlanet);

void insertBefore(jrPlanet* pPlanet, jrPlanet* pTarget);
void insertAfter(jrPlanet* pPlanet, jrPlanet* pTarget);
bool remove(jrPlanet* pPlanet);
jrPlanet* seedPlanetValues(jrPlanet *pPlanet);
jrPlanet* seedCubeValues(jrPlanet *pPlanet);
jrPlanet* seedConeValues(jrPlanet *pPlanet);
jrPlanet* seedTeapotValues(jrPlanet *pPlanet);
jrPlanet* initSun(jrPlanet *pPlanet);

extern jrPlanet *g_pHead;
extern jrPlanet *g_pTail;
