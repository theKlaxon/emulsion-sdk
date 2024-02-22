//==== Blobulator. Copyright © 2004-2024, Ilya D. Rosenberg, All rights reserved. ====//
#pragma once
#include "PhysParticle.h"
#include "..\Point3D.h"
#include "..\smartarray.h"

class PhysParticleListNode {
public:
	PhysParticle* particle;
	PhysParticleListNode* next;
	int mask;
};

class ArrayItem {
	PhysParticleListNode* particlesHere;
};

class CheckListItem {
	int offset;
	int mask;
};

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

	PhysParticleAndDist* get(PhysParticle* particle) {
		
		static PhysParticleAndDist dists[2000];
		int size = 0;

		int cnt = particles_sa.size;

		for (int i = 0; i < particles_sa.size; i++) {

			float dx = (particle->center[0] - particles_sa[i].particle->center[0]) * (particle->center[0] - particles_sa[i].particle->center[0]);
			float dy = (particle->center[1] - particles_sa[i].particle->center[1]) * (particle->center[1] - particles_sa[i].particle->center[1]);
			float dz = (particle->center[2] - particles_sa[i].particle->center[2]) * (particle->center[2] - particles_sa[i].particle->center[2]);

			float distanceSq = dx + dy + dz;

			if (distanceSq <= interactionRadiusSq) {
				dists[size].particle = particles_closeby_sa2[i];
				dists[size].distSq = distanceSq;
				size++;
			}
		}

		dists[size].particle = nullptr;
		dists[size].distSq = 0.0f;

		return &dists[0];
	}

private:

	void generateCheckList(float p1, float p2);

	void recalculateBB();
	void recalculateDimensions();	

	ArrayItem* array[100][100];
	int marginNCubes;
	int marginWidth;

	Point3D offset;
	Point3D outerDimensions;
	Point3D outerBBMins;
	Point3D outerBBMaxs;

	Point3D innerDimensions;
	Point3D innerBBMins;
	Point3D innerBBMaxs;

	int insertCount;
	int getCount;
	int searchCount;
	int clearCount;

	int check_list_size;
	CheckListItem check_list[125];

	float cubeWidth;
	float oneOverCubeWidth;
	float interactionRadius;
	float interactionRadiusSq;
	SmartArray<PhysParticleListNode, 0, 16> particles_sa;
	SmartArray<ArrayItem, 0, 16> clear_list_sa;
	SmartArray<PhysParticle*, 0, 16> particles_closeby_sa2;

};

class PhysParticleCacheFactory {
public:

	PhysParticleCache* getCache();
	void returnCache(PhysParticleCache* pCache);

	static PhysParticleCacheFactory* factory;

private:

	PhysParticleCacheFactory();

};