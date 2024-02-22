#pragma once
#include "basecombatweapon.h"

class CWeaponPaintgun : public CBaseCombatWeapon {
	DECLARE_CLASS(CWeaponPaintgun, CBaseCombatWeapon);
	DECLARE_NETWORKCLASS();
public:

	CWeaponPaintgun();

	void FirePaint(bool erase = false);
	void PrimaryAttack() { FirePaint(); }
	void SecondaryAttack() { FirePaint(true); }

	virtual	int	UpdateClientData(CBasePlayer* pPlayer);

	virtual void Equip(CBaseCombatCharacter* pOwner);
	virtual void Drop(const Vector& vecVelocity);

private:

	float m_flCurPaintDelay;
	CNetworkVar(int, m_nPaintType);

};
