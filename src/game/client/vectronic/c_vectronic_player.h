//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#include "cbase.h"
//#include "c_basesdkcombatweapon.h"
#include "colorcorrectionmgr.h"

class C_VectronicPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_VectronicPlayer, C_BasePlayer );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	void OnDataChanged( DataUpdateType_t type );
	static C_VectronicPlayer* GetLocalPlayer( int nSlot = -1 );

	C_VectronicPlayer();
	~C_VectronicPlayer( void );
	void ClientThink( void );

//	C_BaseSDKCombatWeapon*	GetActiveSDKWeapon( void ) const;

	virtual bool ShouldRegenerateOriginFromCellBits() const
	{
		// C_BasePlayer assumes that we are networking a high-res origin value instead of using a cell
		// (and so returns false here), but this is not by default the case.
		return true; // TODO: send high-precision origin instead?
	}

	const QAngle &GetRenderAngles();

	void UpdateClientSideAnimation();
	virtual	void		BuildTransformations( CStudioHdr *hdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed );

	// Input handling
	virtual bool	CreateMove( float flInputSampleTime, CUserCmd *pCmd );
	void			PerformClientSideObstacleAvoidance( float flFrameTime, CUserCmd *pCmd );
	void			PerformClientSideNPCSpeedModifiers( float flFrameTime, CUserCmd *pCmd );

public:
	void FireBullet( 
		Vector vecSrc, 
		const QAngle &shootAngles, 
		float vecSpread, 
		int iDamage, 
		int iBulletType,
		CBaseEntity *pevAttacker,
		bool bDoEffects,
		float x,
		float y );

	// Are we looking over a useable object?
//	bool IsCursorOverUseable() { return m_bMouseOverUseable; }
//	bool m_bMouseOverUseable;

	// Do we have an object?
	bool PlayerHasObject() { return m_bPlayerPickedUpObject; }
	bool m_bPlayerPickedUpObject;

	// Are we looking at an enemy?
//	bool IsCursorOverEnemy() { return m_bMouseOverEnemy; }
//	bool m_bMouseOverEnemy;

	EHANDLE				m_hClosestNPC;
	float				m_flSpeedModTime;
	float				m_flExitSpeedMod;

	CNetworkVar( int, m_iShotsFired );	// number of shots fired recently

private:
	bool	TestMove( const Vector &pos, float fVertDist, float radius, const Vector &objPos, const Vector &objDir );

	float	m_flSpeedMod;
	QAngle m_angRender;

	ClientCCHandle_t	m_CCDeathHandle;	// handle to death cc effect
	float				m_flDeathCCWeight;	// for fading in cc effect

};

inline C_VectronicPlayer* To_VectronicPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity )
		return NULL;
	Assert( dynamic_cast< C_VectronicPlayer* >( pEntity ) != NULL );
	return static_cast< C_VectronicPlayer* >( pEntity );
}