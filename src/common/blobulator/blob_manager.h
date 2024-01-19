#pragma once
#include "igamesystem.h"
#include "parented_func.h"
//#include "blob_batch.h"
#ifdef CLIENT_DLL
#include "blob_render.h"
#include "ipaintstream.h"
#else
#include "paint_stream.h"
extern CPaintBlobStream* g_pBouncePaintStream;
extern CPaintBlobStream* g_pSpeedPaintStream;
extern CPaintBlobStream* g_pStickPaintStream;
#endif

#define BLOB_BATCH_COUNT 2

class CBlobManager : public CBaseGameSystemPerFrame {
public:

	~CBlobManager();

	char const* Name() { return "Blobulator"; }

	bool Init();
	void Shutdown();

	void LevelInitPostEntity();
	void LevelShutdownPostEntity();

#ifdef CLIENT_DLL
	void PreRender();
	void Update(float frametime);
	void Render();
	void PostRender();

	void AddRenderableStream(IPaintStream* pStream) {
		m_pPaintStreams.AddToTail(pStream);
	}
	
#else
	void PreClientUpdate();

	// blob manufacturing!
	void CreateBlob(Vector origin, Vector velocity, int batch);
#endif
	
protected:
	friend class COpPtr_DoRender;
	friend class COpPtr_DoPhysics;

	float m_flCubeWidth;
	float m_flCutoffRadius;
	float m_flRenderRadius;
	
	int m_nCurBatch;
	
private:
#ifdef CLIENT_DLL
	IParentedFuncPtr* pRenderFunc;

	CUtlVector<IPaintStream*> m_pPaintStreams;

#else
	IParentedFuncPtr* pPhysicsFunc;
#endif
};

extern IGameSystem* BlobulatorSystem();

namespace Paint {

	void CreateBlob(Vector center, Vector velocity);

}