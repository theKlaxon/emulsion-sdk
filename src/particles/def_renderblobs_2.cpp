#include "tier0/platform.h"
#include "particles/particles.h"
#include "filesystem.h"
#include "tier2/tier2.h"
#include "tier2/fileutils.h"
#include "tier2/renderutils.h"
#include "tier2/beamsegdraw.h"
#include "tier1/UtlStringMap.h"
#include "tier1/strtools.h"
#include "materialsystem/imesh.h"
#include "materialsystem/itexture.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "bitmap/psheet.h"
#include "particles_internal.h"
#include "tier0/vprof.h"

#include "..\common\blobulator\implicit\ImpRenderer.h"
#include "..\common\blobulator\implicit\ImpTiler.h"
#include "..\common\blobulator\implicit\UserFunctions.h"
#include "..\common\blobulator\iblob_renderer.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern void SetupParticleVisibility(CParticleCollection* pParticles, CParticleVisibilityData* pVisibilityData, const CParticleVisibilityInputs* pVisibilityInputs, int* nQueryHandle, IMatRenderContext* pRenderContext);

//-----------------------------------------------------------------------------
// Installs renderers
//-----------------------------------------------------------------------------
class C_OP_RenderBlobs2 : public CParticleRenderOperatorInstance
{
	DECLARE_PARTICLE_OPERATOR(C_OP_RenderBlobs2);

	float m_cubeWidth;
	float m_cutoffRadius;
	float m_renderRadius;

	struct C_OP_RenderBlobsContext_t
	{
		CParticleVisibilityData m_VisibilityData;
		int		m_nQueryHandle;
	};

	virtual uint64 GetReadControlPointMask() const
	{
		uint64 nMask = 0;
		if (VisibilityInputs.m_nCPin >= 0)
			nMask |= 1ULL << VisibilityInputs.m_nCPin;
		return nMask;
	}

	size_t GetRequiredContextBytes(void) const
	{
		return sizeof(C_OP_RenderBlobsContext_t);
	}

	virtual void InitializeContextData(CParticleCollection* pParticles, void* pContext) const
	{
		C_OP_RenderBlobsContext_t* pCtx = reinterpret_cast<C_OP_RenderBlobsContext_t*>(pContext);
		if ((VisibilityInputs.m_nCPin >= 0) || (VisibilityInputs.m_flRadiusScaleFOVBase > 0))
			pCtx->m_VisibilityData.m_bUseVisibility = true;
		else
			pCtx->m_VisibilityData.m_bUseVisibility = false;
	}

	uint32 GetWrittenAttributes(void) const
	{
		return 0;
	}

	uint32 GetReadAttributes(void) const
	{
		return PARTICLE_ATTRIBUTE_XYZ_MASK;
	}

	virtual void Render(IMatRenderContext* pRenderContext, CParticleCollection* pParticles, const Vector4D& vecDiffuseModulation, void* pContext, int nViewRecursionDepth) const;

	virtual bool IsBatchable() const
	{
		return false;
	}
};

void C_OP_RenderBlobs2::Render(IMatRenderContext* pRenderContext, CParticleCollection* pParticles, const Vector4D& vecDiffuseModulation, void* pContext, int nViewRecursionDepth) const {
	int uVar1;
	int uVar2;
	int iVar3;
	uint uVar4;
	uint uVar5;
	IMaterial* pIVar7;
	int iVar8;
	uint uVar9;
	int iVar10;
	int iVar11;
	ImpParticle* pIVar12;
	uint* puVar13;
	float* pfVar14;
	int* piVar15;
	Blobulator::BlobRenderInfo_t uStack_74;
	int iStack_64;
	CUtlVector<ImpParticle, CUtlMemoryAligned<ImpParticle, 16>> *auStack_5c;
	ImpParticle* pIStack_4c;
	float fStack_44;
	float fStack_40;
	float fStack_3c;
	float fStack_34;
	float fStack_30;
	float fStack_2c;
	float fStack_24;
	float fStack_20;
	float fStack_1c;
	int iStack_14;

	if (*(char*)((int)pContext + 8) != '\0') {
		SetupParticleVisibility(pParticles, (CParticleVisibilityData*)pContext, (CParticleVisibilityInputs*)(this + 0x58), (int*)((int)pContext + 0xc), pRenderContext);
	}
	pIVar7 = pParticles->m_pDef->GetMaterial();

	piVar15 = &iStack_14;
	const ParticleRenderData_t* pSorted = pParticles->GetRenderList(pRenderContext, false, &iVar8, (CParticleVisibilityData*)piVar15);

	iVar3 = *(int*)(pParticles + 0xe0);
	uVar4 = *(uint*)(pParticles + 0x140);
	pfVar14 = &fStack_34;
	pParticles->GetBounds((Vector*)&fStack_24, (Vector*)pfVar14);

	fStack_44 = (fStack_24 + fStack_34) * 0.5;
	fStack_40 = (fStack_20 + fStack_30) * 0.5;
	fStack_3c = (fStack_1c + fStack_2c) * 0.5;

	typedef void(*fPoint0)(const Vector& pos);
	fPoint0 call0 = (fPoint0)(**(int**)(*(int*)g_pParticleSystemMgr + 0xf0) + 0x70); // g_pParticleSystemMgr->Something()->SomethingElse(param);

	Vector position0 = Vector(fStack_44, fStack_40, fStack_3c);
	call0(position0);

	auStack_5c = new CUtlVector<ImpParticle, CUtlMemoryAligned<ImpParticle, 16>>();
	pIStack_4c = NULL;
	if ((iStack_14 < 1) || (auStack_5c->InsertMultipleBefore(0, iStack_14), iStack_14 < 1)) {
		pIVar12 = (ImpParticle*)auStack_5c;
	}
	else {
		puVar13 = (uint*)(iVar8 + -0xc);
		iVar8 = 0;
		iVar10 = 0x10;
		do {
			uVar5 = *puVar13;
			uVar9 = uVar5 & 3;
			iVar11 = (((uint)((int)uVar5 >> 0x1f) >> 0x1e) + uVar5 >> 2) * (uVar4 >> 2) * 0x10 + iVar3;
			uVar1 = *(float*)(iVar11 + 0x10 + uVar9 * 4);
			uVar2 = *(float*)(iVar11 + 0x20 + uVar9 * 4);

			*(float*)((int)auStack_5c + iVar10 + -0x10) = *(float*)(iVar11 + uVar9 * 4);
			*(float*)((int)auStack_5c + iVar10 + -0xc) = uVar1;
			*(float*)((int)auStack_5c + iVar10 + -8) = uVar2;
			*(float*)((int)auStack_5c + iVar10) = 0x3f800000;
			*(float*)((int)auStack_5c + iVar10 + -4) = 0x3f800000;

			iVar8 = iVar8 + 1;
			puVar13 = puVar13 + -4;
			iVar10 = iVar10 + 0x20;
			pIVar12 = (ImpParticle*)auStack_5c;
		} while (iVar8 < iStack_14);
	}
	
	uStack_74.m_flCubeWidth = *(float*)(this + 0x8c);
	uStack_74.m_flCutoffRadius = *(float*)(this + 0x90);
	uStack_74.m_flRenderRadius = *(float*)(this + 0x94);
	uStack_74.m_flViewScale = 1.0f;
	iStack_64 = nViewRecursionDepth;

	Blobulator::RenderBlob(true, pRenderContext, pIVar7, uStack_74, NULL, 0, pIVar12, iStack_14);
	return;
}