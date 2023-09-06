#pragma once
#include "../../common/blobulator/Implicit/ImpDefines.h"
#include "../../common/blobulator/Implicit/ImpRenderer.h"
#include "../../common/blobulator/Implicit/ImpTiler.h"
#include "../../common/blobulator/Implicit/UserFunctions.h"
#include "../../common/blobulator/smartarray.h"
//#include "..\game\client\viewrender.h" // view_id_t // not worky for particles lol

// RE helpers (like garden gnomes)
static const float g_pReal3f800000 = 1.0f; // little endian 1.0f

namespace Tiler {

	static Point3D g_pointCurTile = Point3D();

	inline void drawTile(ImpTile* pTile, SweepRenderer* pRenderer) {
		int iVar1;

		Point3D _tile_debug_color = *(Point3D*)(pTile + 0x30); // TODO: double check the offsets again
		Point3D _offset = *(Point3D*)(pRenderer + 0x10); // TODO: double check the offsets again
		g_pointCurTile = *(Point3D*)(pTile + 0x38); // TODO: double check this

		pRenderer->setOffset(_offset);
		pRenderer->beginTile(pTile);

		iVar1 = 0;
		if (0 < *(int*)(pTile + 0x48)) {
			do {
				pRenderer->addParticle(*(ImpParticle**)(*(int*)(pTile + 0x44) + iVar1 * 4), false);
				iVar1 = iVar1 + 1;
			} while (iVar1 < *(int*)(pTile + 0x48));
		}

		pRenderer->endTile();
		return;
	}
}

namespace Blobulator {

	// members
	static float g_ExclusionXMin, g_ExclusionXMax;
	static float g_ExclusionYMin, g_ExclusionYMax;
	static float g_ExclusionZMin, g_ExclusionZMax;

	static int g_nNumberOfMatrices;

	// structs / classes
	struct BlobRenderInfo_t {
	
		float m_flCubeWidth;
		float m_flCutoffRadius;
		float m_flRenderRadius;
		float m_flViewScale;

		//view_id_t m_nViewID;
	};

	class CDrawInfo {

	};

	class IBlobRenderer {

	};

	// functions
	ImpTiler**	Paintblob_BeginDraw(IMaterial*, BlobRenderInfo_t const&, ImpParticle*, int, matrix3x4_t const*, int, bool);
	void		Paintblob_DrawFastSphere(CMeshBuilder&, Vector const&, float);
	void		Paintblob_DrawIsoSurfaceSynchronous(IMaterial*, BlobRenderInfo_t const&, CUtlEnvelope<ImpParticle>&, int, CUtlEnvelope<matrix3x4_t>&, int, bool);
	void		Paintblob_DrawSpheres(Point3D const&, IMaterial*, BlobRenderInfo_t const&, ImpParticle*, int, matrix3x4_t const*, int);
	void		Paintblob_WaitForEndOfDraw(IMaterial*, CDrawInfo*, bool);

	inline void		RenderBlob(bool, IMatRenderContext*, IMaterial*, BlobRenderInfo_t const&, matrix3x4_t const*, int, ImpParticle*, int) {}
	void		SetupRenderContext(IMaterial*, CDrawInfo*);
	void		ShutdownBlob();
	void		FrameUpdate(int);

	// todo: undefined4 GetDrawMaterial();
}