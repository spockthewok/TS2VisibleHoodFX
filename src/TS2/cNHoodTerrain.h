#pragma once
#include "TS2/types.h"
#include <vector>

// This is a custom singleton class based on the game's cNHoodTerrain class
// We need our own independent version, as the game destroys its object when loading a lot
namespace nTSSG
{
    class cNHoodTerrain
    {
    private:
        float gridScale;
        int gridWidth;
        std::vector<cVector> heightMap;
        std::vector<cSlice> normals;

        cNHoodTerrain();

    public:
        cNHoodTerrain(const cNHoodTerrain &) = delete;
        cNHoodTerrain &operator=(const cNHoodTerrain &) = delete;

        static cNHoodTerrain &NHoodTerrain();
        void UpdateTerrainHeightmap(const cVector *first, const cVector *last);
        void UpdateTerrainNormals(const cSlice *first, const cSlice *last);
        float Height(float x, float y);
        cSlice *Normal(cSlice *normal, float x, float y);
    };
}