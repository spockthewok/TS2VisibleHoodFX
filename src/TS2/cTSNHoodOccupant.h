#pragma once

class cTSNHoodOccupant
{
public:
    void GetPosition(float *coords);
};

// These are child classes of cTSNHoodOccupant
// We don't use any of their unique methods, so we're typedefing for the sake of correctness
typedef cTSNHoodOccupant cTSNHoodPropOccupant;
typedef cTSNHoodOccupant cTSNHoodRoadOccupantWithModel;