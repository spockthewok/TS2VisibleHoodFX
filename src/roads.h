#pragma once
#define _USE_MATH_DEFINES
#include "hooking.h"
#include <math.h>

namespace Roads
{
    void AllowBridgesInLot();
    void PreserveOccupantManager();
    extern "C" void SkipRoadsDestructor();
    extern "C" void TranslateBridgesToLot();
    extern "C" void RotateBridges();
}