#include "particles_dll.h"
#include "particles_instance.h"

static CParticleSystemMgrPtr g_ParticleMgrInstance(g_pParticleSystemMgr);
static IFileSystem g_FileSystemProxy;
static IMaterialSystem g_MaterialSystemProxy;

IFileSystem* g_pFullFileSystem;
IMaterialSystem* g_pMaterialSystem;
IMaterialSystem* materials;

IFileSystemNew* filesystem;
IMaterialSystemNew* g_pMaterialsNew;

EXPOSE_SINGLE_INTERFACE(CParticles, IParticlesDLL, PARTICLES_INTERFACE_VERSION);

//CParticles g_Particles;
//EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CParticles, IParticlesDLL, PARTICLES_INTERFACE_VERSION, g_Particles);

void CParticles::Initialize(CreateInterfaceFn factory) {

    MathLib_Init(2.2f, 2.2f, 0.0f, 2.0f);

    ConnectTier1Libraries(&factory, 1);
    ConnectTier2Libraries(&factory, 1);
    ConVar_Register(FCVAR_CLIENTDLL);

    filesystem = (IFileSystemNew*)factory(FILESYSTEM_INTERFACE_VERSION, NULL);  // setup the interface the proxy will use
    g_pFullFileSystem = &g_FileSystemProxy;                                     // setup the actual proxy
    
    g_pMaterialsNew = (IMaterialSystemNew*)factory(MATERIAL_SYSTEM_INTERFACE_VERSION, NULL);
    g_pMaterialSystem = &g_MaterialSystemProxy;
    materials = &g_MaterialSystemProxy;
}

void CParticles::Shutdown() {

}

IParticleSystemMgr* CParticles::GetMgrInstance() {
	return &g_ParticleMgrInstance;
}