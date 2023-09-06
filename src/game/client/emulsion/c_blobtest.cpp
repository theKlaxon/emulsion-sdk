#include "cbase.h"
#include "c_baseanimating.h"
#include "..\common\blobulator\iblob_renderer.h"

#define MAX_SURFACE_ELEMENTS 400

class C_BlobTest : public C_BaseAnimating {
	DECLARE_CLASS(C_BlobTest, C_BaseAnimating);
	DECLARE_NETWORKCLASS();
public:
	
	C_BlobTest();

	virtual int DrawModel(int flags, const RenderableInstance_t& instance) override;
	virtual bool ShouldDraw() override { return true; }

protected:

	CNetworkVar(float, m_flCubeWidth);
	CNetworkVar(float, m_flRenderRadius);
	CNetworkVar(float, m_flCutoffRadius);

};

LINK_ENTITY_TO_CLASS(prop_blobtest, C_BlobTest);

IMPLEMENT_CLIENTCLASS_DT(C_BlobTest, DT_BlobTest, CBlobTest)
	RecvPropFloat(RECVINFO(m_flCubeWidth)),
	RecvPropFloat(RECVINFO(m_flRenderRadius)),
	RecvPropFloat(RECVINFO(m_flCutoffRadius))
END_RECV_TABLE()

C_BlobTest::C_BlobTest() {
	m_flCubeWidth = 1.0f;
	m_flRenderRadius = 1.3f;
	m_flCutoffRadius = 5.5f;
}

int C_BlobTest::DrawModel(int flags, const RenderableInstance_t& instance) {

	// setup material and lighting
	Vector vCenter = GetAbsOrigin();
	IMaterial* pMaterial = materials->FindMaterial("paintblobs/blob_surface_bounce", TEXTURE_GROUP_OTHER);
	
	modelrender->SetupLighting(vCenter);

	// setup test particles
	static ImpParticle iParticles[MAX_SURFACE_ELEMENTS];
	int nParticles = 0;
	float flRadius = 3.0f;

	for (int i = -10; i <= 10; i++)
	{
		ImpParticle* pParticle = &iParticles[i + 10];
		pParticle->center.set(i * 2.0f * flRadius, 0.0f, 0.0f);
		nParticles++;
	}
	
	// setup rendering
	factory = ImpRendererFactory();
	static SweepRenderer* pRenderer = NULL;
	static ImpTiler* pTiler = NULL;

	if (!pRenderer)
	{
		pRenderer = new SweepRenderer();
		pTiler = new ImpTiler(pRenderer);
	}
	
	SweepRenderer::setCubeWidth(m_flCubeWidth);
	SweepRenderer::setRenderR(m_flRenderRadius);
	SweepRenderer::setCutoffR(m_flCutoffRadius);

	SweepRenderer::setCalcSignFunc(calcSign);
	SweepRenderer::setCalcSign2Func(calcSign2);
	SweepRenderer::setCalcCornerFunc(0, calcCornerNormal);
	SweepRenderer::setCalcVertexFunc(calcVertexNormalDebugColor);

	Vector transformedCenter = vCenter * (1.0f / flRadius);

	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->MatrixMode(MATERIAL_MODEL);
	pRenderContext->Bind(pMaterial);
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();

	pRenderContext->Translate(vCenter.x, vCenter.y, vCenter.z);
	pRenderContext->Scale(flRadius, flRadius, flRadius);

	VMatrix mRotMat;
	VMatrix mInvRotMat;
	Vector vTransformedEye;

	float angle = 0.0f;

	mRotMat.Identity();
	mInvRotMat.Identity();
	vTransformedEye.Init();

	ImpTile* pTile = new ImpTile(0, 0, 0);

	pTile->center = Point3D(vCenter.x, vCenter.y, vCenter.z);

	pRenderer->beginFrame(true, (void*)&pRenderContext);
	pRenderer->setOffset(Point3D(0.0f, 0.0f, 0.0f));
	pRenderer->beginTile(pTile); // TODO: attention: this param appears to not be used in the decompd func

	for (int i = 0; i < nParticles; i++) {
		ImpParticleWithFourInterpolants* pParticle = (ImpParticleWithFourInterpolants*)(&iParticles[i]);
		//ImpParticle* pParticle = &iParticles[i];// (ImpParticle*)(&iParticles[i]);

		if (pParticle->fieldRScaleSQ <= 0.1f)
			continue;

		Vector vParticle = pParticle->center.AsVector();
		Vector vPartilceTransformed = (vParticle - vCenter) * (1.0f / flRadius);

		Point3D dParticle = Point3D(vPartilceTransformed.x, vPartilceTransformed.y, vPartilceTransformed.z);
		Point3D dCenter = Point3D(transformedCenter.x, transformedCenter.y, transformedCenter.z);
		Point3D vec = dParticle - dCenter;

		pParticle->center = dParticle;
		debugoverlay->AddBoxOverlay(dParticle.AsVector() + vCenter, -Vector(5,5,5), Vector(5,5,5), QAngle(0, angle, 0), 0, 255, 0, 0, 0);

		pParticle->interpolants2.unit();
		pParticle->interpolants4.set(0.0f, 0.0f, -1.0f);

		pParticle->interpolants3 = *pParticle->interpolants2.crossProduct(pParticle->interpolants4);
		pParticle->interpolants3.normalize();

		pParticle->interpolants4 = *pParticle->interpolants2.crossProduct(pParticle->interpolants3);
		pParticle->interpolants4.normalize();

		pRenderer->addParticle(pParticle, true);
	}

	pRenderer->endTile();
	pRenderer->endFrame();

	//Vector overlayCenter = mInvRotMat.ApplyRotation(pTile->center.AsVector() * flRadius);
	////pFacotry->getRenderer()->getInnerDimensions();
	////Vector mins = (pRenderer->getInnerDimensions() * (-0.5f * flRadius)).AsVector();  // used to use tiler->getRenderDim instead of factory->
	////Vector maxs = (pRenderer->getInnerDimensions() * (0.5f * flRadius)).AsVector();	// ^^^^^^^^^^

	//Vector sliceMins = mins;
	//Vector sliceMaxs = maxs;
	//sliceMins.x += (pRenderer->getLastSliceDrawn() - pRenderer->getMarginNCubes()) * pRenderer->getCubeWidth() * flRadius;
	//sliceMaxs.x = sliceMins.x + pRenderer->getCubeWidth() * flRadius;

	//debugoverlay->AddBoxOverlay(overlayCenter + vCenter, sliceMins, sliceMaxs, QAngle(0, angle, 0), 255, 0, 0, 0, 0);

	//pRenderContext->PopMatrix();
	//pRenderContext.SafeRelease();

	return 1;
}