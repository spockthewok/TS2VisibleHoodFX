#include "TS2/cTerrainGeometryData.h"
#include "common.h"
#include "TS2/base.h"
#include "TS2/cNHoodTerrain.h"
#include "TS2/nGZNumerics.h"
#include <cmath>

// Height & Normal are rough recreations of cNHoodTerrain methods of the same names
// Need our own versions as cNHoodTerrain object is destroyed in lot view, so can't use its methods
// cTerrainGeometryData equivalents are never called without nhood effects activated in lot view
// Safe to replace them to use nhood data without affecting standard lot effects
namespace nTSSG
{
    float cTerrainGeometryData::Height(float x, float y)
    {
        cTSGameStateController *gameState = TS::GameStateController();
        cTSLotInfo *lotInfo = gameState->CurrentLotInfo();

        if (!lotInfo)
            // Return large value so later checks fail
            return static_cast<float>(std::numeric_limits<UINT>::max());

        float currX = x, currY = y;

        Common::TranslateEffectCoordsToNHoodCoords(currX, currY);

        currX /= nhoodTerrain.gridScale;
        currY /= nhoodTerrain.gridScale;
        // Game does slightly more involved floating-point maths for gridX/gridY conversion
        // Seems to just round towards 0, which a direct cast already does
        int gridX = static_cast<int>(currX);
        int gridY = static_cast<int>(currY);
        float tX = currX - static_cast<float>(gridX);
        float tY = currY - static_cast<float>(gridY);

        // I'm assuming the coord system is the same as a 2D array, where (0,0) is top left
        cVector h00 = nhoodTerrain.heightMap[gridX + (gridY * nhoodTerrain.gridWidth)];
        cVector h10 = nhoodTerrain.heightMap[(gridX + 1) + (gridY * nhoodTerrain.gridWidth)];
        cVector h01 = nhoodTerrain.heightMap[gridX + ((gridY + 1) * nhoodTerrain.gridWidth)];
        cVector h11 = nhoodTerrain.heightMap[(gridX + 1) + ((gridY + 1) * nhoodTerrain.gridWidth)];

        float top = std::lerp(h00.z, h10.z, tX);
        float bottom = std::lerp(h01.z, h11.z, tX);
        float terrainHeight = std::lerp(top, bottom, tY);

        if (std::isnan(terrainHeight) || std::isinf(terrainHeight) || terrainHeight == 0)
            return static_cast<float>(std::numeric_limits<UINT>::max());

        // Game will subtract water level from this value
        // Because lot water level is negative, calculation becomes height + lotWaterLvl
        // Equivalent to terrainHeight - nhoodWaterLvl, which is what nhood uses to get height
        return terrainHeight - lotInfo->NHoodToLotHeightOffset();
    }

    // Ghidra thinks this is a void method, but its callers try to dereference EAX after it returns
    cSlice *cTerrainGeometryData::Normal(cSlice *normal, float x, float y)
    {
        float currX = x, currY = y;

        Common::TranslateEffectCoordsToNHoodCoords(currX, currY);

        currX /= nhoodTerrain.gridScale;
        currY /= nhoodTerrain.gridScale;
        int gridX = static_cast<int>(currX);
        int gridY = static_cast<int>(currY);
        float tX = currX - static_cast<float>(gridX);
        float tY = currY - static_cast<float>(gridY);

        cSlice n00 = nhoodTerrain.normals[gridX + (gridY * nhoodTerrain.gridWidth)];
        cSlice n10 = nhoodTerrain.normals[(gridX + 1) + (gridY * nhoodTerrain.gridWidth)];
        cSlice n01 = nhoodTerrain.normals[gridX + ((gridY + 1) * nhoodTerrain.gridWidth)];
        cSlice n11 = nhoodTerrain.normals[(gridX + 1) + ((gridY + 1) * nhoodTerrain.gridWidth)];

        cSlice top, bottom;

        lerp(&top, n00.x, n00.y, n00.z, n10.x, n10.y, n10.z, tX);
        lerp(&bottom, n01.x, n01.y, n01.z, n11.x, n11.y, n11.z, tX);
        lerp(normal, top.x, top.y, top.z, bottom.x, bottom.y, bottom.z, tY);

        nGZNumerics::Normalize(normal);

        return normal;
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