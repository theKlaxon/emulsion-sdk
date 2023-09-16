#pragma once
#include "baseentity.h"

class CFuncPortalBumper : public CBaseEntity
{
public:
	DECLARE_CLASS(CFuncPortalBumper, CBaseEntity);

	CFuncPortalBumper();

	// Overloads from base entity
	virtual void	Spawn(void);

	// Inputs to flip functionality on and off
	void InputActivate(inputdata_t& inputdata);
	void InputDeactivate(inputdata_t& inputdata);
	void InputToggle(inputdata_t& inputdata);

	// misc public methods
	bool IsActive() { return m_bActive; }	// is this area currently bumping portals

	DECLARE_DATADESC();

private:
	bool					m_bActive;			// are we currently blocking portals


};