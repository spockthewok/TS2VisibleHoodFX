#include "hooking.h"

namespace Hooking
{
    void WriteToMemory(DWORD addressToWrite, const BYTE *valueToWrite, int byteNum)
    {
        unsigned long OldProtection;

        VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
        memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);
        VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, NULL);
    }

    void MakeJMP(BYTE *pAddress, DWORD dwJumpTo, DWORD dwLen)
    {
        DWORD dwOldProtect, dwBkup, dwRelAddr;

        VirtualProtect(pAddress, dwLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);
        dwRelAddr = (DWORD)(dwJumpTo - (DWORD)pAddress) - 5;
        *pAddress = 0xE9;
        *((DWORD *)(pAddress + 0x1)) = dwRelAddr;

        for (DWORD x = 0x5; x < dwLen; x++)
            *(pAddress + x) = 0x90;

        VirtualProtect(pAddress, dwLen, dwOldProtect, &dwBkup);

        return;
    }
}