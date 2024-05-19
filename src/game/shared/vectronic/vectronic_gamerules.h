//====== Copyright © 1996-2003, Valve Corporation, All rights reserved. =======
//
// Purpose: Game rules for Scratch
//
//=============================================================================

#ifndef VECTRONIC_GAMERULES_H
#define VECTRONIC_GAMERULES_H
#ifdef _WIN32
#pragma once
#endif

#include "gamerules.h"
#include "singleplay_gamerules.h"
#include "teamplayroundbased_gamerules.h"

#ifdef CLIENT_DLL
#include "c_prop_vectronic_projectile.h"
#else
#include "prop_vectronic_projectile.h"
#endif

#ifdef CLIENT_DLL
	#define CVectronicGameRules C_VectronicGameRules
	#define CVectronicGameRulesProxy C_VectronicGameRulesProxy
#endif

extern ConVar sv_gravity;

class CVectronicGameRulesProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS( CVectronicGameRulesProxy, CGameRulesProxy );
	//DECLARE_NETWORKCLASS();
#ifdef CLIENT_DLL
	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else
	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.
	//DECLARE_DATADESC();
	bool KeyValue( const char *szKeyName, const char *szValue );

private:

	int m_ball3_r;
	int m_ball3_g;
	int m_ball3_b;

	int m_ball4_r;
	int m_ball4_g;
	int m_ball4_b;

	int m_ball5_r;
	int m_ball5_g;
	int m_ball5_b;

	bool m_bCustomGun;

	//void	InputSetRedTeamRespawnWaveTime( inputdata_t &inputdata );
	//void	InputSetBlueTeamRespawnWaveTime( inputdata_t &inputdata );
	//void	InputAddRedTeamRespawnWaveTime( inputdata_t &inputdata );
	//void	InputAddBlueTeamRespawnWaveTime( inputdata_t &inputdata );
	//void	InputSetRedTeamGoalString( inputdata_t &inputdata );
	//void	InputSetBlueTeamGoalString( inputdata_t &inputdata );
	//void	InputSetRedTeamRole( inputdata_t &inputdata );
	//void	InputSetBlueTeamRole( inputdata_t &inputdata );

	//virtual void Activate();
#endif
};

class CVectronicGameRules : public CSingleplayRules
{
	DECLARE_CLASS( CVectronicGameRules, CSingleplayRules );
public:

	CVectronicGameRules();
	~CVectronicGameRules() {};

	bool				IsMultiplayer( void );
	void				PlayerThink( CBasePlayer *pPlayer );

	void Activate();

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.


#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.

	virtual const char *GetGameDescription( void ) { return "SDK"; }

//	virtual void PlayerThink( CBasePlayer *pPlayer ) {}

//	virtual void PlayerSpawn( CBasePlayer *pPlayer );

	// Spawing rules.
	CBaseEntity *GetPlayerSpawnSpot( CBasePlayer *pPlayer );
	bool IsSpawnPointValid( CBaseEntity *pSpot, CBasePlayer *pPlayer, bool bIgnorePlayers );

	void				SetBall3Color( int r, int g, int b );
	void				SetBall4Color( int r, int g, int b );
	void				SetBall5Color( int r, int g, int b );
	void				SetVecGunSelection( int value );

	virtual bool		ShouldCollide( int collisionGroup0, int collisionGroup1 );

	virtual void			InitDefaultAIRelationships( void );
	virtual const char*	AIClassText(int classType);

	// Respawning
	int m_bRespawnPlayer;
	bool IsRespawningEnabled()
	{
		bool b;

		if (m_bRespawnPlayer == 1)
		{
			b = true;
		}
		else
		{
			b = false;
		}

		return b;
	};

	void ToggleRespawning(int b);

	float m_flBallLife;
	void SetBallLife( float f)
	{
		m_flBallLife = f;
	};

	float GetBallLife()
	{
		return m_flBallLife;
	};

	float m_flBallSpeed;
	void SetBallSpeed(float f)
	{
		m_flBallSpeed = f;
	};

	float GetBallSpeed()
	{
		return m_flBallSpeed;
	};



#endif // CLIENT_DLL

	void				SetBall3Sprite(const char *szTexture);
	void				SetBall4Sprite(const char *szTexture);
	void				SetBall5Sprite(const char *szTexture);

	// misc
//	virtual void CreateStandardEntities( void );	
};

//-----------------------------------------------------------------------------
// Gets us at the SDK game rules
//-----------------------------------------------------------------------------
inline CVectronicGameRules* rHGameRules()
{
	return static_cast<CVectronicGameRules*>(g_pGameRules);
}

#endif // rh_GAMERULES_H