#pragma once
#include "cbase.h"
#include "game/shared/portal2/paint_enum.h"
#include "dll_offset.h"

extern void PatchAll();

class CMatSysPatch {
public:
    
    CMatSysPatch() {
        m_Offsets = MatSysOffsets();
    }

    // override the paint colors >:D (going goblin mode)
    void Patch();

protected:

    class MatSysOffsets {
    public:

        MatSysOffsets() {
            g_PaintColors = Offset("materialsystem.dll", 0x115ad4);
        }

        Offset g_PaintColors;
    };

    MatSysOffsets m_Offsets;// = MatSysOffsets(); // can't do this in the 2010 compiler...
};
extern CMatSysPatch g_MatSysPatch;

class CEnginePatch {
public:

    CEnginePatch() {
        m_Offsets = EngineOffsets();
    }

    void Patch();

    class EngineOffsets {
    public:

        EngineOffsets() {
            g_HostState_Shutdown = Offset("engine.dll", 0x101a6bc0);
            g_HostState = Offset("engine.dll", 0x1043f7d4);
        }

        Offset g_HostState_Shutdown;
        Offset g_HostState;

    };

    EngineOffsets m_Offsets;
};
extern CEnginePatch g_EnginePatch;