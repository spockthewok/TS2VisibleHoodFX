#pragma once
#include "headers.h"
#include "hooking.h"
#include "config.h"

namespace Effects
{
    extern "C" void CreateHoodFXInLot();
    extern "C" void GetLotSkirtOverlayManager();
    extern "C" void FixDecalOverlayManager();
    extern "C" void ResetOverlayManager();
    extern "C" void PreventCullingDecals();
    void ColourDecals();
}