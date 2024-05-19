//=========== Copyright © 2012, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================
 
#include "cbase.h"
#include <convar.h>
#include "func_break.h"
#include "engine/ienginesound.h"
#include "vphysics/player_controller.h"
#include "soundent.h"
#include "entityinput.h"
#include "entityoutput.h"
#include "eventqueue.h"
#include "mathlib/mathlib.h"
#include "globalstate.h"
#include "ndebugoverlay.h"
#include "saverestore_utlvector.h"
#include "vstdlib/random.h"
#include "gameinterface.h"
#include "soundenvelope.h"
#include "game.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CIndicator : public CBaseAnimating
{
public:
	DECLARE_CLASS( CIndicator, CBaseAnimating );
	DECLARE_DATADESC();
 
	CIndicator()
	{
	 m_bActive = false;
	}
 
	void Spawn( void );
	void Precache( void );
	void Think( void );

	// Sound
	void CreateSounds();
	void StopLoopingSounds();

	// Input functions.
	void InputActivate( inputdata_t &inputData);
	void InputDeactivate( inputdata_t &inputData);
	void ResetIndicator();

private:

	bool	m_bActive;
	bool	m_bisTimed;
	bool	m_bisQuiet;
	float	m_flTimeAmount;

	CSoundPatch		*m_pLoopSound;

	COutputEvent	m_OnActivate;
	COutputEvent	m_OnDeactivate;

};
 
LINK_ENTITY_TO_CLASS( prop_indicator_panel, CIndicator );
 
// Start of our data description for the class
BEGIN_DATADESC( CIndicator )
 
	//Save/load

	DEFINE_FIELD( m_bisTimed, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flTimeAmount, FIELD_FLOAT ),
	DEFINE_FIELD( m_bisQuiet, FIELD_BOOLEAN ),
	DEFINE_SOUNDPATCH( m_pLoopSound ),

	DEFINE_KEYFIELD( m_bisTimed, FIELD_BOOLEAN, "timed" ),
	DEFINE_KEYFIELD( m_flTimeAmount, FIELD_FLOAT, "timeamount" ),
	DEFINE_KEYFIELD( m_bisQuiet, FIELD_BOOLEAN, "quiet" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Activate", InputActivate ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Deactivate", InputDeactivate ),

	DEFINE_OUTPUT( m_OnActivate, "OnActivate" ),
	DEFINE_OUTPUT( m_OnDeactivate, "OnDeactivate" ),
 
END_DATADESC()
 
// Name of our entity's model
#define	ENTITY_MODEL	"models/props_signage/doorstate.mdl" 
 
//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CIndicator::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );

	PrecacheScriptSound( "Vectronic.TickTock_Loop" );
 
	BaseClass::Precache();
}
 
//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CIndicator::Spawn( void )
{
	Precache();
 
	SetModel( ENTITY_MODEL );
	SetSolid( SOLID_VPHYSICS );

	AddEffects(EF_NOSHADOW /*|| EF_NOFLASHLIGHT*/ );	

	m_bActive = true;

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CIndicator::Think( void )
{
	BaseClass::Think();
	StopLoopingSounds();
	ResetIndicator();

	DevMsg("CIndicator: Done Thinking\n");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CIndicator::CreateSounds()
{
	if ( !m_pLoopSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		CReliableBroadcastRecipientFilter filter;

		m_pLoopSound = controller.SoundCreate( filter, entindex(), "Vectronic.TickTock_Loop" );
		controller.Play( m_pLoopSound, 0.78, 100 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CIndicator::StopLoopingSounds()
{
	if ( m_pLoopSound )
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		controller.SoundDestroy( m_pLoopSound );
		m_pLoopSound = NULL;
	}

	BaseClass::StopLoopingSounds();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CIndicator::InputActivate( inputdata_t &inputData )
{
	// 07-9-14: Removed the yellow indicator since the third ball is purple due to color-blindness. ~reep.
	m_nSkin = 1;
	if (!m_bisTimed)
	{
		//m_nSkin = 1;
		m_OnActivate.FireOutput( inputData.pActivator, this );
		CBaseEntity *pEntity = gEntList.FindEntityByName( NULL, m_target );
	
		while( pEntity ) 
		{
			pEntity->SetTextureFrameIndex( 1 );
			pEntity = gEntList.FindEntityByName( pEntity, m_target );  
		}
	}
	else
	{
		//m_nSkin = 2;
		m_OnActivate.FireOutput( inputData.pActivator, this );
		CBaseEntity *pEntity = gEntList.FindEntityByName( NULL, m_target );
		if (!m_bisQuiet)
		{
			CreateSounds();
		}
		SetNextThink( gpGlobals->curtime + m_flTimeAmount );
	
		while( pEntity ) 
		{
			pEntity->SetTextureFrameIndex( 2 );
			pEntity = gEntList.FindEntityByName( pEntity, m_target );  
		}
	}
}

void CIndicator::InputDeactivate( inputdata_t &inputData )
{
	m_nSkin = 0;
	m_OnDeactivate.FireOutput( inputData.pActivator, this );
	CBaseEntity *pEntity = gEntList.FindEntityByName( NULL, m_target );
		
	while( pEntity ) 
	{
		pEntity->SetTextureFrameIndex( 0 );
		pEntity = gEntList.FindEntityByName( pEntity, m_target );  
	}
}

void CIndicator::ResetIndicator()
{
	m_nSkin = 0;
	m_OnDeactivate.FireOutput( this, this );
	CBaseEntity *pEntity = gEntList.FindEntityByName( NULL, m_target );
		
	while( pEntity ) 
	{
		pEntity->SetTextureFrameIndex( 0 );
		pEntity = gEntList.FindEntityByName( pEntity, m_target );  
	}
}
