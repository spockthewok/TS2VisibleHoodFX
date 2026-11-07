#include "hooking.h"

namespace Hooking
{
    void WriteToMemory(DWORD addressToWrite, const BYTE *valueToWrite, size_t byteNum)
    {
        DWORD oldProtection;
        BYTE *pAddress = reinterpret_cast<BYTE *>(addressToWrite);

        VirtualProtect(pAddress, byteNum, PAGE_EXECUTE_READWRITE, &oldProtection);
        memcpy(pAddress, valueToWrite, byteNum);
        VirtualProtect(pAddress, byteNum, oldProtection, NULL);
    }

    void MakeJMP(DWORD jmpFrom, DWORD jmpTo, size_t byteNum)
    {
        DWORD oldProtection, backup, relAddr;
        BYTE *pAddress = reinterpret_cast<BYTE *>(jmpFrom);

        VirtualProtect(pAddress, byteNum, PAGE_EXECUTE_READWRITE, &oldProtection);
        relAddr = (jmpTo - jmpFrom) - 0x5;
        *pAddress = 0xE9;
        *reinterpret_cast<DWORD *>(pAddress + 0x1) = relAddr;

        for (DWORD x = 0x5; x < byteNum; ++x)
            *(pAddress + x) = 0x90;

        VirtualProtect(pAddress, byteNum, oldProtection, &backup);
    }
}