#include "cbase.h"
#include "blobulator/implicit/ProjectingParticleCache.h"


ProjectingParticleCache::ProjectingParticleCache() {
    //void* pvVar1;
    void* pvVar1;
    PCacheElem_t* puVar2;
    int iVar3;
    int iVar4;

    //*(float*)(this + 0x10) = 0;         // curCacheElement ?
    //*(float*)(this + 0x14) = 0x10;      // cachePlane[0] || *cachePlane ??

    //pvVar1 = malloc(0x40);              // 0x40 == 64 bytes
    //*(void**)(this + 0xc) = pvVar1;     // 0xc  == this + 12 bytes
    //*(float*)(this + 0x1c) = 0;         // 0x1c == this + 28 bytes
    //*(float*)(this + 0x20) = 0x10;      // 0x20 == this + 32 bytes

    curCacheElement = 0;                // curCacheElement ?
    *(float*)(this + 0x14) = 0x10;      // cachePlane[0] || *cachePlane ??

    pvVar1 = malloc(0x40);              // 0x40 == 64 bytes
    *(void**)&clearList = pvVar1;       // 0xc  == this + 12 bytes
    *(float*)(this + 0x1c) = 0;         // 0x1c == this + 28 bytes
    *(float*)(this + 0x20) = 0x10;      // 0x20 == this + 32 bytes

    //pvVar1 = malloc(0x40);
    //*(void**)(this + 0x18) = pvVar1;    // 0x18 == this + 24
    //*(float*)(this + 0x28) = 0;         // 0x28 == this + 40
    //*(float*)(this + 0x2c) = 0x10;      // 0x2c == this + 44

    //pvVar1 = malloc(0x20);
    //*(void**)(this + 0x24) = pvVar1;    // 0x24 == this + 36
    //*(float*)(this + 0x30) = 0;         // 0x30 == this + 48

    //pvVar1 = malloc((size_t)&DAT_00009f64);
    //*(void**)(this + 0x34) = pvVar1;

    ////puVar2 = (undefined8*)operator_new(0x10);
    //puVar2 = new PCacheElem_t();
    //if (puVar2 == NULL) {
    //    puVar2 = NULL;
    //}
    //else {
    //    *puVar2 = 0;
    //    puVar2[1] = 0;
    //}
    //botSentinel = puVar2;

    //puVar2 = (undefined8*)operator_new(0x10);
    //if (puVar2 == NULL) {
    //    puVar2 = NULL;
    //}
    //else {
    //    *puVar2 = 0;
    //    puVar2[1] = 0;
    //}
    //midSentinel = puVar2;

    //puVar2 = (undefined8*)operator_new(0x10);
    //if (puVar2 == NULL) {
    //    puVar2 = NULL;
    //}
    //else {
    //    *puVar2 = 0;
    //    puVar2[1] = 0;
    //}
    //topSentinel = puVar2;

    //**(undefined2**)this = 0x8001;
    //*(undefined*)(*(int*)this + 2) = 0;
    //*(undefined*)(*(int*)this + 3) = 0;
    //*(undefined4*)(*(int*)this + 4) = 0;
    //*(undefined4*)(*(int*)this + 8) = *(undefined4*)(this + 4);
    //*(undefined4*)(*(int*)this + 0xc) = 0;
    //**(undefined2**)(this + 4) = 0x7fff;
    //*(undefined*)(*(int*)(this + 4) + 2) = 0;
    //*(undefined*)(*(int*)(this + 4) + 3) = 0;
    //*(undefined4*)(*(int*)(this + 4) + 4) = *(undefined4*)this;
    //*(undefined4*)(*(int*)(this + 4) + 8) = *(undefined4*)(this + 8);
    //*(undefined4*)(*(int*)(this + 4) + 0xc) = 0;
    //**(undefined2**)(this + 8) = 0x7fff;
    //*(undefined*)(*(int*)(this + 8) + 2) = 0;
    //*(undefined*)(*(int*)(this + 8) + 3) = 0;
    //*(undefined4*)(*(int*)(this + 8) + 4) = *(undefined4*)(this + 4);
    //*(undefined4*)(*(int*)(this + 8) + 8) = 0;
    //*(undefined4*)(*(int*)(this + 8) + 0xc) = 0;

    //iVar3 = 0;
    //do {
    //    iVar4 = 0x65;
    //    do {
    //        *(float*)(iVar3 + *(int*)(this + 0x34)) = *(float*)(this + 4);
    //        iVar3 = iVar3 + 4;
    //        iVar4 = iVar4 + -1;
    //    } while (iVar4 != 0);
    //} while (iVar3 < 0x9f64);

}

ProjectingParticleCache::~ProjectingParticleCache() {

}

void ProjectingParticleCache::addParticle(ImpParticle* pParticle) {

}

void ProjectingParticleCache::beginTile() {

}

void ProjectingParticleCache::endTile() {
    //ushort uVar1;
    //int iVar2;

    //iVar2 = 0;
    //*(float*)(this + 0x10) = 0;
    //*(float*)(this + 0x30) = 0;
    //if (0 < *(int*)(this + 0x28)) {
    //    do {
    //        uVar1 = *(ushort*)(*(int*)(this + 0x24) + iVar2 * 2);
    //        iVar2 = iVar2 + 1;
    //        *(float*)(*(int*)(this + 0x34) + ((uint)(uVar1 >> 8) + (uVar1 & 0xff) * 0x65) * 4) =
    //            *(float*)(this + 4);
    //    } while (iVar2 < *(int*)(this + 0x28));
    //}
    //*(float*)(this + 0x28) = 0;
    //return;
}

void ProjectingParticleCache::buildCache(float p1, float p2, Point3D* p3, Point3D* p4) {

}

void ProjectingParticleCache::insertIntoCache(ImpParticle* p1, float p2, float p3, Point3D& p4, Point3D& p5) {

}