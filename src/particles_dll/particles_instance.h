#pragma once
#include "particles/particles.h"
#include "particles/particles_collection.h"
#include "materialsystem_new.h"

//extern CParticleSystemMgr m_pParticleSystemMgr;
static CMatRenderContextNew m_sRenderContext;

class CParticleSystemMgrPtr : public IParticleSystemMgr {
public:

	CParticleSystemMgrPtr(CParticleSystemMgr* pMgr) { m_pParticleSystemMgr = pMgr; }

	// Initialize the particle system
	virtual bool Init(IParticleSystemQuery* pQuery, bool bAllowPrecache) { return m_pParticleSystemMgr->Init(pQuery, bAllowPrecache); }
	virtual bool Init2(IParticleSystemQuery* pQuery, bool bAllowPrecache) { return m_pParticleSystemMgr->Init(pQuery, bAllowPrecache); }

	// methods to add builtin operators. If you don't call these at startup, you won't be able to sim or draw. These are done separately from Init, so that
	// the server can omit the code needed for rendering/simulation, if desired.
	virtual void AddBuiltinSimulationOperators(void) { m_pParticleSystemMgr->AddBuiltinSimulationOperators(); }
	virtual void AddBuiltinRenderingOperators(void) { m_pParticleSystemMgr->AddBuiltinRenderingOperators(); }

	// Registration of known operators
	virtual void AddParticleOperator(ParticleFunctionType_t nOpType, IParticleOperatorDefinition* pOpFactory) { m_pParticleSystemMgr->AddParticleOperator(nOpType, pOpFactory); }

	// Read a particle config file, add it to the list of particle configs
	virtual bool ReadParticleConfigFile(const char* pFileName, bool bPrecache, bool bDecommitTempMemory = true) { return m_pParticleSystemMgr->ReadParticleConfigFile(pFileName, bPrecache, bDecommitTempMemory); }
	virtual bool ReadParticleConfigFile(CUtlBuffer& buf, bool bPrecache, bool bDecommitTempMemory = true, const char* pFileName = NULL) { return m_pParticleSystemMgr->ReadParticleConfigFile(buf, bPrecache, bDecommitTempMemory, pFileName); }
	virtual void DecommitTempMemory() { m_pParticleSystemMgr->DecommitTempMemory(); }

	virtual bool ReadParticleConfigFile2(const char* pFileName, bool bPrecache, bool bDecommitTempMemory = true) { return m_pParticleSystemMgr->ReadParticleConfigFile(pFileName, bPrecache, bDecommitTempMemory); }

	// For recording, write a specific particle system to a CUtlBuffer in DMX format
	virtual bool WriteParticleConfigFile(const char* pParticleSystemName, CUtlBuffer& buf, bool bPreventNameBasedLookup = false) { return m_pParticleSystemMgr->WriteParticleConfigFile(pParticleSystemName, buf, bPreventNameBasedLookup); }
	virtual bool WriteParticleConfigFile(const DmObjectId_t& id, CUtlBuffer& buf, bool bPreventNameBasedLookup = false) { return m_pParticleSystemMgr->WriteParticleConfigFile(id, buf, bPreventNameBasedLookup); }

	// create a particle system by name. returns null if one of that name does not exist
	virtual IParticleCollection* CreateParticleCollection(const char* pParticleSystemName, float flDelay = 0.0f, int nRandomSeed = 0) {
		
		return new CParticleCollectionPtr(m_pParticleSystemMgr->CreateParticleCollection(pParticleSystemName, flDelay, nRandomSeed));
	}

	virtual IParticleCollection* CreateParticleCollection(ParticleSystemHandle_t particleSystemName, float flDelay = 0.0f, int nRandomSeed = 0) {
	
		return new CParticleCollectionPtr(m_pParticleSystemMgr->CreateParticleCollection(particleSystemName, flDelay, nRandomSeed));
	}

	// create a particle system given a particle system id
	virtual IParticleCollection* CreateParticleCollection(const DmObjectId_t& id, float flDelay = 0.0f, int nRandomSeed = 0) {
	
		return new CParticleCollectionPtr(m_pParticleSystemMgr->CreateParticleCollection(id, flDelay, nRandomSeed));
	}

	// Is a particular particle system defined?
	virtual bool IsParticleSystemDefined(const char* pParticleSystemName) { return m_pParticleSystemMgr->IsParticleSystemDefined(pParticleSystemName); }
	virtual bool IsParticleSystemDefined(const DmObjectId_t& id) { return m_pParticleSystemMgr->IsParticleSystemDefined(id); }

	// Returns the index of the specified particle system. 
	virtual ParticleSystemHandle_t GetParticleSystemIndex(const char* pParticleSystemName) { return m_pParticleSystemMgr->GetParticleSystemIndex(pParticleSystemName); }
	virtual ParticleSystemHandle_t FindOrAddParticleSystemIndex(const char* pParticleSystemName) { return m_pParticleSystemMgr->FindOrAddParticleSystemIndex(pParticleSystemName); }

	// Returns the name of the specified particle system.
	virtual const char* GetParticleSystemNameFromIndex(ParticleSystemHandle_t iIndex) { return m_pParticleSystemMgr->GetParticleSystemNameFromIndex(iIndex); }

	// Return the number of particle systems in our dictionary
	virtual int GetParticleSystemCount(void) { return m_pParticleSystemMgr->GetParticleSystemCount(); }

	// Get the label for a filter
	virtual const char* GetFilterName(ParticleFilterType_t nFilterType) const { return m_pParticleSystemMgr->GetFilterName(nFilterType); }

	// call to get available particle operator definitions
	// NOTE: FUNCTION_CHILDREN will return a faked one, for ease of writing the editor
	virtual CUtlVector< IParticleOperatorDefinition*>& GetAvailableParticleOperatorList(ParticleFunctionType_t nWhichList) { return m_pParticleSystemMgr->GetAvailableParticleOperatorList(nWhichList); }

	virtual void GetParticleSystemsInFile(const char* pFileName, CUtlVector<CUtlString>* pOutSystemNameList) { m_pParticleSystemMgr->GetParticleSystemsInFile(pFileName, pOutSystemNameList); }
	virtual void GetParticleSystemsInBuffer(CUtlBuffer& buf, CUtlVector<CUtlString>* pOutSystemNameList) { m_pParticleSystemMgr->GetParticleSystemsInBuffer(buf, pOutSystemNameList); }

	// Returns the unpack structure for a particle system definition
	virtual const DmxElementUnpackStructure_t* GetParticleSystemDefinitionUnpackStructure() { return m_pParticleSystemMgr->GetParticleSystemDefinitionUnpackStructure(); }

	// Particle sheet management
	virtual void ShouldLoadSheets(bool bLoadSheets) { m_pParticleSystemMgr->ShouldLoadSheets(bLoadSheets); }
	virtual CSheet* FindOrLoadSheet(CParticleSystemDefinition* pDef) { return m_pParticleSystemMgr->FindOrLoadSheet(pDef); }
	virtual CSheet* FindOrLoadSheet2(CParticleSystemDefinition* pDef) { return m_pParticleSystemMgr->FindOrLoadSheet2(pDef); }

	virtual void FlushAllSheets(void) { m_pParticleSystemMgr->FlushAllSheets(); }

	// Render cache used to render opaque particle collections
	virtual void ResetRenderCache(void) { m_pParticleSystemMgr->ResetRenderCache(); }
	//virtual void AddToRenderCache(IParticleCollection* pParticles) { m_pParticleSystemMgr->AddToRenderCache(static_cast<CParticleCollectionPtr*>(pParticles)->m_pParticle); }
	virtual void AddToRenderCache(CParticleCollection* pParticles) { m_pParticleSystemMgr->AddToRenderCache(pParticles); }
	virtual void DrawRenderCache(bool bShadowDepth) { m_pParticleSystemMgr->DrawRenderCache(bShadowDepth); }

	virtual IParticleSystemQuery* Query(void) { return m_pParticleSystemMgr->Query(); }

	// return the particle field name
	virtual const char* GetParticleFieldName(int nParticleField) const { return m_pParticleSystemMgr->GetParticleFieldName(nParticleField); }

	// WARNING: the pointer returned by this function may be invalidated 
	// *at any time* by the editor, so do not ever cache it.
	virtual CParticleSystemDefinition* FindParticleSystem(const char* pName) {
		auto ret = m_pParticleSystemMgr->FindParticleSystem(pName);

		// add more to the stack ptr here
		__asm
		{
			sub esp, 8
		}
		
		return ret;
	}
	virtual CParticleSystemDefinition* FindParticleSystem(const DmObjectId_t& id) { return m_pParticleSystemMgr->FindParticleSystem(id); }
	virtual CParticleSystemDefinition* FindParticleSystem(ParticleSystemHandle_t hParticleSystem) { return m_pParticleSystemMgr->FindParticleSystem(hParticleSystem); }
	virtual CParticleSystemDefinition* FindPrecachedParticleSystem(int nPrecacheIndex) { return m_pParticleSystemMgr->FindPrecachedParticleSystem(nPrecacheIndex); }

	// Method for overriding a parameter in a loaded particle system definition
	virtual bool OverrideEffectParameter(const char* pParticleSystemName, const char* pOperatorName, const char* pParameterName, const char* pParameterValue) { return m_pParticleSystemMgr->OverrideEffectParameter(pParticleSystemName, pOperatorName, pParameterName, pParameterValue); }


	virtual void CommitProfileInformation(bool bCommit) { m_pParticleSystemMgr->CommitProfileInformation(bCommit); }			// call after simulation, if you want
	// sim time recorded. if oyu pass
	// flase, info will be thrown away and
	// uncomitted time reset.  Having this
	// function lets you only record
	// profile data for slow frames if
	// desired.


	virtual void DumpProfileInformation(void) { m_pParticleSystemMgr->DumpProfileInformation(); }					// write particle_profile.csv

	virtual void DumpParticleList(const char* pNameSubstring) { m_pParticleSystemMgr->DumpParticleList(pNameSubstring); }

	// Cache/uncache materials used by particle systems
	virtual void PrecacheParticleSystem(int nStringNumber, const char* pName) { m_pParticleSystemMgr->PrecacheParticleSystem(nStringNumber, pName); }
	virtual void UncacheAllParticleSystems() { m_pParticleSystemMgr->UncacheAllParticleSystems(); }

	virtual void PrecacheParticleSystem2(int nStringNumber, const char* pName) { m_pParticleSystemMgr->PrecacheParticleSystem2(nStringNumber, pName); }

	// Sets the last simulation time, used for particle system sleeping logic
	virtual void SetLastSimulationTime(float flTime) { m_pParticleSystemMgr->SetLastSimulationTime(flTime); }
	virtual float GetLastSimulationTime() const { return m_pParticleSystemMgr->GetLastSimulationTime(); }

	// Sets the last simulation duration ( the amount of time we spent simulating particle ) last frame
	// Used to fallback to cheaper particle systems under load
	virtual void SetLastSimulationDuration(float flDuration) { m_pParticleSystemMgr->SetLastSimulationDuration(flDuration); }
	virtual float GetLastSimulationDuration() const { return m_pParticleSystemMgr->GetLastSimulationDuration(); }

	virtual void SetFallbackParameters(float flBase, float flMultiplier, float flSimFallbackBaseMultiplier, float flSimThresholdMs) { m_pParticleSystemMgr->SetFallbackParameters(flBase, flMultiplier, flSimFallbackBaseMultiplier, flSimThresholdMs); }
	virtual float GetFallbackBase() const { return m_pParticleSystemMgr->GetFallbackBase(); }
	virtual float GetFallbackMultiplier() const { return m_pParticleSystemMgr->GetFallbackMultiplier(); }
	virtual float GetSimFallbackThresholdMs() const { return m_pParticleSystemMgr->GetSimFallbackThresholdMs(); }
	virtual float GetSimFallbackBaseMultiplier() const { return m_pParticleSystemMgr->GetSimFallbackBaseMultiplier(); }

	virtual void SetSystemLevel(int nCPULevel, int nGPULevel) { m_pParticleSystemMgr->SetSystemLevel(nCPULevel, nGPULevel); }
	virtual int GetParticleCPULevel() const { return m_pParticleSystemMgr->GetParticleCPULevel(); }
	virtual int GetParticleGPULevel() const { return m_pParticleSystemMgr->GetParticleGPULevel(); }

	virtual void LevelShutdown(void) { m_pParticleSystemMgr->LevelShutdown(); }								// called at level unload time


	virtual void FrameUpdate(void) { /*m_pParticleSystemMgr->FrameUpdate();*/ }								// call this once per frame on main thread

	// Particle attribute query funcs
	virtual int GetParticleAttributeByName(const char* pAttribute) const { return m_pParticleSystemMgr->GetParticleAttributeByName(pAttribute); }		// SLOW! returns -1 on error
	virtual const char* GetParticleAttributeName(int nAttribute) const { return m_pParticleSystemMgr->GetParticleAttributeName(nAttribute); }		// returns 'unknown' on error
	virtual EAttributeDataType GetParticleAttributeDataType(int nAttribute) const { return m_pParticleSystemMgr->GetParticleAttributeDataType(nAttribute); }

	virtual void GetParticleManifest(CUtlVector<CUtlString>& list) { ::GetParticleManifest(list); }
	virtual bool GetShouldAlwaysPrecache(CParticleSystemDefinition* pDef) const { return pDef->ShouldAlwaysPrecache();/*g_pParticleSystemMgr->GetShouldAlwaysPrecache(pDef);*/ }
	virtual IParticleCollection* CreateParticleCollectionPtr() { return reinterpret_cast<IParticleCollection*>(m_pCollectionKeeper.Create()); }

	virtual int CountChildParticleSystems(CParticleCollection* pCollection);

	virtual IParticleCollection* GetDefFirstCollection(CParticleSystemDefinition* pDef) { return new CParticleCollectionPtr(pDef->FirstCollection()); }
	virtual const char* GetDefName(CParticleSystemDefinition* pDef) { return pDef->GetName(); }
	virtual CParticleSystemDefinition* GetDefFallback(CParticleSystemDefinition* pDef) { return pDef->GetFallbackReplacementDefinition(); }

	void InitCollection(CParticleCollection* pCollection, CParticleSystemDefinition* pEffect) { pCollection->Init(pEffect); }
	virtual void RenderCollection(CParticleCollection* pCollection, void* pRenderContext, const Vector4D& vecDiffuseModulation, bool bTranslucentOnly = false, void* pCameraObject = NULL) {

		m_sRenderContext.s_pOldRenderContext = (IMatRenderContextOld*)pRenderContext;
		pCollection->Render(&m_sRenderContext, vecDiffuseModulation, bTranslucentOnly, pCameraObject);
	}

private:

	CParticleSystemMgr* m_pParticleSystemMgr;

	class CParticleCollectionPtrMgr {
	public:

		~CParticleCollectionPtrMgr() {
			m_Ptrs.Purge();
		}

		CParticleCollectionPtr* Create() {
			int nIdx = m_Ptrs.Count();
			m_Ptrs.AddToTail(CParticleCollectionPtr());

			return &m_Ptrs[nIdx];
		}

	private:

		CUtlVector<CParticleCollectionPtr> m_Ptrs;
	} m_pCollectionKeeper;
};