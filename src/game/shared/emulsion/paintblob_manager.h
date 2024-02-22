#pragma once
#include "igamesystem.h"
#ifdef CLIENT_DLL
#include "paint_defs.h"
#else
#include "paintblob_stream.h"
#include "paint_sphere.h"

extern CPaintBlobStream* g_pBouncePaintStream;
extern CPaintBlobStream* g_pSpeedPaintStream;
extern CPaintBlobStream* g_pErasePaintStream;

extern CUtlVector<int> g_pBounceRemovalQueue;
extern CUtlVector<int> g_pSpeedRemovalQueue;
extern CUtlVector<int> g_pStickRemovalQueue;
extern CUtlVector<int> g_pEraseRemovalQueue;
#endif

#define BLOB_BATCH_COUNT 2

class CPaintBlobManager : public CBaseGameSystemPerFrame {
public:

	~CPaintBlobManager();

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
	
	CPaintBlobStream* GetStream(int streamIndex);
	int GetStreamIndex(int paintType);
	CUtlVector<int>* GetRemovalQueue(int streamIndex);

	// blob manufacturing!
	void CreateBlob(Vector origin, Vector velocity, int batch, float radius = 1.0f, int count = 1);
	void QueueBlobForRemoval(int particleIndex, int batch);
	void RemoveQueuedBlobs();

	virtual void FrameUpdatePreEntityThink();
	virtual void FrameUpdatePostEntityThink();

	void QueuePaintSphere(CPaintSphere* pSphere);
	void PaintAllSpheres();
	CUtlVector<CPaintSphere*> m_SphereQueue;

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
	CUtlVector<IPaintStream*> m_pPaintStreams;
#else
#endif
};

extern IGameSystem* PaintBlobManager_System();
extern CPaintBlobManager* PaintBlobManager();

namespace Paint {

	void CreateBlob(Vector center, Vector velocity);

}