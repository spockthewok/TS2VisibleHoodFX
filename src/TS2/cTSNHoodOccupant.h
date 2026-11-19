#pragma once
#include "TS2/types.h"

class cTSNHoodOccupant
{
public:
    void GetPosition(cVector *coords);
};

// These are child classes of cTSNHoodOccupant
// We don't use any of their unique methods, so we're typedefing for the sake of correctness
typedef cTSNHoodOccupant cTSNHoodPropOccupant;
typedef cTSNHoodOccupant cTSNHoodRoadOccupantWithModel;