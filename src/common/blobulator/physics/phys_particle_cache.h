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

	//PhysParticleAndDist* get(PhysParticle* p1);

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

#ifdef BLOBULATOR_UNIT_TESTS
class PhysParticleCache_Test : public IUnitTest {
public:

	void CompileMe() {
		// this will likely cause a mem leak if it's run.
		// this is only to test if everything compiles correctly.
		PhysParticleCache* pCache = new PhysParticleCache();
		pCache = new PhysParticleCache(0, 0);
		pCache->beginFrame();
		pCache->endFrame();
		pCache->beginTile(0);
		pCache->endTile();
		pCache->calcNeighbors(nullptr, nullptr, nullptr);
		pCache->insertParticle(nullptr);
		pCache->setCacheParams(0, 0);
		pCache->setOffset(Point3D());
		pCache->getInnerDimensions();
		pCache->getOuterDimensions();
		pCache->getMarginWidth();
	}

};
#endif