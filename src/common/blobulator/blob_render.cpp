#include "cbase.h"

#ifdef CLIENT_DLL // this file is really only for the client
#include "blob_render.h"
#include "blob_manager.h"
#include "implicit/imp_tiler.h"
#include "implicit/sweep_renderer.h"
#include "smartarray.h"

// defs for sweep renderer
bool calcSign(unsigned char, unsigned char, unsigned char, float, float, float, ProjectingParticleCache*);
void calcSign2(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);

void calcCornerNormal(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
void calcCornerNormalColorUVTan(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
void calcCornerNormalColor(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);

void calcVertexNormal(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
void calcVertexNormalDebugColor(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
void calcVertexNormalNColorUVTan(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);
void calcVertexNormalNColor(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);

void COpPtr_DoRender::Do(void* pData) {
	
	CMatRenderContextPtr pRenderContext(materials);
	CBlobManager* pParent = (CBlobManager*)m_pParent;
	static ImpTiler* pTiler = ImpTilerFactory::factory->getTiler();
	
	SweepRenderer::setCubeWidth(pParent->m_flCubeWidth);
	SweepRenderer::setCutoffR(pParent->m_flCutoffRadius);
	SweepRenderer::setRenderR(pParent->m_flRenderRadius);
	SweepRenderer::setCalcSignFunc(calcSign);
	SweepRenderer::setCalcSign2Func(calcSign2);
	SweepRenderer::setCalcCornerFunc(32, calcCornerNormal);
	SweepRenderer::setCalcVertexFunc(calcVertexNormal);
	
	modelrender->SetupLighting(Vector(0.0f, 0.0f, 0.0f));

	//IBlobBatch* pBatch = (IBlobBatch*)pData;
	IMaterial* pMat;// = pBatch->GetMaterial();
	pRenderContext->Bind(pMat);

	Point3D _offset = Point3D(0.0f, 0.0f, 0.0f);
	pTiler->beginFrame(_offset, (void*)&pRenderContext, true);

	//pBatch->InsertParticles(pTiler);

	pTiler->drawSurface();
	pTiler->endFrame();
	
	// release / return our pointers
	ImpTilerFactory::factory->returnTiler(pTiler);
	pRenderContext.SafeRelease();
}
#endif