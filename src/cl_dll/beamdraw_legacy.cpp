//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//


#include "cbase.h"
//#include "beamdraw.h"
#include "tier2/beamsegdraw.h"
#ifdef CLIENT_DLL
#include "enginesprite.h"
#include "IViewRender_Beams.h"
#include "view.h"
#include "iviewrender.h"
#include "engine/ivmodelinfo.h"
#include "fx_line.h"
#endif

#include "materialsystem/imaterialvar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar r_drawsprites;
extern ConVar r_DrawBeams;

// i flagged this as a cheat, since if anyone were to turn this off in a multiplayer game it could impede the 
// use of certain mechanics (like csgo smoke bomb)
//ConVar cl_force_beamsegdraw_meshbuilder("cl_force_beamsegdraw_meshbuilder", "1", FCVAR_CLIENTDLL | FCVAR_CHEAT);

static IMaterial *g_pBeamWireframeMaterial;

// ------------------------------------------------------------------------------------------ //
// CBeamSegDraw implementation.
// ------------------------------------------------------------------------------------------ //
void CBeamSegDraw::Start(IMatRenderContext* pRenderContext, int nSegs, IMaterial *pMaterial, CMeshBuilder *pMeshBuilder, int nMeshVertCount )
{
	Assert( nSegs >= 2 );

	m_nSegsDrawn = 0;
	m_nTotalSegs = nSegs;

	if ( pMeshBuilder )
	{
		m_Mesh = *pMeshBuilder;
		m_nMeshVertCount = nMeshVertCount;
	}
	else
	{
		//m_pMeshBuilder = NULL;
		m_nMeshVertCount = 0;

#ifdef CLIENT_DLL
		if ( ShouldDrawInWireFrameMode() || r_DrawBeams.GetInt() == 2 )
		{
			if ( !g_pBeamWireframeMaterial )
				g_pBeamWireframeMaterial = materials->FindMaterial("shadertest/wireframevertexcolor", TEXTURE_GROUP_OTHER);
			pMaterial = g_pBeamWireframeMaterial;
		}
#endif

		//IMesh *pMesh = materials->GetDynamicMesh( true, NULL, NULL, pMaterial );
		//pRenderContext->Bind(pMaterial);

		m_pMesh = pRenderContext->GetDynamicMesh( true, NULL, NULL, pMaterial );
		m_Mesh.Begin(m_pMesh, MATERIAL_TRIANGLE_STRIP, (nSegs-1) * 2 );
	}
}

#ifdef CLIENT_DLL
inline void CBeamSegDraw::ComputeNormal( const Vector& vecCameraPos, const Vector &vStartPos, const Vector &vNextPos, Vector *pNormal )
{
	// vTangentY = line vector for beam
	Vector vTangentY;
	VectorSubtract( vStartPos, vNextPos, vTangentY );
	
	// vDirToBeam = vector from viewer origin to beam
	Vector vDirToBeam;
	//VectorSubtract( vStartPos, vecCameraPos, vDirToBeam );
	VectorSubtract( vStartPos, CurrentViewOrigin(), vDirToBeam );

	/// Get a vector that is perpendicular to us and perpendicular to the beam.
	// This is used to fatten the beam.
	CrossProduct( vTangentY, vDirToBeam, *pNormal );
	VectorNormalizeFast( *pNormal );
}

inline void CBeamSegDraw::SpecifySeg( const Vector& vecCameraPos, const Vector &vNormal )
{
	m_vecCameraPos = vecCameraPos; // saving this origin to hopefully fix disappearing sprites

	// SUCKY: Need to do a fair amount more work to get the tangent owing to the averaged normal
	Vector vDirToBeam, vTangentY;
	VectorSubtract( m_Seg.m_vPos, CurrentViewOrigin(), vDirToBeam);
	//VectorSubtract( m_Seg.m_vPos, vecCameraPos, vDirToBeam);
	CrossProduct( vDirToBeam, vNormal, vTangentY );
	VectorNormalizeFast( vTangentY );

	// Build the endpoints.
	Vector vPoint1, vPoint2;
	VectorMA( m_Seg.m_vPos,  m_Seg.m_flWidth*0.5f, vNormal, vPoint1 );
	VectorMA( m_Seg.m_vPos, -m_Seg.m_flWidth*0.5f, vNormal, vPoint2 );

	//Vector4D vecSegColour;
	//m_Seg.GetColor(&vecSegColour);

	// Specify the points.
	m_Mesh.Position3fv(vPoint1.Base());
	//m_Mesh.Color4f(Vector4DExpand(vecSegColour));
	m_Mesh.Color4ub(m_Seg.m_color.r, m_Seg.m_color.g, m_Seg.m_color.b, m_Seg.m_color.a);
	m_Mesh.TexCoord2f(0, 0, m_Seg.m_flTexCoord);
	m_Mesh.TexCoord2f(1, 0, m_Seg.m_flTexCoord);
	m_Mesh.TangentS3fv(vNormal.Base());
	m_Mesh.TangentT3fv(vTangentY.Base());
	m_Mesh.AdvanceVertex();

	m_Mesh.Position3fv(vPoint2.Base());
	m_Mesh.Color4ub(m_Seg.m_color.r, m_Seg.m_color.g, m_Seg.m_color.b, m_Seg.m_color.a);
	m_Mesh.TexCoord2f(0, 1, m_Seg.m_flTexCoord);
	m_Mesh.TexCoord2f(1, 1, m_Seg.m_flTexCoord);
	m_Mesh.TangentS3fv(vNormal.Base());
	m_Mesh.TangentT3fv(vTangentY.Base());
	m_Mesh.AdvanceVertex();

	//if (m_nSegsDrawn > 1)
	//{
	//	int nBase = ((m_nSegsDrawn - 2) * 2) + m_nMeshVertCount;

	//	m_Mesh.FastIndex(nBase);
	//	m_Mesh.FastIndex(nBase + 1);
	//	m_Mesh.FastIndex(nBase + 2);
	//	m_Mesh.FastIndex(nBase + 1);
	//	m_Mesh.FastIndex(nBase + 3);
	//	m_Mesh.FastIndex(nBase + 2);
	//}

	//if ( m_Mesh != NULL )
	//{
	//	// Specify the points.
	//	m_Mesh->Position3fv( vPoint1.Base() );
	//	m_Mesh->Color4f(Vector4DExpand(vecSegColour));
	//	m_Mesh->TexCoord2f( 0, 0, m_Seg.m_flTexCoord );
	//	m_Mesh->TexCoord2f( 1, 0, m_Seg.m_flTexCoord );
	//	m_Mesh->TangentS3fv( vNormal.Base() );
	//	m_Mesh->TangentT3fv( vTangentY.Base() );
	//	m_Mesh->AdvanceVertex();
	//	
	//	m_Mesh->Position3fv( vPoint2.Base() );
	//	m_Mesh->Color4f(Vector4DExpand(vecSegColour));
	//	m_Mesh->TexCoord2f( 0, 1, m_Seg.m_flTexCoord );
	//	m_Mesh->TexCoord2f( 1, 1, m_Seg.m_flTexCoord );
	//	m_Mesh->TangentS3fv( vNormal.Base() );
	//	m_Mesh->TangentT3fv( vTangentY.Base() );
	//	m_Mesh->AdvanceVertex();

	//	if ( m_nSegsDrawn > 1 )
	//	{
	//		int nBase = ( ( m_nSegsDrawn - 2 ) * 2 ) + m_nMeshVertCount;

	//		m_Mesh->FastIndex( nBase );
	//		m_Mesh->FastIndex( nBase + 1 );
	//		m_Mesh->FastIndex( nBase + 2 );
	//		m_Mesh->FastIndex( nBase + 1 );
	//		m_Mesh->FastIndex( nBase + 3 );
	//		m_Mesh->FastIndex( nBase + 2 );
	//	}
	//}
	//else
	//{
	//	// Specify the points.
	//	m_Mesh.Position3fv( vPoint1.Base() );
	//	m_Mesh.Color4f(Vector4DExpand(vecSegColour));
	//	m_Mesh.TexCoord2f( 0, 0, m_Seg.m_flTexCoord );
	//	m_Mesh.TexCoord2f( 1, 0, m_Seg.m_flTexCoord );
	//	m_Mesh.TangentS3fv( vNormal.Base() );
	//	m_Mesh.TangentT3fv( vTangentY.Base() );
	//	m_Mesh.AdvanceVertex();
	//	
	//	m_Mesh.Position3fv( vPoint2.Base() );
	//	m_Mesh.Color4f(Vector4DExpand(vecSegColour));
	//	m_Mesh.TexCoord2f( 0, 1, m_Seg.m_flTexCoord );
	//	m_Mesh.TexCoord2f( 1, 1, m_Seg.m_flTexCoord );
	//	m_Mesh.TangentS3fv( vNormal.Base() );
	//	m_Mesh.TangentT3fv( vTangentY.Base() );
	//	m_Mesh.AdvanceVertex();
	//}
}

void CBeamSegDraw::NextSeg( BeamSeg_t *pSeg )
{
 	if ( m_nSegsDrawn > 0 )
	{
		// Get a vector that is perpendicular to us and perpendicular to the beam.
		// This is used to fatten the beam.
		Vector vNormal, vAveNormal;
		ComputeNormal( m_vecCameraPos, m_Seg.m_vPos, pSeg->m_vPos, &vNormal ); // legacy code note: this version doesnt use camera pos

		if ( m_nSegsDrawn > 1 )
		{
			// Average this with the previous normal
			VectorAdd( vNormal, m_vNormalLast, vAveNormal );
			vAveNormal *= 0.5f;
			VectorNormalizeFast( vAveNormal );
		}
		else
		{
			vAveNormal = vNormal;
		}

		m_vNormalLast = vNormal;
		SpecifySeg(m_vecCameraPos, vAveNormal );
	}

	m_Seg = *pSeg;
	++m_nSegsDrawn;

 	if( m_nSegsDrawn == m_nTotalSegs )
	{
		SpecifySeg(m_vecCameraPos, m_vNormalLast );
	}
}

#endif

void CBeamSegDraw::End()
{
	//if ( m_pMeshBuilder )
	//{
	//	m_pMeshBuilder = NULL;
	//	return;
	//}

	m_Mesh.End( false, true );
	m_pMesh->Draw();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &normal - 
//-----------------------------------------------------------------------------
void CBeamSegDrawArbitrary::SetNormal( const Vector &normal )
{
	m_vNormalLast = normal;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pSeg - 
//-----------------------------------------------------------------------------
void CBeamSegDrawArbitrary::NextSeg( BeamSeg_t *pSeg )
{
 	if ( m_nSegsDrawn > 0 )
	{
		Vector	segDir = ( m_PrevSeg.m_vPos - pSeg->m_vPos );
		VectorNormalize( segDir );

		Vector	normal = CrossProduct( segDir, m_vNormalLast );
		SpecifySeg( normal );
	}

	m_PrevSeg = m_Seg;
	m_Seg = *pSeg;
	++m_nSegsDrawn;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &vNextPos - 
//-----------------------------------------------------------------------------
void CBeamSegDrawArbitrary::SpecifySeg( const Vector &vNormal )
{
	// Build the endpoints.
	Vector vPoint1, vPoint2;
	VectorMA( m_Seg.m_vPos,  m_Seg.m_flWidth*0.5f, vNormal, vPoint1 );
	VectorMA( m_Seg.m_vPos, -m_Seg.m_flWidth*0.5f, vNormal, vPoint2 );

	Vector4D vecSegColour;
	m_Seg.GetColor(&vecSegColour);

	// Specify the points.
	m_Mesh.Position3fv( vPoint1.Base() );
	m_Mesh.Color4f(Vector4DExpand(vecSegColour));
	m_Mesh.TexCoord2f( 0, 0, m_Seg.m_flTexCoord );
	m_Mesh.TexCoord2f( 1, 0, m_Seg.m_flTexCoord );
	m_Mesh.AdvanceVertex();
	
	m_Mesh.Position3fv( vPoint2.Base() );
	m_Mesh.Color4f(Vector4DExpand(vecSegColour) );
	m_Mesh.TexCoord2f( 0, 1, m_Seg.m_flTexCoord );
	m_Mesh.TexCoord2f( 1, 1, m_Seg.m_flTexCoord );
	m_Mesh.AdvanceVertex();
}
