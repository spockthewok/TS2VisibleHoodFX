#include "core.h"
#include "hooking.h"
#include "config.h"
#include "fx.h"
#include "decals.h"
#include "maps.h"
#include "roads.h"

namespace Core
{
    static void InjectPatches()
    {
        Hooking::MakeJMP((BYTE *)0xAD3B73, (DWORD)Effects::CreateHoodFXInLot, 6);
        Hooking::MakeJMP((BYTE *)0xA84A35, (DWORD)Decals::GetLotSkirtOverlayManager, 6);
        Hooking::MakeJMP((BYTE *)0x101C317, (DWORD)Decals::FixDecalOverlayManager, 6);
        Hooking::MakeJMP((BYTE *)0xADA612, (DWORD)Decals::ResetOverlayManager, 6);
        Hooking::MakeJMP((BYTE *)0xFB00E1, (DWORD)Decals::PreventCullingOverlays, 8);
        Hooking::MakeJMP((BYTE *)0xB69D73, (DWORD)Decals::PreventCullingDecals, 6);
        Hooking::MakeJMP((BYTE *)0xB677B5, (DWORD)Decals::GetCurrentMaterial, 6);
        Hooking::MakeJMP((BYTE *)0xB67B10, (DWORD)Decals::ColourDecals, 7);
        Hooking::MakeJMP((BYTE *)0x1021364, (DWORD)Maps::FixMetaParticleEffectMap, 5);
        Hooking::MakeJMP((BYTE *)0xB7EBAE, (DWORD)Maps::IncreaseLotBoundingRect, 5);

        Roads::AllowBridgesInLot();
        Roads::PreserveOccupantManager();
        Hooking::MakeJMP((BYTE *)0xAD55E2, (DWORD)Roads::SkipRoadsDestructor, 12);
        Hooking::MakeJMP((BYTE *)0xAD210D, (DWORD)Roads::TranslateBridgesToLot, 6);
        Hooking::MakeJMP((BYTE *)0xAD214A, (DWORD)Roads::RotateBridges, 7);
    }

    void Init()
    {
        Config::Init();
        InjectPatches();
    }
}