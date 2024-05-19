//=========== Copyright © 2012 - 2014, rHetorical, All rights reserved. =============
//
// Purpose: A brush that sends outputs if it is taking damage, or not.
//		
//===================================================================================

#include "cbase.h"
#include "prop_vecbox.h"
#include "EnvLaser.h"
#include "Sprite.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CFuncLReceiver : public CPointEntity
{
public:
	DECLARE_CLASS( CFuncLReceiver, CPointEntity );
	DECLARE_DATADESC();
 
	void Spawn();
	bool CreateVPhysics( void );
	void Think( void );
	void TurnOff( void );
	
private:

 	void InputEnable( inputdata_t &data );
	void InputDisable( inputdata_t &data );

	virtual int OnTakeDamage( const CTakeDamageInfo &info );

	COutputEvent	m_OnDamaged;
	COutputEvent	m_OnOffDamaged;
	bool m_bDisabled;

	bool m_TakingDamage;
	bool m_FiredOutput;

};

LINK_ENTITY_TO_CLASS( func_laser_receiver, CFuncLReceiver );
 
BEGIN_DATADESC( CFuncLReceiver )

	DEFINE_THINKFUNC( TurnOff ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	DEFINE_KEYFIELD(m_bDisabled, FIELD_BOOLEAN, "StartDisabled"),
	
	DEFINE_INPUTFUNC(FIELD_VOID, "Enable", InputEnable),
	DEFINE_INPUTFUNC(FIELD_VOID, "Disable", InputDisable),

	// Outputs
	DEFINE_OUTPUT( m_OnDamaged, "OnLaserHit" ),
	DEFINE_OUTPUT( m_OnOffDamaged, "OnLaserLost" ),
 
END_DATADESC()

bool CFuncLReceiver::CreateVPhysics( void )
{
	VPhysicsInitStatic();
	return true;
}

void CFuncLReceiver::Spawn()
{
	BaseClass::Spawn();
 
	// We should collide with physics
	SetSolid( SOLID_BSP );

	// Use our brushmodel
	SetModel( STRING( GetModelName() ) );
 
	// We push things out of our way
	SetMoveType( MOVETYPE_PUSH );
	//AddFlag( FL_WORLDBRUSH );

	m_takedamage = DAMAGE_EVENTS_ONLY;

	RegisterThinkContext( "TurnOff" );
	SetContextThink( &CFuncLReceiver::TurnOff, gpGlobals->curtime, "TurnOff" );

	//SetNextThink(gpGlobals->curtime + .1);

	CreateVPhysics();
}

void CFuncLReceiver::Think( void )
{
	// We are from OnTakeDamage. Tell the entity we are no longer taking damage.
	// If we still are, the bool will reset itself to true. so when we get to TurnOff, nothing will happen!

	//DevMsg("CFuncLReceiver: We are at Think! \n");
	m_TakingDamage = false;
	SetNextThink(gpGlobals->curtime + .1, "TurnOff" );
}

void CFuncLReceiver::TurnOff()
{
	//DevMsg("CFuncLReceiver: We are at turnoff! \n");

	// If the laser is not hitting the brush. We will continue what we are doing.
	if (!m_TakingDamage && m_FiredOutput)
	{
		m_OnOffDamaged.FireOutput( this, this );
		m_FiredOutput = false;
	}
}

void CFuncLReceiver::InputEnable( inputdata_t &inputdata )
{
	m_bDisabled = false;
	m_takedamage = DAMAGE_EVENTS_ONLY;
}

void CFuncLReceiver::InputDisable( inputdata_t &inputdata )
{
	m_bDisabled = true;
	m_takedamage = DAMAGE_NO;
}

int CFuncLReceiver::OnTakeDamage( const CTakeDamageInfo &info )
{
	int ret = BaseClass::OnTakeDamage( info );
	CBaseEntity *pInflictor = info.GetInflictor();

	// We are the laser, so hit it!
	//if ( pInflictor->ClassMatches( "env_laser" ) || pInflictor->ClassMatches( "env_vectronic_laser" ) ) 
	if ( pInflictor->ClassMatches( "env_vectronic_laser" ) ) 
	{	
		// We are taking damage.
		m_TakingDamage = true;

		// Fire this output once.
		if( !m_FiredOutput )
		{
			m_OnDamaged.FireOutput( this, this );
			m_FiredOutput = true;
		}

		// Think
		SetNextThink(gpGlobals->curtime + 0.08);

		// Return
		return ret;
	}

	return 0;
}

// Why oh why am I here? 
class CVectronicLaser : public CEnvLaser
{
	DECLARE_CLASS( CVectronicLaser, CEnvLaser );
public:
	void	Spawn( void );
	void	Precache( void );
	bool	KeyValue( const char *szKeyName, const char *szValue );

	void	TurnOn( void );
	void	TurnOff( void );
	int		IsOn( void );

	void	FireAtPoint( trace_t &point );
	void	StrikeThink( void );

	void InputTurnOn( inputdata_t &inputdata );
	void InputTurnOff( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );

	DECLARE_DATADESC();

	string_t m_iszLaserTarget;	// Name of entity or entities to strike at, randomly picked if more than one match.
	CSprite	*m_pSprite;
	string_t m_iszSpriteName;
	Vector  m_firePosition;

	float	m_flStartFrame;
};

LINK_ENTITY_TO_CLASS( env_vectronic_laser, CVectronicLaser );

BEGIN_DATADESC( CVectronicLaser )
	DEFINE_KEYFIELD( m_iszLaserTarget, FIELD_STRING, "LaserTarget" ),
	DEFINE_FIELD( m_pSprite, FIELD_CLASSPTR ),
	DEFINE_KEYFIELD( m_iszSpriteName, FIELD_STRING, "EndSprite" ),
	DEFINE_FIELD( m_firePosition, FIELD_VECTOR ),
	DEFINE_KEYFIELD( m_flStartFrame, FIELD_FLOAT, "framestart" ),

	// Function Pointers
	DEFINE_FUNCTION( StrikeThink ),

	// Input functions
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOn", InputTurnOn ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOff", InputTurnOff ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),
END_DATADESC()


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicLaser::Spawn( void )
{
	if ( !GetModelName() )
	{
		SetThink( &CEnvLaser::SUB_Remove );
		return;
	}

	SetSolid( SOLID_NONE );							// Remove model & collisions
	SetThink( &CVectronicLaser::StrikeThink );

	SetEndWidth( GetWidth() );				// Note: EndWidth is not scaled

	PointsInit( GetLocalOrigin(), GetLocalOrigin() );

	Precache( );

	if ( !m_pSprite && m_iszSpriteName != NULL_STRING )
	{
		m_pSprite = CSprite::SpriteCreate( STRING(m_iszSpriteName), GetAbsOrigin(), TRUE );
	}
	else
	{
		m_pSprite = NULL;
	}

	if ( m_pSprite )
	{
		m_pSprite->SetParent( GetMoveParent() );
		m_pSprite->SetTransparency( kRenderGlow, m_clrRender->r, m_clrRender->g, m_clrRender->b, m_clrRender->a, m_nRenderFX );
	}

	if ( GetEntityName() != NULL_STRING && !(m_spawnflags & SF_BEAM_STARTON) )
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicLaser::Precache( void )
{
	SetModelIndex( PrecacheModel( STRING( GetModelName() ) ) );
	if ( m_iszSpriteName != NULL_STRING )
		PrecacheModel( STRING(m_iszSpriteName) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CVectronicLaser::KeyValue( const char *szKeyName, const char *szValue )
{
	if (FStrEq(szKeyName, "width"))
	{
		SetWidth( atof(szValue) );
	}
	else if (FStrEq(szKeyName, "NoiseAmplitude"))
	{
		SetNoise( atoi(szValue) );
	}
	else if (FStrEq(szKeyName, "TextureScroll"))
	{
		SetScrollRate( atoi(szValue) );
	}
	else if (FStrEq(szKeyName, "texture"))
	{
		SetModelName( AllocPooledString(szValue) );
	}
	else
	{
		BaseClass::KeyValue( szKeyName, szValue );
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Returns whether the laser is currently active.
//-----------------------------------------------------------------------------
int CVectronicLaser::IsOn( void )
{
	if ( IsEffectActive( EF_NODRAW ) )
		return 0;
	return 1;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicLaser::InputTurnOn( inputdata_t &inputdata )
{
	if (!IsOn())
	{
		TurnOn();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicLaser::InputTurnOff( inputdata_t &inputdata )
{
	if (IsOn())
	{
		TurnOff();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicLaser::InputToggle( inputdata_t &inputdata )
{
	if ( IsOn() )
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicLaser::TurnOff( void )
{
	AddEffects( EF_NODRAW );
	if ( m_pSprite )
		m_pSprite->TurnOff();

	SetNextThink( TICK_NEVER_THINK );
	SetThink( NULL );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicLaser::TurnOn( void )
{
	RemoveEffects( EF_NODRAW );
	if ( m_pSprite )
		m_pSprite->TurnOn();

	m_flFireTime = gpGlobals->curtime;

	SetThink( &CVectronicLaser::StrikeThink );

	//
	// Call StrikeThink here to update the end position, otherwise we will see
	// the beam in the wrong place for one frame since we cleared the nodraw flag.
	//
	StrikeThink();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicLaser::FireAtPoint( trace_t &tr )
{
	SetAbsEndPos( tr.endpos );
	if ( m_pSprite )
	{
		UTIL_SetOrigin( m_pSprite, tr.endpos );
	}

	// Apply damage and do sparks every 1/10th of a second.
	if ( gpGlobals->curtime >= m_flFireTime + 0.1 )
	{
		BeamDamage( &tr );
		DoSparks( GetAbsStartPos(), tr.endpos );
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVectronicLaser::StrikeThink( void )
{
	trace_t tr;

	// If the target is missing, use angles.
	if( STRING( m_iszLaserTarget ) == NULL || !m_iszLaserTarget )
	{
		Vector forward, up;
		AngleVectors( GetAbsAngles(), &forward, NULL, &up );
		Vector searchEnd = GetAbsOrigin() + forward * 8192;

		UTIL_TraceLine( GetAbsOrigin(), searchEnd, MASK_OPAQUE_AND_NPCS, NULL, COLLISION_GROUP_PLAYER /*|| COLLISION_GROUP_PROJECTILE*/, &tr );

		//CBaseEntity *pObject = tr.m_pEnt;
		//if (pObject->GetFlags() & FL_DISSOLVING)
			//return;

		FireAtPoint( tr );
		SetThink( &CVectronicLaser::StrikeThink );
		SetNextThink( gpGlobals->curtime );
	}
	else
	{
		CBaseEntity *pEnd = RandomTargetname( STRING( m_iszLaserTarget ) );

		Vector vecFireAt = GetAbsEndPos();
		if ( pEnd )
		{
			vecFireAt = pEnd->GetAbsOrigin();
		}

		UTIL_TraceLine( GetAbsOrigin(), vecFireAt, MASK_OPAQUE_AND_NPCS, NULL, COLLISION_GROUP_PLAYER /*|| COLLISION_GROUP_PROJECTILE*/, &tr );

		FireAtPoint( tr );
		SetThink( &CVectronicLaser::StrikeThink );
		SetNextThink( gpGlobals->curtime );

	}	
}
