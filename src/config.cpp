#include "config.h"
#include "headers.h"
#include "include/ini.h"

namespace
{
    mINI::INIStructure ini;
    const std::filesystem::path configNameBase = "TS2VisibleHoodFX.ini";
    const std::filesystem::path configNameRPC = "mods/TS2VisibleHoodFX.ini";

    // These effects are bugged due to lack of lot skirt effect map
    const std::vector<const char *> blacklistDefaults = {"neighborhood_boulder_sailboats",
                                                         "neighborhood_boulder_punts",
                                                         "neighborhood_boulder_birds",
                                                         "neighborhood_boulder_ducks",
                                                         "neighborhood_boulder_swans",
                                                         "neighborhood_boulder_surf",
                                                         "neighborhood_boulder_twikkiiSurf"};
}

namespace Config
{
    std::vector<const char *> blacklistedFX;

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
                blacklistedFX.push_back(value.c_str());
        }
    }

    void Init()
    {
        std::filesystem::path configName = configNameBase;

        if (GetModuleName().find("RPC.exe") != std::string::npos)
            configName = configNameRPC;

        mINI::INIFile file(configName);

        if (!file.read(ini) || !ini.has("Blacklist"))
        {
            blacklistedFX = blacklistDefaults;
            return;
        }

        PopulateBlacklist("Blacklist");
    }
}