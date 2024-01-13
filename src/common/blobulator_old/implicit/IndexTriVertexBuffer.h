#include "ImpTiler.h"

class vbId_t;

class IndexTriVertexBuffer {
public:

	IndexTriVertexBuffer();
	~IndexTriVertexBuffer();

	void beginFrame(void* p1);
	void beginCube(unsigned char* p1);
	void beginMesh();
	void beginTile(ImpTile* p1);
	void beginVertex(vbId_t* p1);
	
	void endFrame();
	void endCube();
	void endMesh();
	void endTile();
	void endVertex(const vbId_t* p1);

	void drawVertex(const vbId_t* p1);
	void init();

	bool isIdValid(const vbId_t* p1);

protected:

	short	m_vbId0;
	short	m_sUnknown0;
	int		m_nUnknown0;
	float	m_flUnknown0;
};