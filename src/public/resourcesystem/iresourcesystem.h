#pragma once
#include "tier0/basetypes.h"
#include "appframework/IAppSystem.h"
#include "tier2/resourceprecacher.h"

class IResourceSystem : public IBaseInterface
{
public:

	virtual void Init(CreateInterfaceFn factory) = 0;
	virtual void Shutdown() = 0;

	virtual void InstallStringTableCallback(PrecacheSystem_t tModule, const char* pTableName) = 0;

	virtual void PrecacheMaterial(const char* pMaterialName) = 0;
	virtual void PrecacheEffect(const char* pEffectName) = 0;
	virtual int	 PrecacheParticleSystem(const char* pParticleSystemName) = 0;

	virtual void UncacheAllMaterials() = 0;
	virtual void ProcessCacheUsedMaterials() = 0;
};
