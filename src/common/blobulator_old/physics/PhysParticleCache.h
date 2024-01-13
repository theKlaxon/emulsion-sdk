#pragma once
#include "..\Point3D.h"
#include "PhysParticle.h"

class PhysParticleCache {
public:

	PhysParticleCache();
	PhysParticleCache(float p1, float p2);
	~PhysParticleCache();

	void	beginFrame();
	void	endFrame();

	void	beginTile(int p1);
	void	endTile();

	void	recaclulateBB();
	void	recalculateDimensions();

	void	setCacheParams(float p1, float p2);
	void	setOffset(Point3D& p1);

	void	generateCheckList(float p1, float p2);
	void	insertParticle(PhysParticle* p1);

	void	calcNeighbors(PhysParticle* p1, PhysParticle*** p2, int* p3);

	Point3D getInnerDimensions();
	Point3D getOuterDimensions();
	float	getMarginWidth();
};

class PhysParticleCacheFactory {
public:

	PhysParticleCacheFactory();

	PhysParticleCache*	getCache();
	void				returnCache(PhysParticleCache* p1);
};