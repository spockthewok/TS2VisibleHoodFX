#pragma once
#include "headers.h"
#include "hooking.h"

namespace Effects
{
    bool IsBlacklistedEffect(const char *currEffect);
    extern "C" void CreateHoodFXInLot();
    extern "C" void GetLotSkirtOverlayManager();
    extern "C" void FixDecalOverlayManager();
    extern "C" void ResetOverlayManager();
    extern "C" void PreventCullingDecals();
    void ColourDecals();
}