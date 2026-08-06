#include "core.h"

namespace Core
{
    static void InjectPatches()
    {
        Hooking::MakeJMP((BYTE *)0xAD3B73, (DWORD)Effects::CreateHoodFXInLot, 6);
        Hooking::MakeJMP((BYTE *)0xA84A35, (DWORD)Effects::GetLotSkirtOverlayManager, 6);
        Hooking::MakeJMP((BYTE *)0x101C317, (DWORD)Effects::FixDecalOverlayManager, 6);
        Hooking::MakeJMP((BYTE *)0xADA612, (DWORD)Effects::ResetOverlayManager, 6);
        Hooking::MakeJMP((BYTE *)0xB69D73, (DWORD)Effects::PreventCullingDecals, 6);
        Hooking::MakeJMP((BYTE *)0xB677B5, (DWORD)Effects::GetCurrentMaterial, 6);
        Hooking::MakeJMP((BYTE *)0xB67B10, (DWORD)Effects::ColourDecals, 7);
    }

    void Init()
    {
        Config::Init();
        InjectPatches();
    }
}