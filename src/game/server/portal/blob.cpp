#include "cbase.h"
#include "particle_parse.h"
#include "props.h"

static void CreateBlob() {
	CreateEntityByName("ent_blob"); // create an object of this entity class
}

static ConCommand blob_command("blob", CreateBlob, "Create a blob.");

class CBlob : public CBaseEntity {
public:
	DECLARE_CLASS(CBlob, CBaseEntity);
	DECLARE_SERVERCLASS();

	CBlob();

	void Precache();
	void Spawn();

	virtual bool ShouldDraw() { return true; }

protected:

};

LINK_ENTITY_TO_CLASS(ent_blob, CBlob)

IMPLEMENT_SERVERCLASS_ST(CBlob, DT_Blob)
END_SEND_TABLE()

CBlob::CBlob() {

}

void CBlob::Precache() {
	//PrecacheModel("models/props_shapes/sphere.mdl");
	//PrecacheModel("models/cone.mdl");
	//PrecacheParticleSystem("blob_base");
	BaseClass::Precache();
}

void CBlob::Spawn() {
	//SetModel("models/props_shapes/sphere.mdl");
	//SetModel("models/cone.mdl");
	
	BaseClass::Spawn();

	//DispatchParticleEffect("blob_base", GetAbsOrigin(), GetAbsAngles(), this);
}