#include "TS2/nGZNumerics.h"
#include <cmath>

namespace nGZNumerics
{
    void Normalize(cSlice *normal)
    {
        float invLen = 1.0f / std::sqrt((normal->z * normal->z) +
                                        (normal->y * normal->y) +
                                        (normal->x * normal->x));

        normal->x *= invLen;
        normal->y *= invLen;
        normal->z *= invLen;
    }
}