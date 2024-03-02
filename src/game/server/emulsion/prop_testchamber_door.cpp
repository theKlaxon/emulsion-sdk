#include "cbase.h"
#include "baseanimating.h"
#include "props.h"
#include "func_areaportalwindow.h"
#include "physics_bone_follower.h"

#define TESTCHAMBER_DOOR_AREA_PORTAL_NEVER_FADE_DISTANCE 10000.0f

class CPropTestChamberDoor : public CDynamicProp {
	DECLARE_CLASS(CPropTestChamberDoor, CDynamicProp)
	DECLARE_DATADESC()
public:

	void Precache();
	void Spawn();

	void Think();

	void DoorOpen(inputdata_t& input);
	void DoorClose(inputdata_t& input);
	void DoorLock(inputdata_t& input);
	void DoorLockOpen(inputdata_t& input);
	void DoorUnLock(inputdata_t& input);

private:

	void AreaPortalOpen(void);
	void AreaPortalClose(void);
	void CreateBoneFollowers(void);

	int m_seqOpen, m_seqClose, m_seqIdleOpen, m_seqIdleClose;

	COutputEvent m_outDoorOpened;
	COutputEvent m_outDoorClosed;
	COutputEvent m_outDoorFullyOpened;
	COutputEvent m_outDoorFullyClosed;
	
	CHandle<CFuncAreaPortalWindow> m_hAreaPortalWindow;

	CBoneFollowerManager	m_BoneFollowerManager;

	string_t m_strAreaPortalWindowName;

	bool m_bUseAreaPortalFade;
	bool m_bChangingState;
	bool m_bOpened;

	float m_flAreaPortalFadeStartDistance;
	float m_flAreaPortalFadeEndDistance;

};

LINK_ENTITY_TO_CLASS(prop_testchamber_door, CPropTestChamberDoor)

BEGIN_DATADESC(CPropTestChamberDoor)

// decided to match p2's exact in / out to make sure all p2 maps work -Klax
DEFINE_INPUTFUNC(FIELD_VOID, "Open", DoorOpen),
DEFINE_INPUTFUNC(FIELD_VOID, "Close", DoorClose),
DEFINE_INPUTFUNC(FIELD_VOID, "Lock", DoorLock),
DEFINE_INPUTFUNC(FIELD_VOID, "LockOpen", DoorLockOpen),
DEFINE_INPUTFUNC(FIELD_VOID, "Unlock", DoorUnLock),

DEFINE_OUTPUT(m_outDoorOpened, "OnOpen"),
DEFINE_OUTPUT(m_outDoorClosed, "OnClose"),
DEFINE_OUTPUT(m_outDoorFullyClosed, "OnFullyClosed"),
DEFINE_OUTPUT(m_outDoorFullyOpened, "OnFullyOpen"),

DEFINE_FIELD(m_hAreaPortalWindow, FIELD_EHANDLE),

DEFINE_KEYFIELD(m_strAreaPortalWindowName, FIELD_STRING, "AreaPortalWindow"),
DEFINE_KEYFIELD(m_bUseAreaPortalFade, FIELD_BOOLEAN, "UseAreaPortalFade"),
DEFINE_KEYFIELD(m_flAreaPortalFadeStartDistance, FIELD_FLOAT, "AreaPortalFadeStart"),
DEFINE_KEYFIELD(m_flAreaPortalFadeEndDistance, FIELD_FLOAT, "AreaPortalFadeEnd"),

END_DATADESC()

void CPropTestChamberDoor::Precache() {
	BaseClass::Precache();

	PrecacheModel("models/props/portal_door_combined.mdl");

	PrecacheScriptSound("prop_portal_door.open");
	PrecacheScriptSound("prop_portal_door.close");

}

void CPropTestChamberDoor::Spawn() {
	Precache();
	BaseClass::Spawn();
	
	AddEffects(EF_NOSHADOW);
	AddEffects(EF_MARKED_FOR_FAST_REFLECTION);
	
	SetMoveType(MOVETYPE_NONE);
	SetSolid(SOLID_VPHYSICS);

	SetModel("models/props/portal_door_combined.mdl");
	CreateVPhysics();

	SetThink(&CPropTestChamberDoor::Think);
	SetNextThink(gpGlobals->curtime + 0.1f);

	m_seqOpen = LookupSequence("open");
	m_seqClose = LookupSequence("close");
	m_seqIdleOpen = LookupSequence("idleOpen");
	m_seqIdleClose = LookupSequence("idleClose");

	ResetSequence(m_seqOpen); // we need to start closed most likely
	SetPlaybackRate(0.0f);

	//If an area portal window name has been specified
	if (m_strAreaPortalWindowName != NULL_STRING)
	{
		CBaseEntity* pAreaPortalWindow = gEntList.FindEntityByName(NULL, m_strAreaPortalWindowName);
		if (pAreaPortalWindow)
		{
			m_hAreaPortalWindow = (CFuncAreaPortalWindow*)pAreaPortalWindow;

			AreaPortalClose();
		}
		else
		{
			DevWarning("Could not find area portal window named %s for test chamber door %s\n", m_strAreaPortalWindowName.ToCStr(), m_iClassname.ToCStr());
		}
	}
	
	SetFadeDistance(-1.0f, 0.0f);
	SetGlobalFadeScale(0.0f);
}

void CPropTestChamberDoor::Think() {

	StudioFrameAdvance();
	DispatchAnimEvents(this);

	m_BoneFollowerManager.UpdateBoneFollowers(this);

	if(m_bChangingState)
		if (IsSequenceFinished()) {
			if (m_bOpened) {
				m_outDoorFullyOpened.FireOutput(this, this);
				SetSequence(m_seqIdleOpen);
			} else {
				m_outDoorFullyClosed.FireOutput(this, this);
				AreaPortalClose();

				SetSequence(m_seqIdleClose);
				ResetSequence(m_seqIdleClose);
				SetPlaybackRate(1.0f);
			}
			
			m_bChangingState = false;
		}
	
	SetNextThink(gpGlobals->curtime + 0.1f);

}

void CPropTestChamberDoor::DoorOpen(inputdata_t& input) {

	// setup things for chaning door state
	m_bOpened = true;
	m_bChangingState = true;

	SetPlaybackRate(1.0f);

	m_outDoorOpened.FireOutput(this, this);
	AreaPortalOpen();

	EmitSound("prop_portal_door.open");
}

void CPropTestChamberDoor::DoorClose(inputdata_t& input) {
	
	SetSequence(m_seqIdleOpen);
	ResetSequence(m_seqIdleOpen);

	SetSequence(m_seqOpen);
	SetPlaybackRate(-1.0f);

	m_bOpened = false;
	m_bChangingState = true;

	m_outDoorClosed.FireOutput(this, this);

	EmitSound("prop_portal_door.close");
}

void CPropTestChamberDoor::DoorLock(inputdata_t& input) {

}

void CPropTestChamberDoor::DoorLockOpen(inputdata_t& input) {

}

void CPropTestChamberDoor::DoorUnLock(inputdata_t& input) {
	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropTestChamberDoor::CreateBoneFollowers(void)
{
	// already created bone followers?  Don't do so again.
	if (m_BoneFollowerManager.GetNumBoneFollowers())
		return;

	KeyValues* modelKeyValues = new KeyValues("");
	if (modelKeyValues->LoadFromBuffer(modelinfo->GetModelName(GetModel()), modelinfo->GetModelKeyValueText(GetModel())))
	{
		// Do we have a bone follower section?
		KeyValues* pkvBoneFollowers = modelKeyValues->FindKey("bone_followers");
		if (pkvBoneFollowers)
		{
			// Loop through the list and create the bone followers
			KeyValues* pBone = pkvBoneFollowers->GetFirstSubKey();
			while (pBone)
			{
				// Add it to the list
				const char* pBoneName = pBone->GetString();
				m_BoneFollowerManager.AddBoneFollower(this, pBoneName);

				pBone = pBone->GetNextKey();
			}
		}

		modelKeyValues->deleteThis();
	}

	// if we got here, we don't have a bone follower section, but if we have a ragdoll
	// go ahead and create default bone followers for it
	if (m_BoneFollowerManager.GetNumBoneFollowers() == 0)
	{
		vcollide_t* pCollide = modelinfo->GetVCollide(GetModelIndex());
		if (pCollide && pCollide->solidCount > 1)
		{
			CreateBoneFollowersFromRagdoll(this, &m_BoneFollowerManager, pCollide);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Open the area portal window associated with this door
//-----------------------------------------------------------------------------
void CPropTestChamberDoor::AreaPortalOpen(void)
{
	if (m_hAreaPortalWindow)
	{
		float flFadeStart = TESTCHAMBER_DOOR_AREA_PORTAL_NEVER_FADE_DISTANCE;
		float flFadeEnd = TESTCHAMBER_DOOR_AREA_PORTAL_NEVER_FADE_DISTANCE;

		if (m_bUseAreaPortalFade)
		{
			flFadeStart = m_flAreaPortalFadeStartDistance;
			flFadeEnd = m_flAreaPortalFadeEndDistance;
		}

		m_hAreaPortalWindow->m_flFadeStartDist = flFadeStart;
		m_hAreaPortalWindow->m_flFadeDist = flFadeEnd;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Close the area portal window associated with this door
//-----------------------------------------------------------------------------
void CPropTestChamberDoor::AreaPortalClose(void)
{
	if (m_hAreaPortalWindow)
	{
		m_hAreaPortalWindow->m_flFadeStartDist = 0;
		m_hAreaPortalWindow->m_flFadeDist = 0;
	}
}
