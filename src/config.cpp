#include "config.h"
#include "headers.h"
#include "include/ini.h"

namespace
{
    mINI::INIStructure ini;
    const std::filesystem::path configNameBase = "TS2VisibleHoodFX.ini";
    const std::filesystem::path configNameRPC = "mods/TS2VisibleHoodFX.ini";
}

namespace Config
{
    std::unordered_set<std::string> blacklistGlobal;
    std::unordered_set<std::string> blacklistLots;

    static std::string GetModuleName()
    {
        char buffer[MAX_PATH];

        DWORD pathLen = GetModuleFileNameA(nullptr, buffer, MAX_PATH);

        if (pathLen == 0 || pathLen == MAX_PATH)
            return "";

        std::filesystem::path exePath(buffer);
        return exePath.filename().string();
    }

    static void PopulateBlacklist(const std::string &section, std::unordered_set<std::string> &blacklist)
    {
        if (!ini.has(section))
            return;

        const auto &collection = ini[section];

        for (const auto &[key, value] : collection)
        {
            if constexpr (std::is_same_v<decltype(value), const std::string>)
            {
                if (!value.empty())
                    blacklist.insert(value);
            }
        }
    }

    void Init()
    {
        std::filesystem::path configName = configNameBase;

        if (GetModuleName().find("RPC.exe") != std::string::npos)
            configName = configNameRPC;

        mINI::INIFile file(configName);

        if (!file.read(ini))
            return;

        PopulateBlacklist("Global", blacklistGlobal);
        PopulateBlacklist("Lots", blacklistLots);
    }
}