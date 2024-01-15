#include "cbase.h"
#include "blob_manager.h"

#ifdef CLIENT_DLL
#include "implicit/imp_tiler.h"
#include "implicit/sweep_renderer.h"
#include "smartarray.h"
COpPtr_DoRender g_BlobDoRender = COpPtr_DoRender();
#else 
#include "blob_physics.h"
COpPtr_DoPhysics g_BlobDoPhysics = COpPtr_DoPhysics();
#endif

COpPtr_DoNothing g_BlobDoNothing = COpPtr_DoNothing();

// change these defines to change the behaviour of the renderer
#define CUBE_WIDTH 5.0F
#define CUTOFF_RAD 50.0F
#define RENDER_RAD 30.0F

CBlobManager::~CBlobManager() {
#ifdef CLIENT_DLL
	m_Batches.Purge();
#endif
}

bool CBlobManager::Init() {
	
	m_flCubeWidth = CUBE_WIDTH;
	m_flCutoffRadius = CUTOFF_RAD;
	m_flRenderRadius = RENDER_RAD;
	m_nCurBatch = 0;
	
#ifdef CLIENT_DLL
	SetBlobRenderFunc(&g_BlobDoNothing);

	m_Batches.AddToTail(new CBucketBlobBatch());
	m_Batches.AddToTail(new CBucketBlobBatch());

	m_Batches[0]->SetMaterial("paintblobs/blob_surface_bounce");
	m_Batches[1]->SetMaterial("paintblobs/blob_surface_speed");

#else
	SetBlobPhysicsFunc(&g_BlobDoNothing);
#endif
	
	return true;
}

void CBlobManager::Shutdown() {

}

void CBlobManager::LevelInitPostEntity() {

#ifdef CLIENT_DLL
	Vector center = Vector(128, 64, 32);
	Vector center2 = Vector(128, 64, 95);
	Vector center3 = Vector(128, 125, 110);
	Vector center4 = Vector(128, 180, 88);
	BlobData_t data;
	BlobData_t data2;
	BlobData_t data3;
	BlobData_t data4;

	data.m_Center = center;
	data.m_flScale = 1.0f;

	data2.m_Center = center2;
	data2.m_flScale = 1.0f;

	data3.m_Center = center3;
	data3.m_flScale = 1.0f;

	data4.m_Center = center4;
	data4.m_flScale = 1.75f;

	//m_Batches[0]->AddParticle(data);
	//m_Batches[0]->AddParticle(data2);
	//m_Batches[1]->AddParticle(data3);
	//m_Batches[1]->AddParticle(data4);

#else 

	SetBlobPhysicsFunc(&g_BlobDoPhysics);

#endif
}

void CBlobManager::LevelShutdownPostEntity() {

#ifdef CLIENT_DLL
	SetBlobRenderFunc(&g_BlobDoNothing);
	
	for (int i = 0; i < BLOB_BATCH_COUNT; i++) {
		m_Batches[i]->Cleanup();
	}

	m_nCurBatch = 0;

#else 
	SetBlobPhysicsFunc(&g_BlobDoNothing);
#endif
}

#ifdef CLIENT_DLL
void CBlobManager::PreRender() {
	SetBlobRenderFunc(&g_BlobDoRender);
}

void CBlobManager::Update(float frametime) {

}

void CBlobManager::PostRender() {

	for (m_nCurBatch = 0; m_nCurBatch < BLOB_BATCH_COUNT; m_nCurBatch++)
		pRenderFunc->Do(m_Batches[m_nCurBatch]);
	
}

void CBlobManager::SetBlobRenderFunc(IParentedFuncPtr* ptr) {
	ptr->SetParent(this);
	pRenderFunc = ptr;
}

#else

void CBlobManager::SetBlobPhysicsFunc(IParentedFuncPtr* ptr) {
	pPhysicsFunc = ptr;
}

void CBlobManager::PreClientUpdate() {
	pPhysicsFunc->Do(nullptr);
}

void CBlobManager::CreateBlob(Vector origin, float radius, int batch) {

	BlobData_t data;
	data.m_Center = origin;
	data.m_flScale = radius;



}

#endif

static CBlobManager g_Blobulator;

IGameSystem* BlobulatorSystem() {
	return &g_Blobulator;
}