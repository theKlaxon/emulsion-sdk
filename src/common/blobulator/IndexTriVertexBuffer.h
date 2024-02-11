//==== Blobulator. Copyright © 2004-2024, Ilya D. Rosenberg, All rights reserved. ====//
#pragma once
#include "Implicit/ImpTiler.h"
#include "materialsystem/imesh.h"

struct vbId_t;

class IndexTriVertexBuffer {
public:

	IndexTriVertexBuffer();
	~IndexTriVertexBuffer();

	void beginFrame(void* p1);
	void endFrame();

	void beginCube(unsigned char p1);
	void endCube();

	void beginTile(ImpTile* p1);
	void endTile();

	void beginVertex(vbId_t* p1);
	void drawVertex(vbId_t const*);
	void endVertex(vbId_t* p1);

	bool isIdValid(vbId_t const* p1);

private:

	void beginMesh();
	void endMesh();

	unsigned short m_curTime;
	unsigned short m_nVerticesOutput;

	IMesh* m_pMesh;
	CMeshBuilder* m_pMeshBuilder;
	
	unsigned short m_stat_no_flushes;
};