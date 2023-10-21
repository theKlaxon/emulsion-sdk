#pragma once
#include "igamesystem.h"
#include "materialsystem/imesh.h"
#include "game/shared/portal2/paint_enum.h"
#include <utlvector.h>

void DrawFastSphere(CMeshBuilder& meshBuilder, const Vector& center, float radius, int r, int g, int b);

struct kFastParticlePosData {
	kFastParticlePosData() {}

	Vector m_vecVelocity = Vector();
	Vector m_vecCenter = Vector();
	Vector m_vecVertices[51] = {};
};

struct kFastParticle {
	kFastParticle() { m_Type = 0; radius = 0; }

	char m_Type;
	float radius;
	
	kFastParticlePosData m_PosData;
};

struct kFastParticleCluster {
	kFastParticleCluster() {}

	// yoinked from cutlvector since it was bitching at me about
	// this class' copy constructor
	kFastParticleCluster(kFastParticleCluster const& vec) { Assert(0); }

	char m_Type;
	IMaterial* m_pMaterial;
	CUtlVector<kFastParticle> m_Particles;// = CUtlVector<kFastParticle>();
};

struct kClusterInitData {
	kClusterInitData(char type, const char* strMatName) { m_Type = type; m_strMatName = strMatName; }
	char m_Type;
	const char* m_strMatName;
};

class C_FastSphereManager : public CAutoGameSystemPerFrame {
public:

	// Init, shutdown
	// return true on success. false to abort DLL init!
	virtual bool Init();
	virtual void PostInit();
	virtual void Shutdown();

	// Called before rendering
	virtual void PreRender();

	// Gets called each frame
	virtual void Update(float frametime);

	// Called after rendering
	virtual void PostRender();
	
	void AddParticle(PaintPowerType type, Vector start, Vector velocity, float rad);
	void AddParticle(kFastParticle particle);

	int GetParticleCount();

private:
	
	void DrawCluster(int i);
	void UpdateParticlePosition(int i, int j);

	bool m_bInit = false;

	CUtlVector<kFastParticleCluster*> m_Clusters;
	CMeshBuilder m_MeshBuilder;

};

extern C_FastSphereManager g_FastSphere;
IGameSystem* FastSphereManager();

static const float g_FastSpherePosData[51][8] = {
{  0.0000,  0.0000,  1.0000,  0.0000,  0.0000,  0.0000,  0.0000,  1.0000 },
{ -0.0000,  0.5000,  0.8660,  0.2500,  0.1667, -0.0000,  0.5000,  0.8660 },
{  0.5000,  0.0000,  0.8660,  0.0000,  0.1667,  0.5000,  0.0000,  0.8660 },
{  0.0000,  0.0000,  1.0000,  0.5000,  0.0000,  0.0000,  0.0000,  1.0000 },
{ -0.5000, -0.0000,  0.8660,  0.5000,  0.1667, -0.5000, -0.0000,  0.8660 },
{  0.0000,  0.0000, -1.0000,  0.0000,  1.0000,  0.0000,  0.0000, -1.0000 },
{  0.5000,  0.0000, -0.8660,  0.0000,  0.8333,  0.5000,  0.0000, -0.8660 },
{ -0.0000,  0.5000, -0.8660,  0.2500,  0.8333, -0.0000,  0.5000, -0.8660 },
{  0.0000,  0.0000, -1.0000,  0.5000,  1.0000,  0.0000,  0.0000, -1.0000 },
{ -0.5000, -0.0000, -0.8660,  0.5000,  0.8333, -0.5000, -0.0000, -0.8660 },
{  0.0000, -0.5000,  0.8660,  0.7500,  0.1667,  0.0000, -0.5000,  0.8660 },
{  0.0000,  0.0000,  1.0000,  1.0000,  0.0000,  0.0000,  0.0000,  1.0000 },
{  0.5000,  0.0000,  0.8660,  1.0000,  0.1667,  0.5000,  0.0000,  0.8660 },
{  0.0000, -0.5000, -0.8660,  0.7500,  0.8333,  0.0000, -0.5000, -0.8660 },
{  0.0000,  0.0000, -1.0000,  1.0000,  1.0000,  0.0000,  0.0000, -1.0000 },
{  0.5000,  0.0000, -0.8660,  1.0000,  0.8333,  0.5000,  0.0000, -0.8660 },
{  0.6124,  0.6124,  0.5000,  0.1250,  0.3333,  0.6124,  0.6124,  0.5000 },
{  0.8660,  0.0000,  0.5000,  0.0000,  0.3333,  0.8660,  0.0000,  0.5000 },
{ -0.0000,  0.8660,  0.5000,  0.2500,  0.3333, -0.0000,  0.8660,  0.5000 },
{  0.8660,  0.0000, -0.5000,  0.0000,  0.6667,  0.8660,  0.0000, -0.5000 },
{  0.6124,  0.6124, -0.5000,  0.1250,  0.6667,  0.6124,  0.6124, -0.5000 },
{ -0.0000,  0.8660, -0.5000,  0.2500,  0.6667, -0.0000,  0.8660, -0.5000 },
{ -0.6124,  0.6124,  0.5000,  0.3750,  0.3333, -0.6124,  0.6124,  0.5000 },
{ -0.8660, -0.0000,  0.5000,  0.5000,  0.3333, -0.8660, -0.0000,  0.5000 },
{ -0.6124,  0.6124, -0.5000,  0.3750,  0.6667, -0.6124,  0.6124, -0.5000 },
{ -0.8660, -0.0000, -0.5000,  0.5000,  0.6667, -0.8660, -0.0000, -0.5000 },
{ -0.6124, -0.6124,  0.5000,  0.6250,  0.3333, -0.6124, -0.6124,  0.5000 },
{  0.0000, -0.8660,  0.5000,  0.7500,  0.3333,  0.0000, -0.8660,  0.5000 },
{ -0.6124, -0.6124, -0.5000,  0.6250,  0.6667, -0.6124, -0.6124, -0.5000 },
{  0.0000, -0.8660, -0.5000,  0.7500,  0.6667,  0.0000, -0.8660, -0.5000 },
{  0.6124, -0.6124,  0.5000,  0.8750,  0.3333,  0.6124, -0.6124,  0.5000 },
{  0.8660,  0.0000,  0.5000,  1.0000,  0.3333,  0.8660,  0.0000,  0.5000 },
{  0.6124, -0.6124, -0.5000,  0.8750,  0.6667,  0.6124, -0.6124, -0.5000 },
{  0.8660,  0.0000, -0.5000,  1.0000,  0.6667,  0.8660,  0.0000, -0.5000 },
{  0.9239,  0.3827,  0.0000,  0.0625,  0.5000,  0.9239,  0.3827,  0.0000 },
{  1.0000,  0.0000,  0.0000,  0.0000,  0.5000,  1.0000,  0.0000,  0.0000 },
{  0.7071,  0.7071,  0.0000,  0.1250,  0.5000,  0.7071,  0.7071,  0.0000 },
{  0.3827,  0.9239,  0.0000,  0.1875,  0.5000,  0.3827,  0.9239,  0.0000 },
{ -0.0000,  1.0000,  0.0000,  0.2500,  0.5000, -0.0000,  1.0000,  0.0000 },
{ -0.3827,  0.9239,  0.0000,  0.3125,  0.5000, -0.3827,  0.9239,  0.0000 },
{ -0.7071,  0.7071,  0.0000,  0.3750,  0.5000, -0.7071,  0.7071,  0.0000 },
{ -0.9239,  0.3827,  0.0000,  0.4375,  0.5000, -0.9239,  0.3827,  0.0000 },
{ -1.0000, -0.0000,  0.0000,  0.5000,  0.5000, -1.0000, -0.0000,  0.0000 },
{ -0.9239, -0.3827,  0.0000,  0.5625,  0.5000, -0.9239, -0.3827,  0.0000 },
{ -0.7071, -0.7071,  0.0000,  0.6250,  0.5000, -0.7071, -0.7071,  0.0000 },
{ -0.3827, -0.9239,  0.0000,  0.6875,  0.5000, -0.3827, -0.9239,  0.0000 },
{  0.0000, -1.0000,  0.0000,  0.7500,  0.5000,  0.0000, -1.0000,  0.0000 },
{  0.3827, -0.9239,  0.0000,  0.8125,  0.5000,  0.3827, -0.9239,  0.0000 },
{  0.7071, -0.7071,  0.0000,  0.8750,  0.5000,  0.7071, -0.7071,  0.0000 },
{  0.9239, -0.3827,  0.0000,  0.9375,  0.5000,  0.9239, -0.3827,  0.0000 },
{  1.0000,  0.0000,  0.0000,  1.0000,  0.5000,  1.0000,  0.0000,  0.0000 }
};

static const int g_FastSphereTriData[84][3] = {
{ 0, 1, 2 },
{ 0, 3, 1 },
{ 3, 4, 1 },
{ 5, 6, 7 },
{ 5, 7, 8 },
{ 8, 7, 9 },
{ 3, 10, 4 },
{ 3, 11, 10 },
{ 11, 12, 10 },
{ 8, 9, 13 },
{ 8, 13, 14 },
{ 14, 13, 15 },
{ 2, 16, 17 },
{ 2, 1, 16 },
{ 1, 18, 16 },
{ 6, 19, 20 },
{ 6, 20, 7 },
{ 7, 20, 21 },
{ 1, 22, 18 },
{ 1, 4, 22 },
{ 4, 23, 22 },
{ 7, 21, 24 },
{ 7, 24, 9 },
{ 9, 24, 25 },
{ 4, 26, 23 },
{ 4, 10, 26 },
{ 10, 27, 26 },
{ 9, 25, 28 },
{ 9, 28, 13 },
{ 13, 28, 29 },
{ 10, 30, 27 },
{ 10, 12, 30 },
{ 12, 31, 30 },
{ 13, 29, 32 },
{ 13, 32, 15 },
{ 15, 32, 33 },
{ 17, 34, 35 },
{ 17, 16, 34 },
{ 16, 36, 34 },
{ 19, 35, 34 },
{ 19, 34, 20 },
{ 20, 34, 36 },
{ 16, 37, 36 },
{ 16, 18, 37 },
{ 18, 38, 37 },
{ 20, 36, 37 },
{ 20, 37, 21 },
{ 21, 37, 38 },
{ 18, 39, 38 },
{ 18, 22, 39 },
{ 22, 40, 39 },
{ 21, 38, 39 },
{ 21, 39, 24 },
{ 24, 39, 40 },
{ 22, 41, 40 },
{ 22, 23, 41 },
{ 23, 42, 41 },
{ 24, 40, 41 },
{ 24, 41, 25 },
{ 25, 41, 42 },
{ 23, 43, 42 },
{ 23, 26, 43 },
{ 26, 44, 43 },
{ 25, 42, 43 },
{ 25, 43, 28 },
{ 28, 43, 44 },
{ 26, 45, 44 },
{ 26, 27, 45 },
{ 27, 46, 45 },
{ 28, 44, 45 },
{ 28, 45, 29 },
{ 29, 45, 46 },
{ 27, 47, 46 },
{ 27, 30, 47 },
{ 30, 48, 47 },
{ 29, 46, 47 },
{ 29, 47, 32 },
{ 32, 47, 48 },
{ 30, 49, 48 },
{ 30, 31, 49 },
{ 31, 50, 49 },
{ 32, 48, 49 },
{ 32, 49, 33 },
{ 33, 49, 50 }
};

void DrawFastSphere(CMeshBuilder& meshBuilder, const Vector& center, float radius, int r, int g, int b);
//{
//	int i;
//
//	int offset = meshBuilder.GetCurrentVertex();
//
//	Vector pos;
//	for (i = 0; i < 51; i++)
//	{
//		pos.x = g_FastSpherePosData[i][0] + center.x + g_FastSpherePosData[i][5] * radius;
//		pos.y = g_FastSpherePosData[i][1] + center.y + g_FastSpherePosData[i][6] * radius;
//		pos.z = g_FastSpherePosData[i][2] + center.z + g_FastSpherePosData[i][7] * radius;
//
//		meshBuilder.Position3fv(pos.Base());
//		meshBuilder.Normal3fv(&g_FastSpherePosData[i][5]);
//		meshBuilder.TexCoord2fv(0, &g_FastSpherePosData[i][3]);
//		meshBuilder.Color3ub(255, 255, 255);
//		meshBuilder.AdvanceVertex();
//	}
//
//	for (i = 0; i < 84; i++)
//	{
//		meshBuilder.FastIndex(g_FastSphereTriData[i][0] + offset);
//		meshBuilder.FastIndex(g_FastSphereTriData[i][1] + offset);
//		meshBuilder.FastIndex(g_FastSphereTriData[i][2] + offset);
//	}
//}