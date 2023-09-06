#pragma once

// TODO: I don't know where these are actually meant to be defined
#include "blobulator/implicit/impparticle.h"
#include "blobulator/smartarray.h"

// NOTE: The following types I believe are accurate

DECL_ALIGN(1) struct YZ
{
	unsigned char y, z;
};

DECL_ALIGN(1) struct XYZ
{
	unsigned char x, y, z;
};

DECL_ALIGN(1) struct pcache_YZ_t
{
	unsigned char y, z;
};

struct Slice_t {

};

// 16 bytes?
struct PCacheElem_t
{
	short center_x;
	unsigned char bot_x;
	unsigned char top_x;
	PCacheElem_t* prev;
	PCacheElem_t* next;
	ImpParticle* p;
};

struct PSliceData_t
{
	unsigned int zArray[101];
	SmartArray<PCacheElem_t, 0, 16> cacheElements;
	unsigned char field_0x1a0;
	unsigned char field_0x1a1;
	unsigned char field_0x1a2;
	unsigned char field_0x1a3;
	unsigned char field_0x1a4;
	unsigned char field_0x1a5;
	unsigned char field_0x1a6;
	unsigned char field_0x1a7;
	unsigned char field_0x1a8;
	unsigned char field_0x1a9;
	unsigned char field_0x1aa;
	unsigned char field_0x1ab;
	int max_cache_elements_size;
};

struct PCacheSlice_t
{
	PSliceData_t* sliceData;
};

DECL_ALIGN(2) struct vbId_t
{
	unsigned short time;
	unsigned short id;
};

/*DECL_ALIGN(4)*/ struct CornerInfo
{
	float value;
	float normal[3];
	vbId_t edges[3];
	CornerInfo* next;
	unsigned char x, y, z;
	unsigned char dones;
};

// define in blobulator/vertexbuffers/indextrivertexbuffer.h
DECL_ALIGN(4) class IndexTriVertexBuffer
{
	unsigned short m_curTime;
	unsigned short m_nVerticesOutput;
	IMesh* m_pMesh;
	CMeshBuilder* m_pBuilder;
	unsigned short m_stat_no_flashes;
};

class ProjectingParticleCache;

// get these functions from Portal 2 or Source Particle Benchmark
void __cdecl calcCornerNormal(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
void __cdecl calcCornerNormalColor(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
void __cdecl calcCornerNormalHiFreqColor(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
void __cdecl calcCornerNormalColorTanNoUV(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
void __cdecl calcCornerNormalColorUVTan(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);

void __cdecl calcSign2(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
bool __cdecl calcSign(unsigned char, unsigned char, unsigned char, float, float, float, ProjectingParticleCache*);

void __cdecl calcVertexNormal(float, float, float, int, const CornerInfo*, const CornerInfo*, IndexTriVertexBuffer*);
void __cdecl calcVertexNormalDebugColor(float, float, float, int, const CornerInfo*, const CornerInfo*, IndexTriVertexBuffer*);
void __cdecl calcVertexNormalNColor(float, float, float, int, const CornerInfo*, const CornerInfo*, IndexTriVertexBuffer*);
void __cdecl calcVertexNormalNColorTanNoUV(float, float, float, int, const CornerInfo*, const CornerInfo*, IndexTriVertexBuffer*);
void __cdecl calcCornerNormalColorTanNoUV(float, float, float, int, const CornerInfo*, const CornerInfo*, IndexTriVertexBuffer*);
void __cdecl calcVertexNormalNColorUVTan(float, float, float, int, const CornerInfo*, const CornerInfo*, IndexTriVertexBuffer*);

typedef void (__cdecl* CalcCornerFunc)(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
typedef void (__cdecl* CalcSign2Func)(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
typedef bool (__cdecl* CalcSignFunc)(unsigned char, unsigned char, unsigned char, float, float, float, ProjectingParticleCache*);
typedef void (__cdecl* CalcVertexFunc)(float, float, float, int, const CornerInfo*, const CornerInfo*, IndexTriVertexBuffer*);

#pragma warning(push, 0)
//const __m128 Four_Zeros;
#pragma warning(pop)