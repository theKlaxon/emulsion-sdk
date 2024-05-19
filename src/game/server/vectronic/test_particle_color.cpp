//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "prop_vectronic_projectile.h"
#include "baseanimating.h"
#include "sprite.h"
#include "particle_parse.h"
#include "particle_system.h"

class CTestParticle01 : public CPointEntity
{
public:
	DECLARE_CLASS( CTestParticle01, CPointEntity );
	DECLARE_DATADESC();
 
	CTestParticle01()
	{
	}

	void Precache( void );
	void Spawn( void );
	void TurnOn( void );
	void ChangeColor();

private:

	bool m_bDispatched;
	void InputSetColor(inputdata_t &inputdata);
	void InputChangeColor ( inputdata_t &inputData );

	CHandle<CParticleSystem> m_hParticleEffect;
	color32	m_Color;
};



LINK_ENTITY_TO_CLASS( test_particle_color, CTestParticle01 );

BEGIN_DATADESC( CTestParticle01 )
	DEFINE_INPUTFUNC( FIELD_COLOR32, "SetColor",  InputSetColor ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ChangeColor", InputChangeColor ),
	DEFINE_FIELD( m_hParticleEffect, FIELD_EHANDLE ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CTestParticle01::Precache( void )
{
	PrecacheParticleSystem( "vecbox_light" );
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CTestParticle01::Spawn( void )
{
	Precache();

	if (!m_bDispatched)
	{
		TurnOn();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CTestParticle01::TurnOn( void )
{
	m_hParticleEffect = (CParticleSystem *) CreateEntityByName( "info_particle_system" );
	if ( m_hParticleEffect != NULL )
	{
		QAngle angles = GetAbsAngles();
		// Setup our basic parameters
		m_hParticleEffect->KeyValue( "start_active", "1" );
		//m_hParticleEffect->KeyValue( "effect_name", "test_color" );
		m_hParticleEffect->KeyValue( "effect_name", "projectile_ball_shared" );
		m_hParticleEffect->SetParent( this );
		m_hParticleEffect->SetAbsOrigin( WorldSpaceCenter());
		m_hParticleEffect->SetAbsAngles( angles );
		DispatchSpawn( m_hParticleEffect );
		m_hParticleEffect->Activate();

		Vector ParticleColor (255, 255, 255);
		m_hParticleEffect->SetControlPointValue( 7, ParticleColor );

		m_bDispatched = true;
		m_hParticleEffect->StartParticleSystem();
	}
}

void CTestParticle01::ChangeColor( void )
{
	if (m_hParticleEffect == NULL)
		return;

	Vector ParticleColor (176, 169, 109);

	m_hParticleEffect->SetControlPointValue( 7, ParticleColor );

	DevMsg("CTestParticle01: Changed Color!\n");
}

void CTestParticle01::InputChangeColor( inputdata_t &inputData )
{
	ChangeColor();
	DevMsg("CTestParticle01: Changing Color!\n");
}

void CTestParticle01::InputSetColor(inputdata_t &inputdata)
{
	m_Color = inputdata.value.Color32();
	Vector ParticleColor (m_Color.r, m_Color.g, m_Color.b);

	m_hParticleEffect->SetControlPointValue( 7, ParticleColor );
}