#pragma once
#ifdef CLIENT_DLL
#include "parented_func.h"
#include "smartarray.h"
#include "implicit/imp_particle.h"

class COpPtr_DoRender : public IParentedFuncPtr {
public:
	virtual void Do(void* pData);
} extern g_BlobDoRender;
#endif