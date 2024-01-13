#include "cbase.h"
#include "blob_batch.h"

CBucketBlobBatch::CBucketBlobBatch() {
	saBuckets.pushAutoSize(BlobBucket_t());
	m_nParticleCount = 0;
	m_nCurBucket = 0;
	m_nCurBucketInnerIdx = 0;
}

CBucketBlobBatch::~CBucketBlobBatch() {
	Cleanup();
}

void CBucketBlobBatch::AddParticle(BlobData_t blob) {
	
	if (m_nCurBucket >= saBuckets.size)
		saBuckets.pushAutoSize(BlobBucket_t());

	saBuckets[m_nCurBucket].m_Blobs.AddToTail(blob);
	saParticles.pushAutoSize(ImpParticle());
	saBuckets[m_nCurBucket].m_nBlobCount++;
	m_nParticleCount++;
	m_nCurBucketInnerIdx++;
	
	if (m_nCurBucketInnerIdx >= BLOB_BUCKET_SIZE) {
		m_nCurBucketInnerIdx = 0;
		m_nCurBucket++;
	}
}

void CBucketBlobBatch::RemoveParticle(BlobBatchIndex_t index) {

}

void CBucketBlobBatch::InsertParticles(ImpTiler* pTiler) {

	CMatRenderContextPtr pRenderContext(materials);
	IMaterial* pMat = GetMaterial();
	pRenderContext->Bind(pMat);
	
	for (int i = 0, sa = 0; i < saBuckets.size; i++) {
		
		for (int k = 0; k < saBuckets[i].m_nBlobCount; k++) {
			BlobData_t* data = &saBuckets[i].m_Blobs[k];
			ImpParticle* pParticle = &saParticles[sa];
			
			pParticle->center[0] = data->m_Center[0];
			pParticle->center[1] = data->m_Center[1];
			pParticle->center[2] = data->m_Center[2];
			pParticle->scale = data->m_flScale != 1.0f ? data->m_flScale - 1.0f : 1.0f;
			pParticle->fieldRScaleSq = data->m_flScale;

			pTiler->insertParticle(pParticle);
			sa++;
		}
	}
}

void CBucketBlobBatch::Cleanup() {

	while (saParticles.size > 0)
		saParticles.pop();

	for (int i = 0; i < saBuckets.size; i++) {
		saBuckets[i].m_Blobs.Purge();
		saBuckets[i].m_nBlobCount = 0;
	}

	while (saBuckets.size > 0)
		saBuckets.pop();

	m_nCurBucket = 0;
	m_nCurBucketInnerIdx = 0;
	m_nParticleCount = 0;
}