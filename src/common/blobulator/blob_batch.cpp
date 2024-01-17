#include "cbase.h"
#include "blob_batch.h"

CBucketBlobBatch::CBucketBlobBatch() {
	saBuckets.AddToTail(BlobBucket_t());
	saBuckets[0] = BlobBucket_t();
	//m_nBukcetCnt = 1;
	m_nParticleCount = 0;
	m_nCurBucket = 0;
	m_nCurBucketInnerIdx = 0;
}

CBucketBlobBatch::~CBucketBlobBatch() {
	Cleanup();
}

void CBucketBlobBatch::AddParticle(BlobData_t blob) {
	
	saBuckets[m_nCurBucket].m_Blobs.AddToTail(blob);// [m_nCurBucketInnerIdx] = blob;
	saBuckets[m_nCurBucket].m_nBlobCount++;
	saParticles.AddToTail(ImpParticle());
	
	m_nParticleCount++;
	m_nCurBucketInnerIdx++;
	
	if (m_nCurBucketInnerIdx >= BLOB_BUCKET_SIZE) {
		m_nCurBucketInnerIdx = 0;
		m_nCurBucket++;
	}
	
	if (m_nCurBucket >= saBuckets.Count())
		saBuckets.AddToTail(BlobBucket_t());
}

void CBucketBlobBatch::RemoveParticle(BlobBatchIndex_t index) {

}

void CBucketBlobBatch::InsertParticles(ImpTiler* pTiler) {

	CMatRenderContextPtr pRenderContext(materials);
	IMaterial* pMat = GetMaterial();
	pRenderContext->Bind(pMat);
	
	for (int i = 0, sa = 0; i < saBuckets.Count(); i++) {
		
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

	saParticles.Purge();

	for (int i = 0; i < saBuckets.Count(); i++) {
		saBuckets[i].m_Blobs.Purge();
		saBuckets[i].m_nBlobCount = 0;
	}
	
	saBuckets.Purge();

	m_nCurBucket = 0;
	m_nCurBucketInnerIdx = 0;
	m_nParticleCount = 0;
}