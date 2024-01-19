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

namespace Paint {

	void CreateBlob(Vector center, Vector velocity) {

		objectparams_t params = g_PhysDefaultObjectParams;
		params.pGameData = static_cast<void*>(nullptr);

		int surfaceIndex = physprops->GetSurfaceIndex("water");
		IPhysicsObject* pObject = physenv->CreateSphereObject(8.0f, surfaceIndex, center, QAngle(0, 0, 0), &params, false);

		pObject->SetVelocity(&velocity, NULL);
		PhysSetGameFlags(pObject, FVPHYSICS_NO_SELF_COLLISIONS | FVPHYSICS_MULTIOBJECT_ENTITY); // call collisionruleschanged if this changes dynamically
		//pObject->SetGameIndex(i);

		pObject->Sleep(); // sleep it until we are parented correctly
		pObject->SetMass(10.0f);
		pObject->EnableGravity(true);
		pObject->EnableDrag(true);

		float flDamping = 0.5f;
		float flAngDamping = 0.5f;
		pObject->SetDamping(&flDamping, &flAngDamping);

		g_pBouncePaintStream->AddParticle(pObject, velocity);
	}

}

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

	g_pBouncePaintStream->SetAbsOrigin(Vector(128, 256, 64));
	g_pSpeedPaintStream->SetAbsOrigin(Vector(128, 64, 64));
	g_pStickPaintStream->SetAbsOrigin(Vector(128, 64, 64));

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

void CBlobManager::CreateBlob(Vector origin, Vector velocity, int batch) {

	switch (batch) {
	case 0:
		g_pBouncePaintStream->AddParticle(origin, velocity);
		break;
	case 1:
		g_pSpeedPaintStream->AddParticle(origin, velocity);
		break;
	case 2:
		g_pStickPaintStream->AddParticle(origin, velocity);
		break;
	}
}

#endif

static CBlobManager g_Blobulator;

IGameSystem* BlobulatorSystem() {
	return &g_Blobulator;
}