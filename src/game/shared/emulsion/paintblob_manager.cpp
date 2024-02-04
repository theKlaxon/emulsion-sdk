#include "cbase.h"
#include "paintblob_manager.h"
#include "game/shared/portal2/paint_enum.h"

#ifdef CLIENT_DLL
#include "implicit/imp_tiler.h"
#include "implicit/sweep_renderer.h"
#include "smartarray.h"

// defs for sweep renderer
bool calcSign(unsigned char, unsigned char, unsigned char, float, float, float, ProjectingParticleCache*);
void calcSign2(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);

void calcCornerNormal(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
void calcCornerNormalColorUVTan(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
void calcCornerNormalColor(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);

void calcVertexNormal(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
void calcVertexNormalDebugColor(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
void calcVertexNormalNColorUVTan(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
void calcVertexNormalNColor(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
#else 

#endif

CPaintBlobManager::~CPaintBlobManager() {
#ifdef CLIENT_DLL

#endif
}

bool CPaintBlobManager::Init() {
	
	return true;
}

void CPaintBlobManager::Shutdown() {

}

#ifndef CLIENT_DLL
#include "paintblob_stream.h"

CPaintBlobStream* g_pBouncePaintStream;
CPaintBlobStream* g_pSpeedPaintStream;
CPaintBlobStream* g_pStickPaintStream;
CPaintBlobStream* g_pErasePaintStream;

CUtlVector<int> g_BounceRemovalQueue;
CUtlVector<int> g_SpeedRemovalQueue;
CUtlVector<int> g_StickRemovalQueue;
CUtlVector<int> g_EraseRemovalQueue;
#endif

void CPaintBlobManager::LevelInitPostEntity() {

#ifdef CLIENT_DLL
	SweepRenderer::setCalcSignFunc(calcSign);
	SweepRenderer::setCalcSign2Func(calcSign2);
	SweepRenderer::setCalcCornerFunc(32, calcCornerNormalColor);
	SweepRenderer::setCalcVertexFunc(calcVertexNormalNColorUVTan);
#else 
	// create the paint streams
	g_pBouncePaintStream = (CPaintBlobStream*)CreateEntityByName("env_paint_stream");
	g_pSpeedPaintStream = (CPaintBlobStream*)CreateEntityByName("env_paint_stream");
	g_pStickPaintStream = (CPaintBlobStream*)CreateEntityByName("env_paint_stream");
	g_pErasePaintStream = (CPaintBlobStream*)CreateEntityByName("env_paint_stream");

	g_pBouncePaintStream->SetPaintType(BOUNCE_POWER);
	g_pSpeedPaintStream->SetPaintType(SPEED_POWER);
	g_pStickPaintStream->SetPaintType(PORTAL_POWER);
	g_pErasePaintStream->SetPaintType(NO_POWER);

	g_pBouncePaintStream->SetAbsOrigin(Vector(-2056, -2056, -2056));
	g_pSpeedPaintStream->SetAbsOrigin(Vector(-2056, -2056, -2056));
	g_pStickPaintStream->SetAbsOrigin(Vector(-2056, -2056, -2056));
	g_pErasePaintStream->SetAbsOrigin(Vector(-2056, -2056, -2056));

	g_pBouncePaintStream->Spawn();
	g_pSpeedPaintStream->Spawn();
	g_pStickPaintStream->Spawn();
	g_pErasePaintStream->Spawn();

#endif
}

void CPaintBlobManager::LevelShutdownPostEntity() {

#ifdef CLIENT_DLL

#else 

#endif
}

#ifdef CLIENT_DLL
void CPaintBlobManager::PreRender() {

}

void CPaintBlobManager::Update(float frametime) {

}

void CPaintBlobManager::Render() {

}

void CPaintBlobManager::PostRender() {

}

#else

void CPaintBlobManager::PreClientUpdate() {

}

CPaintBlobStream* CPaintBlobManager::GetStream(int streamIndex) {
	switch (streamIndex) {
	case 0:
		return g_pBouncePaintStream;
	case 1:
		return g_pSpeedPaintStream;
	case 2:
		return g_pStickPaintStream;
	case 3:
		return g_pErasePaintStream;
	}
}

int CPaintBlobManager::GetStreamIndex(int paintType) {
	switch (paintType) {
	case BOUNCE_POWER:
		return 0;
	case SPEED_POWER:
		return 1;
	case PORTAL_POWER:
		return 2;
	case NO_POWER:
		return 3;
	}
}

CUtlVector<int>* CPaintBlobManager::GetRemovalQueue(int streamIndex) {
	switch (streamIndex) {
	case 0:
		return &g_BounceRemovalQueue;
	case 1:
		return &g_SpeedRemovalQueue;
	case 2:
		return &g_StickRemovalQueue;
	case 3:
		return &g_EraseRemovalQueue;
	}
}

void CPaintBlobManager::CreateBlob(Vector origin, Vector velocity, int batch) {
	GetStream(batch)->AddParticle(origin, velocity);
}

void CPaintBlobManager::QueueBlobForRemoval(int particleIndex, int batch) {
	GetRemovalQueue(batch)->AddToTail(particleIndex);
}

void CPaintBlobManager::RemoveQueuedBlobs() {
	
	for (int i = 0; i < 4; i++) {

		CPaintBlobStream* pStream = GetStream(i);
		CUtlVector<int>* pQueue = GetRemovalQueue(i);
		
		// for the safety
		if (!pStream || !pQueue)
			continue;

		if (pQueue->Count() == 0)
			continue;

		// destroy all phys particles and set pointers -> nullptr
		for (int k = 0; k < pQueue->Count(); k++) {

			int idx = pQueue->Element(k);
			IPhysicsObject* pPhys = pStream->m_vecParticles[idx];

			if (!pPhys)
				continue;

			pStream->m_vecParticles[idx] = nullptr;
			pStream->m_nFreeIndices.AddToTail(idx);
			pStream->m_nIndices.FindAndFastRemove(idx);
			PhysDestroyObject(pPhys, pStream);
			pStream->m_nActiveParticlesInternal--;
			pStream->m_vecSurfacePositions[idx] = pStream->m_vecStart;
		}

		pQueue->RemoveAll();
	}
}

void CPaintBlobManager::FrameUpdatePreEntityThink() {
	g_pBouncePaintStream->PurgeNewIndices();
	g_pSpeedPaintStream->PurgeNewIndices();
	g_pStickPaintStream->PurgeNewIndices();
	g_pErasePaintStream->PurgeNewIndices();
}

void CPaintBlobManager::FrameUpdatePostEntityThink() {
	RemoveQueuedBlobs();
}

#endif

static CPaintBlobManager g_Blobulator;

IGameSystem* PaintBlobManager_System() {
	return &g_Blobulator;
}

CPaintBlobManager* PaintBlobManager() {
	return &g_Blobulator;
}