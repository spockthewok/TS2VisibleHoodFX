#pragma once
#include <unordered_set>
#include <string>

namespace Config
{
    extern std::unordered_set<std::string> blacklistGlobal;
    extern std::unordered_set<std::string> blacklistLots;

    void Init();
}