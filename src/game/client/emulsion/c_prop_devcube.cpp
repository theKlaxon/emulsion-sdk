#include "cbase.h"
#include "c_props.h"
#include "c_breakableprop.h"
#include "c_baseanimating.h"

ConVar sv_portal2_pickup_hint_range("sv_portal2_pickup_hint_range", "350.0f", FCVAR_CHEAT | FCVAR_REPLICATED);

class C_PropDevCube : public C_BaseAnimating {
	DECLARE_CLASS(C_PropDevCube, C_BaseAnimating)
	DECLARE_CLIENTCLASS()
public:

};

LINK_ENTITY_TO_CLASS(prop_weighted_cube, C_PropDevCube)

IMPLEMENT_CLIENTCLASS_DT(C_PropDevCube, DT_PropDevCube, CPropDevCube)
END_RECV_TABLE()