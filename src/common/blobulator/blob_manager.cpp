#include "cbase.h"
#include "blob_manager.h"

#ifdef CLIENT_DLL
#include "implicit/imp_tiler.h"
#include "implicit/sweep_renderer.h"
#include "smartarray.h"
#else 

#endif

CBlobManager::~CBlobManager() {
#ifdef CLIENT_DLL

#endif
}

bool CBlobManager::Init() {
	
	return true;
}

void CBlobManager::Shutdown() {

}

#ifndef CLIENT_DLL
#include "paint_stream.h"

CPaintBlobStream* g_pBouncePaintStream;
CPaintBlobStream* g_pSpeedPaintStream;
CPaintBlobStream* g_pStickPaintStream;

CUtlVector<int> g_BounceRemovalQueue;
CUtlVector<int> g_SpeedRemovalQueue;
CUtlVector<int> g_StickRemovalQueue;
#endif

void CBlobManager::LevelInitPostEntity() {

#ifdef CLIENT_DLL

#else 
	// create the paint streams
	g_pBouncePaintStream = (CPaintBlobStream*)CreateEntityByName("env_paint_stream");
	g_pSpeedPaintStream = (CPaintBlobStream*)CreateEntityByName("env_paint_stream");
	g_pStickPaintStream = (CPaintBlobStream*)CreateEntityByName("env_paint_stream");

	g_pBouncePaintStream->SetPaintType(0);
	g_pSpeedPaintStream->SetPaintType(2);
	g_pStickPaintStream->SetPaintType(3);

	g_pBouncePaintStream->SetAbsOrigin(Vector(0, 0, 0));
	g_pSpeedPaintStream->SetAbsOrigin(Vector(0, 0, 0));
	g_pStickPaintStream->SetAbsOrigin(Vector(0, 0, 0));

	g_pBouncePaintStream->Spawn();
	g_pSpeedPaintStream->Spawn();
	g_pStickPaintStream->Spawn();

#endif
}

void CBlobManager::LevelShutdownPostEntity() {

#ifdef CLIENT_DLL

#else 

#endif
}

#ifdef CLIENT_DLL
void CBlobManager::PreRender() {

}

void CBlobManager::Update(float frametime) {

}

void CBlobManager::Render() {

}

void CBlobManager::PostRender() {

}

#else

void CBlobManager::PreClientUpdate() {

}

CPaintBlobStream* CBlobManager::GetStream(int streamIndex) {
	switch (streamIndex) {
	case 0:
		return g_pBouncePaintStream;
	case 1:
		return g_pSpeedPaintStream;
	case 2:
		return g_pStickPaintStream;
	}
}

int CBlobManager::GetStreamIndex(int paintType) {
	switch (paintType) {
	case 0:
		return 0;
	case 2:
		return 1;
	case 3:
		return 2;
	}
}

CUtlVector<int>* CBlobManager::GetRemovalQueue(int streamIndex) {
	switch (streamIndex) {
	case 0:
		return &g_BounceRemovalQueue;
	case 1:
		return &g_SpeedRemovalQueue;
	case 2:
		return &g_StickRemovalQueue;
	}
}

void CBlobManager::CreateBlob(Vector origin, Vector velocity, int batch) {
	GetStream(batch)->AddParticle(origin, velocity);
}

void CBlobManager::QueueBlobForRemoval(int particleIndex, int batch) {
	GetRemovalQueue(batch)->AddToTail(particleIndex);
}

void CBlobManager::RemoveQueuedBlobs() {
	
	for (int i = 0; i < 3; i++) {

		CPaintBlobStream* pStream = GetStream(i);
		CUtlVector<int>* pQueue = GetRemovalQueue(i);
		
		// for the safety
		if (!pStream || !pQueue)
			continue;

		if (pQueue->Count() == 0)
			continue;

		// destroy all phys particles and set pointers -> nullptr
		for (int k = 0; k < pQueue->Count(); k++) {

			IPhysicsObject* pPhys = pStream->m_vecParticles[pQueue->Element(k)];
			pStream->m_vecParticles[pQueue->Element(k)] = nullptr;

			PhysDestroyObject(pPhys, pStream);
		}

		// remove all phys objs that are nullptr
		int deficit = 0;
		for (int k = 0; k < pStream->m_nActiveParticlesInternal; k++) 
			if (pStream->m_vecParticles[k] == nullptr) {
				pStream->m_vecParticles.Remove(k);
				deficit++;
			}

		pQueue->Purge();
		pStream->m_nActiveParticlesInternal -= deficit; // account for the particles we deleted
	}
}

void CBlobManager::FrameUpdatePostEntityThink() {
	RemoveQueuedBlobs();
}

#endif

static CBlobManager g_Blobulator;

IGameSystem* BlobulatorSystem() {
	return &g_Blobulator;
}

CBlobManager* PaintBlobManager() {
	return &g_Blobulator;
}