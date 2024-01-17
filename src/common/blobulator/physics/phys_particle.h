#pragma once
#include "point_3D.h"

class PhysParticle {
public:

	PhysParticle();

	Point3D center;
	float radius;
	int temp1;

	// these last 2 params are un-educated gusses,
	// vs wouldn't give any info on them
	Point3D force;

	short group;
	short neighbor_count;

	//unsigned char undefined[8];
};

class PhysParticleAndDist {
public:
	PhysParticle* particle;
	float distSq;
};