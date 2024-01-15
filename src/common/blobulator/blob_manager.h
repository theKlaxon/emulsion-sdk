#pragma once
#include "igamesystem.h"
#include "parented_func.h"
#include "blob_batch.h"
#ifdef CLIENT_DLL
#include "blob_render.h"
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
	void PostRender();

	void SetBlobRenderFunc(IParentedFuncPtr* ptr);
	
#else
	void SetBlobPhysicsFunc(IParentedFuncPtr* ptr);
	void PreClientUpdate();

	// blob manufacturing!
	void CreateBlob(Vector origin, float radius, int batch);
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
	CUtlVector<IBlobBatch*> m_Batches;
#else
	IParentedFuncPtr* pPhysicsFunc;
#endif
};

extern IGameSystem* BlobulatorSystem();