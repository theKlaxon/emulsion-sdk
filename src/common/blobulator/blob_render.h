#pragma once
#ifdef CLIENT_DLL
#include "parented_func.h"
#include "smartarray.h"
#include "implicit/imp_particle.h"

class COpPtr_DoRender : public IParentedFuncPtr {
public:
	virtual void Do(void* pData);
} extern g_BlobDoRender;

class CornerInfo;
class IndexTriVertexBuffer;
class ProjectingParticleCache;

extern bool calcSign(unsigned char, unsigned char, unsigned char, float, float, float, ProjectingParticleCache*);
extern void calcSign2(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);

extern void calcCornerNormal(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
extern void calcCornerNormalColorUVTan(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
extern void calcCornerNormalColor(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);

extern void calcVertexNormal(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
extern void calcVertexNormalDebugColor(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
extern void calcVertexNormalNColorUVTan(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
extern void calcVertexNormalNColor(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);

#endif