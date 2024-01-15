#pragma once
#include "materialsystem/imaterial.h"
#include "utlvector.h"
#include "smartarray.h"
#include "point_3D.h"
#include "implicit/imp_particle.h"
#include "implicit/imp_tiler.h"
#include <utlvector.h>

class CVectorBlobBatch;
class CBucketBlobBatch;

// change this def to change batch behaviours
typedef CBucketBlobBatch CBaseBlobBatch;

#define BLOB_BUCKET_SIZE 8

class BlobData_t {
public:
	BlobData_t() {}
	Vector m_Center;
	float m_flScale;
};

// (*, -1) == vector index,
// (*, *) == bucket index
struct BlobBatchIndex_t {
	BlobBatchIndex_t(short _x) { x = _x; y = -1; }				// vector index constructor
	BlobBatchIndex_t(short _x, short _y) { x = _x, y = _y; }	// bucket index constructor
	short x, y;
};

struct BlobBucket_t {
	BlobBucket_t() { m_nBlobCount = 0; /*m_Blobs = CUtlVectorFixed<BlobData_t, BLOB_BUCKET_SIZE>(); m_Blobs.EnsureCount(BLOB_BUCKET_SIZE); */ }
	CUtlVectorFixed<BlobData_t, BLOB_BUCKET_SIZE> m_Blobs;
	int m_nBlobCount;
};

class IBlobBatch {
public:

	virtual void SetMaterial(const char* material) { m_strMatName = material; m_pMaterial = materials->FindMaterial(material, TEXTURE_GROUP_OTHER); }
	virtual IMaterial* GetMaterial() { return m_pMaterial; }
	
	virtual void AddParticle(BlobData_t blob) = 0;
	virtual void RemoveParticle(BlobBatchIndex_t index) = 0;
	virtual void InsertParticles(ImpTiler* pTiler) = 0;
	virtual void Cleanup() = 0;

	virtual int GetParticleCount() = 0;

protected:

	const char* m_strMatName;
	IMaterial* m_pMaterial;
};

class CVectorBlobBatch : public IBlobBatch {
public:

	void AddParticle(BlobData_t blob) {}
	void RemoveParticle(BlobBatchIndex_t index) {}
	void InsertParticles(ImpTiler* pTiler) {}
	void Cleanup() {}

	int GetParticleCount() { return 0; }

protected:

	CUtlVector<BlobData_t> m_Blobs;
};

class CBucketBlobBatch : public IBlobBatch {
public:

	CBucketBlobBatch();
	~CBucketBlobBatch();

	void AddParticle(BlobData_t blob);
	void RemoveParticle(BlobBatchIndex_t index);
	void InsertParticles(ImpTiler* pTiler);
	void Cleanup();

	int GetParticleCount() { return m_nParticleCount; }

protected:
	
	// strictly for allocation and sizing
	int m_nCurBucket;
	int m_nCurBucketInnerIdx;
	int m_nParticleCount;
	//int m_nBukcetCnt;

	CUtlVector<BlobBucket_t> saBuckets;
	CUtlVector<ImpParticle> saParticles;
};