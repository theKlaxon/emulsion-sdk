#pragma once
#include "../Point3D.h"

class ImpParticle {
public:

	ImpParticle();

	//void setFieldScale(float p1);

	Point3D center;
	float fieldRScaleSQ;
	float scale;
	Point3D interpolants;

	int group;
	int neighbor_count;
	int temp1;
	Point3D force;
};

class ImpParticleWithOneInterpolant : public ImpParticle {
public:

	//ImpParticleWithOneInterpolant();

	Point3D center;			/* offset: 0	*/
	float fieldRScaleSQ;	/* offset: 16	*/
	float scale;			/* offset: 20	*/

	Point3D interpolants1;	/* offset: 32	*/
};

class ImpParticleWithTwoInterpolants : public ImpParticleWithOneInterpolant {
public:

	//ImpParticleWithTwoInterpolants();

	Point3D center;			/* offset: 0	*/
	float fieldRScaleSQ;	/* offset: 16	*/
	float scale;			/* offset: 20	*/

	//Point3D interpolants1;
	Point3D interpolants2;
};

class ImpParticleWithFourInterpolants : public ImpParticleWithTwoInterpolants {
public:

	ImpParticleWithFourInterpolants();

	Point3D center;			/* offset: 0	*/
	float fieldRScaleSQ;	/* offset: 16	*/
	float scale;			/* offset: 20	*/
	
	//Point3D interpolants1;
	//Point3D interpolants2;
	Point3D interpolants3;
	Point3D interpolants4;
};