//==== Blobulator. Copyright © 2004-2007, Rosware LLC, All rights reserved. ====//
#pragma once
#include "phys_particle.h"
#include "point_3D.h"

class PhysParticleCache {
public:

	PhysParticleCache();
	PhysParticleCache(float p1, float p2);

	void beginFrame();
	void endFrame();

	void beginTile(int p1);
	void endTile();

	void calcNeighbors(PhysParticle* p1, PhysParticle*** p2, int* p3);
	void insertParticle(PhysParticle* p1);
	void setCacheParams(float p1, float p2);
	void setOffset(Point3D& p1);

	Point3D getInnerDimensions();
	Point3D getOuterDimensions();
	float	getMarginWidth();

private:

	void generateCheckList(float p1, float p2);

	void recalculateBB();
	void recalculateDimensions();


};

class PhysParticleCacheFactory {
public:

	PhysParticleCache* getCache();
	void returnCache(PhysParticleCache* pCache);

	static PhysParticleCacheFactory* factory;

private:

	PhysParticleCacheFactory();

};