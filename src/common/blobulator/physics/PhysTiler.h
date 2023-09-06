#pragma once
#include "PhysParticleCache.h"
#include "PhysTile.h"
#include "..\Point3D.h"

class PhysTiler;

class PhysTileIterator {
public:

	PhysTileIterator(PhysTiler* p1);

	void reset();
};

class PhysTiler {
public:

	PhysTiler(PhysParticleCache* particleCache);
	PhysTiler(float p1, float p2);
	~PhysTiler();
	
	void beginIteration();
	void beginFrame(Point3D& par1);
	void endIteration();
	void endFrame();

	PhysTileIterator*	acquireIterator();
	void				releaseIterator();
	
	PhysTile*	createTile(int p1, int p2, int p3);
	Point3D		calcTileCorner(int p1, int p2, int p3);
	Point3D		calcTileOffset(int p1, int p2, int p3);

	// TODO: add an overload (defined here) that calls this, 
	// and takes 1 point3d to get the 3 int params to feed the beast
	//PhysTile*			findTile(Point3D point) { return findTile(point.x, point.y, point.z); }
	
	PhysTile*		findTile(int p1, int p2, int p3);
	PhysParticle*	getNextParticleAndNeighbors(PhysParticle*** p1, int* p2);
	void			insertParticle(PhysParticle* p1);
	void			setCacheParams(float p1, float p2);

};

class PhysTilerFactory {

	PhysTilerFactory();

	void		returnTiler(PhysTiler* p1);
	PhysTiler*	getTiler();
};