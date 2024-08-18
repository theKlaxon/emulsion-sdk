#pragma once
#include "mathlib/mathlib.h"

class CParticleDefinition {
public:

protected:

	bool m_bIsKilled; // TODO: this was here in the Source2Viewer, but might not be the best thing here
	
	int m_nIndex;
	int m_nParticleId;
	int m_nSequence;
	int m_nSequence2;
	
	float m_flAge;
	float m_flLifetime;
	float m_flAlpha;
	float m_flAlphaAlternate;
	float m_flRadius;
	float m_flTrailLength;
	float m_flSpeed;

	Vector m_vecPos;
	Vector m_vecPrevPos;
	Vector m_vecRotation;
	Vector m_vecRotationSpeed;
	Vector m_vecVelocity;
};