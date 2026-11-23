#include "TS2/cNHoodTerrain.h"
#include "common.h"
#include "TS2/base.h"
#include "TS2/nGZNumerics.h"
#include <cmath>

// Height and Normal are rough recreations of the original methods, tailored for use in lot view
namespace nTSSG
{
    cNHoodTerrain::cNHoodTerrain()
    {
        this->gridScale = 10.0f;
        this->gridWidth = static_cast<int>(Common::GetNHoodSizeX() / this->gridScale) + 1;
    }

    cNHoodTerrain &cNHoodTerrain::NHoodTerrain()
    {
        static cNHoodTerrain nhoodTerrain;
        return nhoodTerrain;
    }

    void cNHoodTerrain::UpdateTerrainHeightmap(const cVector *first, const cVector *last)
    {
        this->heightMap.assign(first, last);
    }

    void cNHoodTerrain::UpdateTerrainNormals(const cSlice *first, const cSlice *last)
    {
        this->normals.assign(first, last);
    }

    float cNHoodTerrain::Height(float x, float y)
    {
        cTSGameStateController *gameState = TS::GameStateController();
        cTSLotInfo *lotInfo = gameState->CurrentLotInfo();

        if (!lotInfo)
            // Return large value so later checks fail
            return static_cast<float>(std::numeric_limits<UINT>::max());

        float currX = x, currY = y;

        Common::TranslateEffectCoordsToNHoodCoords(currX, currY);

        currX /= this->gridScale;
        currY /= this->gridScale;
        // Game does slightly more involved floating-point maths for gridX/gridY conversion
        // Seems to just round towards 0, which a direct cast already does
        int gridX = static_cast<int>(currX);
        int gridY = static_cast<int>(currY);
        float tX = currX - static_cast<float>(gridX);
        float tY = currY - static_cast<float>(gridY);

        // I'm assuming the coord system is the same as a 2D array, where (0,0) is top left
        cVector h00 = this->heightMap[gridX + (gridY * this->gridWidth)];
        cVector h10 = this->heightMap[(gridX + 1) + (gridY * this->gridWidth)];
        cVector h01 = this->heightMap[gridX + ((gridY + 1) * this->gridWidth)];
        cVector h11 = this->heightMap[(gridX + 1) + ((gridY + 1) * this->gridWidth)];

        float top = std::lerp(h00.z, h10.z, tX);
        float bottom = std::lerp(h01.z, h11.z, tX);
        float terrainHeight = std::lerp(top, bottom, tY);

        if (!std::isfinite(terrainHeight) || terrainHeight == 0)
            return static_cast<float>(std::numeric_limits<UINT>::max());

        // Game will subtract water level from this value
        // Because lot water level is negative, calculation becomes height + lotWaterLvl
        // Equivalent to terrainHeight - nhoodWaterLvl, which is what nhood uses to get height
        return terrainHeight - lotInfo->NHoodToLotHeightOffset();
    }

    // Ghidra thinks this is a void method, but its callers try to dereference EAX after it returns
    cSlice *cNHoodTerrain::Normal(cSlice *normal, float x, float y)
    {
        float currX = x, currY = y;

        Common::TranslateEffectCoordsToNHoodCoords(currX, currY);

        currX /= this->gridScale;
        currY /= this->gridScale;
        int gridX = static_cast<int>(currX);
        int gridY = static_cast<int>(currY);
        float tX = currX - static_cast<float>(gridX);
        float tY = currY - static_cast<float>(gridY);

        cSlice n00 = this->normals[gridX + (gridY * this->gridWidth)];
        cSlice n10 = this->normals[(gridX + 1) + (gridY * this->gridWidth)];
        cSlice n01 = this->normals[gridX + ((gridY + 1) * this->gridWidth)];
        cSlice n11 = this->normals[(gridX + 1) + ((gridY + 1) * this->gridWidth)];

        cSlice top, bottom;

        lerp(&top, n00.x, n00.y, n00.z, n10.x, n10.y, n10.z, tX);
        lerp(&bottom, n01.x, n01.y, n01.z, n11.x, n11.y, n11.z, tX);
        lerp(normal, top.x, top.y, top.z, bottom.x, bottom.y, bottom.z, tY);

        nGZNumerics::Normalize(normal);

        return normal;
    }
}