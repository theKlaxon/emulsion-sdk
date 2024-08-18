#pragma once
#include "particles/particles.h"
#include "basehandle.h"

#ifdef PARTICLES_DLL

class CParticleCollectionPtr : public IParticleCollection {
public:
	CParticleCollectionPtr() {
		m_pParticle = new CParticleCollection;
	}
	CParticleCollectionPtr(CParticleCollection* pParticleCollection) {
		m_pParticle = pParticleCollection;
	}

	// Restarts the particle collection, stopping all non-continuous emitters
	virtual void Restart(EParticleRestartMode_t eMode = RESTART_NORMAL) { m_pParticle->Restart(eMode); }

	// compute bounds from particle list
	virtual void RecomputeBounds(void) { m_pParticle->RecomputeBounds(); }
	
	virtual void SetControlPoint(int nWhichPoint, const Vector& v) { m_pParticle->SetControlPoint(nWhichPoint, v); }
	virtual void SetControlPointObject(int nWhichPoint, void* pObject) { m_pParticle->SetControlPointObject(nWhichPoint, pObject); }

	virtual void SetControlPointOrientation(int nWhichPoint, const Vector& forward,
		const Vector& right, const Vector& up) { m_pParticle->SetControlPointOrientation(nWhichPoint, forward, right, up); }

	virtual void SetControlPointForwardVector(int nWhichPoint, const Vector& v) { m_pParticle->SetControlPointForwardVector(nWhichPoint, v); }
	virtual void SetControlPointUpVector(int nWhichPoint, const Vector& v) { m_pParticle->SetControlPointUpVector(nWhichPoint, v); }
	virtual void SetControlPointRightVector(int nWhichPoint, const Vector& v) { m_pParticle->SetControlPointRightVector(nWhichPoint, v); }
	virtual void SetControlPointParent(int nWhichPoint, int n) { m_pParticle->SetControlPointParent(nWhichPoint, n); }
	virtual void SetControlPointSnapshot(int nWhichPoint, CParticleSnapshot* pSnapshot) { m_pParticle->SetControlPointSnapshot(nWhichPoint, pSnapshot); }

	virtual void SetControlPointOrientation(int nWhichPoint, const Quaternion& q) { m_pParticle->SetControlPointOrientation(nWhichPoint, q); }

	// get the pointer to an attribute for a given particle.  
	// !!speed!! if you find yourself calling this anywhere that matters, 
	// you're not handling the simd-ness of the particle system well
	// and will have bad perf.
	virtual const float* GetFloatAttributePtr(int nAttribute, int nParticleNumber) const { return m_pParticle->GetFloatAttributePtr(nAttribute, nParticleNumber); }
	virtual const int* GetIntAttributePtr(int nAttribute, int nParticleNumber) const { return m_pParticle->GetIntAttributePtr(nAttribute, nParticleNumber); }
	virtual const fltx4* GetM128AttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pParticle->GetM128AttributePtr(nAttribute, pStrideOut); }
	virtual const FourVectors* Get4VAttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pParticle->Get4VAttributePtr(nAttribute, pStrideOut); }
	virtual const FourInts* Get4IAttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pParticle->Get4IAttributePtr(nAttribute, pStrideOut); }
	virtual const int* GetIntAttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pParticle->GetIntAttributePtr(nAttribute, pStrideOut); }

	virtual Vector GetVectorAttributeValue(int nAttribute, int nParticleNumber) const { return m_pParticle->GetVectorAttributeValue(nAttribute, nParticleNumber); }


	virtual int* GetIntAttributePtrForWrite(int nAttribute, int nParticleNumber) { return m_pParticle->GetIntAttributePtrForWrite(nAttribute, nParticleNumber); }

	virtual float* GetFloatAttributePtrForWrite(int nAttribute, int nParticleNumber) { return m_pParticle->GetFloatAttributePtrForWrite(nAttribute, nParticleNumber); }
	virtual fltx4* GetM128AttributePtrForWrite(int nAttribute, size_t* pStrideOut) { return m_pParticle->GetM128AttributePtrForWrite(nAttribute, pStrideOut); }
	virtual FourVectors* Get4VAttributePtrForWrite(int nAttribute, size_t* pStrideOut) { return m_pParticle->Get4VAttributePtrForWrite(nAttribute, pStrideOut); }

	virtual const float* GetInitialFloatAttributePtr(int nAttribute, int nParticleNumber) const { return m_pParticle->GetInitialFloatAttributePtr(nAttribute, nParticleNumber); }
	virtual const fltx4* GetInitialM128AttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pParticle->GetInitialM128AttributePtr(nAttribute, pStrideOut); }
	virtual const FourVectors* GetInitial4VAttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pParticle->GetInitial4VAttributePtr(nAttribute, pStrideOut); }
	virtual float* GetInitialFloatAttributePtrForWrite(int nAttribute, int nParticleNumber) { return m_pParticle->GetInitialFloatAttributePtrForWrite(nAttribute, nParticleNumber); }
	virtual fltx4* GetInitialM128AttributePtrForWrite(int nAttribute, size_t* pStrideOut) { return m_pParticle->GetInitialM128AttributePtrForWrite(nAttribute, pStrideOut); }

	virtual void Simulate(float dt) { m_pParticle->Simulate(dt); }
	virtual void SkipToTime(float t) { m_pParticle->SkipToTime(t); }

	// the camera objetc may be compared for equality against control point objects
	virtual void Render(IMatRenderContext* pRenderContext, const Vector4D& vecDiffuseModulation, bool bTranslucentOnly = false, void* pCameraObject = NULL) { m_pParticle->Render(pRenderContext, vecDiffuseModulation, bTranslucentOnly, pCameraObject); }

	virtual bool IsValid(void) const { return m_pParticle->IsValid(); }

	// this system and all children are valid
	virtual bool IsFullyValid(void) const { return m_pParticle->IsFullyValid(); }

	virtual const char* GetName() const { return m_pParticle->GetName(); }

	virtual bool DependsOnSystem(const char* pName) const { return m_pParticle->DependsOnSystem(pName); }

	// IsFinished returns true when a system has no particles and won't be creating any more
	virtual bool IsFinished(void) const { return m_pParticle->IsFinished(); }

	// Used to make sure we're accessing valid memory
	virtual bool IsValidAttributePtr(int nAttribute, const void* pPtr) const { return m_pParticle->IsValidAttributePtr(nAttribute, pPtr); }

	virtual void SwapPosAndPrevPos(void) { m_pParticle->SwapPosAndPrevPos(); }

	virtual void SetNActiveParticles(int nCount) { m_pParticle->SetNActiveParticles(nCount); }
	virtual void KillParticle(int nPidx, unsigned int nFlags = 0) { m_pParticle->KillParticle(nPidx, nFlags); }

	virtual void StopEmission(bool bInfiniteOnly = false, bool bRemoveAllParticles = false, bool bWakeOnStop = false, bool bPlayEndCap = false) { m_pParticle->StopEmission(bInfiniteOnly, bRemoveAllParticles, bWakeOnStop, bPlayEndCap); }
	virtual void StartEmission(bool bInfiniteOnly = false) { m_pParticle->StartEmission(bInfiniteOnly); }
	virtual void SetDormant(bool bDormant) { m_pParticle->SetDormant(bDormant); }
	virtual bool IsEmitting() const { return m_pParticle->IsEmitting(); }

	virtual const Vector& GetControlPointAtCurrentTime(int nControlPoint) const { return m_pParticle->GetControlPointAtCurrentTime(nControlPoint); }
	virtual void GetControlPointOrientationAtCurrentTime(int nControlPoint, Vector* pForward, Vector* pRight, Vector* pUp) const { m_pParticle->GetControlPointOrientationAtCurrentTime(nControlPoint, pForward, pRight, pUp); }
	virtual void GetControlPointTransformAtCurrentTime(int nControlPoint, matrix3x4_t* pMat) { m_pParticle->GetControlPointTransformAtCurrentTime(nControlPoint, pMat); }
	virtual void GetControlPointTransformAtCurrentTime(int nControlPoint, VMatrix* pMat) { m_pParticle->GetControlPointTransformAtCurrentTime(nControlPoint, pMat); }
	virtual int GetControlPointParent(int nControlPoint) const { return m_pParticle->GetControlPointParent(nControlPoint); }
	virtual CParticleSnapshot* GetControlPointSnapshot(int nWhichPoint) const { return m_pParticle->GetControlPointSnapshot(nWhichPoint); }

	// Used to retrieve the position of a control point
	// somewhere between m_fCurTime and m_fCurTime - m_fPreviousDT
	virtual void GetControlPointAtTime(int nControlPoint, float flTime, Vector* pControlPoint) { m_pParticle->GetControlPointAtTime(nControlPoint, flTime, pControlPoint); }
	virtual void GetControlPointAtPrevTime(int nControlPoint, Vector* pControlPoint) { m_pParticle->GetControlPointAtPrevTime(nControlPoint, pControlPoint); }
	virtual void GetControlPointOrientationAtTime(int nControlPoint, float flTime, Vector* pForward, Vector* pRight, Vector* pUp) { m_pParticle->GetControlPointOrientationAtTime(nControlPoint, flTime, pForward, pRight, pUp); }
	virtual void GetControlPointTransformAtTime(int nControlPoint, float flTime, matrix3x4_t* pMat) { m_pParticle->GetControlPointTransformAtTime(nControlPoint, flTime, pMat); }
	virtual void GetControlPointTransformAtTime(int nControlPoint, float flTime, VMatrix* pMat) { m_pParticle->GetControlPointTransformAtTime(nControlPoint, flTime, pMat); }
	virtual void GetControlPointTransformAtTime(int nControlPoint, float flTime, CParticleSIMDTransformation* pXForm) { m_pParticle->GetControlPointTransformAtTime(nControlPoint, flTime, pXForm); }
	virtual int GetHighestControlPoint(void) const { return m_pParticle->GetHighestControlPoint(); }

	// Control point accessed:
	// NOTE: Unlike the definition's version of these methods,
	// these OR-in the masks of their children.
	virtual bool ReadsControlPoint(int nPoint) const { return m_pParticle->ReadsControlPoint(nPoint); }
	virtual bool IsNonPositionalControlPoint(int nPoint) const { return m_pParticle->IsNonPositionalControlPoint(nPoint); }

	// Used by particle systems to generate random numbers. Do not call these methods - use sse
	// code
	virtual int RandomInt(int nMin, int nMax) { return m_pParticle->RandomInt(nMin, nMax); }
	virtual float RandomFloat(float flMin, float flMax) { return m_pParticle->RandomFloat(flMin, flMax); }
	virtual float RandomFloatExp(float flMin, float flMax, float flExponent) { return m_pParticle->RandomFloatExp(flMin, flMax, flExponent); }
	virtual void RandomVector(float flMin, float flMax, Vector* pVector) { m_pParticle->RandomVector(flMin, flMax, pVector); }
	virtual void RandomVector(const Vector& vecMin, const Vector& vecMax, Vector* pVector) { m_pParticle->RandomVector(vecMin, vecMax, pVector); }
	virtual float RandomVectorInUnitSphere(Vector* pVector) { return m_pParticle->RandomVectorInUnitSphere(pVector); }	// Returns the length sqr of the vector

	// NOTE: These versions will produce the *same random numbers* if you give it the same random
	// sample id. do not use these methods.
	virtual int RandomInt(int nRandomSampleId, int nMin, int nMax) { return m_pParticle->RandomInt(nRandomSampleId, nMin, nMax); }
	virtual float RandomFloat(int nRandomSampleId, float flMin, float flMax) { return m_pParticle->RandomFloat(nRandomSampleId, flMin, flMax); }
	virtual float RandomFloatExp(int nRandomSampleId, float flMin, float flMax, float flExponent) { return m_pParticle->RandomFloatExp(nRandomSampleId, flMin, flMax, flExponent); }
	virtual void RandomVector(int nRandomSampleId, float flMin, float flMax, Vector* pVector) { m_pParticle->RandomVector(nRandomSampleId, flMin, flMax, pVector); }
	virtual void RandomVector(int nRandomSampleId, const Vector& vecMin, const Vector& vecMax, Vector* pVector) { m_pParticle->RandomVector(nRandomSampleId, vecMin, vecMax, pVector); }
	virtual float RandomVectorInUnitSphere(int nRandomSampleId, Vector* pVector) { return m_pParticle->RandomVectorInUnitSphere(nRandomSampleId, pVector); }	// Returns the length sqr of the vector

	virtual fltx4 RandomFloat(const FourInts& ParticleID, int nRandomSampleOffset) { return m_pParticle->RandomFloat(ParticleID, nRandomSampleOffset); }


	// Random number offset (for use in getting Random #s in operators)
	virtual int OperatorRandomSampleOffset() const { return m_pParticle->OperatorRandomSampleOffset(); }

	// Returns the render bounds
	virtual void GetBounds(Vector* pMin, Vector* pMax) { m_pParticle->GetBounds(pMin, pMax); }

	// Visualize operators (for editing/debugging)
	virtual void VisualizeOperator(const DmObjectId_t* pOpId = NULL) { m_pParticle->VisualizeOperator(pOpId); }

	// Does the particle system use the power of two frame buffer texture (refraction?)
	virtual bool UsesPowerOfTwoFrameBufferTexture(bool bThisFrame) const { return m_pParticle->UsesPowerOfTwoFrameBufferTexture(bThisFrame); }

	// Does the particle system use the full frame buffer texture (soft particles)
	virtual bool UsesFullFrameBufferTexture(bool bThisFrame) const { return m_pParticle->UsesFullFrameBufferTexture(bThisFrame); }

	// Is the particle system translucent?
	virtual bool IsTranslucent() const { return m_pParticle->IsTranslucent(); }

	// Is the particle system two-pass?
	virtual bool IsTwoPass() const { return m_pParticle->IsTwoPass(); }

	// Is the particle system batchable?
	virtual bool IsBatchable() const { return m_pParticle->IsBatchable(); }

	// Is the order of the particles important
	virtual bool IsOrderImportant() const { return m_pParticle->IsOrderImportant(); }

	// Should this system be run want to read its parent's kill list inside ApplyKillList?
	virtual bool ShouldRunForParentApplyKillList(void) const { return m_pParticle->ShouldRunForParentApplyKillList(); }

	// Renderer iteration
	virtual int GetRendererCount() const { return m_pParticle->GetRendererCount(); }
	virtual CParticleOperatorInstance* GetRenderer(int i) { return m_pParticle->GetRenderer(i); }
	virtual void* GetRendererContext(int i) { return m_pParticle->GetRendererContext(i); }

	virtual bool CheckIfOperatorShouldRun(CParticleOperatorInstance const* op, float* pflCurStrength, bool bApplyingParentKillList = false) { return m_pParticle->CheckIfOperatorShouldRun(op, pflCurStrength, bApplyingParentKillList); }

	virtual Vector TransformAxis(const Vector& SrcAxis, bool bLocalSpace, int nControlPointNumber = 0) { return m_pParticle->TransformAxis(SrcAxis, bLocalSpace, nControlPointNumber); }

	// return backwards-sorted particle list. use --addressing
	virtual const ParticleRenderData_t* GetRenderList(IMatRenderContext* pRenderContext, bool bSorted, int* pNparticles, CParticleVisibilityData* pVisibilityData) { return m_pParticle->GetRenderList(pRenderContext, bSorted, pNparticles, pVisibilityData); }

	// calculate the points of a curve for a path
	virtual void CalculatePathValues(
		CPathParameters const& PathIn,
		float flTimeStamp,
		Vector* pStartPnt,
		Vector* pMidPnt,
		Vector* pEndPnt
	) {
		m_pParticle->CalculatePathValues(PathIn, flTimeStamp, pStartPnt, pMidPnt, pEndPnt);
	}

	virtual int GetGroupID() const { return m_pParticle->GetGroupID(); }

	virtual void InitializeNewParticles(int nFirstParticle, int nParticleCount, uint32 nInittedMask, bool bApplyingParentKillList = false) { m_pParticle->InitializeNewParticles(nFirstParticle, nParticleCount, nInittedMask, bApplyingParentKillList); }

	// update hit boxes for control point if not updated yet for this sim step
	virtual void UpdateHitBoxInfo(int nControlPointNumber) { m_pParticle->UpdateHitBoxInfo(nControlPointNumber); }

	// Used by particle system definitions to manage particle collection lists
	virtual void UnlinkFromDefList() { m_pParticle->UnlinkFromDefList(); }

	virtual uint8 const* GetPrevAttributeMemory(void) const { return m_pParticle->GetPrevAttributeMemory(); }
	virtual uint8 const* GetAttributeMemory(void) const { return m_pParticle->GetAttributeMemory(); }
	virtual bool IsUsingInterpolatedRendering(void) const { return m_pParticle->IsUsingInterpolatedRendering(); }

	// render helpers
	virtual int GenerateCulledSortedIndexList(ParticleRenderData_t* pOut, Vector vecCamera, Vector vecFwd, CParticleVisibilityData* pVisibilityData, bool bSorted) { return m_pParticle->GenerateCulledSortedIndexList(pOut, vecCamera, vecFwd, pVisibilityData, bSorted); }
	virtual int GenerateSortedIndexList(ParticleRenderData_t* pOut, Vector vecCameraPos, CParticleVisibilityData* pVisibilityData, bool bSorted) { return m_pParticle->GenerateSortedIndexList(pOut, vecCameraPos, pVisibilityData, bSorted); }

	virtual IParticleCollection* GetNextCollectionUsingSameDef() {

		// TODO: check this doeesnt make a mem leek!
		return new CParticleCollectionPtr(m_pParticle->GetNextCollectionUsingSameDef());

	}

protected:

	// Used by client code
	virtual bool Init(const char* pParticleSystemName) { return m_pParticle->Init(pParticleSystemName); }
	virtual bool Init(CParticleSystemDefinition* pDef) { return m_pParticle->Init(pDef); }

	// Bloat the bounding box by bounds around the control point
	virtual void BloatBoundsUsingControlPoint() { m_pParticle->BloatBoundsUsingControlPoint(); }

	// to run emitters on restart, out of main sim.
	virtual void RunRestartedEmitters(void) { m_pParticle->RunRestartedEmitters(); }

	virtual void SetRenderable(void* pRenderable) { m_pParticle->SetRenderable(pRenderable); }

	virtual void Init(CParticleSystemDefinition* pDef, float flDelay, int nRandomSeed) { m_pParticle->Init(pDef, flDelay, nRandomSeed); }
	virtual void InitStorage(CParticleSystemDefinition* pDef) { m_pParticle->InitStorage(pDef); }
	virtual void InitParticleCreationTime(int nFirstParticle, int nNumToInit) { /*m_pParticle->InitParticleCreationTime(nFirstParticle, nNumToInit);*/ }
	virtual void CopyInitialAttributeValues(int nStartParticle, int nNumParticles) { m_pParticle->CopyInitialAttributeValues(nStartParticle, nNumParticles); }
	virtual void ApplyKillList(void) { m_pParticle->ApplyKillList(); }
	virtual void SetAttributeToConstant(int nAttribute, float fValue) { m_pParticle->SetAttributeToConstant(nAttribute, fValue); }
	virtual void SetAttributeToConstant(int nAttribute, float fValueX, float fValueY, float fValueZ) { m_pParticle->SetAttributeToConstant(nAttribute, fValueX, fValueY, fValueZ); }
	virtual void InitParticleAttributes(int nStartParticle, int nNumParticles, int nAttrsLeftToInit) { m_pParticle->InitParticleAttributes(nStartParticle, nNumParticles, nAttrsLeftToInit); }

	// call emitter and initializer operators on the specified system
	// NOTE: this may be called from ApplyKillList, so the child can access about-to-be-killed particles
	virtual void EmitAndInit(IParticleCollection* pCollection, bool bApplyingParentKillList = false) { 
	
		m_pParticle->EmitAndInit(static_cast<CParticleCollectionPtr*>(pCollection)->m_pParticle, bApplyingParentKillList);

	}

	// initialize this attribute for all active particles
	virtual void FillAttributeWithConstant(int nAttribute, float fValue) { m_pParticle->FillAttributeWithConstant(nAttribute, fValue); }

	// Updates the previous control points
	virtual void UpdatePrevControlPoints(float dt) { m_pParticle->UpdatePrevControlPoints(dt); }

	// Returns the memory for a particular constant attribute
	virtual float* GetConstantAttributeMemory(int nAttribute) { return m_pParticle->GetConstantAttributeMemory(nAttribute); }

	// Swaps two particles in the particle list
	virtual void SwapAdjacentParticles(int hParticle) { /*m_pParticle->SwapAdjacentParticles(hParticle);*/ }

	// Unlinks a particle from the list
	virtual void UnlinkParticle(int hParticle) { /*m_pParticle->UnlinkParticle(hParticle);*/ }

	// Inserts a particle before another particle in the list
	virtual void InsertParticleBefore(int hParticle, int hBefore) { /*m_pParticle->InsertParticleBefore(hParticle, hBefore);*/ }

	// Move a particle from one index to another
	virtual void MoveParticle(int nInitialIndex, int nNewIndex) { m_pParticle->MoveParticle(nInitialIndex, nNewIndex); }

	// Computes the sq distance to a particle position
	virtual float ComputeSqrDistanceToParticle(int hParticle, const Vector& vecPosition) const { return m_pParticle->ComputeSqrDistanceToParticle(hParticle, vecPosition); }

	// Grows the dist sq range for all particles
	virtual void GrowDistSqrBounds(float flDistSqr) { m_pParticle->GrowDistSqrBounds(flDistSqr); }

	// Simulates the first frame
	virtual void SimulateFirstFrame() { m_pParticle->SimulateFirstFrame(); }

	virtual bool SystemContainsParticlesWithBoolSet(bool IParticleCollection::* pField) const { 
	

		// TODO
		return false;// m_pParticle->SystemContainsParticlesWithBoolSet(static_cast<CParticleCollectionPtr*>(pField).m_pParticle);

	}

	// Does the particle collection contain opaque particle systems
	virtual bool ContainsOpaqueCollections() { return m_pParticle->ContainsOpaqueCollections(); }
	virtual bool ComputeUsesPowerOfTwoFrameBufferTexture() { return m_pParticle->ComputeUsesPowerOfTwoFrameBufferTexture(); }
	virtual bool ComputeUsesFullFrameBufferTexture() { return m_pParticle->ComputeUsesFullFrameBufferTexture(); }
	virtual bool ComputeIsTranslucent() { return m_pParticle->ComputeIsTranslucent(); }
	virtual bool ComputeIsTwoPass() { return m_pParticle->ComputeIsTwoPass(); }
	virtual bool ComputeIsBatchable() { return m_pParticle->ComputeIsBatchable(); }
	virtual bool ComputeIsOrderImportant() { return m_pParticle->ComputeIsOrderImportant(); }
	virtual bool ComputeRunForParentApplyKillList() { return m_pParticle->ComputeRunForParentApplyKillList(); }

	virtual void LabelTextureUsage(void) { m_pParticle->LabelTextureUsage(); }

	virtual void LinkIntoDefList() { m_pParticle->LinkIntoDefList(); }

	// Return the number of particle systems sharing the same definition
	virtual int GetCurrentParticleDefCount(CParticleSystemDefinition* pDef) { return m_pParticle->GetCurrentParticleDefCount(pDef); }

	virtual void CopyParticleAttributesToPreviousAttributes(void) const { m_pParticle->CopyParticleAttributesToPreviousAttributes(); }

public:

	virtual CParticleControlPoint& ControlPoint(int nIdx) const { return m_pParticle->ControlPoint(nIdx); }
	virtual CModelHitBoxesInfo& ControlPointHitBox(int nIdx) const { return m_pParticle->ControlPointHitBox(nIdx); }

	virtual void LoanKillListTo(IParticleCollection* pBorrower) const {
	
		m_pParticle->LoanKillListTo(static_cast<CParticleCollectionPtr*>(pBorrower)->m_pParticle);

	}

	virtual bool HasAttachedKillList(void) const { return m_pParticle->HasAttachedKillList(); }

	virtual int GetAllocatedParticles() { return m_pParticle->m_nAllocatedParticles; }
	virtual int GetMaxParticles() { return m_pParticle->m_nMaxAllowedParticles; }
	virtual int GetActiveParticles() { return m_pParticle->m_nActiveParticles; }
	virtual int GetHighestCP() { return m_pParticle->m_nHighestCP; }

	virtual float GetCurTime() { return m_pParticle->m_flCurTime; }
	virtual float NextSleep() { return m_pParticle->m_flNextSleepTime; }

	virtual bool BoundsValid() { return m_pParticle->m_bBoundsValid; }
	virtual bool QueuedStartEmission() { return m_pParticle->m_bQueuedStartEmission; }
	virtual bool Dormant() { return m_pParticle->m_bDormant; }
	virtual bool EmissionStopped() { return m_pParticle->m_bEmissionStopped; }
	virtual bool Remove() { return false; }

	virtual CUtlReference< CParticleSystemDefinition > GetDef() { return m_pParticle->m_pDef; }

	virtual Vector GetMinBounds() { return m_pParticle->m_MinBounds; }
	virtual Vector GetMaxBounds() { return m_pParticle->m_MaxBounds; }

	virtual void SetMinBounds(Vector vBounds) { m_pParticle->m_MinBounds = vBounds; }
	virtual void SetMaxBounds(Vector vBounds) { m_pParticle->m_MaxBounds = vBounds; }
	virtual void SetQueuedStartEmission(bool bQueued) { m_pParticle->m_bQueuedStartEmission = bQueued; }
	virtual void SetRemove(bool bRemove) { /*m_pParticle->m_bRemove = bRemove;*/ }

	virtual IParticleCollection* GetParent() { return new CParticleCollectionPtr(m_pParticle->m_pParent); }

	virtual void SetEffectHandle(CBaseHandle handle) { /*m_pParticle->m_hEffectHandle = handle;*/ }
	virtual CBaseHandle GetEffectHandle() { return NULL; }// m_pParticle->m_hEffectHandle; }

	virtual bool InternalIsValid() { return m_pParticle != nullptr; }

protected:
	friend class CParticleSystemMgrPtr;

	CParticleCollection* m_pParticle;

};

#else
//
//class IParticleCollectionInheritor : public IParticleCollection {
//
//private:
//	IParticleCollectionInheritor() {
//		m_pCollection = g_pParticleSystemMgr->CreateParticleCollectionPtr();
//	}
//	IParticleCollectionInheritor(IParticleCollection* pParticleCollection) {
//		//m_pCollection = pParticleCollection;
//	}
//
//	// Restarts the particle collection, stopping all non-continuous emitters
//	virtual void Restart(EParticleRestartMode_t eMode = RESTART_NORMAL) { m_pCollection->Restart(eMode); }
//
//	// compute bounds from particle list
//	virtual void RecomputeBounds(void) { m_pCollection->RecomputeBounds(); }
//
//	virtual void SetControlPoint(int nWhichPoint, const Vector& v) { m_pCollection->SetControlPoint(nWhichPoint, v); }
//	virtual void SetControlPointObject(int nWhichPoint, void* pObject) { m_pCollection->SetControlPointObject(nWhichPoint, pObject); }
//
//	virtual void SetControlPointOrientation(int nWhichPoint, const Vector& forward,
//		const Vector& right, const Vector& up) {
//		m_pCollection->SetControlPointOrientation(nWhichPoint, forward, right, up);
//	}
//
//	virtual void SetControlPointForwardVector(int nWhichPoint, const Vector& v) { m_pCollection->SetControlPointForwardVector(nWhichPoint, v); }
//	virtual void SetControlPointUpVector(int nWhichPoint, const Vector& v) { m_pCollection->SetControlPointUpVector(nWhichPoint, v); }
//	virtual void SetControlPointRightVector(int nWhichPoint, const Vector& v) { m_pCollection->SetControlPointRightVector(nWhichPoint, v); }
//	virtual void SetControlPointParent(int nWhichPoint, int n) { m_pCollection->SetControlPointParent(nWhichPoint, n); }
//	virtual void SetControlPointSnapshot(int nWhichPoint, CParticleSnapshot* pSnapshot) { m_pCollection->SetControlPointSnapshot(nWhichPoint, pSnapshot); }
//
//	virtual void SetControlPointOrientation(int nWhichPoint, const Quaternion& q) { m_pCollection->SetControlPointOrientation(nWhichPoint, q); }
//
//	// get the pointer to an attribute for a given particle.  
//	// !!speed!! if you find yourself calling this anywhere that matters, 
//	// you're not handling the simd-ness of the particle system well
//	// and will have bad perf.
//	virtual const float* GetFloatAttributePtr(int nAttribute, int nParticleNumber) const { return m_pCollection->GetFloatAttributePtr(nAttribute, nParticleNumber); }
//	virtual const int* GetIntAttributePtr(int nAttribute, int nParticleNumber) const { return m_pCollection->GetIntAttributePtr(nAttribute, nParticleNumber); }
//	virtual const fltx4* GetM128AttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pCollection->GetM128AttributePtr(nAttribute, pStrideOut); }
//	virtual const FourVectors* Get4VAttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pCollection->Get4VAttributePtr(nAttribute, pStrideOut); }
//	virtual const FourInts* Get4IAttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pCollection->Get4IAttributePtr(nAttribute, pStrideOut); }
//	virtual const int* GetIntAttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pCollection->GetIntAttributePtr(nAttribute, pStrideOut); }
//
//	virtual Vector GetVectorAttributeValue(int nAttribute, int nParticleNumber) const { return m_pCollection->GetVectorAttributeValue(nAttribute, nParticleNumber); }
//
//
//	virtual int* GetIntAttributePtrForWrite(int nAttribute, int nParticleNumber) { return m_pCollection->GetIntAttributePtrForWrite(nAttribute, nParticleNumber); }
//
//	virtual float* GetFloatAttributePtrForWrite(int nAttribute, int nParticleNumber) { return m_pCollection->GetFloatAttributePtrForWrite(nAttribute, nParticleNumber); }
//	virtual fltx4* GetM128AttributePtrForWrite(int nAttribute, size_t* pStrideOut) { return m_pCollection->GetM128AttributePtrForWrite(nAttribute, pStrideOut); }
//	virtual FourVectors* Get4VAttributePtrForWrite(int nAttribute, size_t* pStrideOut) { return m_pCollection->Get4VAttributePtrForWrite(nAttribute, pStrideOut); }
//
//	virtual const float* GetInitialFloatAttributePtr(int nAttribute, int nParticleNumber) const { return m_pCollection->GetInitialFloatAttributePtr(nAttribute, nParticleNumber); }
//	virtual const fltx4* GetInitialM128AttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pCollection->GetInitialM128AttributePtr(nAttribute, pStrideOut); }
//	virtual const FourVectors* GetInitial4VAttributePtr(int nAttribute, size_t* pStrideOut) const { return m_pCollection->GetInitial4VAttributePtr(nAttribute, pStrideOut); }
//	virtual float* GetInitialFloatAttributePtrForWrite(int nAttribute, int nParticleNumber) { return m_pCollection->GetInitialFloatAttributePtrForWrite(nAttribute, nParticleNumber); }
//	virtual fltx4* GetInitialM128AttributePtrForWrite(int nAttribute, size_t* pStrideOut) { return m_pCollection->GetInitialM128AttributePtrForWrite(nAttribute, pStrideOut); }
//
//	virtual void Simulate(float dt) { m_pCollection->Simulate(dt); }
//	virtual void SkipToTime(float t) { m_pCollection->SkipToTime(t); }
//
//	// the camera objetc may be compared for equality against control point objects
//	virtual void Render(IMatRenderContext* pRenderContext, const Vector4D& vecDiffuseModulation, bool bTranslucentOnly = false, void* pCameraObject = NULL) { m_pCollection->Render(pRenderContext, vecDiffuseModulation, bTranslucentOnly, pCameraObject); }
//
//	virtual bool IsValid(void) const { return m_pCollection->IsValid(); }
//
//	// this system and all children are valid
//	virtual bool IsFullyValid(void) const { return m_pCollection->IsFullyValid(); }
//
//	virtual const char* GetName() const { return m_pCollection->GetName(); }
//
//	virtual bool DependsOnSystem(const char* pName) const { return m_pCollection->DependsOnSystem(pName); }
//
//	// IsFinished returns true when a system has no particles and won't be creating any more
//	virtual bool IsFinished(void) const { return m_pCollection->IsFinished(); }
//
//	// Used to make sure we're accessing valid memory
//	virtual bool IsValidAttributePtr(int nAttribute, const void* pPtr) const { return m_pCollection->IsValidAttributePtr(nAttribute, pPtr); }
//
//	virtual void SwapPosAndPrevPos(void) { m_pCollection->SwapPosAndPrevPos(); }
//
//	virtual void SetNActiveParticles(int nCount) { m_pCollection->SetNActiveParticles(nCount); }
//	virtual void KillParticle(int nPidx, unsigned int nFlags = 0) { m_pCollection->KillParticle(nPidx, nFlags); }
//
//	virtual void StopEmission(bool bInfiniteOnly = false, bool bRemoveAllParticles = false, bool bWakeOnStop = false, bool bPlayEndCap = false) { m_pCollection->StopEmission(bInfiniteOnly, bRemoveAllParticles, bWakeOnStop, bPlayEndCap); }
//	virtual void StartEmission(bool bInfiniteOnly = false) { m_pCollection->StartEmission(bInfiniteOnly); }
//	virtual void SetDormant(bool bDormant) { m_pCollection->SetDormant(bDormant); }
//	virtual bool IsEmitting() const { return m_pCollection->IsEmitting(); }
//
//	virtual const Vector& GetControlPointAtCurrentTime(int nControlPoint) const { return m_pCollection->GetControlPointAtCurrentTime(nControlPoint); }
//	virtual void GetControlPointOrientationAtCurrentTime(int nControlPoint, Vector* pForward, Vector* pRight, Vector* pUp) const { m_pCollection->GetControlPointOrientationAtCurrentTime(nControlPoint, pForward, pRight, pUp); }
//	virtual void GetControlPointTransformAtCurrentTime(int nControlPoint, matrix3x4_t* pMat) { m_pCollection->GetControlPointTransformAtCurrentTime(nControlPoint, pMat); }
//	virtual void GetControlPointTransformAtCurrentTime(int nControlPoint, VMatrix* pMat) { m_pCollection->GetControlPointTransformAtCurrentTime(nControlPoint, pMat); }
//	virtual int GetControlPointParent(int nControlPoint) const { return m_pCollection->GetControlPointParent(nControlPoint); }
//	virtual CParticleSnapshot* GetControlPointSnapshot(int nWhichPoint) const { return m_pCollection->GetControlPointSnapshot(nWhichPoint); }
//
//	// Used to retrieve the position of a control point
//	// somewhere between m_fCurTime and m_fCurTime - m_fPreviousDT
//	virtual void GetControlPointAtTime(int nControlPoint, float flTime, Vector* pControlPoint) { m_pCollection->GetControlPointAtTime(nControlPoint, flTime, pControlPoint); }
//	virtual void GetControlPointAtPrevTime(int nControlPoint, Vector* pControlPoint) { m_pCollection->GetControlPointAtPrevTime(nControlPoint, pControlPoint); }
//	virtual void GetControlPointOrientationAtTime(int nControlPoint, float flTime, Vector* pForward, Vector* pRight, Vector* pUp) { m_pCollection->GetControlPointOrientationAtTime(nControlPoint, flTime, pForward, pRight, pUp); }
//	virtual void GetControlPointTransformAtTime(int nControlPoint, float flTime, matrix3x4_t* pMat) { m_pCollection->GetControlPointTransformAtTime(nControlPoint, flTime, pMat); }
//	virtual void GetControlPointTransformAtTime(int nControlPoint, float flTime, VMatrix* pMat) { m_pCollection->GetControlPointTransformAtTime(nControlPoint, flTime, pMat); }
//	virtual void GetControlPointTransformAtTime(int nControlPoint, float flTime, CParticleSIMDTransformation* pXForm) { m_pCollection->GetControlPointTransformAtTime(nControlPoint, flTime, pXForm); }
//	virtual int GetHighestControlPoint(void) const { return m_pCollection->GetHighestControlPoint(); }
//
//	// Control point accessed:
//	// NOTE: Unlike the definition's version of these methods,
//	// these OR-in the masks of their children.
//	virtual bool ReadsControlPoint(int nPoint) const { return m_pCollection->ReadsControlPoint(nPoint); }
//	virtual bool IsNonPositionalControlPoint(int nPoint) const { return m_pCollection->IsNonPositionalControlPoint(nPoint); }
//
//	// Used by particle systems to generate random numbers. Do not call these methods - use sse
//	// code
//	virtual int RandomInt(int nMin, int nMax) { return m_pCollection->RandomInt(nMin, nMax); }
//	virtual float RandomFloat(float flMin, float flMax) { return m_pCollection->RandomFloat(flMin, flMax); }
//	virtual float RandomFloatExp(float flMin, float flMax, float flExponent) { return m_pCollection->RandomFloatExp(flMin, flMax, flExponent); }
//	virtual void RandomVector(float flMin, float flMax, Vector* pVector) { m_pCollection->RandomVector(flMin, flMax, pVector); }
//	virtual void RandomVector(const Vector& vecMin, const Vector& vecMax, Vector* pVector) { m_pCollection->RandomVector(vecMin, vecMax, pVector); }
//	virtual float RandomVectorInUnitSphere(Vector* pVector) { return m_pCollection->RandomVectorInUnitSphere(pVector); }	// Returns the length sqr of the vector
//
//	// NOTE: These versions will produce the *same random numbers* if you give it the same random
//	// sample id. do not use these methods.
//	virtual int RandomInt(int nRandomSampleId, int nMin, int nMax) { return m_pCollection->RandomInt(nRandomSampleId, nMin, nMax); }
//	virtual float RandomFloat(int nRandomSampleId, float flMin, float flMax) { return m_pCollection->RandomFloat(nRandomSampleId, flMin, flMax); }
//	virtual float RandomFloatExp(int nRandomSampleId, float flMin, float flMax, float flExponent) { return m_pCollection->RandomFloatExp(nRandomSampleId, flMin, flMax, flExponent); }
//	virtual void RandomVector(int nRandomSampleId, float flMin, float flMax, Vector* pVector) { m_pCollection->RandomVector(nRandomSampleId, flMin, flMax, pVector); }
//	virtual void RandomVector(int nRandomSampleId, const Vector& vecMin, const Vector& vecMax, Vector* pVector) { m_pCollection->RandomVector(nRandomSampleId, vecMin, vecMax, pVector); }
//	virtual float RandomVectorInUnitSphere(int nRandomSampleId, Vector* pVector) { return m_pCollection->RandomVectorInUnitSphere(nRandomSampleId, pVector); }	// Returns the length sqr of the vector
//
//	virtual fltx4 RandomFloat(const FourInts& ParticleID, int nRandomSampleOffset) { return m_pCollection->RandomFloat(ParticleID, nRandomSampleOffset); }
//
//
//	// Random number offset (for use in getting Random #s in operators)
//	virtual int OperatorRandomSampleOffset() const { return m_pCollection->OperatorRandomSampleOffset(); }
//
//	// Returns the render bounds
//	virtual void GetBounds(Vector* pMin, Vector* pMax) { m_pCollection->GetBounds(pMin, pMax); }
//
//	// Visualize operators (for editing/debugging)
//	virtual void VisualizeOperator(const DmObjectId_t* pOpId = NULL) { m_pCollection->VisualizeOperator(pOpId); }
//
//	// Does the particle system use the power of two frame buffer texture (refraction?)
//	virtual bool UsesPowerOfTwoFrameBufferTexture(bool bThisFrame) const { return m_pCollection->UsesPowerOfTwoFrameBufferTexture(bThisFrame); }
//
//	// Does the particle system use the full frame buffer texture (soft particles)
//	virtual bool UsesFullFrameBufferTexture(bool bThisFrame) const { return m_pCollection->UsesFullFrameBufferTexture(bThisFrame); }
//
//	// Is the particle system translucent?
//	virtual bool IsTranslucent() const { return m_pCollection->IsTranslucent(); }
//
//	// Is the particle system two-pass?
//	virtual bool IsTwoPass() const { return m_pCollection->IsTwoPass(); }
//
//	// Is the particle system batchable?
//	virtual bool IsBatchable() const { return m_pCollection->IsBatchable(); }
//
//	// Is the order of the particles important
//	virtual bool IsOrderImportant() const { return m_pCollection->IsOrderImportant(); }
//
//	// Should this system be run want to read its parent's kill list inside ApplyKillList?
//	virtual bool ShouldRunForParentApplyKillList(void) const { return m_pCollection->ShouldRunForParentApplyKillList(); }
//
//	// Renderer iteration
//	virtual int GetRendererCount() const { return m_pCollection->GetRendererCount(); }
//	virtual CParticleOperatorInstance* GetRenderer(int i) { return m_pCollection->GetRenderer(i); }
//	virtual void* GetRendererContext(int i) { return m_pCollection->GetRendererContext(i); }
//
//	virtual bool CheckIfOperatorShouldRun(CParticleOperatorInstance const* op, float* pflCurStrength, bool bApplyingParentKillList = false) { return m_pCollection->CheckIfOperatorShouldRun(op, pflCurStrength, bApplyingParentKillList); }
//
//	virtual Vector TransformAxis(const Vector& SrcAxis, bool bLocalSpace, int nControlPointNumber = 0) { return m_pCollection->TransformAxis(SrcAxis, bLocalSpace, nControlPointNumber); }
//
//	// return backwards-sorted particle list. use --addressing
//	virtual const ParticleRenderData_t* GetRenderList(IMatRenderContext* pRenderContext, bool bSorted, int* pNparticles, CParticleVisibilityData* pVisibilityData) { return m_pCollection->GetRenderList(pRenderContext, bSorted, pNparticles, pVisibilityData); }
//
//	// calculate the points of a curve for a path
//	virtual void CalculatePathValues(
//		CPathParameters const& PathIn,
//		float flTimeStamp,
//		Vector* pStartPnt,
//		Vector* pMidPnt,
//		Vector* pEndPnt
//	) {
//		m_pCollection->CalculatePathValues(PathIn, flTimeStamp, pStartPnt, pMidPnt, pEndPnt);
//	}
//
//	virtual int GetGroupID() const { return m_pCollection->GetGroupID(); }
//
//	virtual void InitializeNewParticles(int nFirstParticle, int nParticleCount, uint32 nInittedMask, bool bApplyingParentKillList = false) { m_pCollection->InitializeNewParticles(nFirstParticle, nParticleCount, nInittedMask, bApplyingParentKillList); }
//
//	// update hit boxes for control point if not updated yet for this sim step
//	virtual void UpdateHitBoxInfo(int nControlPointNumber) { m_pCollection->UpdateHitBoxInfo(nControlPointNumber); }
//
//	// Used by particle system definitions to manage particle collection lists
//	virtual void UnlinkFromDefList() { m_pCollection->UnlinkFromDefList(); }
//
//	virtual uint8 const* GetPrevAttributeMemory(void) const { return m_pCollection->GetPrevAttributeMemory(); }
//	virtual uint8 const* GetAttributeMemory(void) const { return m_pCollection->GetAttributeMemory(); }
//	virtual bool IsUsingInterpolatedRendering(void) const { return m_pCollection->IsUsingInterpolatedRendering(); }
//
//	// render helpers
//	virtual int GenerateCulledSortedIndexList(ParticleRenderData_t* pOut, Vector vecCamera, Vector vecFwd, CParticleVisibilityData* pVisibilityData, bool bSorted) { return m_pCollection->GenerateCulledSortedIndexList(pOut, vecCamera, vecFwd, pVisibilityData, bSorted); }
//	virtual int GenerateSortedIndexList(ParticleRenderData_t* pOut, Vector vecCameraPos, CParticleVisibilityData* pVisibilityData, bool bSorted) { return m_pCollection->GenerateSortedIndexList(pOut, vecCameraPos, pVisibilityData, bSorted); }
//
//	virtual IParticleCollection* GetNextCollectionUsingSameDef() {
//
//		// TODO: check this doeesnt make a mem leek!
//		return m_pCollection->GetNextCollectionUsingSameDef();
//
//	}
//
//protected:
//
//	// Used by client code
//	virtual bool Init(const char* pParticleSystemName) { return m_pCollection->Init(pParticleSystemName); }
//	virtual bool Init(CParticleSystemDefinition* pDef) { return m_pCollection->Init(pDef); }
//
//	// Bloat the bounding box by bounds around the control point
//	virtual void BloatBoundsUsingControlPoint() { m_pCollection->BloatBoundsUsingControlPoint(); }
//
//	// to run emitters on restart, out of main sim.
//	virtual void RunRestartedEmitters(void) { m_pCollection->RunRestartedEmitters(); }
//
//	virtual void SetRenderable(void* pRenderable) { m_pCollection->SetRenderable(pRenderable); }
//
//	virtual void Init(CParticleSystemDefinition* pDef, float flDelay, int nRandomSeed) { m_pCollection->Init(pDef, flDelay, nRandomSeed); }
//	virtual void InitStorage(CParticleSystemDefinition* pDef) { m_pCollection->InitStorage(pDef); }
//	virtual void InitParticleCreationTime(int nFirstParticle, int nNumToInit) { /*m_pCollection->InitParticleCreationTime(nFirstParticle, nNumToInit);*/ }
//	virtual void CopyInitialAttributeValues(int nStartParticle, int nNumParticles) { m_pCollection->CopyInitialAttributeValues(nStartParticle, nNumParticles); }
//	virtual void ApplyKillList(void) { m_pCollection->ApplyKillList(); }
//	virtual void SetAttributeToConstant(int nAttribute, float fValue) { m_pCollection->SetAttributeToConstant(nAttribute, fValue); }
//	virtual void SetAttributeToConstant(int nAttribute, float fValueX, float fValueY, float fValueZ) { m_pCollection->SetAttributeToConstant(nAttribute, fValueX, fValueY, fValueZ); }
//	virtual void InitParticleAttributes(int nStartParticle, int nNumParticles, int nAttrsLeftToInit) { m_pCollection->InitParticleAttributes(nStartParticle, nNumParticles, nAttrsLeftToInit); }
//
//	// call emitter and initializer operators on the specified system
//	// NOTE: this may be called from ApplyKillList, so the child can access about-to-be-killed particles
//	virtual void EmitAndInit(IParticleCollection* pCollection, bool bApplyingParentKillList = false) {
//
//		m_pCollection->EmitAndInit(m_pCollection, bApplyingParentKillList);
//
//	}
//
//	// initialize this attribute for all active particles
//	virtual void FillAttributeWithConstant(int nAttribute, float fValue) { m_pCollection->FillAttributeWithConstant(nAttribute, fValue); }
//
//	// Updates the previous control points
//	virtual void UpdatePrevControlPoints(float dt) { m_pCollection->UpdatePrevControlPoints(dt); }
//
//	// Returns the memory for a particular constant attribute
//	virtual float* GetConstantAttributeMemory(int nAttribute) { return m_pCollection->GetConstantAttributeMemory(nAttribute); }
//
//	// Swaps two particles in the particle list
//	virtual void SwapAdjacentParticles(int hParticle) { /*m_pCollection->SwapAdjacentParticles(hParticle);*/ }
//
//	// Unlinks a particle from the list
//	virtual void UnlinkParticle(int hParticle) { /*m_pCollection->UnlinkParticle(hParticle);*/ }
//
//	// Inserts a particle before another particle in the list
//	virtual void InsertParticleBefore(int hParticle, int hBefore) { /*m_pCollection->InsertParticleBefore(hParticle, hBefore);*/ }
//
//	// Move a particle from one index to another
//	virtual void MoveParticle(int nInitialIndex, int nNewIndex) { m_pCollection->MoveParticle(nInitialIndex, nNewIndex); }
//
//	// Computes the sq distance to a particle position
//	virtual float ComputeSqrDistanceToParticle(int hParticle, const Vector& vecPosition) const { return m_pCollection->ComputeSqrDistanceToParticle(hParticle, vecPosition); }
//
//	// Grows the dist sq range for all particles
//	virtual void GrowDistSqrBounds(float flDistSqr) { m_pCollection->GrowDistSqrBounds(flDistSqr); }
//
//	// Simulates the first frame
//	virtual void SimulateFirstFrame() { m_pCollection->SimulateFirstFrame(); }
//
//	virtual bool SystemContainsParticlesWithBoolSet(bool IParticleCollection::* pField) const {
//
//
//		// TODO
//		return false;// m_pCollection->SystemContainsParticlesWithBoolSet(static_cast<CParticleCollectionPtr*>(pField).m_pCollection);
//
//	}
//
//	// Does the particle collection contain opaque particle systems
//	virtual bool ContainsOpaqueCollections() { return m_pCollection->ContainsOpaqueCollections(); }
//	virtual bool ComputeUsesPowerOfTwoFrameBufferTexture() { return m_pCollection->ComputeUsesPowerOfTwoFrameBufferTexture(); }
//	virtual bool ComputeUsesFullFrameBufferTexture() { return m_pCollection->ComputeUsesFullFrameBufferTexture(); }
//	virtual bool ComputeIsTranslucent() { return m_pCollection->ComputeIsTranslucent(); }
//	virtual bool ComputeIsTwoPass() { return m_pCollection->ComputeIsTwoPass(); }
//	virtual bool ComputeIsBatchable() { return m_pCollection->ComputeIsBatchable(); }
//	virtual bool ComputeIsOrderImportant() { return m_pCollection->ComputeIsOrderImportant(); }
//	virtual bool ComputeRunForParentApplyKillList() { return m_pCollection->ComputeRunForParentApplyKillList(); }
//
//	virtual void LabelTextureUsage(void) { m_pCollection->LabelTextureUsage(); }
//
//	virtual void LinkIntoDefList() { m_pCollection->LinkIntoDefList(); }
//
//	// Return the number of particle systems sharing the same definition
//	virtual int GetCurrentParticleDefCount(CParticleSystemDefinition* pDef) { return m_pCollection->GetCurrentParticleDefCount(pDef); }
//
//	virtual void CopyParticleAttributesToPreviousAttributes(void) const { m_pCollection->CopyParticleAttributesToPreviousAttributes(); }
//
//public:
//
//	virtual CParticleControlPoint& ControlPoint(int nIdx) const { return m_pCollection->ControlPoint(nIdx); }
//	virtual CModelHitBoxesInfo& ControlPointHitBox(int nIdx) const { return m_pCollection->ControlPointHitBox(nIdx); }
//
//	virtual void LoanKillListTo(IParticleCollection* pBorrower) const {
//
//		m_pCollection->LoanKillListTo(pBorrower);
//
//	}
//
//	virtual bool HasAttachedKillList(void) const { return m_pCollection->HasAttachedKillList(); }
//
//	friend class CParticleSystemMgrPtr;
//
//	IParticleCollection* m_pCollection;
//
//};

#endif