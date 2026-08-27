#pragma once
#include "headers.h"

namespace Hooking
{
	void WriteToMemory(DWORD addressToWrite, const BYTE *valueToWrite, int byteNum);
	void MakeJMP(BYTE *pAddress, DWORD dwJumpTo, DWORD dwLen);
}