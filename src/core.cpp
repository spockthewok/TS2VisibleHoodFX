#include "core.h"
#include "hooking.h"
#include "config.h"
#include "TS2/base.h"
#include "fx/main.h"
#include "fx/decals.h"
#include "fx/maps.h"
#include "fx/roads.h"

namespace Core
{
    static void InjectPatches()
    {
        // Main
        Hooking::MakeJMP((BYTE *)(0x6D3B73 + Module::base), (DWORD)Effects::CreateHoodFXInLot, 6);

        // Decals
        Hooking::MakeJMP((BYTE *)(0x684A35 + Module::base), (DWORD)Decals::GetLotSkirtOverlayManager, 6);
        Hooking::MakeJMP((BYTE *)(0xC1C317 + Module::base), (DWORD)Decals::FixDecalOverlayManager, 6);
        Hooking::MakeJMP((BYTE *)(0x6DA612 + Module::base), (DWORD)Decals::ResetOverlayManager, 6);
        Hooking::MakeJMP((BYTE *)(0x769D73 + Module::base), (DWORD)Decals::PreventCullingDecals, 6);
        Hooking::MakeJMP((BYTE *)(0x766AB6 + Module::base), (DWORD)Decals::IncreaseOverlayBoundingBox, 7);
        Hooking::MakeJMP((BYTE *)(0x7677B5 + Module::base), (DWORD)Decals::GetCurrentMaterial, 6);
        Hooking::MakeJMP((BYTE *)(0x767B10 + Module::base), (DWORD)Decals::ColourDecals, 7);
        Hooking::MakeJMP((BYTE *)(0x76A1FC + Module::base), (DWORD)Decals::CheckOverlayBounds, 5);

        // Effect Maps
        Hooking::MakeJMP((BYTE *)(0xBE2EB5 + Module::base), (DWORD)Maps::FixParticleEffectMap, 5);
        Hooking::MakeJMP((BYTE *)(0xC21295 + Module::base), (DWORD)Maps::EnableWaterSparkles, 5);
        Hooking::MakeJMP((BYTE *)(0xC21364 + Module::base), (DWORD)Maps::FixMetaParticleEffectMap, 5);
        Hooking::MakeJMP((BYTE *)(0x77EBAE + Module::base), (DWORD)Maps::IncreaseLotBoundingRect, 5);
        Hooking::MakeJMP((BYTE *)(0x77ECB0 + Module::base), (DWORD)Maps::GetRelativeWaterLevel, 6);
        Maps::HandleNegativeValues();

        // Bridges
        if (Config::lotBridges)
        {
            Roads::AllowBridgesInLot();
            Roads::PreserveOccupantManager();
            Hooking::MakeJMP((BYTE *)(0x6D55E2 + Module::base), (DWORD)Roads::SkipRoadsDestructor, 12);
            Hooking::MakeJMP((BYTE *)(0x6D210D + Module::base), (DWORD)Roads::TranslateBridgesToLot, 6);
            Hooking::MakeJMP((BYTE *)(0x6D214A + Module::base), (DWORD)Roads::RotateBridges, 7);
        }
    }

    void Init()
    {
        Module::base = reinterpret_cast<DWORD>(GetModuleHandleA(NULL));

        Config::Init();
        InjectPatches();
    }
}