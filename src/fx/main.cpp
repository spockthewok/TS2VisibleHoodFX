#include "fx/main.h"
#include "headers.h"
#include "config.h"
#include "TS2/base.h"
#include "TS2/cTSNHoodPropOccupant.h"
#include <array>

namespace
{
    const DWORD CreateSceneGraphNodeForPropOccupant_Exit_1 = 0xAD3B85;
    const DWORD CreateSceneGraphNodeForPropOccupant_Exit_2 = 0xAD3BAA;
}

// This is the main driver for activating neighbourhood effects in lot view
namespace Effects
{
    static bool IsOccupantInLotBounds(cTSNHoodPropOccupant *propOccupant)
    {
        nTSWorld::cWorldDB *worldDB = TS::WorldDB();

        if (!worldDB)
            return false;

        std::array<float, 3> coords = {0.0f, 0.0f, 0.0f};

        propOccupant->GetPosition(coords.data());
        OffsetPositionToLot(coords.data());

        float propX = coords[0];
        float propY = coords[1];
        float lotX = worldDB->LotSizeX();
        float lotY = worldDB->LotSizeY();

        if ((propX >= 0 && propX <= lotX) && (propY >= 0 && propY <= lotY))
            return true;

        return false;
    }

    static bool IsBlacklistedEffect(cTSNHoodPropOccupant *propOccupant, const char *currProp)
    {
        if (!propOccupant || !currProp)
            return true;

        std::string propName(currProp);

        if (Config::blacklistLots.contains(propName))
            return IsOccupantInLotBounds(propOccupant);

        return Config::blacklistGlobal.contains(propName);
    }

    // cNHoodOccupantManager::CreateSceneGraphNodeForPropOccupant
    // Allows non-blacklisted neighbourhood effects to be created in lot view
    void __declspec(naked) CreateHoodFXInLot()
    {
        __asm {
            mov al,[ebp+0xC1] // bool inNeighbourhood
            test al,al
            mov byte ptr [esp+0xA0],0x5
            jnz LAB_CreateEffect
        LAB_InLot:
            push [esp+0x30] // Name of current neighbourhood object stored on stack
            push [esp+0x28]
            call IsBlacklistedEffect
            add esp,0x8
            test al,al
            jnz LAB_Skip
        LAB_CreateEffect:
            jmp CreateSceneGraphNodeForPropOccupant_Exit_1
        LAB_Skip:
            jmp CreateSceneGraphNodeForPropOccupant_Exit_2
        }
    }
}