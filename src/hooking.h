#pragma once
#include "headers.h"

namespace Hooking
{
	void MakeJMP(BYTE *pAddress, DWORD dwJumpTo, DWORD dwLen);
}