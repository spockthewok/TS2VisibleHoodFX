#pragma once
#include "headers.h"

namespace Common
{
    template <typename Fn, typename T>
    Fn GetVTableFn(T *object, const DWORD offset)
    {
        constexpr size_t indexSize = sizeof(uintptr_t);
        auto vTable = *reinterpret_cast<uintptr_t **>(object);

        return reinterpret_cast<Fn>(vTable[offset / indexSize]);
    }
}