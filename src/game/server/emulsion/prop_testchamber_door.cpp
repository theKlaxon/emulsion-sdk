#include "cbase.h"
#include "baseanimating.h"
#include "props.h"

class CPropTestChamberDoor : public CDynamicProp {
	DECLARE_CLASS(CPropTestChamberDoor, CDynamicProp)
	DECLARE_DATADESC()
public:

	void Precache();
	void Spawn();

	void DoorOpen(inputdata_t& input);
	void DoorClose(inputdata_t& input);
	void DoorLock(inputdata_t& input);
	void DoorLockOpen(inputdata_t& input);
	void DoorUnLock(inputdata_t& input);

private:

	int m_seqOpen, m_seqClose, m_seqIdleOpen, m_seqIdleClose;

	COutputEvent m_outDoorOpened;
	COutputEvent m_outDoorClosed;
	COutputEvent m_outDoorFullyOpened;
	COutputEvent m_outDoorFullyClosed;

};

//LINK_ENTITY_TO_CLASS(prop_testchamber_door, CPropTestChamberDoor)

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
	
	SetModel("models/props/portal_door_combined.mdl");
	SetMoveType(MOVETYPE_NONE);
	SetSolid(SOLID_VPHYSICS);
	CreateVPhysics();

	m_seqOpen = LookupSequence("open");
	m_seqClose = LookupSequence("close");
	m_seqIdleOpen = LookupSequence("idleOpen");
	m_seqIdleClose = LookupSequence("idleClose");

	ResetSequence(m_seqIdleClose); // we need to start closed most likely

}

void CPropTestChamberDoor::DoorOpen(inputdata_t& input) {

}

void CPropTestChamberDoor::DoorClose(inputdata_t& input) {

}

void CPropTestChamberDoor::DoorLock(inputdata_t& input) {

}

void CPropTestChamberDoor::DoorLockOpen(inputdata_t& input) {

}

void CPropTestChamberDoor::DoorUnLock(inputdata_t& input) {

}

