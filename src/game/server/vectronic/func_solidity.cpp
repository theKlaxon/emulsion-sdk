//=========== Copyright © 2013, rHetorical, All rights reserved. =============
//
// Purpose:
//		
//=============================================================================

#include "cbase.h"
#include "saverestore_utlvector.h"

//-----------------------------------------------------------------------------
// func_solidity 
//-----------------------------------------------------------------------------
class CFuncSolidity  : public CPointEntity
{
public:
	DECLARE_CLASS( CFuncSolidity, CPointEntity );
	DECLARE_DATADESC();
 
	void Spawn();
	bool CreateVPhysics( void );
	
private:

 	void InputEnable( inputdata_t &data );
	void InputDisable( inputdata_t &data );
	bool m_bDisabled;

	void InputSetSoild( inputdata_t &data );
	void InputSetNonSoild( inputdata_t &data );
	bool m_bSoild;
};

LINK_ENTITY_TO_CLASS( func_solidity, CFuncSolidity );
 
BEGIN_DATADESC( CFuncSolidity )

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	DEFINE_KEYFIELD(m_bDisabled, FIELD_BOOLEAN, "StartDisabled"),
	DEFINE_KEYFIELD(m_bSoild, FIELD_BOOLEAN, "StartSoild"),
	
	DEFINE_INPUTFUNC(FIELD_VOID, "SetSoild", InputSetSoild),
	DEFINE_INPUTFUNC(FIELD_VOID, "SetNonSoild", InputSetNonSoild),
 
END_DATADESC()

bool CFuncSolidity::CreateVPhysics( void )
{
	VPhysicsInitStatic();
	return true;
}

void CFuncSolidity::Spawn()
{
	BaseClass::Spawn();
 
	// We should collide with physics

	if ( m_bDisabled )
	{
		AddEffects( EF_NODRAW );
	}
	else
	{
		RemoveEffects( EF_NODRAW );
	}

	if ( m_bSoild )
	{
		SetSolid( SOLID_NONE );
	}
	else
	{
		SetSolid( SOLID_BSP );
	}
 
	// Use our brushmodel
	SetModel( STRING( GetModelName() ) );
 
	// We push things out of our way
	SetMoveType( MOVETYPE_PUSH );
	AddFlag( FL_WORLDBRUSH );

	CreateVPhysics();
}

void CFuncSolidity::InputEnable( inputdata_t &inputdata )
{
	m_bDisabled = false;
	RemoveEffects( EF_NODRAW );
	SetSolid( SOLID_BSP );
	//VPhysicsGetObject()->EnableCollisions(true);
}

void CFuncSolidity::InputDisable( inputdata_t &inputdata )
{
	m_bDisabled = true;
	AddEffects( EF_NODRAW );
	SetSolid( SOLID_NONE );
	//VPhysicsGetObject()->EnableCollisions(false);
}

void CFuncSolidity::InputSetSoild( inputdata_t &inputdata )
{
	m_bSoild = false;
	SetSolid( SOLID_BSP );
	//VPhysicsGetObject()->EnableCollisions(true);
}

void CFuncSolidity::InputSetNonSoild( inputdata_t &inputdata )
{
	m_bSoild = true;
	SetSolid( SOLID_NONE );
	//VPhysicsGetObject()->EnableCollisions(false);
}