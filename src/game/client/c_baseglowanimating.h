//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#ifndef C_BASEGLOWANIMATING_H
#define C_BASEGLOWANIMATING_H

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "c_baseanimating.h"
#include "glow_outline_effect.h"

//-----------------------------------------------------------------------------
// Purpose: Base
//-----------------------------------------------------------------------------
class C_BaseGlowAnimating : public C_BaseAnimating
{
	DECLARE_CLASS( C_BaseGlowAnimating, C_BaseAnimating );
public:

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	C_BaseGlowAnimating( void );
	~C_BaseGlowAnimating();

	virtual void	OnPreDataChanged( DataUpdateType_t updateType );
	virtual void	OnDataChanged( DataUpdateType_t updateType );

	CGlowObject			*GetGlowObject( void ){ return m_pGlowEffect; }
	virtual void		GetGlowEffectColor( float *r, float *g, float *b );

protected:

	virtual void		UpdateGlowEffect( void );
	virtual void		DestroyGlowEffect( void );

private:

	bool				m_bGlowEnabled;
	bool				m_bOldGlowEnabled;
	//bool				m_bRenderWhenOccluded;
	//bool				m_bRenderWhenUnOccluded;
	CGlowObject			*m_pGlowEffect;
	
	float m_flRedGlowColor;
	float m_flGreenGlowColor;
	float m_flBlueGlowColor;
	float m_flAlphaGlowColor;

	CNetworkColor32(m_clrGlow);
};

#endif //C_BASEGLOWANIMATING_H