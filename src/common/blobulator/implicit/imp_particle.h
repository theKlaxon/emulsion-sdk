#pragma once
#include "point_3D.h"

class ImpParticle {
public:

	ImpParticle();

	Point3D center;
	float fieldRScaleSq;
	float scale;
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