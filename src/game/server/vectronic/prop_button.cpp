//=========== Copyright © 2013, rHetorical, All rights reserved. =============
//
// Purpose: button. 
//		
//=============================================================================

#include "cbase.h"
#include "vectronic_player.h"
#include "engine/ienginesound.h"
#include "soundent.h"
#include "player_pickup.h"
#include "soundenvelope.h"
#include "game.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define BUTTON_MODEL "models/props/switch001.mdl"

//-----------------------------------------------------------------------------
// Purpose: Base
//-----------------------------------------------------------------------------
class CPropButton : public CBaseAnimating
{
	public:
	DECLARE_CLASS( CPropButton, CBaseAnimating );
	DECLARE_DATADESC();

	//Constructor
	CPropButton()
	{
		m_bLocked = false;
		m_bJustPressed = false;

		m_flTimeAmount = 1;
	}

	void Spawn( void );
	void Precache( void );
	void Think( void );

	//virtual void	TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr );

	void Press();
	void Release();

	// Input functions.
	void InputPress( inputdata_t &inputData);
	void InputLock( inputdata_t &inputData);
	void InputUnlock( inputdata_t &inputData);
	void InputCancelPress( inputdata_t &inputData);

	//Use
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int ObjectCaps();

	//Sounds
	void CreateSounds();
	void StopLoopingSounds();

private:

	bool	m_bLocked;
	bool	m_bisTimed;
	float	m_flTimeAmount;
	bool	m_bJustPressed;

	COutputEvent	m_OnPressed;
	COutputEvent	m_OnButtonReset;

	CSoundPatch		*m_pTickSound;
};

LINK_ENTITY_TO_CLASS( prop_button, CPropButton );

// Start of our data description for the class
BEGIN_DATADESC( CPropButton )
 
	//Save/load

	DEFINE_USEFUNC( Use ),
	DEFINE_SOUNDPATCH( m_pTickSound ),

	DEFINE_FIELD( m_bLocked, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bisTimed, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flTimeAmount, FIELD_FLOAT ),

	DEFINE_KEYFIELD( m_bisTimed, FIELD_BOOLEAN, "istimer" ),
	DEFINE_KEYFIELD( m_flTimeAmount, FIELD_FLOAT, "Delay" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Press", InputPress ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Lock", InputLock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Unlock", InputUnlock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "CancelPress", InputCancelPress ),

	DEFINE_OUTPUT( m_OnPressed, "OnPressed" ),
	DEFINE_OUTPUT( m_OnButtonReset, "OnButtonReset" ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropButton::Precache( void )
{
	PrecacheModel( BUTTON_MODEL );

	PrecacheScriptSound( "Vectronic.TickTock_Loop" );
	PrecacheScriptSound ( "Vectronic.SwitchLocked" );
	PrecacheScriptSound( "Vectronic.SwitchDown" );
	PrecacheScriptSound( "Vectronic.SwitchUp" );
 
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPropButton::Spawn( void )
{
	Precache();
	SetModel( BUTTON_MODEL );
	SetSolid( SOLID_VPHYSICS );

	// Make it soild to other entities.
	VPhysicsInitStatic();

//	SetCollisionGroup( COLLISION_GROUP_VPHYSICS );

	SetUse( &CPropButton::Use );
	SetSequence( LookupSequence("idle") );

	AddEffects(EF_NOSHADOW);	

	m_bLocked = false;

	BaseClass::Spawn();

}
//-----------------------------------------------------------------------------
// Purpose: Play and stop the sounds
//-----------------------------------------------------------------------------
void CPropButton::CreateSounds()
{
	if ( !m_pTickSound /*&& m_bLocked*/ )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		CReliableBroadcastRecipientFilter filter;

		m_pTickSound = controller.SoundCreate( filter, entindex(), "Vectronic.TickTock_Loop" );
		controller.Play( m_pTickSound, 1.0, 100 );
	}
}

void CPropButton::StopLoopingSounds()
{
	if ( m_pTickSound && !m_bLocked )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		controller.SoundDestroy( m_pTickSound );
		m_pTickSound = NULL;
	}

	BaseClass::StopLoopingSounds();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropButton::Think( void )
{
	BaseClass::Think();
	Release();
	m_OnButtonReset.FireOutput( this, this );
}

//-----------------------------------------------------------------------------
// Here we link +USE to the entity
//-----------------------------------------------------------------------------
int CPropButton::ObjectCaps()
{ 
	int caps = BaseClass::ObjectCaps();

	caps |= FCAP_IMPULSE_USE;

	return caps;
}

void CPropButton::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBasePlayer *pPlayer = ToBasePlayer( pActivator );
	
	if ( pPlayer == NULL)
		return;

	// 9/29/16: Wow, been a while since I last touch this crap. 
	// We're going to show off this mod to Valve for Steam DevDays and we need to disable this bullshit methoid of 
	// using a hitbox for use.
#ifdef BUTTON_HITBOX_TEST
	// Find out if the player's looking at our ammocrate hitbox 
	Vector vecForward;
	pPlayer->EyeVectors( &vecForward, NULL, NULL );

	trace_t tr;
	Vector vecStart = pPlayer->EyePosition();
	UTIL_TraceLine( vecStart, vecStart + vecForward * 1024, MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, pPlayer, COLLISION_GROUP_NONE, &tr );

	if ( tr.m_pEnt == this && tr.hitgroup == 5 )
	{
		CBasePlayer *pPlayer = ToBasePlayer( pActivator );
		if ( pPlayer )
		{
			if (!m_bLocked)
			{
				Press();
			}
			else
			{
				EmitSound ("Vectronic.SwitchLocked");
			}
		}
	}
	else //if ( tr.m_pEnt == this && tr.hitgroup != 5 )
	{
			// Shall we make the player play a deny sound?
			pPlayer->EmitSound( SOUND_USE_DENY );
	}
#else
	if (pPlayer)
	{
		if (!m_bLocked)
		{
			Press();
		}
		else
		{
			EmitSound("Portal.Button_Locked");
		}
	}
#endif
}


// Whoa sister, let's do the lock stuff before we forget!
void CPropButton::InputLock( inputdata_t &inputData)
{
	if (!m_bLocked)
	{
		m_bLocked = true;
	}
}

void CPropButton::InputUnlock( inputdata_t &inputData)
{
	if (m_bLocked)
	{
		m_bLocked = false;
	}
}

void CPropButton::InputCancelPress( inputdata_t &inputData)
{
	if (m_bJustPressed)
	{
		Release();
	}
}

void CPropButton::InputPress( inputdata_t &inputData)
{
	Press();
}


void CPropButton::Press()
{
	// 07-9-14: Removed the yellow indicator since the third ball is purple due to color-blindness. ~reep.

	if (!m_bJustPressed)
	{
		m_nSkin = 1;
		EmitSound( "Vectronic.SwitchDown" );
		// Select the scanner's idle sequence
		SetSequence( LookupSequence("down") );
		// Set the animation speed to 100%
		SetPlaybackRate( 1.0f );
		// Tell the client to animate this model
		UseClientSideAnimation();

		// If we are timed, then make a sound!
		if (m_bisTimed)
		{
		//	m_nSkin = 2;
			CreateSounds();
		}
		/*
		else
		{
			m_nSkin = 1;
		}
		*/

		m_OnPressed.FireOutput( this, this );
		SetNextThink( gpGlobals->curtime + m_flTimeAmount );
		m_bJustPressed = true;
	}
}

void CPropButton::Release()
{
	if (m_bJustPressed)
	{
		EmitSound( "Vectronic.SwitchUp" );

#ifdef SWARM_DLL
		int nSequence = LookupSequence( "idle" );
		ResetSequence( nSequence );
		SetPlaybackRate( 1.0f );
		UseClientSideAnimation();
#else
		// Select the scanner's idle sequence
		SetSequence( LookupSequence("up") );
		// Set the animation speed to 100%
		SetPlaybackRate( 1.0f );
		// Tell the client to animate this model
		UseClientSideAnimation();
#endif

		// If we are timed, then stop the sound!
		if (m_bisTimed)
		{
			StopLoopingSounds();
		}

		m_bJustPressed = false;
		m_nSkin = 0;
	}
}