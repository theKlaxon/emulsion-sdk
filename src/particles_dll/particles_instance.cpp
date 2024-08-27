#include "particles/particles.h"
#include "particles_instance.h"
#include "particles_dll.h"
#include <KeyValues.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//static CParticleSystemDictionary* g_Dict = new CParticleSystemDictionary;

bool bInitted = false;

bool CParticleSystemMgr::Init2(IParticleSystemQuery* pQuery, bool bAllowPrecache) {

	if (bInitted)
		return true;

	if (pQuery == nullptr)
		return false;

	KeyValues* pKVar3;
	//CParticleSystemDictionary* pThis00 = new CParticleSystemDictionary();

	m_pParticleSystemDictionary = new CParticleSystemDictionary();// std::make_unique<CParticleSystemDictionary>().get();
	m_pParticleSystemDictionary->m_ParticleNameMap.Clear();

	m_pQuery = pQuery;

	m_bAllowPrecache = bAllowPrecache;
	m_bUsingDefaultQuery = false;

	COM_TimestampedLog("CParticleSystemMgr->Init2 Start");

	if (g_pMaterialSystem != nullptr) {
		pKVar3 = new KeyValues("DepthWrite");
		pKVar3->SetInt("$no_fullbright", 1);
		pKVar3->SetInt("$model", 0);
		pKVar3->SetInt("$alphatest", 1);

		m_pShadowDepthMaterial = g_pMaterialSystem->CreateMaterial("__particlesDepthWrite", pKVar3);
		bInitted = true;
	}

	SeedRandSIMD(0xbc614e);

	return true;
}

bool CParticleSystemMgr::ReadParticleConfigFile2(const char* pFileName, bool bPrecache, bool bDecommitTempMemory /*= true*/) {

    bool cache = false;

    if (*pFileName == '!') {
        cache = true;
        pFileName += 1;
    }

    char local0[244];
    V_StripExtension(pFileName, local0, 0x104);

#ifdef DEBUG
    Msg("Reading particle file: %s\n", pFileName);
#endif

    CUtlBuffer local1;
    if (!g_pFullFileSystem->ReadFile(pFileName, "GAME", local1)) {
        Msg("Invalid or absent particle file: %s\n", local0);
        return false;
    }
	
    ReadParticleDefinitions(local1, local0, cache, false);

    return true;
}

void CParticleSystemMgr::PrecacheParticleSystem2(int nStringNumber, const char* pName) {
    PrecacheParticleSystem(nStringNumber, pName); // TEMPORARY, REMOVE THIS 2ND FUNCTION (not need no more)
}

CSheet* CParticleSystemMgr::FindOrLoadSheet2(CParticleSystemDefinition* pDef) {

    return nullptr;
}

//void CParticleSystemMgr::GetParticleManifest(CUtlVector<CUtlString>& list) {
//
//	// get the global version
//	
//
//}

static int CountChildParticleSystems( CParticleCollection *p )
{
	int nCount = 1;
	for ( CParticleCollection *pChild = p->m_Children.m_pHead; pChild; pChild = pChild->m_pNext )
	{
		nCount += CountChildParticleSystems( pChild );
	}
	return nCount;
}

//int CParticleSystemMgrPtr::CountChildParticleSystems(IParticleCollection* pCollection) {
//
//	CParticleCollection* p = static_cast<CParticleCollectionPtr*>(pCollection)->m_pParticle;
//
//	return ::CountChildParticleSystems(p);
//}

int CParticleSystemMgrPtr::CountChildParticleSystems(CParticleCollection* pCollection) {

	return ::CountChildParticleSystems(pCollection);
}

// CParticleCollection accessors

#include "particles_materialsystem.h"
#include "materialsystem/imesh.h"

IMesh* CMatRenderContextNew::GetDynamicMesh(
	bool buffered,
	IMesh* pVertexOverride,
	IMesh* pIndexOverride,
	IMaterial* pAutoBind) {

	IMeshOld* old = s_pOldRenderContext->GetDynamicMesh(buffered, pVertexOverride, pIndexOverride, pAutoBind);

	return (IMesh*)(new CMeshPtr(old));

}
