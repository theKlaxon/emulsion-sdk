#include "cbase.h"
#include "blob_networked_batch.h"

#ifdef CLIENT_DLL
IMPLEMENT_CLIENTCLASS_DT(C_NetworkedBlobBatch, DT_NetworkedBlobBatch, CNetworkedBlobBatch)
END_RECV_TABLE()

#else

IMPLEMENT_SERVERCLASS_ST(CNetworkedBlobBatch, DT_NetworkedBlobBatch)
END_SEND_TABLE()

#endif