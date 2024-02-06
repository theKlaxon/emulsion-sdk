#pragma once

class Offset {
public:

    Offset() {}

    Offset(const char* _dllName, unsigned int _offset);

    const char*     GetDllName()    { return dllName; }
    unsigned int    GetOffset()     { return offset; }
    unsigned int    GetAddress()    { return address; }

protected:

    const char* dllName;
    unsigned int offset;
    unsigned int address;
};