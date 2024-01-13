#pragma once
#include "implicit/imp_tiler.h"
#include "implicit/imp_particle.h"
#include "point_3D.h"
#include "index_tri_vertex_buffer.h"
#include "projecting_particle_cache.h"

struct CubeInfo {
	unsigned short cornerInfoNo;
	bool doneAbove, doneBelow;
	unsigned int everything;
};

struct YZ {
	unsigned char y, z;
};

struct Slice_t {

	CubeInfo corners[101];
	SmartArray<unsigned char, 1, 0> corner_info;
	SmartArray<YZ, 0, 0> todo_list;
	SmartArray<YZ, 0, 16> seed_list;

};

struct vbId_t {

	unsigned short time;
	unsigned short id;
};

struct CornerInfo {

	float value;
	float normal[3];
	vbId_t edges[3];

	unsigned char x, y;
	unsigned char pad1, pad2;
};

// func pointer types
typedef void (*CalcCornerFunc_t)(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
typedef bool (*CalcSignFunc_t)(unsigned char, unsigned char, unsigned char, float, float, float, ProjectingParticleCache*);
typedef void (*CalcSign2Func_t)(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*);
typedef void (*CalcVertexFunc_t)(float, float, float, int, CornerInfo const*, CornerInfo const*, IndexTriVertexBuffer*);

class SweepRenderer {
public:

	SweepRenderer();

	void beginFrame(bool tile_mode, void* pRenderContext);
	void endFrame();
	void beginTile(ImpTile* pTile);
	void endTile();

	void addParticle(ImpParticle* pParticle, bool p2);
	
	static float getCubeWidth();
	static float getCutoffR();
	static float getRenderR();
	static float getMarginWidth();
	static int		getMarginNCubes();
	static Point3D&	getInnerDimensions();

	bool isParticleWithinBounds(ImpParticle* pParticle);

	static void setCalcCornerFunc(int p1, CalcCornerFunc_t pFunc);
	static void setCalcSignFunc(CalcSignFunc_t pFunc);
	static void setCalcSign2Func(CalcSign2Func_t pFunc);
	static void setCalcVertexFunc(CalcVertexFunc_t pFunc);

	static void setCubeWidth(float flWidth);
	static void setCutoffR(float flRad);
	static void setRenderR(float flRad);
	void setOffset(Point3D& offset);

private:

	void allocSliceCorners(Slice_t* pSlice);
	void allocSliceTodoList(Slice_t* pSlice);
	void deallocSliceCorners(Slice_t* pSlice);
	void deallocSliceTodoList(Slice_t* pSlice);

	void changeCubeWidth(float flWidth);
	void changeRadii(float flRad0, float flRad1);

	void recalculateBB();
	void recalculateDimensions();

	void render_slice(unsigned char p1, Slice_t* p2, Slice_t* p3, Slice_t* p4);
	void render_slices();

	void seed_surface(Point3D& p1);

	//unsigned char undefined0[175];

	ProjectingParticleCache* pCache;
	IndexTriVertexBuffer* vertexBuffer;

	int maxNoSlicesToDraw;
	int last_slice_drawn;

	bool polygonizationEnabled;

	Point3D offset;
	Point3D outerBBMins;
	Point3D outerBBMaxs;
	Point3D innerBBMins;
	Point3D innerBBMaxs;

	SmartArray<ImpParticle*, 0, 16> particles;
	Slice_t slices[102];

	SmartArray<CubeInfo(*)[101], 0, 16> unused_slice_corners;
	SmartArray<SmartArray<unsigned char, 1, 0>, 0, 16> unused_corner_info;
	SmartArray<SmartArray<YZ, 0, 0>, 0, 16> unused_todo_lists;

	int n_allocated_slice_corners;
	int n_allocated_slice_corner_infos;
	int n_allocated_slice_todo_lists;
	SmartArray<unsigned char, 0, 16> recursion_stack_src;

};