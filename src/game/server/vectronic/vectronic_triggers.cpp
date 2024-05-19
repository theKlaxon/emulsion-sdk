//=========== Copyright © 2013, rHetorical, All rights reserved. =============
//
// Purpose:
//		
//=============================================================================

#include "cbase.h"
#include "vectronic_player.h"
#include "prop_vectronic_projectile.h"
#include "vectronic_vecall_system.h"
#include "prop_vecbox.h"
#include <convar.h>
#include "saverestore_utlvector.h"
#include "triggers.h"
#include "physics.h"
#include "vphysics_interface.h"
#include "weapon_vecgun.h"

extern ConVar	sv_gravity;

//-----------------------------------------------------------------------------
// noball volume
//-----------------------------------------------------------------------------
class CFuncNoBallVol : public CTriggerMultiple
{
	DECLARE_CLASS( CFuncNoBallVol, CTriggerMultiple );
	DECLARE_DATADESC();

public:
	void Precache( void );
	void Spawn( void );
	void Touch( CBaseEntity *pOther );


private:
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );
};


//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( func_noball_volume, CFuncNoBallVol );

BEGIN_DATADESC( CFuncNoBallVol )

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncNoBallVol::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncNoBallVol::Spawn( void )
{
	BaseClass::Spawn();
}

//------------------------------------------------------------------------------
// Inputs
//------------------------------------------------------------------------------
void CFuncNoBallVol::InputToggle( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		InputEnable( inputdata );
	}
	else
	{
		InputDisable( inputdata );
	}
}

void CFuncNoBallVol::InputEnable( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		Enable();
	}
}

void CFuncNoBallVol::InputDisable( inputdata_t &inputdata )
{
	if ( !m_bDisabled )
	{
		Disable();
	}
}

//-----------------------------------------------------------------------------
// Traps the entities
//-----------------------------------------------------------------------------
void CFuncNoBallVol::Touch( CBaseEntity *pEntity )
{
	CBaseAnimating *pAnim = pEntity->GetBaseAnimating();
	if ( !pAnim )
		return;

	CPropParticleBall *pBall = dynamic_cast<CPropParticleBall*>( pEntity );

	if (pBall)
	{
		//if (pBall->GetType() == 0)
		pBall->DoExplosion();
	}
}


//-----------------------------------------------------------------------------
// Cleanser
//-----------------------------------------------------------------------------
class CTriggerCleanser : public CTriggerMultiple
{
	DECLARE_CLASS( CTriggerCleanser, CTriggerMultiple );
	DECLARE_DATADESC();

public:

	//Constructor
	CTriggerCleanser()
	{
		m_nFilterType = CLEAR_ALL;
	}

	void Precache( void );
	void Spawn( void );
	void Touch( CBaseEntity *pOther );

private:

	enum FilterType_t
	{
		CLEAR_ALL				= -1,	// Remove all balls.
		CLEAR_BLUE_ONLY			= 0,	// Only Remove the Blue Ball
		CLEAR_GREEN_ONLY		= 1,	// Only Remove the Green Ball
		CLEAR_YELLOW_ONLY       = 2,
		CLEAR_3_ONLY			= 3,
		CLEAR_4_ONLY			= 4,
		CLEAR_5_ONLY			= 5,

	};

	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );

	int m_nFilterType;
	bool m_bDissolveBox;

	COutputEvent m_OnBoxTouch;
	COutputEvent m_OnPlayerTouch;
	COutputEvent m_OnPlayerBallRemoved;
};


//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_vecball_cleanser, CTriggerCleanser );

BEGIN_DATADESC( CTriggerCleanser )

	DEFINE_KEYFIELD( m_nFilterType,	FIELD_INTEGER,	"filtertype" ),
	DEFINE_KEYFIELD( m_bDissolveBox, FIELD_BOOLEAN,	"dissolvebox" ),

	// I/O
	DEFINE_OUTPUT( m_OnBoxTouch, "OnBoxTouch" ),
	DEFINE_OUTPUT( m_OnPlayerTouch, "OnPlayerTouch" ),
	DEFINE_OUTPUT( m_OnPlayerBallRemoved, "OnPlayerBallRemoved" ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerCleanser::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerCleanser::Spawn( void )
{
	BaseClass::Spawn();
}

//------------------------------------------------------------------------------
// Inputs
//------------------------------------------------------------------------------
void CTriggerCleanser::InputToggle( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		InputEnable( inputdata );
	}
	else
	{
		InputDisable( inputdata );
	}
}

void CTriggerCleanser::InputEnable( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		Enable();
	}
}

void CTriggerCleanser::InputDisable( inputdata_t &inputdata )
{
	if ( !m_bDisabled )
	{
		Disable();
	}
}

//-----------------------------------------------------------------------------
// Traps the entities
//-----------------------------------------------------------------------------
void CTriggerCleanser::Touch( CBaseEntity *pOther )
{
	if ( !PassesTriggerFilters(pOther) )
		return;

	CBaseAnimating *pAnim = pOther->GetBaseAnimating();
	if ( !pAnim )
		return;

	// Player walks though it!
	if ( pOther->IsPlayer())
	{
		CVectronicPlayer *pPlayer = dynamic_cast<CVectronicPlayer*>( pOther );

		if ( pPlayer )
		{
			CWeaponVecgun *pVecgun = dynamic_cast<CWeaponVecgun*>( pPlayer->Weapon_OwnsThisType( "weapon_vecgun" ) );

			if ( pVecgun )
			{
				if (m_nFilterType == CLEAR_ALL )
				{
					pVecgun->ClearGun();
					m_OnPlayerBallRemoved.FireOutput( this, this );
				}

				// If the gun is set to fire custom balls, but we come across our balls, ingnore touching.
				if (pVecgun->IsUsingCustomBalls() && m_nFilterType <= CLEAR_YELLOW_ONLY )
					return;

				// If the gun is set to fire our balls, but we come across custom balls, ingnore touching.
				if (pVecgun->IsUsingCustomBalls() == false && m_nFilterType >= CLEAR_3_ONLY )
					return;

				if (m_nFilterType == CLEAR_BLUE_ONLY && pVecgun->CanFireBlue())
				{
					pVecgun->ClearBlue();
					m_OnPlayerBallRemoved.FireOutput( this, this );
				}

				if (m_nFilterType == CLEAR_GREEN_ONLY && pVecgun->CanFireGreen())
				{
					pVecgun->ClearGreen();
					m_OnPlayerBallRemoved.FireOutput( this, this );
				}
				if (m_nFilterType == CLEAR_YELLOW_ONLY && pVecgun->CanFireYellow())
				{
					pVecgun->ClearYellow();
					m_OnPlayerBallRemoved.FireOutput( this, this );
				}

				if (m_nFilterType == CLEAR_3_ONLY && pVecgun->CanFireBlue())
				{
					pVecgun->ClearBlue();
					m_OnPlayerBallRemoved.FireOutput( this, this );
				}

				if (m_nFilterType == CLEAR_4_ONLY && pVecgun->CanFireGreen())
				{
					pVecgun->ClearGreen();
					m_OnPlayerBallRemoved.FireOutput( this, this );
				}
				if (m_nFilterType == CLEAR_5_ONLY && pVecgun->CanFireYellow())
				{
					pVecgun->ClearYellow();
					m_OnPlayerBallRemoved.FireOutput( this, this );
				}
			}
		}

		m_OnPlayerTouch.FireOutput( this, this );
	}
	else 
	{
		// Ok, now a box comes in.
		if ( pOther->ClassMatches("prop_vecbox") )
		{
			CVecBox *pVecBox = dynamic_cast<CVecBox*>( pOther );

			if ( m_bDissolveBox )
			{
				pVecBox->OnDissolve();
			}
			else
			{
				// If we are a blue filter and the box is not blue, return.
				if (m_nFilterType == CLEAR_BLUE_ONLY && pVecBox->GetState() != 0 )
					return;

				// If we are a green filter and the box is not green, return.
				if (m_nFilterType == CLEAR_GREEN_ONLY && pVecBox->GetState() != 1 )
					return;

				// If we are a yellow filter and the box is not yellow, return.
				if (m_nFilterType == CLEAR_YELLOW_ONLY && pVecBox->GetState() != 2 )
					return;

				// If we are a 3 filter and the box is not 3, return.
				if (m_nFilterType == CLEAR_3_ONLY && pVecBox->GetState() != 3 )
					return;

				// If we are a 4 filter and the box is not 4, return.
				if (m_nFilterType == CLEAR_4_ONLY && pVecBox->GetState() != 4 )
					return;

				// If we are a 5 filter and the box is not 5, return.
				if (m_nFilterType == CLEAR_5_ONLY && pVecBox->GetState() != 5 )
					return;

				pVecBox->Clear();
				m_OnBoxTouch.FireOutput( this, this );
			}
		}

		CPropParticleBall *pBall = dynamic_cast<CPropParticleBall*>( pOther );
		if (pBall)
		{
			if (m_nFilterType == CLEAR_BLUE_ONLY && pBall->GetType() != 0)
				return;

			if (m_nFilterType == CLEAR_GREEN_ONLY && pBall->GetType() != 1)
				return;

			if (m_nFilterType == CLEAR_YELLOW_ONLY && pBall->GetType() != 2)
				return;

			if (m_nFilterType == CLEAR_3_ONLY && pBall->GetType() != 3)
				return;

			if (m_nFilterType == CLEAR_4_ONLY && pBall->GetType() != 4)
				return;

			if (m_nFilterType == CLEAR_5_ONLY && pBall->GetType() != 5)
				return;

			pBall->DoExplosion();
		}
	}
}

//-----------------------------------------------------------------------------
// Equip Punt
//-----------------------------------------------------------------------------
class CTriggerBallEquip : public CTriggerMultiple
{
	DECLARE_CLASS( CTriggerBallEquip, CTriggerMultiple );
	DECLARE_DATADESC();

public:

	//Constructor
	CTriggerBallEquip()
	{

	}

	enum Types_t
	{
		BALL0 = 0,
		BALL1 = 1,
		BALL2 = 2,
		BALL3 = 3,
		BALL4 = 4,
		BALL5 = 5,
	};

	void Precache( void );
	void Spawn( void );
	void Touch( CBaseEntity *pOther );
	void EndTouch( CBaseEntity *pOther );

private:
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );

	float m_flBalltype;

};

//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_vecball_equip, CTriggerBallEquip );

BEGIN_DATADESC( CTriggerBallEquip )

	DEFINE_KEYFIELD( m_flBalltype, FIELD_FLOAT, "BallType" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerBallEquip::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerBallEquip::Spawn( void )
{
	SetNextThink( gpGlobals->curtime );

	BaseClass::Spawn();
}

//------------------------------------------------------------------------------
// Inputs
//------------------------------------------------------------------------------
void CTriggerBallEquip::InputToggle( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		InputEnable( inputdata );
	}
	else
	{
		InputDisable( inputdata );
	}
}

void CTriggerBallEquip::InputEnable( inputdata_t &inputdata )
{
	if ( m_bDisabled )
	{
		Enable();
	}
}

void CTriggerBallEquip::InputDisable( inputdata_t &inputdata )
{
	if ( !m_bDisabled )
	{
		Disable();
	}
}

//-----------------------------------------------------------------------------
// Traps the entities
//-----------------------------------------------------------------------------
void CTriggerBallEquip::Touch( CBaseEntity *pOther )
{
	if ( pOther->IsPlayer())
	{
		CVectronicPlayer *pPlayer = dynamic_cast<CVectronicPlayer*>( pOther );

		if ( pPlayer )
		{
			CWeaponVecgun *pVecgun = dynamic_cast<CWeaponVecgun*>( pPlayer->Weapon_OwnsThisType( "weapon_vecgun" ) );

			if ( pVecgun )
			{
				// If the gun is set to fire custom balls, but we come across our balls, ingnore touching.
				if (pVecgun->IsUsingCustomBalls() && m_flBalltype <= BALL2 )
					return;

				// If the gun is set to fire our balls, but we come across custom balls, ingnore touching.
				if (pVecgun->IsUsingCustomBalls() == false && m_flBalltype >= BALL3 )
					return;

				if ( m_flBalltype == BALL0 || m_flBalltype == BALL3 )
				{
					pVecgun->SetCanFireBlue();
				}

				if ( m_flBalltype == BALL1 || m_flBalltype == BALL4 )
				{
					pVecgun->SetCanFireGreen();
				}

				if ( m_flBalltype == BALL2 || m_flBalltype == BALL5 )
				{
					pVecgun->SetCanFireYellow();
				}

				if (m_flBalltype >= 6 )
				{
					Msg ("ERROR: Attempted to give player unauthorized ball. Ignoring!\n");
				}

				// Disable when we touch.
				Disable();
			}
			else
			{
				// We don't want to anything if the player touches the trigger without the gun.
				return;
			}
		}
	}
}

void CTriggerBallEquip::EndTouch( CBaseEntity *pOther )
{
	CBaseAnimating *pAnim = pOther->GetBaseAnimating();
	if ( !pAnim )
		return;
}

//-----------------------------------------------------------------------------
// A trigger that only fires outputs for vecboxes!
//-----------------------------------------------------------------------------
class CTriggerVecBox : public CTriggerMultiple
{
	DECLARE_CLASS(CTriggerVecBox, CTriggerMultiple);
	DECLARE_DATADESC();

public:

	//Constructor
	CTriggerVecBox()
	{
		m_bHadPlayer = false;
	}

	void Precache(void);
	void Spawn(void);
	void Touch(CBaseEntity *pOther);
	void EndTouch(CBaseEntity *pOther);

	bool TestFilter(CVecBox *pVecBox, int filtertype );

private:

	enum FilterType_t
	{
		BALL0 = 0,
		BALL1 = 1,
		BALL2 = 2,
		BALL3 = 3,
		BALL4 = 4,
		BALL5 = 5,

		BALL_ANY = -1,

		BOX_GHOST = GHOSTID,
		BOX_ANY = 10,
	};

	void InputEnable(inputdata_t &inputdata);
	void InputDisable(inputdata_t &inputdata);
	void InputToggle(inputdata_t &inputdata);

	//int m_nFilterType;
	bool m_bIgnoreProtection;

	bool m_bNegated;
	bool IsNegetedMode()
	{
		return m_bNegated;
	};

	int m_intFilterType;
	bool m_bHadPlayer;

	COutputEvent m_OnPlayerTouch;
	COutputEvent m_OnEndPlayerTouch;

	COutputEvent m_OnBoxTouch;
	COutputEvent m_OnEndBoxTouch;

};


//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(trigger_vecbox, CTriggerVecBox);

BEGIN_DATADESC(CTriggerVecBox)

	//DEFINE_KEYFIELD(m_nFilterType, FIELD_INTEGER, "filtertype"),
	DEFINE_FIELD(m_bHadPlayer, FIELD_BOOLEAN),
	DEFINE_KEYFIELD(m_bIgnoreProtection, FIELD_BOOLEAN, "ignoreprotection"),
	DEFINE_KEYFIELD(m_bNegated, FIELD_BOOLEAN, "negated"),
	DEFINE_KEYFIELD(m_intFilterType, FIELD_INTEGER, "filtertype"),

	DEFINE_OUTPUT(m_OnBoxTouch, "OnBoxTouch"),
	DEFINE_OUTPUT(m_OnEndBoxTouch, "OnEndBoxTouch"),

	DEFINE_OUTPUT(m_OnPlayerTouch, "OnPlayerTouch"),
	DEFINE_OUTPUT(m_OnEndPlayerTouch, "OnEndPlayerTouch"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerVecBox::Precache(void)
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerVecBox::Spawn(void)
{
	BaseClass::Spawn();
}

//------------------------------------------------------------------------------
// Inputs
//------------------------------------------------------------------------------
void CTriggerVecBox::InputToggle(inputdata_t &inputdata)
{
	if (m_bDisabled)
	{
		InputEnable(inputdata);
	}
	else
	{
		InputDisable(inputdata);
	}
}

void CTriggerVecBox::InputEnable(inputdata_t &inputdata)
{
	if (m_bDisabled)
	{
		Enable();
	}
}

void CTriggerVecBox::InputDisable(inputdata_t &inputdata)
{
	if (!m_bDisabled)
	{
		Disable();
	}
}

//-----------------------------------------------------------------------------
// Purpose Return true to pass, false for no pass
//-----------------------------------------------------------------------------

#define PASS return true
#define FAIL return false

bool CTriggerVecBox::TestFilter(CVecBox *pVecBox, int filtertype)
{
	if (filtertype == BALL0)
	{
		if (pVecBox->LastBallHit() == BALL0)
		{
			// Flag is Ball0, Box is Ball 0, All good! PASS!
			PASS;
		}

		// If our state is 2 (purple), check to see if we are protecting the previous activaton!
		if (pVecBox->LastBallHit() != BALL0 && pVecBox->IsProtecting() == BALL0)
		{
			if (m_bIgnoreProtection)
			{
				PASS;
			}
			else
			{
				FAIL;
			}
		}
		
	}

	if (filtertype == BALL1)
	{
		if (pVecBox->LastBallHit() == BALL1)
		{
			// Flag is Ball0, Box is Ball 0, All good! PASS!
			PASS;
		}

		// If our state is 2 (purple), check to see if we are protecting the previous activaton!
		if (pVecBox->LastBallHit() != BALL1 && pVecBox->IsProtecting() == BALL1)
		{
			if (m_bIgnoreProtection)
			{
				PASS;
			}
			else
			{
				FAIL;
			}
		}

	}

	if (filtertype == BALL2)
	{
		if (pVecBox->LastBallHit() == BALL2)
		{
			// Flag is Ball0, Box is Ball 0, All good! PASS!
			PASS;
		}
		else
		{
			FAIL;
		}
	}

	if (filtertype == BALL3)
	{
		if (pVecBox->LastBallHit() == BALL3)
		{
			// Flag is Ball0, Box is Ball 0, All good! PASS!
			PASS;
		}

		// If our state is 2 (purple), check to see if we are protecting the previous activaton!
		if (pVecBox->LastBallHit() != BALL3 && pVecBox->IsProtecting() == BALL3)
		{
			if (m_bIgnoreProtection)
			{
				PASS;
			}
			else
			{
				FAIL;
			}
		}

	}

	if (filtertype == BALL4)
	{
		if (pVecBox->LastBallHit() == BALL4)
		{
			// Flag is Ball0, Box is Ball 0, All good! PASS!
			PASS;
		}

		// If our state is 2 (purple), check to see if we are protecting the previous activaton!
		if (pVecBox->LastBallHit() != BALL4 && pVecBox->IsProtecting() == BALL4)
		{
			if (m_bIgnoreProtection)
			{
				PASS;
			}
			else
			{
				FAIL;
			}
		}

	}

	if (filtertype == BALL5)
	{
		if (pVecBox->LastBallHit() == BALL5)
		{
			// Flag is Ball0, Box is Ball 0, All good! PASS!
			PASS;
		}

		// If our state is 2 (purple), check to see if we are protecting the previous activaton!
		if (pVecBox->LastBallHit() != BALL5 && pVecBox->IsProtecting() == BALL5)
		{
			if (m_bIgnoreProtection)
			{
				PASS;
			}
			else
			{
				FAIL;
			}
		}

	}

	// Ghost Boxes.
	if (filtertype == BOX_GHOST)
	{
		if (pVecBox->IsGhost() || pVecBox->LastBallHit() == GHOSTID)
			PASS;
	}

	// No Activations!
	if (filtertype == BALL_ANY)
	{
		if (pVecBox->LastBallHit() == BALL_ANY)
			PASS;
	}

	if (filtertype == BOX_ANY)
	{
		PASS;
	}


	FAIL;
}

//-----------------------------------------------------------------------------
// Traps the entities
//-----------------------------------------------------------------------------
void CTriggerVecBox::Touch(CBaseEntity *pOther)
{
	if (pOther->ClassMatches("prop_vecbox"))
	{
		CVecBox *pVecBox = dynamic_cast<CVecBox*>(pOther);
		bool bResult = TestFilter(pVecBox, m_intFilterType);

		if (IsNegetedMode())
		{
			if (bResult) // If Pass
				return;
		}
		else
		{
			if (!bResult) // If Fail
				return;
		}

		m_OnBoxTouch.FireOutput(pVecBox, this);

		if (HasSpawnFlags(SF_TRIG_PUSH_ONCE))
		{
			UTIL_Remove(this);
		}
	}

	else if (pOther->IsPlayer())
	{
		m_OnPlayerTouch.FireOutput(pOther, this);
	}
}

//-----------------------------------------------------------------------------
// Traps the entities
//-----------------------------------------------------------------------------
void CTriggerVecBox::EndTouch(CBaseEntity *pOther)
{
	if (m_bHadPlayer)
	{
		m_OnEndPlayerTouch.FireOutput(pOther, this);
		m_bHadPlayer = false;
	}

	if (pOther->ClassMatches("prop_vecbox"))
	{
		m_OnEndBoxTouch.FireOutput(pOther, this);
	}

}

//-----------------------------------------------------------------------------
// Purpose: A trigger that pushes vecboxes that have been activated by ball0.
//-----------------------------------------------------------------------------
class CTriggerBlower : public CBaseTrigger
{
public:
	DECLARE_CLASS(CTriggerBlower, CBaseTrigger);

	void Spawn(void);
	void Activate(void);
	void Touch(CBaseEntity *pOther);
	void Untouch(CBaseEntity *pOther);
	//void DrawDebugGeometryOverlays();
	virtual bool IsVecBox(CBaseEntity *pOther);

	void InputSetPushDirection(inputdata_t &inputdata);

	Vector m_vecPushDir;

	DECLARE_DATADESC();

	float m_flAlternateTicksFix; // Scale factor to apply to the push speed when running with alternate ticks
	float m_flPushSpeed;
};

BEGIN_DATADESC(CTriggerBlower)
	DEFINE_KEYFIELD(m_vecPushDir, FIELD_VECTOR, "pushdir"),
	DEFINE_KEYFIELD(m_flAlternateTicksFix, FIELD_FLOAT, "alternateticksfix"),

	DEFINE_INPUTFUNC(FIELD_VECTOR, "SetPushDirection", InputSetPushDirection),
END_DATADESC()

LINK_ENTITY_TO_CLASS(trigger_blower, CTriggerBlower);

//-----------------------------------------------------------------------------
// Purpose: Called when spawning, after keyvalues have been handled.
//-----------------------------------------------------------------------------
void CTriggerBlower::Spawn()
{
	// Convert pushdir from angles to a vector
	Vector vecAbsDir;
	QAngle angPushDir = QAngle(m_vecPushDir.x, m_vecPushDir.y, m_vecPushDir.z);
	AngleVectors(angPushDir, &vecAbsDir);

	// Transform the vector into entity space
	VectorIRotate(vecAbsDir, EntityToWorldTransform(), m_vecPushDir);

	BaseClass::Spawn();

	InitTrigger();

	if (m_flSpeed == 0)
	{
		m_flSpeed = 100;
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CTriggerBlower::Activate()
{
	// Fix problems with triggers pushing too hard under sv_alternateticks.
	// This is somewhat hacky, but it's simple and we're really close to shipping.
	if ((m_flAlternateTicksFix != 0) && IsSimulatingOnAlternateTicks())
	{
		m_flPushSpeed = m_flSpeed * m_flAlternateTicksFix;
	}
	else
	{
		m_flPushSpeed = m_flSpeed;
	}

	BaseClass::Activate();
}

bool CTriggerBlower::IsVecBox(CBaseEntity *pOther)
{
	if (pOther->ClassMatches("prop_vecbox"))
	{
		CVecBox *pVecBox = dynamic_cast<CVecBox*>(pOther);
		if (pVecBox->GetState() == 0 || pVecBox->IsProtecting() == 0)
		{ 
			return true;
		}
	}
	else if (HasSpawnFlags(SF_TRIGGER_ALLOW_CLIENTS) && (pOther->GetFlags() & FL_CLIENT))
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CTriggerBlower::Touch(CBaseEntity *pOther)
{
	if (!pOther->IsSolid() || (pOther->GetMoveType() == MOVETYPE_PUSH || pOther->GetMoveType() == MOVETYPE_NONE))
		return;

	if (!IsVecBox(pOther))
		return;

	// FIXME: If something is hierarchically attached, should we try to push the parent?
	if (pOther->GetMoveParent())
		return;

	// Transform the push dir into global space
	Vector vecAbsDir;
	VectorRotate(m_vecPushDir, EntityToWorldTransform(), vecAbsDir);

	// Instant trigger, just transfer velocity and remove
	if (HasSpawnFlags(SF_TRIG_PUSH_ONCE))
	{
		pOther->ApplyAbsVelocityImpulse(m_flPushSpeed * vecAbsDir);

		if (vecAbsDir.z > 0)
		{
			pOther->SetGroundEntity(NULL);
		}
		UTIL_Remove(this);
		return;
	}

	switch (pOther->GetMoveType())
	{
	case MOVETYPE_NONE:
	case MOVETYPE_PUSH:
	case MOVETYPE_NOCLIP:
		break;

	case MOVETYPE_VPHYSICS:
	{
		const float DEFAULT_MASS = 100.0f;
		if (HasSpawnFlags(SF_TRIGGER_PUSH_USE_MASS))
		{
			// New-style code, affects all physobjs and accounts for mass
			IPhysicsObject *ppPhysObjs[VPHYSICS_MAX_OBJECT_LIST_COUNT];
			int nNumPhysObjs = pOther->VPhysicsGetObjectList(ppPhysObjs, VPHYSICS_MAX_OBJECT_LIST_COUNT);
			for (int i = 0; i < nNumPhysObjs; i++)
			{
				Vector force = m_flPushSpeed * vecAbsDir * DEFAULT_MASS * gpGlobals->frametime;
				force *= ppPhysObjs[i]->GetMass() / DEFAULT_MASS;
				ppPhysObjs[i]->ApplyForceCenter(force);
			}
		}
		else
		{
			// Old-style code (Ep2 and before), affects the first physobj and assumes the mass is 100kg
			IPhysicsObject *pPhys = pOther->VPhysicsGetObject();
			if (pPhys)
			{
				pPhys->ApplyForceCenter(m_flPushSpeed * vecAbsDir * DEFAULT_MASS * gpGlobals->frametime);
				return;
			}
		}
	}
	break;

	default:
	{
		Vector vecPush = (m_flPushSpeed * vecAbsDir);
		if (pOther->GetFlags() & FL_BASEVELOCITY)
		{
			vecPush = vecPush + pOther->GetBaseVelocity();
		}
		if (vecPush.z > 0 && (pOther->GetFlags() & FL_ONGROUND))
		{
			pOther->SetGroundEntity(NULL);
			Vector origin = pOther->GetAbsOrigin();
			origin.z += 1.0f;
			pOther->SetAbsOrigin(origin);
		}

		pOther->SetBaseVelocity(vecPush);
		pOther->AddFlag(FL_BASEVELOCITY);
	}
	break;
	}
}

void CTriggerBlower::InputSetPushDirection(inputdata_t &inputdata)
{
	inputdata.value.Vector3D(m_vecPushDir);

	// Convert pushdir from angles to a vector
	Vector vecAbsDir;
	QAngle angPushDir = QAngle(m_vecPushDir.x, m_vecPushDir.y, m_vecPushDir.z);
	AngleVectors(angPushDir, &vecAbsDir);

	// Transform the vector into entity space
	VectorIRotate(vecAbsDir, EntityToWorldTransform(), m_vecPushDir);
}

//-----------------------------------------------------------------------------
// Purpose: A clip just for Vecboxes.
//-----------------------------------------------------------------------------
#include "filters.h"
class CFuncVecBoxClip : public CBaseEntity
{
	DECLARE_DATADESC();
	DECLARE_CLASS(CFuncVecBoxClip, CBaseEntity);

public:
	void Spawn();
	void Activate();
	bool CreateVPhysics(void);

	bool EntityPassesFilter(CBaseEntity *pOther);
	bool ForceVPhysicsCollide(CBaseEntity *pEntity);

	void InputEnable(inputdata_t &inputdata);
	void InputDisable(inputdata_t &inputdata);

private:

	string_t						m_iFilterName;
	CHandle<CBaseFilter>			m_hFilter;
	bool							m_bDisabled;
};

// Global Savedata for base trigger
BEGIN_DATADESC(CFuncVecBoxClip)

// Keyfields
DEFINE_KEYFIELD(m_iFilterName, FIELD_STRING, "filtername"),
	DEFINE_FIELD(m_hFilter, FIELD_EHANDLE),
	DEFINE_FIELD(m_bDisabled, FIELD_BOOLEAN),

	DEFINE_INPUTFUNC(FIELD_VOID, "Enable", InputEnable),
	DEFINE_INPUTFUNC(FIELD_VOID, "Disable", InputDisable),

END_DATADESC()


LINK_ENTITY_TO_CLASS(func_clip_vecbox, CFuncVecBoxClip);

void CFuncVecBoxClip::Spawn(void)
{
	SetMoveType(MOVETYPE_PUSH);  // so it doesn't get pushed by anything
	SetSolid(SOLID_VPHYSICS);
	AddSolidFlags(FSOLID_NOT_SOLID);
	SetModel(STRING(GetModelName()));
	AddEffects(EF_NODRAW);
	CreateVPhysics();
	VPhysicsGetObject()->EnableCollisions(!m_bDisabled);
}


bool CFuncVecBoxClip::CreateVPhysics(void)
{
	VPhysicsInitStatic();
	return true;
}


void CFuncVecBoxClip::Activate(void)
{
	// Get a handle to my filter entity if there is one
	if (m_iFilterName != NULL_STRING)
	{
		m_hFilter = dynamic_cast<CBaseFilter *>(gEntList.FindEntityByName(NULL, m_iFilterName));
	}
	BaseClass::Activate();
}

bool CFuncVecBoxClip::EntityPassesFilter(CBaseEntity *pOther)
{
	CBaseFilter* pFilter = (CBaseFilter*)(m_hFilter.Get());

	if (pFilter)
		return pFilter->PassesFilter(this, pOther);

	if (!pOther->VPhysicsGetObject())
		return false;

	//if (pOther->GetMoveType() == MOVETYPE_VPHYSICS && pOther->VPhysicsGetObject()->IsMoveable())
	if (pOther->ClassMatches("prop_vecbox"))
		return true;

	return false;
}


bool CFuncVecBoxClip::ForceVPhysicsCollide(CBaseEntity *pEntity)
{
	return EntityPassesFilter(pEntity);
}

void CFuncVecBoxClip::InputEnable(inputdata_t &inputdata)
{
	VPhysicsGetObject()->EnableCollisions(true);
	m_bDisabled = false;
}

void CFuncVecBoxClip::InputDisable(inputdata_t &inputdata)
{
	VPhysicsGetObject()->EnableCollisions(false);
	m_bDisabled = true;
}

//-----------------------------------------------------------------------------
// A trigger that only fires outputs for vecboxes!
//-----------------------------------------------------------------------------
class CTriggerVecButton : public CTriggerMultiple
{
	DECLARE_CLASS(CTriggerVecButton, CTriggerMultiple);
	DECLARE_DATADESC();

public:

	//Constructor
	CTriggerVecButton()
	{
	}

	void Precache(void);
	void Spawn(void);
	void Touch(CBaseEntity *pOther);
	void OnEndTouchAll(CBaseEntity *pOther);

	bool IsPressed()
	{
		return m_bPressed;
	};

private:

	void InputEnable(inputdata_t &inputdata);
	void InputDisable(inputdata_t &inputdata);
	void InputToggle(inputdata_t &inputdata);

	bool m_bPressed;
	bool m_bPressedByBox;
	bool m_bPressedByPlayer;

	COutputEvent	m_OnPressed;
	COutputEvent	m_OnUnPressed;

	COutputEvent	m_OnPressedPlayer;
	COutputEvent	m_OnPressedBox;

};


//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(trigger_button, CTriggerVecButton);

BEGIN_DATADESC(CTriggerVecButton)
	DEFINE_FIELD(m_bPressed, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bPressedByBox, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bPressedByPlayer, FIELD_BOOLEAN),

	DEFINE_OUTPUT(m_OnPressed, "OnPress"),
	DEFINE_OUTPUT(m_OnUnPressed, "OnUnPress"),

	DEFINE_OUTPUT(m_OnPressedPlayer, "OnPressPlayer"),
	DEFINE_OUTPUT(m_OnPressedBox, "OnPressBox"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerVecButton::Precache(void)
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerVecButton::Spawn(void)
{
	DevMsg("CTriggerVecButton: WE EXIST\n");
	BaseClass::Spawn();
}

//------------------------------------------------------------------------------
// Inputs
//------------------------------------------------------------------------------
void CTriggerVecButton::InputToggle(inputdata_t &inputdata)
{
	if (m_bDisabled)
	{
		InputEnable(inputdata);
	}
	else
	{
		InputDisable(inputdata);
	}
}

void CTriggerVecButton::InputEnable(inputdata_t &inputdata)
{
	if (m_bDisabled)
	{
		Enable();
	}
}

void CTriggerVecButton::InputDisable(inputdata_t &inputdata)
{
	if (!m_bDisabled)
	{
		Disable();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTriggerVecButton::Touch(CBaseEntity *pOther)
{
	if (m_bPressed || m_bPressedByBox || m_bPressedByPlayer)
		return;

	if (pOther->IsPlayer() || pOther->ClassMatches("prop_vecbox"))
	{
		if (pOther->IsPlayer() && !m_bPressedByPlayer)
		{
			DevMsg("CPropButtonFloorTop: Player is on me!\n");
			m_bPressedByPlayer = true;
			m_OnPressedPlayer.FireOutput(this, this);
		}
		else if (pOther->ClassMatches("prop_vecbox") && !m_bPressedByBox)
		{
			if (pOther->GetFlags() & FL_DISSOLVING)
				return;

			DevMsg("CPropButtonFloorTop: Box is on me!\n");
			m_bPressedByBox = true;
			m_OnPressedBox.FireOutput(this, this);
		}

		EmitSound("Vectronic.ButtonDown");
		
		if (!m_bPressed)
		{
			m_OnPressed.FireOutput(this, this);
			m_bPressed = true;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTriggerVecButton::OnEndTouchAll(CBaseEntity *pOther)
{
	if (m_bPressed || m_bPressedByBox || m_bPressedByPlayer || pOther->GetFlags() & FL_DISSOLVING)
	{
		DevMsg("CPropButtonFloorTop: EndTouch!\n");
		m_bPressed = false;
		m_bPressedByBox = false;
		m_bPressedByPlayer = false;
		m_OnUnPressed.FireOutput(this, this);
	}
}