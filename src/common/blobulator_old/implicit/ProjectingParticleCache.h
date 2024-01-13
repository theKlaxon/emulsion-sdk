#pragma once
#include "blobulator/iblob_renderer.h"
#include "blobulator/implicit/Userfunctions.h"

class ProjectingParticleCache {
public:

	ProjectingParticleCache();
	~ProjectingParticleCache();

	void addParticle(ImpParticle* pParticle);
	void beginTile();
	void endTile();

	void buildCache(float p1, float p2, Point3D* p3, Point3D* p4);
	void insertIntoCache(ImpParticle* p1, float p2, float p3, Point3D& p4, Point3D& p5);

protected:

	/*
		PCacheElem_t* botSentinel = 
		short center_x;					// 0x0
		unsigned char bot_x;			// 0x1
		unsigned char top_x;			// 0x2
		PCacheElem_t* prev;				// 0x3
		PCacheElem_t* next;				// 0x4
		ImpParticle* p;					// 0x5

		PCacheElem_t* midSentinel =
		short center_x;
		unsigned char bot_x;
		unsigned char top_x;
		PCacheElem_t* prev;
		PCacheElem_t* next;
		ImpParticle* p;

		PCacheElem_t* topSentinel =
		short center_x;
		unsigned char bot_x;
		unsigned char top_x;
		PCacheElem_t* prev;
		PCacheElem_t* next;
		ImpParticle* p;
	*/


	PCacheElem_t* botSentinel;					// 0x0 (*this) 
	PCacheElem_t* midSentinel;					// 0x4
	PCacheElem_t* topSentinel;					// 0x8

	SmartArray<pcache_YZ_t, 0, 16> clearList;	// 0xc (12) ? 4 bytes : idfk;	// CORRECT POSITION
	int curCacheElement;						// 0x10 (16)					// CORRECT POSITION
	PCacheElem_t** cachePlane[101];				// 0x14 (20)

};