#pragma once
#include "point_3D.h"
#include "smartarray.h"
#include "implicit/imp_particle.h"

struct PCacheElem_t {
	short center_x;
	unsigned char bot_x, top_x;

	PCacheElem_t* prev, *next;
	ImpParticle* p;
};

struct pcache_YZ_t {
	unsigned char y, z;
};

class ProjectingParticleCache {
public:

	ProjectingParticleCache();
	~ProjectingParticleCache();

	void beginTile();
	void endTile();

	void addParticle(ImpParticle* pParticle);
	void buildCache(float p1, float p2, Point3D& p3, Point3D& p4);
	
	PCacheElem_t* cachePlane[101];

private:

	void insertIntoCache(ImpParticle* p1, float p2, float p3, Point3D& p4, Point3D& p5);

	PCacheElem_t* botSentinel;
	PCacheElem_t* midSentinel;
	PCacheElem_t* topSentinel;

	SmartArray<ImpParticle*, 0, 16> particles;
	SmartArray<PCacheElem_t*, 0, 16> cacheElements;
	SmartArray<pcache_YZ_t, 0, 16> clearList;

	int curCacheElement;

};
