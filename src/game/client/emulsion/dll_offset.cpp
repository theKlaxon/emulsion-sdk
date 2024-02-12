#include "cbase.h"
#include "dll_offset.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Offset::Offset(const char* _dllName, unsigned int _offset) {
    dllName = _dllName;
    offset = _offset;

    address = (uintptr_t)GetModuleHandleA(dllName) + offset;
}