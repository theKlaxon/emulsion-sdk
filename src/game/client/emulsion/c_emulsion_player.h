#include "cbase.h"

class C_EmulsionPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS(C_EmulsionPlayer, C_BasePlayer );
	DECLARE_NETWORKCLASS();
	
	C_EmulsionPlayer();

	void CalcPlayerView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov);
	virtual bool ShouldRegenerateOriginFromCellBits() const
	{
		// C_BasePlayer assumes that we are networking a high-res origin value instead of using a cell
		// (and so returns false here), but this is not by default the case.
		//return true; // TODO: send high-precision origin instead?

		// NOTE: ignore the above. some smart people said this should be false for singleplayer games
		return true;
	}

	CBaseEntity* GetStickParent();

	friend class C_WeaponPaintgun;
protected:

	CBaseHandle* m_hStickParent;

	// stick lerp
	Vector m_vecCurLerpUp;
	Vector m_vecGravity;
	int m_nPaintPower;
};