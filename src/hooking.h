#pragma once
#include "headers.h"

namespace Hooking
{
    void WriteToMemory(DWORD addressToWrite, const BYTE *valueToWrite, size_t byteNum);
    void MakeJMP(DWORD jmpFrom, DWORD jmpTo, size_t byteNum);
}