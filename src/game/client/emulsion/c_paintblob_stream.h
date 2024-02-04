#pragma once
#include "c_baseanimating.h"
#include "paint_defs.h"
#include "c_ai_basenpc.h"

class C_PaintBlobStream : public C_AI_BaseNPC, public IPaintStream {
	DECLARE_CLASS(C_PaintBlobStream, C_AI_BaseNPC)
	DECLARE_CLIENTCLASS();
	DECLARE_INTERPOLATION();
	DECLARE_PREDICTABLE();
public:

	C_PaintBlobStream();

	void Spawn();

	bool ShouldInterpolate() { return true; }
	void PostDataUpdate(DataUpdateType_t updateType);
	void ClientThink();

	virtual void GetRenderBounds(Vector& theMins, Vector& theMaxs);
	ShadowType_t ShadowCastType() { return SHADOWS_NONE; }

	virtual bool IsTransparent();
	bool UsesPowerOfTwoFrameBufferTexture(void);
	bool UsesFullFrameBufferTexture(void);

	virtual int DrawModel(int flags, const RenderableInstance_t& instance) override;

	virtual bool ShouldRegenerateOriginFromCellBits() const {
		return true;
	}

	// network + interpolated particle info
	int	m_nActiveParticles;
	int m_nPaintType;

	CUtlVector< Vector	> m_vecSurfacePositions;
	CUtlVector< CInterpolatedVar< Vector > > m_iv_vecSurfacePositions;

	CUtlVector< float > m_vecSurfaceVs;
	CUtlVector< CInterpolatedVar< float > > m_iv_vecSurfaceVs;

	CUtlVector< float > m_vecSurfaceRs;
	CUtlVector< CInterpolatedVar< float > > m_iv_vecSurfaceRs;

	CUtlVector< float > m_vecRadii;

	CUtlVector<short> m_nIndices;
	CUtlVector<short> m_nNewIndices;

private:

	int m_nActiveParticlesInternal;

};