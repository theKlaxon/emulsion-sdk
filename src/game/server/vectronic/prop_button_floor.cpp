//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "vectronic_player.h"
#include "prop_vecbox.h"
#include "engine/ienginesound.h"
#include "soundent.h"
#include "player_pickup.h"
#include "soundenvelope.h"
#include "game.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define BUTTON_MODEL	"models/props/button_combined.mdl"
#define BUTTON_TOP		"models/props/button_top.mdl"

//-----------------------------------------------------------------------------
// Purpose: Top
//-----------------------------------------------------------------------------
class CPropButtonFloorTop : public CBaseAnimating
{
public:
	DECLARE_CLASS(CPropButtonFloorTop, CBaseAnimating);
	DECLARE_DATADESC();

	//Constructor
	CPropButtonFloorTop()
	{
		m_bPressed = false;
	}

	/*
	bool CreateVPhysics()
	{
	VPhysicsInitNormal( SOLID_VPHYSICS, 0, false );
	return true;
	}
	*/

	void Precache(void);

	void Spawn(void);
	virtual void PostClientActive(void);
	virtual void Enable(void);
	virtual void Disable(void);

	void Touch(CBaseEntity *pOther);
	void EndTouch(CBaseEntity *pOther);

	// Input handlers
	virtual void InputEnable(inputdata_t &inputdata);
	virtual void InputDisable(inputdata_t &inputdata);

	bool IsPressed()
	{
		return m_bPressed;
	};

	bool IsPressedByPlayer()
	{
		return m_bPressedByPlayer;
	};

	bool IsPressedByBox()
	{
		return m_bPressedByPlayer;
	};


private:

	bool m_bPressed;
	bool m_bPressedByPlayer;
	bool m_bPressedByBox;
	bool m_bDisabled;

	/*!!TEMP!!*/
	COutputEvent	m_OnPressed;
	COutputEvent	m_OnPressedPlayer;
	COutputEvent	m_OnPressedBox;
	COutputEvent	m_OnUnPressed;

};

LINK_ENTITY_TO_CLASS(prop_floorbutton_top, CPropButtonFloorTop);

// Start of our data description for the class
BEGIN_DATADESC(CPropButtonFloorTop)
DEFINE_FIELD(m_bPressed, FIELD_BOOLEAN),
DEFINE_FIELD(m_bPressedByBox, FIELD_BOOLEAN),
DEFINE_FIELD(m_bPressedByPlayer, FIELD_BOOLEAN),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropButtonFloorTop::Precache(void)
{
	PrecacheModel(BUTTON_TOP);

	/*!!TEMP!!*/
	PrecacheScriptSound("Vectronic.ButtonDown");
	PrecacheScriptSound("Vectronic.ButtonUp");

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPropButtonFloorTop::Spawn(void)
{
	Precache();

	SetModel(BUTTON_TOP);
	SetSolid(SOLID_VPHYSICS);
	AddEffects(EF_NOSHADOW);

	VPhysicsInitStatic();
	AddSolidFlags(FSOLID_TRIGGER);
	PhysicsTouchTriggers();


	BaseClass::Spawn();

}

//------------------------------------------------------------------------------
// Purpose: Input handler to turn on this trigger.
//------------------------------------------------------------------------------
void CPropButtonFloorTop::InputEnable(inputdata_t &inputdata)
{
	Enable();
}


//------------------------------------------------------------------------------
// Purpose: Input handler to turn off this trigger.
//------------------------------------------------------------------------------
void CPropButtonFloorTop::InputDisable(inputdata_t &inputdata)
{
	Disable();
}

//------------------------------------------------------------------------------
// Purpose: Turns on this trigger.
//------------------------------------------------------------------------------
void CPropButtonFloorTop::Enable(void)
{
	m_bDisabled = false;

	if (VPhysicsGetObject())
	{
		VPhysicsGetObject()->EnableCollisions(true);
	}

	if (!IsSolidFlagSet(FSOLID_TRIGGER))
	{
		AddSolidFlags(FSOLID_TRIGGER);
		PhysicsTouchTriggers();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called after player becomes active in the game
//-----------------------------------------------------------------------------
void CPropButtonFloorTop::PostClientActive(void)
{
	BaseClass::PostClientActive();

	if (!m_bDisabled)
	{
		PhysicsTouchTriggers();
	}
}



//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CPropButtonFloorTop::Touch(CBaseEntity *pOther)
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
			DevMsg("CPropButtonFloorTop: Box is on me!\n");
			m_bPressedByBox = true;
			m_OnPressedBox.FireOutput(this, this);
		}

		EmitSound("Vectronic.ButtonDown");
		m_bPressed = true;
		m_OnPressed.FireOutput(this, this);
	}

}

void CPropButtonFloorTop::EndTouch(CBaseEntity *pEntity)
{
	if (m_bPressed)
	{
		DevMsg("CPropButtonFloorTop: EndTouch!\n");
		m_bPressed = false;
		m_bPressedByBox = false;
		m_bPressedByPlayer = false;
		m_OnUnPressed.FireOutput(this, this);
		EmitSound("Vectronic.ButtonUp");
	}
}
/*
//-----------------------------------------------------------------------------
// Purpose: Base
//-----------------------------------------------------------------------------
class CPropButtonFloor : public CBaseAnimating
{
public:
DECLARE_CLASS( CPropButtonFloor, CBaseAnimating );
DECLARE_DATADESC();

//Constructor
CPropButtonFloor()
{
}

void Precache( void );
void Spawn( void );
void Think (void);

void Press();
void Release();

void SpawnTop();


private:

CHandle <CPropButtonFloorTop> m_hButtonTop;

bool m_bPressed;

COutputEvent	m_OnPressed;
COutputEvent	m_OnPressedPlayer;
COutputEvent	m_OnPressedBox;
COutputEvent	m_OnUnPressed;

};

LINK_ENTITY_TO_CLASS( prop_button_floor, CPropButtonFloor );

// Start of our data description for the class
BEGIN_DATADESC( CPropButtonFloor )
DEFINE_FIELD( m_bPressed, FIELD_BOOLEAN ),
DEFINE_FIELD( m_hButtonTop, FIELD_EHANDLE ),

DEFINE_OUTPUT( m_OnPressed, "OnPressed" ),
DEFINE_OUTPUT( m_OnPressedPlayer, "OnPressedPlayer" ),
DEFINE_OUTPUT( m_OnPressedBox, "OnPressedBox" ),
DEFINE_OUTPUT( m_OnUnPressed, "OnUnPressed" ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CPropButtonFloor::Precache( void )
{
PrecacheModel( BUTTON_MODEL );

AddEffects(EF_NODRAW );
//AddEffects(EF_NOSHADOW);

PrecacheScriptSound( "Vectronic.ButtonDown" );
PrecacheScriptSound( "Vectronic.ButtonUp" );

BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPropButtonFloor::Spawn( void )
{
Precache();
SetModel( BUTTON_MODEL );
SetSolid( SOLID_VPHYSICS );

SpawnTop();

VPhysicsInitStatic();
SetNextThink( gpGlobals->curtime );

BaseClass::Spawn();

}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CPropButtonFloor::SpawnTop( void )
{
bool bMadeTop = false;

if(!bMadeTop)
{
m_hButtonTop = (CPropButtonFloorTop *) CreateEntityByName( "prop_trigger_button" );

if ( m_hButtonTop == NULL )
{
return;
}

// Get Our Origin
Vector vecAbsOrigin = GetAbsOrigin();

QAngle qAngles = this->GetAbsAngles();

int		attachment;
attachment = LookupAttachment( "attach_root" );

// Get Our Origin
if( attachment > -1 )
{
GetAttachment( attachment, vecAbsOrigin, qAngles );

m_hButtonTop->SetAbsOrigin( vecAbsOrigin );
m_hButtonTop->SetAbsAngles( qAngles );
m_hButtonTop->SetOwnerEntity( this );
m_hButtonTop->SetParent( this, attachment );
m_hButtonTop->Spawn();
}

//SetOwnerEntity(m_hButtonTop);
}
}

void CPropButtonFloor::Think( void )
{
if (!m_hButtonTop)
return;

if(!m_bPressed && m_hButtonTop->IsPressed())
{
Press();
}
else if (m_bPressed && m_hButtonTop->IsPressed() == false )
{
Release();
}

SetNextThink( gpGlobals->curtime );
}

void CPropButtonFloor::Press()
{
if (!m_bPressed)
{
m_nSkin = 1;
EmitSound( "Vectronic.ButtonDown" );
// Select the scanner's idle sequence
SetSequence( LookupSequence("press") );
// Set the animation speed to 100%
SetPlaybackRate( 1.0f );
// Tell the client to animate this model
UseClientSideAnimation();

if (m_hButtonTop->IsPressedByPlayer())
{
m_OnPressedPlayer.FireOutput(this,this);
}
if (m_hButtonTop->IsPressedByBox())
{
m_OnPressedBox.FireOutput(this,this);
}

m_OnPressed.FireOutput(this,this);

m_bPressed = true;

}
}

void CPropButtonFloor::Release()
{
if (m_bPressed)
{
EmitSound( "Vectronic.ButtonUp" );

int nSequence = LookupSequence( "idle" );
ResetSequence( nSequence );
SetPlaybackRate( 1.0f );
UseClientSideAnimation();
m_OnUnPressed.FireOutput( this, this );
m_bPressed = false;
}
}

*/