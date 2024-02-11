//==== Blobulator. Copyright © 2004-2024, Ilya D. Rosenberg, All rights reserved. ====//
#pragma once
#include "..\Point3D.h"

class ImpParticle {
public:

	ImpParticle();

	Point3D center;
	float fieldRScaleSq;	// particle scale (0.25f - 1.0f == safe range)
	float scale;			// texture influence scale 
};

class ImpParticleWithOneInterpolant : public ImpParticle {
public:

	ImpParticleWithOneInterpolant();
	
	Point3D interpolants1;
};

class ImpParticleWithTwoInterpolants : public ImpParticleWithOneInterpolant {
public:

	ImpParticleWithTwoInterpolants();

	Point3D interpolants2;
};

class ImpParticleWithFourInterpolants : public ImpParticleWithTwoInterpolants {
public:

	ImpParticleWithFourInterpolants();

	Point3D interpolants3;
	Point3D interpolants4;
};