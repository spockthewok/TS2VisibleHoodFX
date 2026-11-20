#include "TS2/cTerrainGeometryData.h"
#include "TS2/base.h"
#include "TS2/cNHoodTerrain.h"

// Height and Normal are never called without nhood effects activated in lot view
// Safe to replace them to use nhood data without affecting standard lot effects
namespace nTSSG
{
    float cTerrainGeometryData::Height(float x, float y)
    {
        cNHoodTerrain &nhoodTerrain = cNHoodTerrain::NHoodTerrain();

        return nhoodTerrain.Height(x, y);
    }

    // Ghidra thinks this is a void method, but its callers try to dereference EAX after it returns
    cSlice *cTerrainGeometryData::Normal(cSlice *normal, float x, float y)
    {
        cNHoodTerrain &nhoodTerrain = cNHoodTerrain::NHoodTerrain();

        return nhoodTerrain.Normal(normal, x, y);
    }

    // Lot effect map always returns 0 for water level, regardless of lot height above sea level
    // Our version does the same calculations as cLotSkirt::ComputeLotSkirtParameters
    // Returns sea level relative to lot z, ensuring water effects spawn at the correct height
    // Can't rely on the cLotSkirt method as some effects init before it's executed
    float cTerrainGeometryData::WaterLevel()
    {
        cTSGlobals *globals = TS::Globals();
        cTSNHoodTerrain *nhoodTerrain = globals->NeighborhoodTerrain();
        cTSGameStateController *gameState = TS::GameStateController();
        cTSLotInfo *lotInfo = gameState->CurrentLotInfo();

        if (!nhoodTerrain || !lotInfo)
            return 0.0f;

        return nhoodTerrain->SeaLevel() - lotInfo->NHoodToLotHeightOffset();
    }
}