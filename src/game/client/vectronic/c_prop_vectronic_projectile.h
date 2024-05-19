//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef CPROPPARTILCEPROJECTILE_H_
#define CPROPPARTILCEPROJECTILE_H_

#ifdef _WIN32
#pragma once
#endif

class C_PropParticleBall : public C_BaseAnimating
{
	DECLARE_CLASS( C_PropParticleBall, C_BaseAnimating );
	DECLARE_CLIENTCLASS();
public:

	C_PropParticleBall( void );

	virtual RenderGroup_t GetRenderGroup( void );

	virtual void	OnDataChanged( DataUpdateType_t updateType );

#ifndef SWARM_DLL
	virtual int		DrawModel( int flags );
#else
	virtual int		DrawModel( int flags, const RenderableInstance_t &instance );
#endif

protected:

	void	DrawMotionBlur( void );
	void	DrawFlicker( void );
	virtual bool	InitMaterials( void );

	Vector	m_vecLastOrigin;
	bool	m_bEmit;
	float	m_flRadius;
	bool	m_bHeld;
	bool	m_bLaunched;
	int		m_intType;

	IMaterial	*m_pFlickerMaterial;
	IMaterial	*m_pBodyMaterial;
	IMaterial	*m_pBlurMaterial;
};


#endif