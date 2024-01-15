#include "cbase.h"
#include "blob_batch.h"

CBucketBlobBatch::CBucketBlobBatch() {
	saBuckets.AddToTail(BlobBucket_t());
	//saBuckets[0] = BlobBucket_t();
	//m_nBukcetCnt = 1;
	m_nParticleCount = 0;
	m_nCurBucket = 0;
	m_nCurBucketInnerIdx = 0;
}

CBucketBlobBatch::~CBucketBlobBatch() {
	Cleanup();
}

void CBucketBlobBatch::AddParticle(BlobData_t blob) {
	
	//saBuckets[m_nCurBucket].m_Blobs[m_nCurBucketInnerIdx] = blob; // causes assert i < m_Size;
	
	
	saBuckets[m_nCurBucket].m_Blobs[m_nCurBucketInnerIdx] = blob;
	//saBuckets[m_nCurBucket].m_Blobs.SetCount(saBuckets[m_nCurBucket].m_nBlobCount + 1);
	saBuckets[m_nCurBucket].m_nBlobCount++;
	
	//saBuckets[m_nCurBucket].m_nBlobCount++;
	//saBuckets[m_nCurBucket].m_Blobs.SetSize(saBuckets[m_nCurBucket].m_nBlobCount);
	//saBuckets[m_nCurBucket].m_Blobs[m_nCurBucketInnerIdx] = blob;
	saParticles.AddToTail(ImpParticle());
	
	m_nParticleCount++;
	m_nCurBucketInnerIdx++;
	
	if (m_nCurBucketInnerIdx >= BLOB_BUCKET_SIZE) {
		m_nCurBucketInnerIdx = 0;
		m_nCurBucket++;
	}

	//if (m_nCurBucket >= m_nCurBucket) {
	//	saBuckets[m_nCurBucket + 1] = BlobBucket_t();
	//}

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

	//while (saParticles.Count() > 0)
	//	saParticles.pop();
	saParticles.Purge();

	for (int i = 0; i < saBuckets.Count(); i++) {
		saBuckets[i].m_Blobs.Purge();
		saBuckets[i].m_nBlobCount = 0;
	}

	//while (saBuckets.size > 0)
	//	saBuckets.pop();
	saBuckets.Purge();

	m_nCurBucket = 0;
	m_nCurBucketInnerIdx = 0;
	m_nParticleCount = 0;
}