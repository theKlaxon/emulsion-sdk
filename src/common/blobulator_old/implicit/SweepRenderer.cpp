#include "cbase.h"
#include "..\common\blobulator\implicit\SweepRenderer.h"
#include "blobulator/implicit/ProjectingParticleCache.h"

SweepRenderer::SweepRenderer() {

}

void SweepRenderer::beginFrame(bool p1, void* p2) {

}

void SweepRenderer::beginTile(ImpTile* pTile) {

}

void SweepRenderer::endFrame() {

}

void SweepRenderer::endTile() {

}

void SweepRenderer::addParticle(ImpParticle* pParticle, bool p2) {

}

void SweepRenderer::allocSliceCorners(Slice_t* pSlice) {

}

void SweepRenderer::allocSliceTodoList(Slice_t* pSlice) {

}

void SweepRenderer::deallocSliceCorners(Slice_t* pSlice) {

}

void SweepRenderer::deallocSliceTodoList(Slice_t* pSlice) {

}

void SweepRenderer::changeCubeWidth(float flWidth) {

}

void SweepRenderer::changeRadii(float flRadii0, float flRadii1) {

}

float SweepRenderer::getCubeWidth() {
	return cubeWidth;
}

float SweepRenderer::getCutoffR() {
	return cutoffR;
}

Point3D* SweepRenderer::getInnerDimensions() {
	return &innerDimensions;
}

int SweepRenderer::getMarginNCubes() {
	return marginNCubes;
}

float SweepRenderer::getMarginWidth() {
	return marginWidth;
}

float SweepRenderer::getRenderR() {
	return renderR;
}

bool SweepRenderer::isParticleWithinBounds(ImpParticle* pParticle) {
    float fVar1;

    //fVar1 = *(float*)param_1; // pParticle->center[0];
    fVar1 = *pParticle->center[0];

    float flX = *pParticle->center[0], flY = *pParticle->center[1], flZ = *pParticle->center[2];

    if ((((
        *(float*)(this + 0x30) <= fVar1 && fVar1 != *(float*)(this + 0x30)) && (fVar1 < *(float*)(this + 0x40))) && 

        ( fVar1 = *pParticle->center[1], *(float*)(this + 0x34) <= fVar1 && fVar1 != *(float*)(this + 0x34) ) ) &&

        (((fVar1 < *(float*)(this + 0x44) && (fVar1 = *pParticle->center[2], *(float*)(this + 0x38) <= fVar1 && fVar1 != *(float*)(this + 0x38))) &&

            (fVar1 < *(float*)(this + 0x48))))) {
        return true;
    }

    if (() &&
        () &&
        ())
        return true;

    return false;
}

void SweepRenderer::recalculateBB() {

}

void SweepRenderer::recalculateDimensions() {

}

void SweepRenderer::renderSlice(unsigned char* p1, Slice_t* pSlice0, Slice_t* pSlice1, Slice_t* pSlice3) {

}

void SweepRenderer::renderSlices() {

}

void SweepRenderer::seed_surface(Point3D& p1) {

}

