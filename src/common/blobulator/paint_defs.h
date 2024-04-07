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

//enum PaintBlobType_t {
//	BOUNCE_POWER = 0,
//	REFLECT_POWER,
//	SPEED_POWER,
//	PORTAL_POWER,
//	NO_POWER,
//};