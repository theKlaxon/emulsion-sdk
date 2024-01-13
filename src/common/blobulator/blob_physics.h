#pragma once
#include "physics/phys_particle.h"
#include "physics/phys_particle_cache.h"
#include "physics/phys_tiler.h"
#include "parented_func.h"

class COpPtr_DoPhysics : public IParentedFuncPtr {
public:
	virtual void Do(void* pData);
} extern g_BlobDoPhysics;
