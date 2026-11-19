#pragma once

struct cVector
{
    float x, y, z;
};

typedef cVector cSlice;

struct cQuaternion
{
    float x, y, z, w;
};

struct cRZRect
{
    int minX, minY, maxX, maxY;
};