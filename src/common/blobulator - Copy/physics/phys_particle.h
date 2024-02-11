//==== Blobulator. Copyright © 2004-2007, Rosware LLC, All rights reserved. ====//
#pragma once
#include "point_3D.h"

class PhysParticle {
public:

	PhysParticle();

	Point3D center;
	float radius;
	int temp1;
	
	Point3D force;

	short group;
	short neighbor_count;
};

class PhysParticleAndDist {
public:
	PhysParticle* particle;
	float distSq;
};