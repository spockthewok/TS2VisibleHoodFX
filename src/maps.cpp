#include "maps.h"
#include "TS2.h"
#include <string>

namespace
{
    const DWORD GetBoundingRect_Exit = 0xB7EBDE;
    const DWORD Init_Exit = 0x1021369;

    // All TS2 neighbourhoods are small SC4 maps of this size
    const int nhoodSize = 1280;
}

// This is for fixing issues with effects tied to an effect map
namespace Maps
{
    static bool IsInLot(const char *currLotName)
    {
        if (!currLotName)
            return false;

        std::string lotName(currLotName);

        // All lot names follow this structure: "NHoodID-LotX", where X is lot number
        // CAS lots are either "CAS!" or "YACAS!"
        // String will be blank if in neighbourhood
        if (!lotName.empty() && lotName.find("Lot"))
            return true;

        return false;
    }

    // cTSMetaParticlesEffect::Init
    // Majority of lot meta particle effects are started before game sets lot state and effect map
    // This causes them to be assigned to nhood effect map and therefore never appear in lot view
    // Game thinks we're still in nhood at this point, so we check name of lot being loaded to determine state
    // If we are loading a lot, we manually get lot effect map and assign it to meta particle
    void __declspec(naked) FixMetaParticleEffectMap()
    {
        __asm {
            mov ecx,eax
            call [edx+0x1C] // cTSEffectsMap::EffectMap
            mov [esp+0xC],eax // Stash returned effect map vtable pointer
            call TS::Globals
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x5C] // cTSGlobals::GameStateController
            test eax,eax
            jz LAB_Exit
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x24] // cTSGameStateController::CurrentLotInfo
            test eax,eax
            jz LAB_Exit
            push [eax+0x1C] // Object var holding current lot name
            call IsInLot
            add esp,0x4
            test al,al
            jz LAB_Exit
            mov eax,dword ptr ds:[0x1478F10] // nTSSG::TSSGSystem
            test eax,eax
            jz LAB_Exit
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x9C] // cTSSGSystem::Terrain
            test eax,eax
            jz LAB_Exit
            mov eax,[eax+0x40] // cTerrainGeometryData object
            add eax,0x8 // Object var holding vtable address of lot effect map methods
            cmp eax,[esp+0xC]
            je LAB_Exit
            mov [esp+0xC],eax
        LAB_Exit:
            mov eax,[esp+0xC]
            jmp cTSMetaParticlesEffect_Init_Exit
        }
    }

    // cTerrainGeometryData::GetBoundingRect
    // Meta particle effects can only exist within the bounds of their assigned effect map
    // Nhood effect map bounds cover entire nhood, lot effect map only covers size of current lot
    // This increases bounds of lot effect map to that of nhood, so effects can appear anywhere in world
    void __declspec(naked) IncreaseLotBoundingRect()
    {
        __asm {
            fild [nhoodSize]
            fstp [edx]
            fild [nhoodSize]
            jmp GetBoundingRect_Exit
        }
    }
}