#pragma once
#include "ImpParticle.h"
#include "Userfunctions.h"
#include "..\Point3D.h"

class ImpTile;

class SweepRenderer {
public:

	SweepRenderer();

	void beginFrame(bool p1, void* p2);
	void beginTile(ImpTile* p1);
	void endFrame();
	void endTile();

	void addParticle(ImpParticle* p1, bool p2);

	void allocSliceCorners(Slice_t* p1);
	void allocSliceTodoList(Slice_t* p1);
	void deallocSliceCorners(Slice_t* p1);
	void deallocSliceTodoList(Slice_t* p1);

	static void changeCubeWidth(float p1);
	static void changeRadii(float p1, float p2);

	static float	getCubeWidth();
	static float	getCutoffR();
	static Point3D* getInnerDimensions();
	static int		getMarginNCubes();
	static float	getMarginWidth();
	static float	getRenderR();

	bool isParticleWithinBounds(ImpParticle* p1);

	void		recalculateBB();
	static void recalculateDimensions();

	void renderSlice(unsigned char* p1, Slice_t* p2, Slice_t* p3, Slice_t* p4);
	void renderSlices();

	// this is used by the ep3 blobulator, relace it
	int getLastSliceDrawn() { return 0; }

	void seed_surface(Point3D& p1);

	//static void setCalcCornerFunc(int p1, void(*p2)(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*));
	static void setCalcCornerFunc(int p1, void(*p2)(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*));
	static void setCalcSignFunc(bool(*p1)(unsigned char, unsigned char, unsigned char, float, float, float, ProjectingParticleCache*));
	static void setCalcSign2Func(void(*p1)(unsigned char, unsigned char, unsigned char, float, float, float, CornerInfo* const, ProjectingParticleCache*));
	static void setCalcVertexFunc(void(*p1)(float, float, float, int, const CornerInfo*, const CornerInfo*, IndexTriVertexBuffer*));
	static void setCubeWidth(float p1);
	static void setCutoffR(float p1);
	static void setRenderR(float p1);
	void		setOffset(Point3D& p1);
	void		setMaxNoSlicesToDraw(int p1) {}

protected:

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

	Slice_t slices[102];

	SmartArray<SmartArray<YZ, 0, 0>, 0, 16> unused_todo_lists;
	int n_alloced_slice_corners;
	int n_alloced_slice_corner_infos;
	int n_alloced_slice_todo_lists;

	static int marginNCubes;
	static int corerInfoSize;
	
	static float addAmt;// = 0.25f;
	static float cubeWidth;// = 0.8f;
	static float cutoffR;// = 3.3f;
	static float cutoffRSq;
	static float marginWidth;
	static float oneOverCubeWidth;// = 1.25f;
	static float oneOverThreshold;
	static float renderR;// = 1.3f;
	static float renderRSq;
	static float scaler;
	static float scalerSq;
	static float threshold;

	static bool tile_mode;

	static short fieldCalcSteps;

	static Point3D		innerDimensions;
	static Point3D		outerDimensions;
};

// made these like, not static n stuff
//float SweepRenderer::addAmt = 0.25f;
//float SweepRenderer::cubeWidth = 0.8f;
//float SweepRenderer::cutoffR = 3.3f;
//float SweepRenderer::oneOverCubeWidth = 1.25f;
//float SweepRenderer::renderR = 1.3f;