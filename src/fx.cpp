#include "fx.h"
#include "headers.h"
#include "config.h"

namespace
{
    const DWORD CreateSceneGraphNodeForPropOccupant_Exit_1 = 0xAD3B85;
    const DWORD CreateSceneGraphNodeForPropOccupant_Exit_2 = 0xAD3BAA;
}

// This is the main driver for activating neighbourhood effects in lot view
namespace Effects
{
    static bool IsBlacklistedEffect(const char *currEffect)
    {
        if (!currEffect)
            return true;

        for (const char *effect : Config::blacklistedFX)
        {
            if (_stricmp(currEffect, effect) == 0)
                return true;
        }

        return false;
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
            call IsBlacklistedEffect
            add esp,0x4
            test al,al
            jnz LAB_Skip
        LAB_CreateEffect:
            jmp CreateSceneGraphNodeForPropOccupant_Exit_1
        LAB_Skip:
            jmp CreateSceneGraphNodeForPropOccupant_Exit_2
        }
    }
}