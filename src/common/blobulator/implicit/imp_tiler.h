#pragma once
#include "..\blobulator\point_3D.h"
#include "imp_particle.h"
#include "smartarray.h"

class SweepRenderer;

class ImpTile {
public:

	//ImpTile();

	bool done;
	int x, y, z;
	Point3D center;
	bool old_tile;
	Point3D debug_color;
	float dist;
	SmartArray<ImpParticle*, 0, 16> particles;
};

class ImpTiler {
public:

	ImpTiler(SweepRenderer* pSweepRenderer);

	void beginFrame(Point3D& _offset, void* pRenderContext, bool tile_mode);
	void endFrame();

	void insertParticle(ImpParticle* pParticle);
	
	void drawSurface();
	void drawSurfaceSorted(Point3D& pPoint);
	void drawTile(ImpTile* pTile);
	void drawTile(int x, int y, int z);

	int getNoTiles();
	ImpTile* getTile(int index);
	Point3D getTileOffset(int index);

private:

	void addParticleToTile(ImpParticle* pParticle, int x, int y, int z);
	ImpTile* findTile(int x, int y, int z);
	ImpTile* createTile(int x, int y, int z);

public:

	SweepRenderer* m_sweepRenderer;
	SmartArray<ImpTile*, 0, 16> tiles;

	int maxNoTileToDraw;

	Point3D last_tiles_offset;
	Point3D offset;
	Point3D render_dim;
	float	render_margin;
	Point3D corner_offset;
};

class ImpTilerFactory {
public:

	ImpTiler* getTiler();
	void returnTiler(ImpTiler* pTiler);

	static ImpTilerFactory* factory;

private:

	ImpTilerFactory();

};

#ifdef BLOBULATOR_UNIT_TESTS
class ImpTiler_Test {
public:

	void CompileMe() {
		
		ImpTiler* pTiler = new ImpTiler(nullptr);
		pTiler->beginFrame(Point3D(), nullptr, false);
		pTiler->endFrame();
		pTiler->insertParticle(nullptr);
		//pTiler->addParticleToTile(nullptr, 0, 0, 0);
		//pTiler->createTile(0, 0, 0);
		//pTiler->findTile(0, 0, 0);
		pTiler->drawSurface();
		pTiler->drawSurfaceSorted(Point3D());
		pTiler->drawTile(nullptr);
		pTiler->drawTile(0, 0, 0);
		pTiler->getNoTiles();
		pTiler->getTile(0);
		pTiler->getTileOffset(0);

		// dont run game with this PLEASE, this is just to test if everything compiles
		ImpTilerFactory* pFactory = nullptr;
		pTiler = pFactory->getTiler();
		pFactory->returnTiler(pTiler);
	}
};
#endif