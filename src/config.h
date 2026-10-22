#pragma once
#include <unordered_set>

namespace Config
{
    extern std::unordered_set<const char *> blacklistedFX;

    void Init();
}