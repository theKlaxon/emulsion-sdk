#pragma once
#include "materialsystem/imaterial.h"
#include "parented_func.h"

class IPaintStream {
public:

	void SetMaterial(const char* material) { m_strMatName = material; m_pMaterial = materials->FindMaterial(material, TEXTURE_GROUP_OTHER); }
	IMaterial* GetMaterial() { return m_pMaterial; }

	//virtual void Render() = 0;

protected:

	const char* m_strMatName;
	IMaterial* m_pMaterial;
};

