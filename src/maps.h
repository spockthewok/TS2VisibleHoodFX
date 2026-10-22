#pragma once

namespace Maps
{
    extern "C" void FixParticleEffectMap();
    extern "C" void FixMetaParticleEffectMap();
    extern "C" void IncreaseLotBoundingRect();
    float GetRelativeWaterLevel();
    void HandleNegativeValues();
}