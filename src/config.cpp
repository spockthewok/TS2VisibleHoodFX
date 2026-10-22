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
    std::unordered_set<const char *> blacklistedFX;

    static std::string GetModuleName()
    {
        char buffer[MAX_PATH];

        DWORD pathLen = GetModuleFileNameA(nullptr, buffer, MAX_PATH);

        if (pathLen == 0 || pathLen == MAX_PATH)
            return "";

        std::filesystem::path exePath(buffer);
        return exePath.filename().string();
    }

    static void PopulateBlacklist(const std::string &section)
    {
        const auto &collection = ini[section];

        for (const auto &[key, value] : collection)
        {
            if (!value.empty())
                blacklistedFX.insert(value.c_str());
        }
    }

    void Init()
    {
        std::filesystem::path configName = configNameBase;

        if (GetModuleName().find("RPC.exe") != std::string::npos)
            configName = configNameRPC;

        mINI::INIFile file(configName);

        if (!file.read(ini) || !ini.has("Blacklist"))
            return;

        PopulateBlacklist("Blacklist");
    }
}