#pragma once
#include "baseanimating.h"

// this is just an example, so if used in the furture then this code should only 
// be use as a base class. ex. ents like prop_weighted_cube would be placeable props
class CPropPlaceable : public CBaseAnimating {
	DECLARE_CLASS(CPropPlaceable, CBaseAnimating)
public:

	void Spawn();
	void Precache();

	bool SuggestOrigin(Vector origin, cplane_t plane);
	bool IsPlaceable() { return m_bIsPlaceable; }
	void SetGhost(bool isGhost);

	//virtual bool TestCollision(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr) override;

private:

	bool m_bIsGhost; // controls wheteher or not this ent is in placement mode
	bool m_bIsPlaceable;
	float m_flNormalGravity;

};