#pragma once
#include "basecombatweapon.h"

class CWeaponPaintgun : public CBaseCombatWeapon {
	DECLARE_CLASS(CWeaponPaintgun, CBaseCombatWeapon);
public:

	CWeaponPaintgun();

	void FirePaint(bool erase = false);
	void PrimaryAttack() { FirePaint(); }
	void SecondaryAttack() { FirePaint(true); }

private:

	float m_flCurPaintDelay;

};
