#pragma once
#include "hooking.h"
#include "config.h"

namespace Effects
{
    extern "C" void CreateHoodFXInLot();
    extern "C" void GetLotSkirtOverlayManager();
    extern "C" void FixDecalOverlayManager();
    extern "C" void ResetOverlayManager();
    extern "C" void PreventCullingOverlays();
    extern "C" void PreventCullingDecals();
    extern "C" void GetCurrentMaterial();
    extern "C" void ColourDecals();
}