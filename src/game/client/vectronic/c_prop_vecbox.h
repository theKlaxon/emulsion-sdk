//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//		
//=============================================================================

#ifndef C_PROP_VECBOX_H
#define C_PROP_VECBOX_H

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "c_baseanimating.h"

#ifdef GLOWS_ENABLE
#include "glow_outline_effect.h"
#endif // GLOWS_ENABLE

//-----------------------------------------------------------------------------
// Purpose: Base
//-----------------------------------------------------------------------------
class C_VecBox : public C_BaseAnimating
{
	DECLARE_CLASS( C_VecBox, C_BaseAnimating );
public:

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	C_VecBox( void );
	~C_VecBox();

	virtual void	OnPreDataChanged( DataUpdateType_t updateType );
	virtual void	OnDataChanged( DataUpdateType_t updateType );

#ifdef GLOWS_ENABLE
	CGlowObject			*GetGlowObject( void ){ return m_pGlowEffect; }
	virtual void		GetGlowEffectColor( float *r, float *g, float *b );
#endif // GLOWS_ENABLE

protected:

#ifdef GLOWS_ENABLE	
	virtual void		UpdateGlowEffect( void );
	virtual void		DestroyGlowEffect( void );
#endif // GLOWS_ENABLE

private:

#ifdef GLOWS_ENABLE
	bool				m_bGlowEnabled;
	bool				m_bOldGlowEnabled;
	CGlowObject			*m_pGlowEffect;
	
	float m_flRedGlowColor;
	float m_flGreenGlowColor;
	float m_flBlueGlowColor;

#endif // GLOWS_ENABLE

};
#endif // C_PROP_VECBOX_H
