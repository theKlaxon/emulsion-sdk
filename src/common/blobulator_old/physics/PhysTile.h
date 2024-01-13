#include "PhysParticleCache.h"

class PhysTile {
public:

	// TODO: create an overload that take a point3D in place of 3 ints
	PhysTile(PhysParticleCache* p1, int p2, int p3, int p4);
	~PhysTile();

	void			beginIteration();
	void			endIteration();

	PhysParticle*	getNextParticleAndNeighbors(PhysParticle*** p1, int* p2);

};