#include "stdlib.h"
#include <stdio.h>
#include <conio.h>
#include "raaMaths/raaMaths.h"
#include "jrLinkedList.h"

jrPlanet *g_pHead=0;
jrPlanet *g_pTail=0;

jrPlanet* createNewElement()
{
	jrPlanet *pPlanet=new jrPlanet;

	pPlanet->m_pLast = 0;
	pPlanet->m_pNext = 0;

	pPlanet->fSize = 0.0f;
	pPlanet->fMass = 0.0f;
	pPlanet->fRotationAngle = 0.0f;

	pPlanet->afPosition[0] = 0.0f;
	pPlanet->afPosition[1] = 0.0f;
	pPlanet->afPosition[2] = 0.0f;
	pPlanet->afPosition[4] = 0.0f;


	for (int i = 0; i < iHistoryMax; i++) {
		pPlanet->afPositionHistory[i] = 0.0f;
	}

	pPlanet->afAmbient[0] = 0.0f;
	pPlanet->afAmbient[1] = 0.0f;
	pPlanet->afAmbient[2] = 0.0f;
	pPlanet->afAmbient[3] = 1.0f;

	pPlanet->afDiffuse[0] = 0.0f;
	pPlanet->afDiffuse[1] = 0.0f;
	pPlanet->afDiffuse[2] = 0.0f;
	pPlanet->afDiffuse[3] = 1.0f;

	pPlanet->afVelocity[0] = 0.0f;
	pPlanet->afVelocity[1] = 0.0f;
	pPlanet->afVelocity[2] = 0.0f;
	pPlanet->afVelocity[3] = 0.0f;

	pPlanet->afAcceleration[0] = 0.0f;
	pPlanet->afAcceleration[1] = 0.0f;
	pPlanet->afAcceleration[2] = 0.0f;
	pPlanet->afAcceleration[3] = 0.0f;

	pPlanet->afForce[0] = 0.0f;
	pPlanet->afForce[1] = 0.0f;
	pPlanet->afForce[2] = 0.0f;
	pPlanet->afForce[3] = 0.0f;

	pPlanet->iSegments = 0;
	pPlanet->iSlices = 0;
	pPlanet->iHistoryCount = 0;
	pPlanet->iType = SPHERE;

	pPlanet->m_bFixed = 1;
	return pPlanet;
}

jrPlanet* seedPlanetValues(jrPlanet *pPlanet) {

	pPlanet->m_pLast=0;
	pPlanet->m_pNext=0;

	for (int i = 0; i < 3; i++) {
		pPlanet->afDiffuse[i] = randFloat(0.3f, 0.8f);
		pPlanet->afAmbient[i] = pPlanet->afDiffuse[i] / 2.0f;
		pPlanet->afPosition[i] = randFloat(-3000.0f, 3000.0f);
		pPlanet->afAcceleration[i] = randFloat(-0.0001f, 0.0001f);
		pPlanet->afVelocity[i] = randFloat(-0.003f, 0.003f);
	}

	for (int i = 0; i < iHistoryMax; i++) {
		pPlanet->afPositionHistory[i] = pPlanet->afPosition[0];
		i++;
		pPlanet->afPositionHistory[i] = pPlanet->afPosition[1];
		i++;
		pPlanet->afPositionHistory[i] = pPlanet->afPosition[2];
	}

	pPlanet->fSize = randFloat(5.0f, 15.0f);
	pPlanet->fMass = pPlanet->fSize * 20.0f;
	pPlanet->fRotationAngle = randFloat(0.0f, 360.0f);

	pPlanet->iSegments = 10;
	pPlanet->iSlices = 10;
	pPlanet->m_bFixed = false;
	pPlanet->m_bParticles = false;
	return pPlanet;

}

jrPlanet* seedCubeValues(jrPlanet *pPlanet) {
	pPlanet->iType = CUBE;
	return seedPlanetValues(pPlanet);
}

jrPlanet* seedConeValues(jrPlanet *pPlanet) {
	pPlanet->iType = CONE;
	return seedPlanetValues(pPlanet);
}

jrPlanet* seedTeapotValues(jrPlanet *pPlanet) {
	pPlanet->iType = TEAPOT;
	return seedPlanetValues(pPlanet);
}

jrPlanet* initSun(jrPlanet *pPlanet) {

	pPlanet->m_pLast=0;
	pPlanet->m_pNext=0;

	for (int i = 0; i < 3; i++) {
		pPlanet->afDiffuse[i] = randFloat(0.3f, 0.9f);
		pPlanet->afAmbient[i] = pPlanet->afDiffuse[i] / 2.0f;
	}

	pPlanet->afDiffuse[3] = 1.0f;
	pPlanet->afAmbient[3] = 1.0f;

	pPlanet->fSize = randFloat(50.0f, 100.0f);
	pPlanet->fMass = pPlanet->fSize * 80.0f;

	pPlanet->iSegments = 30;
	pPlanet->iSlices = 30;
	pPlanet->m_bFixed = true;

	pPlanet->iType = SUN;
	pPlanet->m_bParticles = false;

	return pPlanet;
}

jrPlanet* deleteElement(jrPlanet *pPlanet)
{
	jrPlanet *pE=pPlanet;

	if(pE)
	{

		remove(pE);

		pE->fSize = 0.0f;
		pE->fMass = 0.0f;
		pE->fRotationAngle = 0.0f;
		pE->afPosition[0] = 0.0f;
		pE->afPosition[1] = 0.0f;
		pE->afPosition[2] = 0.0f;
		pE->afPosition[4] = 0.0f;


		for (int i = 0; i < iHistoryMax; i++) {
			pE->afPositionHistory[i] = 0.0f;
		}

		pE->afAmbient[0] = 0.0f;
		pE->afAmbient[1] = 0.0f;
		pE->afAmbient[2] = 0.0f;
		pE->afAmbient[3] = 0.0f;
		pE->afDiffuse[0] = 0.0f;
		pE->afDiffuse[1] = 0.0f;
		pE->afDiffuse[2] = 0.0f;
		pE->afDiffuse[3] = 0.0f;
		pE->afVelocity[0] = 0.0f;
		pE->afVelocity[1] = 0.0f;
		pE->afVelocity[2] = 0.0f;
		pE->afVelocity[3] = 0.0f;
		pE->afAcceleration[0] = 0.0f;
		pE->afAcceleration[1] = 0.0f;
		pE->afAcceleration[2] = 0.0f;
		pE->afAcceleration[3] = 0.0f;
		pE->afForce[0] = 0.0f;
		pE->afForce[1] = 0.0f;
		pE->afForce[2] = 0.0f;
		pE->afForce[3] = 0.0f;
		pE->iSegments = 0;
		pE->iSlices = 0;
		pE->iHistoryCount = 0;
		pE->iType = 0;
		pE->m_bFixed = 0;


		// remove from list

		delete pE;
		pE = 0;
	}

	return pE;
}

bool pushHead(jrPlanet *pPlanet) {

	if (!g_pHead && !g_pTail) { //if head and tail are 0
		g_pHead = pPlanet;
		g_pTail = pPlanet;
		return true;
	}

	if (!pPlanet->m_pLast && !pPlanet->m_pNext) { //if the element does not have values for last and next
		pPlanet->m_pNext = g_pHead;
		pPlanet->m_pLast = 0;
		g_pHead->m_pLast = pPlanet;
		g_pHead = pPlanet;
	} else {
		printf("pushHead error: item already in list\n");
		return false;
	}
	return true;

}

bool pushTail(jrPlanet *pPlanet) {

	if (!g_pHead && !g_pTail) { //if head and tail are 0
		g_pHead = pPlanet;
		g_pTail = pPlanet;
		return true;
	}

	if (!pPlanet->m_pLast && !pPlanet->m_pNext) { //if the element does not have values for last and next
		pPlanet->m_pLast = g_pTail;
		pPlanet->m_pNext = 0;
		g_pTail->m_pNext = pPlanet;
		g_pTail = pPlanet;
	} else {
		printf("pushTail error: item already in list\n");
		return false;
	}
	return true;
}

jrPlanet* popTail() {
	if (g_pTail) {

		jrPlanet* pTemp = g_pTail;

		if (g_pHead == g_pTail) { //if the head and the tail are the same
			g_pHead = 0;
			g_pTail = 0;
		} 
		else {
			g_pTail = g_pTail->m_pLast;
			g_pTail->m_pNext = 0;
			pTemp->m_pLast = 0;
		}
		return pTemp;
	}
	return 0;
}

jrPlanet* popHead() {

	if (g_pHead) {
		jrPlanet* pTemp = g_pHead;

		if (g_pHead == g_pTail) {
			g_pHead = 0;
			g_pTail = 0;
		}
		else {
			g_pHead = pTemp->m_pNext;
			g_pHead->m_pLast = 0;
			pTemp->m_pNext = 0;
		}
		return pTemp;
	}
	return 0;
}

bool destroy(jrPlanet *pPlanet) {

	if (pPlanet) {
		if ((pPlanet->m_pLast != 0 || pPlanet->m_pNext != 0) || pPlanet == g_pTail || pPlanet == g_pHead) {
			return false;
		}

		delete pPlanet;
		pPlanet = 0;
		return true;
	}
	return false;
}

void insertBefore(jrPlanet* pPlanet, jrPlanet* pTarget) {

	if (!pPlanet || !pTarget || (pPlanet->m_pLast || pPlanet->m_pNext)) {
		return; //invalid input
	}

	if (pTarget == g_pHead) {
		pushHead(pPlanet);
		return;
	}

	pTarget->m_pLast->m_pNext = pPlanet;
	pPlanet->m_pLast = pTarget->m_pLast;
	pTarget->m_pLast = pPlanet;
	pPlanet->m_pNext = pTarget;

}

void insertAfter(jrPlanet* pPlanet, jrPlanet* pTarget) {

	if (!pPlanet || !pTarget || (pPlanet->m_pLast || pPlanet->m_pNext)) {
		return; //invalid input
	}

	if (pTarget == g_pTail) {
		pushTail(pPlanet);
		return;
	}

	pTarget->m_pNext->m_pLast = pPlanet;
	pPlanet->m_pNext = pTarget->m_pNext;
	pTarget->m_pNext = pPlanet;
	pPlanet->m_pLast = pTarget;
}

bool remove(jrPlanet* pPlanet) {

	if (pPlanet) {
		if (pPlanet == g_pTail) {
			popTail();
		}
		else if (pPlanet == g_pHead) {
			popHead();
		}
		else if (pPlanet->m_pNext == 0 && pPlanet->m_pLast == 0) {
			return true;
		}
		else {
			pPlanet->m_pNext->m_pLast = pPlanet->m_pLast;
			pPlanet->m_pLast->m_pNext = pPlanet->m_pNext;
			pPlanet->m_pNext=0;
			pPlanet->m_pLast=0;
		}
		return true;
	}
	return false;
}


