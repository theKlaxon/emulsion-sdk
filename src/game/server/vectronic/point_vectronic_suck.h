//=========== Copyright © 2015, rHetorical, All rights reserved. =============
//
// Purpose:
//		
//=============================================================================

#ifndef VECTRONIC_SUCK_H
#define VECTRONIC_SUCK_H

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"

// =============================================================
//  point_vectronic_suck
// =============================================================

class CPointSuck : public CPointEntity
{
public:
	DECLARE_CLASS(CPointSuck, CPointEntity);

	virtual void Activate(void);
	void SuckThink(void);

	void	InputEnable(inputdata_t &inputdata);
	void	InputDisable(inputdata_t &inputdata);
	void	InputSetMagitude(inputdata_t &inputData);
	void	InputSetRadius(inputdata_t &inputData);

	virtual void DrawDebugGeometryOverlays(void);

	DECLARE_DATADESC();

	void Enable(bool b)
	{
		m_bEnabled = b;
	};

	bool IsEnabled()
	{
		return m_bEnabled;
	};

	void SetMagitude(float f)
	{
		m_flMagnitude = f;
	};

	void SetRadius(float f)
	{
		m_flRadius = f;
	};

	void SetInnerRadius(float f)
	{
		m_flInnerRadius = f;
	};

	void SetConeOfInfluence(float f)
	{
		m_flConeOfInfluence = f;
	};

private:
	inline void SuckEntity(CBaseEntity *pTarget, const Vector &vecPushPoint);

	bool	m_bEnabled;
	bool	m_bFoundPlayer;
	int		m_intBallType;

	float	m_flMagnitude;
	float	m_flRadius;
	float	m_flInnerRadius;		// Inner radius where the push eminates from (on a sphere)
	float	m_flConeOfInfluence;	// Cone (in degrees) in which an entity must lie to be affected
};

#endif // VECTRONIC_SUCK_H