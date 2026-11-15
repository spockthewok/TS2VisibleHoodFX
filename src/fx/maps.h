#pragma once

namespace Maps
{
    extern "C" void FixParticleEffectMap();
    extern "C" void EnableWaterSparkles();
    extern "C" void FixMetaParticleEffectMap();
    extern "C" void IncreaseLotBoundingRect();
    float GetRelativeWaterLevel();
    extern "C" void GetNHoodNormalData();
    void HandleNegativeValues();
}