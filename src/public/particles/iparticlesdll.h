#pragma once
#include "appframework/IAppSystem.h"
#include "particles/particles.h"

class IParticleSystemMgrPtr;

class IParticlesDLL : public IBaseInterface {
public:

	virtual void Initialize(CreateInterfaceFn factory) = 0;
	virtual void Shutdown() = 0;

	virtual IParticleSystemMgr* GetMgrInstance() = 0;
};

extern IParticlesDLL* pParticles;

#define PARTICLES_INTERFACE_VERSION "ParticlesDLL_001"
