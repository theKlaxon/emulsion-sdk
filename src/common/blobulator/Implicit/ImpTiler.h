//==== Blobulator. Copyright © 2004-2024, Ilya D. Rosenberg, All rights reserved. ====//
#pragma once
#include "ImpParticle.h"
#include "..\SmartArray.h"
#include "..\Point3D.h"

class SweepRenderer;

class ImpTile {
public:

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