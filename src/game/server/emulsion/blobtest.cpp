#include "cbase.h"
#include "baseanimating.h"

class CBlobTest : public CBaseAnimating {
	DECLARE_CLASS(CBlobTest, CBaseAnimating);
	DECLARE_NETWORKCLASS();
public:

	CBlobTest();

	void Precache();
	void Spawn();

	virtual bool ShouldDraw() { return true; }

protected:

	CNetworkVar(float, m_flCubeWidth);
	CNetworkVar(float, m_flRenderRadius);
	CNetworkVar(float, m_flCutoffRadius);

};

LINK_ENTITY_TO_CLASS(prop_blobtest, CBlobTest);

IMPLEMENT_SERVERCLASS_ST(CBlobTest, DT_BlobTest)
	SendPropFloat(SENDINFO(m_flCubeWidth), -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT),
	SendPropFloat(SENDINFO(m_flRenderRadius), -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT),
	SendPropFloat(SENDINFO(m_flCutoffRadius), -1, SPROP_NOSCALE, 0.0f, HIGH_DEFAULT)
END_SEND_TABLE()

CBlobTest::CBlobTest() {
	m_flCubeWidth = 1.0f;
	m_flRenderRadius = 1.3f;
	m_flCutoffRadius = 5.5f;
}

void CBlobTest::Precache() {

	PrecacheModel("models/props/metal_box.mdl");
	BaseClass::Spawn();
}

void CBlobTest::Spawn() {

	Precache();
	SetModel("models/props/metal_box.mdl");

	BaseClass::Spawn();
}

void CreateTestBlob() {
	CBaseEntity* pEnt = CreateEntityByName("prop_blobtest");
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	pEnt->SetAbsOrigin(pPlayer->GetAbsOrigin() + (pPlayer->Forward() * 5));
	pEnt->Spawn();
}
static ConCommand create_blob("create_blob", CreateTestBlob);