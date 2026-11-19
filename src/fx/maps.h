#pragma once
#include "TS2/cTerrainGeometryData.h"

namespace Maps
{
    extern "C" void FixParticleEffectMap();
    extern "C" void EnableWaterSparkles();
    extern "C" void FixMetaParticleEffectMap();
    extern "C" void GetNHoodHeightAndNormalData();
    extern "C" void IncreaseLotBoundingRect();
    float __fastcall WaterLevelHook(nTSSG::cTerrainGeometryData *terrainGeomData);
    void HandleNegativeValues();
}