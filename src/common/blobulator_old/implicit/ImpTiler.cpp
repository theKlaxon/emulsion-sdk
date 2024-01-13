#include "cbase.h"
#include "..\common\blobulator\iblob_renderer.h"

void ImpTiler::drawSurface2() {
    ImpTile* pIVar1;
    int iVar2;

    iVar2 = 0;
    if (0 < *(int*)(this + 8)) {
        do {
            if (*(int*)(this + 0x10) == iVar2) { // if (somthing) == null, return
                return;
            }

            pIVar1 = *(ImpTile**)(*(int*)(this + 4) + iVar2 * 4); // find tile

            Tiler::drawTile(pIVar1, *(SweepRenderer**)this); // global drawTile(ImpTile*, SweepRenderer)

            *(Point3D*)(this + 0x20) = *(Point3D*)(pIVar1 + 0x10); // center
            iVar2 = iVar2 + 1;
            *(Point3D*)(this + 0x28) = *(Point3D*)(pIVar1 + 0x18); // debug color?
        } while (iVar2 < *(int*)(this + 8));
    }
    return;
}