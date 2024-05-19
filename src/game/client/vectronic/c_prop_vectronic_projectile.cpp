//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_prop_vectronic_projectile.h"
#include "materialsystem/IMaterial.h"
#include "model_types.h"
#include "c_physicsprop.h"
#include "c_te_effect_dispatch.h"
#include "fx_quad.h"
#include "fx.h"
#ifndef SWARM_DLL
#include "clienteffectprecachesystem.h"
#endif
#include "view.h"
#include "view_scene.h"
#include "beamdraw.h"
#include "vectronic_gamerules.h"
#include "particles_new.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// vecball 3-5 defines moved to vectronic_gamerules.cpp (maddi)
extern ConVar	vectronic_vecball3_sprite;
extern ConVar	vectronic_vecball4_sprite;
extern ConVar	vectronic_vecball5_sprite;

// Precache our effects
extern ConVar vectronic_vecball3_color_r;
extern ConVar vectronic_vecball3_color_g;
extern ConVar vectronic_vecball3_color_b;

extern ConVar vectronic_vecball4_color_r;
extern ConVar vectronic_vecball4_color_g;
extern ConVar vectronic_vecball4_color_b;

extern ConVar vectronic_vecball5_color_r;
extern ConVar vectronic_vecball5_color_g;
extern ConVar vectronic_vecball5_color_b;

#define BALL3_SPRITE vectronic_vecball3_sprite.GetString()
#define BALL4_SPRITE vectronic_vecball4_sprite.GetString()
#define BALL5_SPRITE vectronic_vecball5_sprite.GetString()


#ifndef SWARM_DLL
CLIENTEFFECT_REGISTER_BEGIN( PrecacheEffectParticleBall )

CLIENTEFFECT_MATERIAL( "Effects/ball_muzzle" )
CLIENTEFFECT_MATERIAL( "Effects/ball_blur" )

CLIENTEFFECT_MATERIAL( "Effects/ball_blue" )
CLIENTEFFECT_MATERIAL( "Effects/ball_green" )
CLIENTEFFECT_MATERIAL( "Effects/ball_purple")
CLIENTEFFECT_MATERIAL( "Effects/ball_red")


CLIENTEFFECT_MATERIAL( "Effects/ball_blue_impact" )
CLIENTEFFECT_MATERIAL( "Effects/ball_green_impact" )
CLIENTEFFECT_MATERIAL( "Effects/ball_purple_impact" )
CLIENTEFFECT_MATERIAL( "Effects/ball_red_impact" )

CLIENTEFFECT_MATERIAL( "Effects/ball_blue_flash" )
CLIENTEFFECT_MATERIAL( "Effects/ball_green_flash" )
CLIENTEFFECT_MATERIAL( "Effects/ball_yellow_flash" )
CLIENTEFFECT_MATERIAL( "Effects/ball_yellow_flash"  )

CLIENTEFFECT_REGISTER_END()
#else
//Precahce the effects
PRECACHE_REGISTER_BEGIN( GLOBAL, PrecacheBall )
	PRECACHE( MATERIAL, "Effects/ball_muzzle" )
	PRECACHE( MATERIAL, "Effects/ball_blur"  )

	PRECACHE( MATERIAL, BALL_SPRITE_IMPACT  )
	PRECACHE( MATERIAL, BALL_SPRITE_FLASH  )

	PRECACHE( MATERIAL, BALL0_SPRITE )
	PRECACHE( MATERIAL, BALL1_SPRITE )
	PRECACHE( MATERIAL, BALL2_SPRITE )

	PRECACHE( MATERIAL, BALL3_SPRITE )
	PRECACHE( MATERIAL, BALL4_SPRITE )
	PRECACHE( MATERIAL, BALL5_SPRITE )

//	PRECACHE( MATERIAL, BALL0_SPRITE_IMPACT )
//	PRECACHE( MATERIAL, BALL1_SPRITE_IMPACT )
//	PRECACHE( MATERIAL, BALL2_SPRITE_IMPACT )

//	PRECACHE( MATERIAL, BALL0_SPRITE_FLASH )
//	PRECACHE( MATERIAL, BALL1_SPRITE_FLASH )
//	PRECACHE( MATERIAL, BALL2_SPRITE_FLASH )

//	PRECACHE( MATERIAL, PRECACHED_BALL3_IMPACT )
//	PRECACHE( MATERIAL, PRECACHED_BALL4_IMPACT )
//	PRECACHE( MATERIAL, PRECACHED_BALL5_IMPACT )

//	PRECACHE( MATERIAL, PRECACHED_BALL3_FLASH )
//	PRECACHE( MATERIAL, PRECACHED_BALL4_FLASH )
//	PRECACHE( MATERIAL, PRECACHED_BALL5_FLASH )
	


PRECACHE_REGISTER_END()
#endif

IMPLEMENT_CLIENTCLASS_DT( C_PropParticleBall, DT_PropParticleBall, CPropParticleBall )
	RecvPropBool( RECVINFO( m_bEmit ) ),
	RecvPropFloat( RECVINFO( m_flRadius ) ),
	RecvPropBool( RECVINFO( m_bHeld ) ),
	RecvPropBool( RECVINFO( m_intType ) ),
	RecvPropBool( RECVINFO( m_bLaunched ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_PropParticleBall::C_PropParticleBall( void )
{
	m_pFlickerMaterial = NULL;
	m_pBodyMaterial = NULL;
	m_pBlurMaterial = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : updateType - 
//-----------------------------------------------------------------------------
void C_PropParticleBall::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_vecLastOrigin = GetAbsOrigin();
		InitMaterials();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : RenderGroup_t
//-----------------------------------------------------------------------------
RenderGroup_t C_PropParticleBall::GetRenderGroup( void )
{
#ifndef SWARM_DLL
	return RENDER_GROUP_TRANSLUCENT_ENTITY;
#else
	return RENDER_GROUP_TRANSLUCENT;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Cache the material handles
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C_PropParticleBall::InitMaterials( void )
{
	// Motion blur
	if ( m_pBlurMaterial == NULL )
	{
		m_pBlurMaterial = materials->FindMaterial( "Effects/ball_blur", NULL, false );

		if ( m_pBlurMaterial == NULL )
			return false;
	}

	if ( m_pBodyMaterial == NULL )
	{
		if (m_intType == 0)
		{
			m_pBodyMaterial = materials->FindMaterial( BALL0_SPRITE, NULL, false );
		}
		if (m_intType == 1)
		{
			m_pBodyMaterial = materials->FindMaterial( BALL1_SPRITE, NULL, false );
		}
		if (m_intType == 2)
		{
			m_pBodyMaterial = materials->FindMaterial( BALL2_SPRITE, NULL, false );
		}
		// Custom
		if (m_intType == 3)
		{
			m_pBodyMaterial = materials->FindMaterial( BALL3_SPRITE, NULL, false );
		}
		if (m_intType == 4)
		{
			m_pBodyMaterial = materials->FindMaterial( BALL4_SPRITE, NULL, false );
		}
		if (m_intType == 5)
		{
			m_pBodyMaterial = materials->FindMaterial( BALL5_SPRITE, NULL, false );
		}

		if ( m_pBodyMaterial == NULL )
			return false;
	}

	// Flicker material
	if ( m_pFlickerMaterial == NULL )
	{
		m_pFlickerMaterial = materials->FindMaterial( "Effects/ball_muzzle", NULL, false );

		if ( m_pFlickerMaterial == NULL )
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_PropParticleBall::DrawMotionBlur( void )
{
	float color[3];

	Vector	vecDir = GetAbsOrigin() - m_vecLastOrigin;
	float	speed = VectorNormalize( vecDir );
	
	speed = clamp( speed, 0, 32 );

#ifndef SWARM_DLL
	float	stepSize = min( ( speed * 0.5f ), 4.0f );
#else
	float	stepSize = MIN( ( speed * 0.5f ), 4.0f );
#endif

	Vector	spawnPos = GetAbsOrigin();
	Vector	spawnStep = -vecDir * stepSize;

	float base = RemapValClamped( speed, 4, 32, 0.0f, 1.0f );

	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->Bind( m_pBlurMaterial );

	// Draw the motion blurred trail
	for ( int i = 0; i < 8; i++ )
	{
		spawnPos += spawnStep;

		color[0] = color[1] = color[2] = base * ( 1.0f - ( (float) i / 12.0f ) );

		DrawHalo( m_pBlurMaterial, spawnPos, m_flRadius, color );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_PropParticleBall::DrawFlicker( void )
{
	float rand1 = random->RandomFloat( 0.2f, 0.3f );
	float rand2 = random->RandomFloat( 1.5f, 2.5f );

	if ( gpGlobals->frametime == 0.0f )
	{
		rand1 = 0.2f;
		rand2 = 1.5f;
	}

	float color[3];
	color[0] = color[1] = color[2] = rand1;

	// Draw the flickering glow
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->Bind( m_pFlickerMaterial );
	DrawHalo( m_pFlickerMaterial, GetAbsOrigin(), m_flRadius * rand2, color );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : pMaterial - 
//			source - 
//			color - 
//-----------------------------------------------------------------------------
void DrawRingOriented( const Vector& source, float scale, float const *color, float roll )
{
	Vector point, screen;
	
	CMatRenderContextPtr pRenderContext( materials );
	IMesh* pMesh = pRenderContext->GetDynamicMesh();

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	// Transform source into screen space
	ScreenTransform( source, screen );

	Vector right, up;
	float sr, cr;

	SinCos( roll, &sr, &cr );

	for ( int i = 0; i < 3; i++ )
	{
		right[i] = CurrentViewRight()[i] * cr + CurrentViewUp()[i] * sr;
		up[i] = CurrentViewRight()[i] * -sr + CurrentViewUp()[i] * cr;
	}

	meshBuilder.Color3fv (color);
	meshBuilder.TexCoord2f (0, 0, 1);
	VectorMA (source, -scale, up, point);
	VectorMA (point, -scale, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color3fv (color);
	meshBuilder.TexCoord2f (0, 0, 0);
	VectorMA (source, scale, up, point);
	VectorMA (point, -scale, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color3fv (color);
	meshBuilder.TexCoord2f (0, 1, 0);
	VectorMA (source, scale, up, point);
	VectorMA (point, scale, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color3fv (color);
	meshBuilder.TexCoord2f (0, 1, 1);
	VectorMA (source, -scale, up, point);
	VectorMA (point, scale, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();
	
	meshBuilder.End();
	pMesh->Draw();
}

#ifndef SWARM_DLL
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flags - 
// Output : int
//-----------------------------------------------------------------------------
int C_PropParticleBall::DrawModel( int flags )
{
	if ( !m_bEmit )
		return 0;
	
	// Make sure our materials are cached
	if ( !InitMaterials() )
	{
		//NOTENOTE: This means that a material was not found for the combine ball, so it may not render!
		AssertOnce( 0 );
		return 0;
	}

	// Draw the flickering overlay
	DrawFlicker();
	
	// Draw the motion blur from movement
	if ( m_bHeld || m_bLaunched )
	{
		DrawMotionBlur();
	}

	// Draw the model if we're being held
	if ( m_bHeld )
	{
		QAngle	angles;
		VectorAngles( -CurrentViewForward(), angles );

		// Always orient towards the camera!
		SetAbsAngles( angles );

		BaseClass::DrawModel( flags );
	}
	
	else
	{
		float color[3];
		color[0] = color[1] = color[2] = 1.0f;

		float sinOffs = 1.0f * sin( gpGlobals->curtime * 25 );

		float roll = SpawnTime();

		// Draw the main ball body
		CMatRenderContextPtr pRenderContext( materials );
		pRenderContext->Bind( m_pBodyMaterial, (C_BaseEntity*) this );
		DrawRingOriented( GetAbsOrigin(), m_flRadius + sinOffs, color, roll );
	}
	m_vecLastOrigin = GetAbsOrigin();

	return 1;
}
#else
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flags - 
// Output : int
//-----------------------------------------------------------------------------
int C_PropParticleBall::DrawModel( int flags, const RenderableInstance_t &instance )
{
	if ( !m_bEmit )
		return 0;
	
	// Make sure our materials are cached
	if ( !InitMaterials() )
	{
		//NOTENOTE: This means that a material was not found for the combine ball, so it may not render!
		AssertOnce( 0 );
		return 0;
	}

	// Draw the flickering overlay
	DrawFlicker();
	
	// Draw the motion blur from movement
	if ( m_bHeld || m_bLaunched )
	{
		DrawMotionBlur();
	}

	// Draw the model if we're being held
	if ( m_bHeld )
	{
		QAngle	angles;
		VectorAngles( -CurrentViewForward(), angles );

		// Always orient towards the camera!
		SetAbsAngles( angles );

		BaseClass::DrawModel( flags, instance );
	}
	
	else
	{
		float color[3];
		color[0] = color[1] = color[2] = 1.0f;

		float sinOffs = 1.0f * sin( gpGlobals->curtime * 25 );

		float roll = SpawnTime();

		// Draw the main ball body
		CMatRenderContextPtr pRenderContext( materials );
		pRenderContext->Bind( m_pBodyMaterial, (C_BaseEntity*) this );
		DrawRingOriented( GetAbsOrigin(), m_flRadius + sinOffs, color, roll );
	}
	m_vecLastOrigin = GetAbsOrigin();

	return 1;
}
#endif
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &data - 
//-----------------------------------------------------------------------------
void ParticleBall0ImpactCallback( const CEffectData &data )
{
	float r = BALL0_COLOR_R * FLOAT_TO_VECTOR_MULTIPLER;
	float g = BALL0_COLOR_G * FLOAT_TO_VECTOR_MULTIPLER;
	float b = BALL0_COLOR_B * FLOAT_TO_VECTOR_MULTIPLER;

	// Quick flash
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal,
				data.m_flRadius * 10.0f,
				0,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.25f, 
				BALL_SPRITE_FLASH,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Lingering burn
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal, 
				data.m_flRadius * 2.0f,
				data.m_flRadius * 4.0f,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.5f, 
				BALL_SPRITE_IMPACT,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Throw sparks
	FX_ElectricSpark( data.m_vOrigin, 2, 1, &data.m_vNormal );
	}

#ifndef SWARM_DLL
DECLARE_CLIENT_EFFECT( "cball_blue_bounce", ParticleBall0ImpactCallback );
#else
DECLARE_CLIENT_EFFECT( cball_blue_bounce, ParticleBall0ImpactCallback );
#endif
void ParticleBall1ImpactCallback( const CEffectData &data )
{
	float r = BALL1_COLOR_R * FLOAT_TO_VECTOR_MULTIPLER;
	float g = BALL1_COLOR_G * FLOAT_TO_VECTOR_MULTIPLER;
	float b = BALL1_COLOR_B * FLOAT_TO_VECTOR_MULTIPLER;

	// Quick flash
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal,
				data.m_flRadius * 10.0f,
				0,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.25f, 
				BALL_SPRITE_FLASH,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Lingering burn
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal, 
				data.m_flRadius * 2.0f,
				data.m_flRadius * 4.0f,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.5f, 
				BALL_SPRITE_IMPACT,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Throw sparks
	FX_ElectricSpark( data.m_vOrigin, 2, 1, &data.m_vNormal );
	}

#ifndef SWARM_DLL
DECLARE_CLIENT_EFFECT( "cball_green_bounce", ParticleBall1ImpactCallback );
#else
DECLARE_CLIENT_EFFECT( cball_green_bounce, ParticleBall1ImpactCallback );
#endif

void ParticleBall2ImpactCallback( const CEffectData &data )
{
	float r = BALL2_COLOR_R * FLOAT_TO_VECTOR_MULTIPLER;
	float g = BALL2_COLOR_G * FLOAT_TO_VECTOR_MULTIPLER;
	float b = BALL2_COLOR_B * FLOAT_TO_VECTOR_MULTIPLER;

	// Quick flash
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal,
				data.m_flRadius * 10.0f,
				0,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.25f, 
				BALL_SPRITE_FLASH,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Lingering burn
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal, 
				data.m_flRadius * 2.0f,
				data.m_flRadius * 4.0f,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				//Vector( 1.0f, 1.0f, 1.0f ), 
				Vector( r, g, b ), 
				0.5f, 
				BALL_SPRITE_IMPACT,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Throw sparks
	FX_ElectricSpark( data.m_vOrigin, 2, 1, &data.m_vNormal );
	}

#ifndef SWARM_DLL
DECLARE_CLIENT_EFFECT( "cball_purple_bounce", ParticleBall2ImpactCallback );
#else
DECLARE_CLIENT_EFFECT( cball_purple_bounce, ParticleBall2ImpactCallback  );
#endif

void ParticleBall3ImpactCallback( const CEffectData &data )
{
	float r = vectronic_vecball3_color_r.GetFloat() * FLOAT_TO_VECTOR_MULTIPLER; 
	float g = vectronic_vecball3_color_g.GetFloat() * FLOAT_TO_VECTOR_MULTIPLER; 
	float b = vectronic_vecball3_color_b.GetFloat() * FLOAT_TO_VECTOR_MULTIPLER; 

	// Quick flash
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal,
				data.m_flRadius * 10.0f,
				0,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.25f, 
				BALL_SPRITE_FLASH,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Lingering burn
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal, 
				data.m_flRadius * 2.0f,
				data.m_flRadius * 4.0f,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.5f, 
				BALL_SPRITE_IMPACT,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Throw sparks
	FX_ElectricSpark( data.m_vOrigin, 2, 1, &data.m_vNormal );
	}

#ifndef SWARM_DLL
DECLARE_CLIENT_EFFECT( "cball_3_bounce", ParticleBall3ImpactCallback );
#else
DECLARE_CLIENT_EFFECT( cball_3_bounce, ParticleBall3ImpactCallback  );
#endif

void ParticleBall4ImpactCallback( const CEffectData &data )
{
	float r = vectronic_vecball4_color_r.GetFloat() * FLOAT_TO_VECTOR_MULTIPLER; 
	float g = vectronic_vecball4_color_g.GetFloat() * FLOAT_TO_VECTOR_MULTIPLER; 
	float b = vectronic_vecball4_color_b.GetFloat() * FLOAT_TO_VECTOR_MULTIPLER; 

	// Quick flash
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal,
				data.m_flRadius * 10.0f,
				0,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ),  
				0.25f, 
				BALL_SPRITE_FLASH,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Lingering burn
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal, 
				data.m_flRadius * 2.0f,
				data.m_flRadius * 4.0f,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.5f, 
				BALL_SPRITE_IMPACT,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Throw sparks
	FX_ElectricSpark( data.m_vOrigin, 2, 1, &data.m_vNormal );
	}

#ifndef SWARM_DLL
DECLARE_CLIENT_EFFECT( "cball_4_bounce", ParticleBall4ImpactCallback );
#else
DECLARE_CLIENT_EFFECT( cball_4_bounce, ParticleBall4ImpactCallback  );
#endif

void ParticleBall5ImpactCallback( const CEffectData &data )
{
	float r = vectronic_vecball5_color_r.GetFloat() * FLOAT_TO_VECTOR_MULTIPLER; 
	float g = vectronic_vecball5_color_g.GetFloat() * FLOAT_TO_VECTOR_MULTIPLER; 
	float b = vectronic_vecball5_color_b.GetFloat() * FLOAT_TO_VECTOR_MULTIPLER; 

	// Quick flash
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal,
				data.m_flRadius * 10.0f,
				0,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.25f, 
				BALL_SPRITE_FLASH,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Lingering burn
	FX_AddQuad( data.m_vOrigin,
				data.m_vNormal, 
				data.m_flRadius * 2.0f,
				data.m_flRadius * 4.0f,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( r, g, b ), 
				0.5f, 
				BALL_SPRITE_IMPACT,
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );

	// Throw sparks
	FX_ElectricSpark( data.m_vOrigin, 2, 1, &data.m_vNormal );
	}

#ifndef SWARM_DLL
DECLARE_CLIENT_EFFECT( "cball_5_bounce", ParticleBall5ImpactCallback );
#else
DECLARE_CLIENT_EFFECT( cball_5_bounce, ParticleBall5ImpactCallback  );
#endif

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &data - 
//-----------------------------------------------------------------------------
void ParticleBallExplosionCallback( const CEffectData &data )
{
	Vector normal(0,0,1);

	// Throw sparks
	FX_ElectricSpark( data.m_vOrigin, 4, 1, &normal );
}
#ifndef SWARM_DLL
DECLARE_CLIENT_EFFECT( "cball_explode", ParticleBallExplosionCallback );
#else
DECLARE_CLIENT_EFFECT( cball_explode, ParticleBallExplosionCallback  );
#endif