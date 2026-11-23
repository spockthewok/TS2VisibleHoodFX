#include "TS2/cTerrainGeometryData.h"
#include "common.h"
#include "TS2/base.h"
#include "TS2/cNHoodTerrain.h"
#include "TS2/nGZNumerics.h"
#include <cmath>
#include <vector>

namespace nTSSG
{
    float cTerrainGeometryData::Height(float x, float y)
    {
        if (!Common::IsInLotBounds(x, y)) // Pond fish fx rely on lot data
        {
            cNHoodTerrain &nhoodTerrain = cNHoodTerrain::NHoodTerrain();
            return nhoodTerrain.Height(x, y);
        }

        std::vector<cVector> &heightMap = *reinterpret_cast<std::vector<cVector> *>(this + 0x8);
        int gridWidth = *reinterpret_cast<int *>(this + 0x188);
        float gridScale = 1.0f / *reinterpret_cast<float *>(this + 0x184);
        float currX = x * gridScale;
        float currY = y * gridScale;
        int gridX = static_cast<int>(currX);
        int gridY = static_cast<int>(currY);
        float tX = currX - static_cast<float>(gridX);
        float tY = currY - static_cast<float>(gridY);

        cVector h00 = heightMap[(gridX * gridWidth) + (gridY * 2)];
        cVector h10 = heightMap[((gridX + 1) * gridWidth) + (gridY * 2)];
        cVector h01 = heightMap[(gridX * gridWidth) + ((gridY + 1) * 2)];
        cVector h11 = heightMap[((gridX + 1) * gridWidth) + ((gridY + 1) * 2)];

        float top = std::lerp(h00.z, h10.z, tX);
        float bottom = std::lerp(h01.z, h11.z, tX);
        float height = std::lerp(top, bottom, tY);

        if (!std::isfinite(height) || height == 0)
            return static_cast<float>(std::numeric_limits<UINT>::max());

        return height;
    }

    cSlice *cTerrainGeometryData::Normal(cSlice *normal, float x, float y)
    {
        if (!Common::IsInLotBounds(x, y))
        {
            cNHoodTerrain &nhoodTerrain = cNHoodTerrain::NHoodTerrain();
            return nhoodTerrain.Normal(normal, x, y);
        }

        std::vector<cSlice> &normals = *reinterpret_cast<std::vector<cSlice> *>(this + 0x14);
        int gridWidth = *reinterpret_cast<int *>(this + 0x188);
        float gridScale = 1.0f / *reinterpret_cast<float *>(this + 0x184);
        float currX = x * gridScale;
        float currY = y * gridScale;
        int gridX = static_cast<int>(currX);
        int gridY = static_cast<int>(currY);
        float tX = currX - static_cast<float>(gridX);
        float tY = currY - static_cast<float>(gridY);

        cSlice n00 = normals[(gridX * gridWidth) + (gridY * 2)];
        cSlice n10 = normals[((gridX + 1) * gridWidth) + (gridY * 2)];
        cSlice n01 = normals[(gridX * gridWidth) + ((gridY + 1) * 2)];
        cSlice n11 = normals[((gridX + 1) * gridWidth) + ((gridY + 1) * 2)];

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