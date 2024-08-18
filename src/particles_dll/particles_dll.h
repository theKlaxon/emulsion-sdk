#pragma once
#include "particles/iparticlesdll.h"
#include "proxy_filesystem.h"
#include "proxy_imaterialsystem.h"
#include "particles_instance.h"

//#undef g_pFullFilesystem
//#undef g_pMaterialSystem

extern IFileSystem* g_pFullFileSystem;
extern IMaterialSystem* g_pMaterialSystem;
extern IMaterialSystem* materials;

class CParticles : public IParticlesDLL {
public:

	virtual void Initialize(CreateInterfaceFn factory);
	virtual void Shutdown();

	virtual IParticleSystemMgr* GetMgrInstance();

protected:

};

