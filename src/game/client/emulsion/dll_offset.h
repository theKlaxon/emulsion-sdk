#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Offset {
public:

    Offset() {}

    Offset(const char* _dllName, int _offset) {
        dllName = _dllName;
        offset = _offset;

        address = (uintptr_t)GetModuleHandleA(dllName) + offset;
    }

    const char* GetDllName() { return dllName; }
    int         GetOffset() { return offset; }
    int         GetAddress() { return address; }

protected:

    const char* dllName;
    int offset;
    int address;
};