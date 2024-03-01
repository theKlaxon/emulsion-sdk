#include "cbase.h"
#include "paintblob_manager.h"

class CInfoPaintSprayer : public CPointEntity {
public:

	void Spawn();
	void Think();

private:

	int m_nBlobsPerSec;

};