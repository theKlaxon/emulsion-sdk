#include "cbase.h"
#include "c_fastsphere.h"

C_FastSphereManager g_FastSphere = C_FastSphereManager();

IGameSystem* FastSphereManager() {
	return &g_FastSphere;
}

kClusterInitData g_PaintClusters[] = {
	{BOUNCE_POWER, "paintblobs/blob_surface_bounce"},
	{SPEED_POWER, "paintblobs/blob_surface_speed"},
	{PORTAL_POWER, "paintblobs/blob_surface_portal"},
};

bool C_FastSphereManager::Init() {

	// init the clusters we're using
	{
		kClusterInitData* data = nullptr;

		int c = sizeof(g_PaintClusters) / sizeof(kClusterInitData);
		for (int i = 0; i < c; i++) {
			kClusterInitData* pData = &g_PaintClusters[i];
			kFastParticleCluster* cluster = new kFastParticleCluster();

			cluster->m_Particles = CUtlVector<kFastParticle>();
			cluster->m_pMaterial = materials->FindMaterial(pData->m_strMatName, TEXTURE_GROUP_OTHER);
			cluster->m_Type = pData->m_Type;
			m_Clusters.AddToTail(cluster);
		}
	}

	return m_bInit = true;
}

void C_FastSphereManager::PostInit() {

}

void C_FastSphereManager::Shutdown() {
	for (int i = 0; i < m_Clusters.Count(); i++)
		m_Clusters[i]->m_Particles.Purge();

	m_Clusters.Purge();
}

// calculate new particle positions
void C_FastSphereManager::PreRender() {
	for (int i = 0; i < m_Clusters.Count(); i++)
		DrawCluster(i);
}

// Gets called each frame
void C_FastSphereManager::Update(float frametime) {
	//for (int i = 0; i < m_Clusters.Count(); i++)
	//	for (int j = 0; j < m_Clusters[i].m_Particles.Count(); j++)
	//		UpdateParticlePosition(i, j);
}

// draw the particles
void C_FastSphereManager::PostRender() {

}

void C_FastSphereManager::AddParticle(PaintPowerType type, Vector start, Vector velocity, float rad) {

	Vector pos;
	kFastParticle particle;

	particle.m_Type = type;
	particle.radius = rad;
	particle.m_PosData.m_vecCenter = start;
	particle.m_PosData.m_vecVelocity = velocity;

	// calculate the particle's position center;
	for (int i = 0; i < 51; i++)
	{
		pos.x = g_FastSpherePosData[i][0] + g_FastSpherePosData[i][5] * rad;
		pos.y = g_FastSpherePosData[i][1] + g_FastSpherePosData[i][6] * rad;
		pos.z = g_FastSpherePosData[i][2] + g_FastSpherePosData[i][7] * rad;
		particle.m_PosData.m_vecVertices[i] = pos;
	}

	AddParticle(particle);
}

void C_FastSphereManager::AddParticle(kFastParticle particle) {

	if (!m_bInit) {
		Msg("Particle could not be added, Fast Sphere not initialised...\n");
		return;
	}
	
	// TODO: make dynamic like the rest of this shit, duh
	switch (particle.m_Type) {
		case BOUNCE_POWER:
			m_Clusters[0]->m_Particles.AddToTail(particle);
			break;
		case SPEED_POWER:
			m_Clusters[1]->m_Particles.AddToTail(particle);
			break;
		case PORTAL_POWER:
			m_Clusters[2]->m_Particles.AddToTail(particle);
			break;
		default:
			Msg("Particle could not be bucketed, invalid type...\n");
			break;
	}
}

void C_FastSphereManager::DrawCluster(int index) {

	kFastParticleCluster* pCurCluster = m_Clusters[index];
	CMatRenderContextPtr pRenderContext(materials);
	IMesh* pMesh = pRenderContext->GetDynamicMesh();

	int total = pCurCluster->m_Particles.Count();

	m_MeshBuilder.Begin(pMesh, MATERIAL_TRIANGLES, total * 51, total * 84 * 3);
	pRenderContext->Bind(pCurCluster->m_pMaterial); // bind the material for this cluster

	for (int i = 0; i < pCurCluster->m_Particles.Count(); i++) {

		int j = 0, offset = m_MeshBuilder.GetCurrentVertex();
		Vector pos;

		for (; j < 51; i++) {
			kFastParticle* pCur = &pCurCluster->m_Particles[j];

			pos.x = pCur->m_PosData.m_vecVertices[j].x + pCur->m_PosData.m_vecCenter.x;
			pos.y = pCur->m_PosData.m_vecVertices[j].y + pCur->m_PosData.m_vecCenter.y;
			pos.z = pCur->m_PosData.m_vecVertices[j].z + pCur->m_PosData.m_vecCenter.z;

			m_MeshBuilder.Position3fv(pos.Base());
			m_MeshBuilder.Normal3fv(&g_FastSpherePosData[i][5]);
			m_MeshBuilder.TexCoord2fv(0, &g_FastSpherePosData[i][3]);
			m_MeshBuilder.Color3ub(255, 255, 255);
			m_MeshBuilder.AdvanceVertex();
		}

		for (i = 0; i < 84; i++)
		{
			m_MeshBuilder.FastIndex(g_FastSphereTriData[i][0] + offset);
			m_MeshBuilder.FastIndex(g_FastSphereTriData[i][1] + offset);
			m_MeshBuilder.FastIndex(g_FastSphereTriData[i][2] + offset);
		}
	}

	m_MeshBuilder.End();
	pMesh->Draw();
}

void C_FastSphereManager::UpdateParticlePosition(int i, int j) {

}

int C_FastSphereManager::GetParticleCount() {

	if (!m_bInit)
		return 0;

	int count = 0;
	for (int i = 0; i < m_Clusters.Count(); i++)
		count += m_Clusters[i]->m_Particles.Count();

	return count;
}

void DrawFastSphere(CMeshBuilder& meshBuilder, const Vector& center, float radius, int r, int g, int b)
{
	int i;

	int offset = meshBuilder.GetCurrentVertex();

	Vector pos;
	for (i = 0; i < 51; i++)
	{
		pos.x = g_FastSpherePosData[i][0] + center.x + g_FastSpherePosData[i][5] * radius;
		pos.y = g_FastSpherePosData[i][1] + center.y + g_FastSpherePosData[i][6] * radius;
		pos.z = g_FastSpherePosData[i][2] + center.z + g_FastSpherePosData[i][7] * radius;

		meshBuilder.Position3fv(pos.Base());
		meshBuilder.Normal3fv(&g_FastSpherePosData[i][5]);
		meshBuilder.TexCoord2fv(0, &g_FastSpherePosData[i][3]);
		meshBuilder.Color3ub(255, 255, 255);
		meshBuilder.AdvanceVertex();
	}

	for (i = 0; i < 84; i++)
	{
		meshBuilder.FastIndex(g_FastSphereTriData[i][0] + offset);
		meshBuilder.FastIndex(g_FastSphereTriData[i][1] + offset);
		meshBuilder.FastIndex(g_FastSphereTriData[i][2] + offset);
	}
}