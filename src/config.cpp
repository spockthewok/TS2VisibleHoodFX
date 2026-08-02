#include "config.h"

namespace
{
    mINI::INIStructure ini;
    const char *configNameBase = "TS2VisibleHoodFX.ini";
    const char *configNameRPC = "mods/TS2VisibleHoodFX.ini";

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

    static void PopulateBlacklist(const std::string &section)
    {
        auto &collection = ini[section];

        for (auto const &[key, value] : collection)
        {
            if (!value.empty())
                blacklistedFX.push_back(value.c_str());
        }
    }

    void Init()
    {
        const char *configName = configNameBase;

        if (std::filesystem::exists(configNameRPC))
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