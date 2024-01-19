#include "cbase.h"
#include "c_paint_stream.h"
#include "dt_utlvector_recv.h"
#include "implicit/imp_particle.h"
#include "implicit/imp_tiler.h"
#include "implicit/sweep_renderer.h"
#include "blob_render.h"

ConVar paintblob_stream_radius("r_paintblob_stream_radius", "8", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);
ConVar paintblob_stream_max_blobs("r_paintblob_stream_max_blobs", "350", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);
ConVar paintblob_surface_max_tiles("r_paintblob_surface_max_tiles", "-1", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);

ConVar paintblob_blr_cubewidth("r_paintblob_blr_cubewidth", "0.8", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);
ConVar paintblob_blr_render_radius("r_paintblob_blr_render_radius", "1.3", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);
ConVar paintblob_blr_cutoff_radius("r_paintblob_blr_cutoff_radius", "3.3", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY);

// must match layout of PaintPowerType
const char* g_PaintTypeMaterials[] = {
	"paintblobs/blob_surface_bounce",	// bounce
	"paintblobs/blob_surface_portal",	// reflect / stick
	"paintblobs/blob_surface_speed",	// speed
	"paintblobs/blob_surface_portal",	// portal
	"paintblobs/blob_surface_portal",	// no power
};

#define MAX_SURFACE_ELEMENTS paintblob_stream_max_blobs.GetInt()

LINK_ENTITY_TO_CLASS(env_paint_stream, C_PaintBlobStream)

IMPLEMENT_CLIENTCLASS_DT(C_PaintBlobStream, DT_PaintBlobStream, CPaintBlobStream)
	RecvPropUtlVector(
		RECVINFO_UTLVECTOR(m_vecSurfacePositions),
		paintblob_stream_max_blobs.GetInt(),
		RecvPropVector(NULL, 0, sizeof(Vector))),
	RecvPropUtlVector(
		RECVINFO_UTLVECTOR(m_vecSurfaceVs),
		paintblob_stream_max_blobs.GetInt(),
		RecvPropFloat(NULL, 0, sizeof(float))),
	RecvPropUtlVector(
		RECVINFO_UTLVECTOR(m_vecSurfaceRs),
		paintblob_stream_max_blobs.GetInt(),
		RecvPropFloat(NULL, 0, sizeof(float))),
	RecvPropUtlVector(
		RECVINFO_UTLVECTOR(m_vecRadii),
		paintblob_stream_max_blobs.GetInt(),
		RecvPropFloat(NULL, 0, sizeof(float))),

	RecvPropInt(RECVINFO(m_nActiveParticles)),
	RecvPropInt(RECVINFO(m_nPaintType)),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA(C_PaintBlobStream)
	DEFINE_PRED_FIELD(m_vecSurfacePositions, FIELD_VECTOR, FTYPEDESC_INSENDTABLE | FTYPEDESC_NOERRORCHECK),
END_PREDICTION_DATA()

C_PaintBlobStream::C_PaintBlobStream() {

	m_nActiveParticlesInternal = 0;

	m_pMaterial = materials->FindMaterial("paintblobs/blob_surface_portal", TEXTURE_GROUP_OTHER, 0);
	
	m_vecSurfacePositions.EnsureCount(MAX_SURFACE_ELEMENTS);
	m_iv_vecSurfacePositions.EnsureCount(MAX_SURFACE_ELEMENTS);

	m_vecSurfaceVs.EnsureCount(MAX_SURFACE_ELEMENTS);
	m_iv_vecSurfaceVs.EnsureCount(MAX_SURFACE_ELEMENTS);

	m_vecSurfaceRs.EnsureCount(MAX_SURFACE_ELEMENTS);
	m_iv_vecSurfaceRs.EnsureCount(MAX_SURFACE_ELEMENTS);

	for (int i = 0; i < MAX_SURFACE_ELEMENTS; i++) {
		IInterpolatedVar* pWatcher = &m_iv_vecSurfacePositions.Element(i);
		pWatcher->SetDebugName("m_iv_vecSurfacePositions");
		AddVar(&m_vecSurfacePositions.Element(i), pWatcher, LATCH_ANIMATION_VAR, true);

		pWatcher = &m_iv_vecSurfaceVs.Element(i);
		pWatcher->SetDebugName("m_iv_vecSurfaceVs");
		AddVar(&m_vecSurfaceVs.Element(i), pWatcher, LATCH_ANIMATION_VAR);

		pWatcher = &m_iv_vecSurfaceRs.Element(i);
		pWatcher->SetDebugName("m_iv_vecSurfaceRs");
		AddVar(&m_vecSurfaceRs.Element(i), pWatcher, LATCH_ANIMATION_VAR);
	}
}

#include "blob_manager.h"

void C_PaintBlobStream::Spawn() {

	m_pMaterial = materials->FindMaterial(g_PaintTypeMaterials[m_nPaintType], TEXTURE_GROUP_OTHER, true);
	SetNextClientThink(CLIENT_THINK_ALWAYS);
}

void C_PaintBlobStream::PostDataUpdate(DataUpdateType_t updateType) {

	SetNextClientThink(CLIENT_THINK_ALWAYS);
	BaseClass::PostDataUpdate(updateType);
}

void C_PaintBlobStream::ClientThink() {

	for (int i = m_nActiveParticlesInternal; i < m_nActiveParticles && m_nActiveParticles > 0; i++) {
		m_iv_vecSurfacePositions[i].RestoreToLastNetworked(); // need this to not show blobs at the stupid origin >: (
		m_iv_vecSurfacePositions[i].ClearHistory();
		m_nActiveParticlesInternal++;
	}

	SetNextClientThink(CLIENT_THINK_ALWAYS);
}

bool C_PaintBlobStream::IsTransparent()
{
	// TODO: Fix this
	return false;
}

bool C_PaintBlobStream::UsesPowerOfTwoFrameBufferTexture()
{
	if (!m_pMaterial) return false;
	return m_pMaterial->NeedsPowerOfTwoFrameBufferTexture();
}

bool C_PaintBlobStream::UsesFullFrameBufferTexture()
{
	if (!m_pMaterial) return false;
	return m_pMaterial->NeedsFullFrameBufferTexture();
}

void C_PaintBlobStream::GetRenderBounds(Vector& theMins, Vector& theMaxs) {
	theMins = m_vecSurfacePositions[0];
	theMaxs = m_vecSurfacePositions[0];
	float surfaceRadius = paintblob_stream_radius.GetFloat() * 3.0f;
	for (int i = 0; i < m_nActiveParticlesInternal; i++)
	{
		VectorMin(m_vecSurfacePositions[i] - Vector(surfaceRadius, surfaceRadius, surfaceRadius), theMins, theMins);
		VectorMax(m_vecSurfacePositions[i] + Vector(surfaceRadius, surfaceRadius, surfaceRadius), theMaxs, theMaxs);
	}
	theMins -= GetRenderOrigin();
	theMaxs -= GetRenderOrigin();
}

int C_PaintBlobStream::DrawModel(int flags, const RenderableInstance_t& instance) {

	m_pMaterial = materials->FindMaterial(g_PaintTypeMaterials[m_nPaintType], TEXTURE_GROUP_OTHER, true);
	modelrender->SetupLighting(GetRenderOrigin());

	static CUtlVector<ImpParticleWithOneInterpolant> imp_particles;
	imp_particles.EnsureCount(paintblob_stream_max_blobs.GetInt());
	int n_particles = 0;

	for (int i = 0; i < m_nActiveParticlesInternal; i++) {
		ImpParticleWithOneInterpolant* imp_particle = &imp_particles[i];
		imp_particle->center = Point3D(m_vecSurfacePositions[i].x, m_vecSurfacePositions[i].y, m_vecSurfacePositions[i].z);
		imp_particle->fieldRScaleSq = m_vecSurfaceRs[i];
		imp_particle->scale = m_vecRadii[i];
		imp_particle->interpolants1[3] = m_vecSurfaceVs[i];
		n_particles++;
	}

	static ImpTiler* tiler = ImpTilerFactory::factory->getTiler();
	tiler->maxNoTileToDraw = paintblob_surface_max_tiles.GetInt();

	SweepRenderer::setCubeWidth(paintblob_blr_cubewidth.GetFloat());
	SweepRenderer::setRenderR(paintblob_blr_render_radius.GetFloat());
	SweepRenderer::setCutoffR(paintblob_blr_cutoff_radius.GetFloat());

	SweepRenderer::setCalcSignFunc(calcSign);
	SweepRenderer::setCalcSign2Func(calcSign2);
	SweepRenderer::setCalcCornerFunc(32, calcCornerNormalColor);
	SweepRenderer::setCalcVertexFunc(calcVertexNormalNColorUVTan);

	Vector center;
	center.Init();

	float flRadius = paintblob_stream_radius.GetFloat();
	float rdiv1 = (1.0f / flRadius); // dont do the same math twice
	Vector transformedCenter = center * rdiv1;

	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->MatrixMode(MATERIAL_MODEL);
	pRenderContext->Bind(m_pMaterial);
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();
	pRenderContext->Translate(center.x, center.y, center.z);
	pRenderContext->Scale(flRadius, flRadius, flRadius);

	// TODO: enable paintblob stream rotation here?
	VMatrix rotationMatrix;
	VMatrix invRotationMatrix;
	Vector transformedEye;
	float angle = 0.0f;

	rotationMatrix.Identity();
	invRotationMatrix.Identity();
	transformedEye.Init();
	angle = 0.0f;
	// --

	tiler->beginFrame(Point3D(0.0f, 0.0f, 0.0f), (void*)&pRenderContext, 1);

	for (int i = 0; i < n_particles; i++) {
		ImpParticleWithOneInterpolant* imp_particle = &imp_particles[i];
		if (imp_particle->scale <= 0.1f) continue;

		Vector vParticle = imp_particle->center.AsVector();
		Vector transformedParticle = (vParticle - center) * rdiv1;

		Point3D pParticle = transformedParticle;
		Point3D pCenter = transformedCenter;
		Point3D vec = (pParticle - pCenter);

		imp_particle->center = pParticle;

		tiler->insertParticle(imp_particle);
	}

	{
		tiler->drawSurface();
	}

	tiler->endFrame();
	ImpTilerFactory::factory->returnTiler(tiler);

	pRenderContext->PopMatrix();
	return 1;
}
