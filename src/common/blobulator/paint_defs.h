#pragma once
#include "materialsystem/imaterial.h"
#include "parented_func.h"

class IPaintStream {
public:

	void SetMaterial(const char* material) { m_strMatName = material; m_pMaterial = materials->FindMaterial(material, TEXTURE_GROUP_OTHER); }
	IMaterial* GetMaterial() { return m_pMaterial; }

protected:

	const char* m_strMatName;
	IMaterial* m_pMaterial;
};

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