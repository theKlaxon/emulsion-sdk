#pragma once
#include "..\common\blobulator\implicit\SweepRenderer.h"
#include "..\common\blobulator\Point3D.h"

class ImpTile {
public:

	ImpTile(int p1, int p2, int p3);
	~ImpTile();// { free(*(void**)(this + 0x44)); }

	bool done;
	int x, y, z;

	Point3D center;
	bool old_tile;

	Point3D debugColor;
	float dist;

};

class ImpTiler {
public:

	ImpTiler(SweepRenderer* p1);
	~ImpTiler();

	void beginFrame(Point3D& p1, void* p2, bool p3);
	void endFrame();

	Point3D calcTileCorner(int p1, int p2, int p3);
	Point3D calcTileOffset(int p1, int p2, int p3);

	ImpTile* createTile(int p1, int p2, int p3);
	ImpTile* findTile(int p1, int p2, int p3);

	void drawTile(ImpTile* p1);
	void drawTile(int p1, int p2, int p3);
	void drawSurface();
	void drawSurfaceSorted(Point3D& p1);

	int			getNoTiles();
	ImpTile*	getTile(int p1);
	Point3D		getTileOffset(int p1);
	
	void addParticleToTile(ImpParticle* p1, int p2, int p3, int p4);
	void insertParticle(ImpParticle* p1);

	// TODO: only used in ep3 blobulator, remove
	void setMaxNoTilesToDraw(int p1) {}

	SweepRenderer* m_sweepRenderer;

	int maxNoTilesToDraw;

	Point3D lastTilesOffset;
	Point3D offset;
	Point3D renderDim;
	float renderMargin;

	Point3D corner_offset;
	
	void drawSurface2();

private:

	ImpTiler(ImpTiler& tiler);
};