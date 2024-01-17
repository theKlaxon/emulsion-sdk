#pragma once
#include "point_3D.h"
#include "physics/phys_particle_cache.h"
#include "smartarray.h"

class PhysTile {
public:

	PhysTile(PhysParticleCache* pCache, int x, int y, int z);
	~PhysTile();

	void beginIteration();
	void endIteration();

	PhysParticle* getNextParticleAndNeighbors(PhysParticle*** p1, int* p2);
};

class PhysTiler;
class PhysTileIterator {
public:

private:

	PhysTileIterator(PhysTiler* pTiler);

	void reset();

};

class PhysTiler {
public:

	PhysTiler(PhysParticleCache* pCache);
	PhysTiler(float p1, float p2 = 0);
	~PhysTiler();

	void beginFrame(Point3D& pPos);
	void endFrame();

	PhysTileIterator* acquireIterator();
	void releaseIterator(PhysTileIterator* pIterator);

	void beginIteration();
	void endIteration();
	
	PhysParticle* getNextParticleAndNeighbors(PhysParticle*** p1, int* p2);

	void insertParticle(PhysParticle* pParticle);
	void setCacheParams(float p1, float p2);

	void processTiles();

	//PhysParticleCache* getParticleCache();

private:

	Point3D calcTileCorner(int x, int y, int z);
	Point3D calcTileOffset(int x, int y, int z);

	PhysTile* createTile(int x, int y, int z);
	PhysTile* findTile(int x, int y, int z);

	PhysParticleCache* m_pParticleCache;
	SmartArray<PhysTile*, 0, 16> tiles;
	SmartArray<PhysTile*, 0, 16> tile_cache;
	PhysTileIterator* iterator;

	bool iterator_active;
	PhysTile* iteration_tile;
	int iteration_tile_num;
	int iteration_particle_count;
	int iteration_num;

	Point3D offset;
	Point3D outer_cache_dim;
	Point3D render_dim;

	float render_margin;
	Point3D corner_offset;

};

class PhysTilerFactory {
public:

	PhysTiler* getTiler();
	void returnTiler(PhysTiler* pTiler);

	static PhysTilerFactory* factory;

private:

	PhysTilerFactory();

};
