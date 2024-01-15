#pragma once
//#include "cbase.h"
//#include "paint.h"
#include "dll_offset.h"

extern void PatchAll();

class CMatSysPatch {
public:
    
    // override the paint colors >:D (going goblin mode)
    void Patch();

protected:

    class MatSysOffsets {
    public:
        //Offset g_PaintColors = { "materialsystem.dll", 0x115ad4 };
    };

    MatSysOffsets m_Offsets = MatSysOffsets();
};
extern CMatSysPatch g_MatSysPatch;

class CEnginePatch {
public:

    void Patch();

protected:

    class EngineOffsets {
    public:

    };

    EngineOffsets m_Offsets = EngineOffsets();
};
extern CEnginePatch g_EnginePatch;

class CClientPatch {
public:

    void Patch();

protected:

    class ClientOffsets {
    public:

    };

    ClientOffsets m_Offsets = ClientOffsets();
};
extern CClientPatch g_ClientPatch;

class CServerPatch {
public:

    void Patch();

protected:

    class ServerOffsets {
    public:

    };

    ServerOffsets m_Offsets = ServerOffsets();
};
extern CServerPatch g_ServerPatch;