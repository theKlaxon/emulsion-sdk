#include "cbase.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialproxy.h"
#include "imaterialproxydict.h"
#include "proxyentity.h"

class C_FizzlerProxy : public CEntityMaterialProxy {
public:

	virtual bool Init(IMaterial* pMaterial, KeyValues* pKeyValues);
	virtual IMaterial* GetMaterial()
	{
		return m_pMaterial;
	}

private:

	virtual void OnBind(C_BaseEntity* pC_BaseEntity);

	int pad;					// idek
	IMaterial* m_pMaterial;		// this + 4
	IMaterialVar* m_pMatVars[6];
};

EXPOSE_MATERIAL_PROXY(C_FizzlerProxy, FizzlerVortexProxy)

bool C_FizzlerProxy::Init(IMaterial* pMaterial, KeyValues* pKeyValues) {

	IMaterialVar* pVar = 0;
	unsigned int nTokenCache = 0;

	m_pMaterial = pMaterial;

	pVar = pMaterial->FindVarFast("$FLOW_VORTEX_POS1", &nTokenCache); // this had a 3rd param when i was poking through ghidra, TODO update this to reflect that in the interface (default == 1 i think?)
	if (pVar)
		m_pMatVars[0] = pVar;
	else
		return 0;

	pVar = pMaterial->FindVarFast("$flow_vortex1", &nTokenCache);
	if (pVar)
		m_pMatVars[1] = pVar;
	else
		return 0;

	pVar = pMaterial->FindVarFast("$FLOW_VORTEX_POS2", &nTokenCache);
	if (pVar)
		m_pMatVars[2] = pVar;
	else
		return 0;

	pVar = pMaterial->FindVarFast("$flow_vortex2", &nTokenCache);
	if (pVar)
		m_pMatVars[3] = pVar;
	else
		return 0;

	pVar = pMaterial->FindVarFast("$flow_color_intensity", &nTokenCache);
	if (pVar)
		m_pMatVars[4] = pVar;
	else
		return 0;

	pVar = pMaterial->FindVarFast("$powerup", &nTokenCache);
	if (pVar)
		m_pMatVars[5] = pVar;
	else
		return 0;

	return nTokenCache;
}

void C_FizzlerProxy::OnBind(C_BaseEntity* pEntity) {

	// TODO: add a response to ents in here (mimic portal cleanser) for solid field
}