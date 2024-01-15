#pragma once
#include "blob_batch.h"


#ifdef CLIENT_DLL
#include "c_baseentity.h"
#include "c_props.h"

class C_NetworkedBlobBatch : public C_BaseEntity {
	DECLARE_CLASS(C_NetworkedBlobBatch, C_BaseEntity)
	DECLARE_CLIENTCLASS()
public:


	
};

#else
#include "baseentity.h"
#include "props.h"

class CNetworkedBlobBatch : public CBaseEntity {
	DECLARE_CLASS(CNetworkedBlobBatch, CBaseEntity)
	DECLARE_NETWORKCLASS()
public:

	int UpdateTransmitState()	// always send to all clients
	{
		return SetTransmitState(FL_EDICT_ALWAYS);
	}

};


#endif