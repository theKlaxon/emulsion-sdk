//======== Copyright © 2013 - 2014, rHetorical, All rights reserved. ==========
//
// Purpose:
//		
//=============================================================================

#include "cbase.h"
#include "model_types.h"
#ifndef SWARM_DLL
#include "clienteffectprecachesystem.h"
#endif
#include "basevectroniccombatweapon_shared.h"
#include "fx.h"
#include "c_te_effect_dispatch.h"
#include "beamdraw.h"

class C_WeaponVecgun : public C_BaseVectronicCombatWeapon
{
	DECLARE_CLASS( C_WeaponVecgun, C_BaseVectronicCombatWeapon );
	DECLARE_CLIENTCLASS();
public:
	
	C_WeaponVecgun( void );

	//virtual RenderGroup_t GetRenderGroup( void )
	//{
	//	// We want to draw translucent bits as well as our main model
	//	return RENDER_GROUP_TWOPASS;
	//}

	//virtual void	ClientThink( void );

	//virtual void	OnDataChanged( DataUpdateType_t updateType );
	//virtual int		DrawModel( int flags );
	//virtual void	DrawCrosshair();

	int		m_nCurrentSelection;
	int		GetCurrentSelection()
	{
		return m_nCurrentSelection;
	};

	int		m_nNumShotsFired;
	int	GetNumShots()
	{
		return m_nNumShotsFired;
	};

	int m_nDelay;
	int	GetDelay()
	{
		return m_nDelay;
	};


	bool	CanFireBlue(){return m_bCanFireBlue;};
	bool	CanFireGreen(){return m_bCanFireGreen;};
	bool	CanFireYellow(){return m_bCanFireYellow;};
	bool	IsUsingCustomBalls(){return m_bUseCustoms;};

private:

//	Vector	m_vecLastOrigin;
//	bool	m_bUpdated;

	bool m_bCanFireBlue;
	bool m_bCanFireGreen;
	bool m_bCanFireYellow;
	bool m_bUseCustoms;

};

inline C_WeaponVecgun* To_Vecgun( CBaseEntity *pEntity )
{
	if ( !pEntity )
		return NULL;
	Assert( dynamic_cast< C_WeaponVecgun* >( pEntity ) != NULL );
	return static_cast< C_WeaponVecgun* >( pEntity );
}