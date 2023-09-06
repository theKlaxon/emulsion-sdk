#include "cbase.h"
#include "FunctionProxy.h"
#include "materialsystem/IMaterial.h"
#include "materialsystem/imaterialproxy.h"
#include "materialsystem/imaterialvar.h"
#include "imaterialproxydict.h"

// gonna use to make fancy paint on cubes n shit
class C_CubePaintProxy : public IMaterialProxy {
public:

	virtual bool Init(IMaterial* pMaterial, KeyValues* pKeyValues);
	virtual void OnBind(void* pEnt);
	virtual void Release(void) { delete this; }
	virtual IMaterial* GetMaterial();

private:

};

bool C_CubePaintProxy::Init(IMaterial* pMaterial, KeyValues* pKeyValues) {
	return false;
}

void C_CubePaintProxy::OnBind(void* pEnt) {

}

IMaterial* C_CubePaintProxy::GetMaterial() {
	return NULL;
}

EXPOSE_MATERIAL_PROXY(C_CubePaintProxy, CubePaintProxy);