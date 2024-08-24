#pragma once
#include "cbase.h"
#include "game/shared/portal2/paint_enum.h"
#include "dll_offset.h"

extern void PatchAll();
extern void UnPatchAll();

class CMatSysPatch {
public:
    
    CMatSysPatch() {
        m_Offsets = MatSysOffsets();
    }

    // override the paint colors >:D (going goblin mode)
    void Patch();
    void UnPatch();

protected:

    class MatSysOffsets {
    public:

        MatSysOffsets() {
            g_PaintColors = Offset("materialsystem.dll", 0x115ad4);
        }

        Offset g_PaintColors;
    };

    MatSysOffsets m_Offsets;

private:

    Color m_pOldData[3];
};
extern CMatSysPatch g_MatSysPatch;
