//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "C_PortalGhostRenderable.h"
#include "PortalRender.h"
#include "c_portal_player.h"
#include "model_types.h"

ConVar portal_ghosts_disable( "portal_ghosts_disable", "0", FCVAR_NONE, "Disables rendering of ghosted objects in portal environments" );

C_PortalGhostRenderable::C_PortalGhostRenderable( C_Prop_Portal *pOwningPortal, C_BaseEntity *pGhostSource, const VMatrix &matGhostTransform, float *pSharedRenderClipPlane, bool bLocalPlayer )
: m_hGhostedRenderable( pGhostSource ), 
	m_matGhostTransform( matGhostTransform ), 
	m_pSharedRenderClipPlane( pSharedRenderClipPlane ),
	m_bLocalPlayer( bLocalPlayer ),
	m_pOwningPortal( pOwningPortal )
{
	m_bSourceIsBaseAnimating = (dynamic_cast<C_BaseAnimating *>(pGhostSource) != NULL);

	RenderWithViewModels( pGhostSource->IsRenderingWithViewModels() );
	SetModelName( pGhostSource->GetModelName() );
	SetSize( pGhostSource->CollisionProp()->OBBMins(), pGhostSource->CollisionProp()->OBBMaxs() );
}

C_PortalGhostRenderable::~C_PortalGhostRenderable( void )
{
	m_hGhostedRenderable = NULL;
}

void C_PortalGhostRenderable::PerFrameUpdate( void )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if ( pGhostedRenderable == NULL )
		return;

	SetModelName( pGhostedRenderable->GetModelName() );
	SetModelIndex( pGhostedRenderable->GetModelIndex() );
	SetEffects( pGhostedRenderable->GetEffects() | EF_NOINTERP );		
	m_flAnimTime = pGhostedRenderable->m_flAnimTime;		

	if( m_bSourceIsBaseAnimating )
	{
		C_BaseAnimating *pSource = (C_BaseAnimating *)pGhostedRenderable;
		SetCycle( pSource->GetCycle() );
		SetSequence( pSource->GetSequence() );

		SetBody(m_nBody);
		SetSkin(m_nSkin);
	}

	SetSize( pGhostedRenderable->CollisionProp()->OBBMins(), pGhostedRenderable->CollisionProp()->OBBMaxs() );

	// Set position and angles relative to the object it's ghosting
	Vector ptNewOrigin = m_matGhostTransform * pGhostedRenderable->GetAbsOrigin();
	QAngle qNewAngles = TransformAnglesToWorldSpace( pGhostedRenderable->GetAbsAngles(), m_matGhostTransform.As3x4() );

	SetNetworkOrigin( ptNewOrigin );
	SetLocalOrigin( ptNewOrigin );
	SetAbsOrigin( ptNewOrigin );
	SetNetworkAngles( qNewAngles );
	SetLocalAngles( qNewAngles );
	SetAbsAngles( qNewAngles );

	g_pClientLeafSystem->RenderableChanged( RenderHandle() );
}

Vector const& C_PortalGhostRenderable::GetRenderOrigin( void )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
		return m_ReferencedReturns.vRenderOrigin;

	m_ReferencedReturns.vRenderOrigin = m_matGhostTransform * pGhostedRenderable->GetRenderOrigin();
	return m_ReferencedReturns.vRenderOrigin;
}

QAngle const& C_PortalGhostRenderable::GetRenderAngles( void )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
		return m_ReferencedReturns.qRenderAngle;

	m_ReferencedReturns.qRenderAngle = TransformAnglesToWorldSpace( pGhostedRenderable->GetRenderAngles(), m_matGhostTransform.As3x4() );
	return m_ReferencedReturns.qRenderAngle;
}

bool C_PortalGhostRenderable::SetupBones( matrix3x4a_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
		return false;

	if( pGhostedRenderable->SetupBones( pBoneToWorldOut, nMaxBones, boneMask, currentTime ) && pBoneToWorldOut )
	{
		int nBoneCount = MIN( nMaxBones, GetModelPtr()->numbones() );
		for( int i = 0; i != nBoneCount; ++i )
		{
			matrix3x4a_t transform;
			transform = m_matGhostTransform.As3x4();
			//pBoneToWorldOut[i] = (m_matGhostTransform * pBoneToWorldOut[i]).As3x4();
			ConcatTransforms_Aligned( transform, pBoneToWorldOut[i], pBoneToWorldOut[i] );
		}
	}

	return true;
}

void C_PortalGhostRenderable::GetRenderBounds( Vector& mins, Vector& maxs )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
	{
		mins = maxs = vec3_origin;
		return;
	}

	pGhostedRenderable->GetRenderBounds( mins, maxs );
}

void C_PortalGhostRenderable::GetRenderBoundsWorldspace( Vector& mins, Vector& maxs )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
	{
		mins = maxs = vec3_origin;
		return;
	}

	pGhostedRenderable->GetRenderBoundsWorldspace( mins, maxs );
	TransformAABB( m_matGhostTransform.As3x4(), mins, maxs, mins, maxs );
}

void C_PortalGhostRenderable::GetShadowRenderBounds( Vector &mins, Vector &maxs, ShadowType_t shadowType )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
	{
		mins = maxs = vec3_origin;
		return;
	}

	pGhostedRenderable->GetShadowRenderBounds( mins, maxs, shadowType );
	TransformAABB( m_matGhostTransform.As3x4(), mins, maxs, mins, maxs );
}

/*bool C_PortalGhostRenderable::GetShadowCastDistance( float *pDist, ShadowType_t shadowType ) const
{
	if( m_pGhostedRenderable == NULL )
		return false;

	return m_pGhostedRenderable->GetShadowCastDistance( pDist, shadowType );
}

bool C_PortalGhostRenderable::GetShadowCastDirection( Vector *pDirection, ShadowType_t shadowType ) const
{
	if( m_pGhostedRenderable == NULL )
		return false;

	if( m_pGhostedRenderable->GetShadowCastDirection( pDirection, shadowType ) )
	{
		if( pDirection )
			*pDirection = m_matGhostTransform.ApplyRotation( *pDirection );

		return true;
	}
	return false;
}*/

const matrix3x4_t & C_PortalGhostRenderable::RenderableToWorldTransform()
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
		return m_ReferencedReturns.matRenderableToWorldTransform;

	ConcatTransforms( m_matGhostTransform.As3x4(), pGhostedRenderable->RenderableToWorldTransform(), m_ReferencedReturns.matRenderableToWorldTransform );
	return m_ReferencedReturns.matRenderableToWorldTransform;
}

bool C_PortalGhostRenderable::GetAttachment( int number, Vector &origin, QAngle &angles )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
		return false;

	if( pGhostedRenderable->GetAttachment( number, origin, angles ) )
	{
		origin = m_matGhostTransform * origin;
		angles = TransformAnglesToWorldSpace( angles, m_matGhostTransform.As3x4() );
		return true;
	}
	return false;
}

bool C_PortalGhostRenderable::GetAttachment( int number, matrix3x4_t &matrix )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
		return false;

	if( pGhostedRenderable->GetAttachment( number, matrix ) )
	{
		ConcatTransforms( m_matGhostTransform.As3x4(), matrix, matrix );
		return true;
	}
	return false;
}

bool C_PortalGhostRenderable::GetAttachment( int number, Vector &origin )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
		return false;

	if( pGhostedRenderable->GetAttachment( number, origin ) )
	{
		origin = m_matGhostTransform * origin;
		return true;
	}
	return false;
}

bool C_PortalGhostRenderable::GetAttachmentVelocity( int number, Vector &originVel, Quaternion &angleVel )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;
	
	if( pGhostedRenderable == NULL )
		return false;

	Vector ghostVel;
	if( pGhostedRenderable->GetAttachmentVelocity( number, ghostVel, angleVel ) )
	{
		Vector3DMultiply( m_matGhostTransform, ghostVel, originVel );
		Vector3DMultiply( m_matGhostTransform, *(Vector*)( &angleVel ), *(Vector*)( &angleVel ) );
		return true;
	}
	return false;
}


int C_PortalGhostRenderable::DrawModel( int flags, const RenderableInstance_t& instance )
{
	if ( portal_ghosts_disable.GetBool() )
		return 0;

	if( m_bSourceIsBaseAnimating )
	{
		if( m_bLocalPlayer )
		{
			C_Portal_Player *pPlayer = C_Portal_Player::GetLocalPlayer();

			if ( !pPlayer->IsAlive() )
			{
				// Dead player uses a ragdoll to draw, so don't ghost the dead entity
				return 0;
			}
			else if( GetPortalRender().GetViewRecursionLevel() == 0 )
			{
				if( pPlayer->m_bEyePositionIsTransformedByPortal )
					return 0;
			}
			else if( GetPortalRender().GetViewRecursionLevel() == 1 )
			{
				if( !pPlayer->m_bEyePositionIsTransformedByPortal )
					return 0;
			}
		}

		return C_BaseAnimating::DrawModel( flags, instance );
	}
	else
	{
		render->DrawBrushModel( m_hGhostedRenderable, 
								(model_t *)m_hGhostedRenderable->GetModel(), 
								GetRenderOrigin(), 
								GetRenderAngles(), 
								flags & STUDIO_TRANSPARENCY ? true : false );
		
		return 1;
	}

	return 0;
}

bool C_PortalGhostRenderable::OnInternalDrawModel( ClientModelRenderInfo_t *pInfo )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;
	
	if( pGhostedRenderable == NULL )
		return false;

	// TODO: account for m_pGhostedRenderable having a custom lighting origin
	pInfo->pLightingOrigin = &(pGhostedRenderable->GetAbsOrigin());
	return true;
}

ModelInstanceHandle_t C_PortalGhostRenderable::GetModelInstance()
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if ( pGhostedRenderable )
		return pGhostedRenderable->GetModelInstance();

	return BaseClass::GetModelInstance();
}

RenderableTranslucencyType_t C_PortalGhostRenderable::ComputeTranslucencyType( void ) 
{ 
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if ( pGhostedRenderable == NULL )
		return RENDERABLE_IS_OPAQUE;

	return pGhostedRenderable->ComputeTranslucencyType();
}

int C_PortalGhostRenderable::GetRenderFlags()
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if(pGhostedRenderable == NULL )
		return false;

	return pGhostedRenderable->GetRenderFlags();
}

/*const model_t* C_PortalGhostRenderable::GetModel( ) const
{
	if( m_pGhostedRenderable == NULL )
		return NULL;

	return m_pGhostedRenderable->GetModel();
}

int C_PortalGhostRenderable::GetBody()
{
	if( m_pGhostedRenderable == NULL )
		return 0;

	return m_pGhostedRenderable->GetBody();
}*/

void C_PortalGhostRenderable::GetColorModulation( float* color )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
		return;

	return pGhostedRenderable->GetColorModulation( color );
}

/*ShadowType_t C_PortalGhostRenderable::ShadowCastType()
{
	if( m_pGhostedRenderable == NULL )
		return SHADOWS_NONE;

	return m_pGhostedRenderable->ShadowCastType();
}*/

int C_PortalGhostRenderable::LookupAttachment( const char *pAttachmentName )
{
	C_BaseEntity* pGhostedRenderable = m_hGhostedRenderable;

	if( pGhostedRenderable == NULL )
		return -1;

	return pGhostedRenderable->LookupAttachment( pAttachmentName );
}

/*int C_PortalGhostRenderable::GetSkin()
{
	if( m_pGhostedRenderable == NULL )
		return -1;


	return m_pGhostedRenderable->GetSkin();
}

bool C_PortalGhostRenderable::IsTwoPass( void )
{
	if( m_pGhostedRenderable == NULL )
		return false;

	return m_pGhostedRenderable->IsTwoPass();
}*/









