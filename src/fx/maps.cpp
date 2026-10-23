#include "fx/maps.h"
#include "hooking.h"
#include "common.h"
#include "config.h"
#include "TS2/base.h"
#include "TS2/cTSSGSystem.h"
#include "TS2/cTSNHoodTerrain.h"
#include <array>
#include <cmath>

namespace
{
    const DWORD GetBoundingRect_Exit = 0xB7EBDE;
    const DWORD Particles_Init_Exit = 0xFE2EBA;
    const DWORD DoHeightBasedEmit_Exit_1 = 0x101F517;
    const DWORD DoHeightBasedEmit_Exit_2 = 0x101F599;
    const DWORD MetaParticles_Init_Exit_1 = 0x102129A;
    const DWORD MetaParticles_Init_Exit_2 = 0x1021369;
    const DWORD MetaParticles_Init_Exit_3 = 0x10213E3;
    const DWORD ApplyTerrainForces_Exit_1 = 0x10217AB;
    const DWORD ApplyTerrainForces_Exit_2 = 0x1021C3C;
    const DWORD UpdateParticlePhysicsTimeStep_Exit_1 = 0x1022498;
    const DWORD UpdateParticlePhysicsTimeStep_Exit_2 = 0x10224DA;

    const char waterSparkleFX[] = "watersparklecontrol";
    bool inLot = false;
}

// This is for fixing issues with effects tied to an effect map
namespace Maps
{
    static bool IsInLot(const char *currLotName)
    {
        inLot = false;

        if (!currLotName)
            return inLot;

        std::string lotName(currLotName);

        // All lot names follow this structure: "NHoodID-LotX", where X is lot number
        // CAS lots are either "CAS!" or "YACAS!"
        // String will be blank if in neighbourhood
        if (!lotName.empty() && lotName != "CAS!" && lotName != "YACAS!")
            inLot = true;

        return inLot;
    }

    // Many lot effects are started before game sets lot state and switches to lot effect map
    // This causes them to be assigned to nhood effect map and therefore never appear in lot view
    // Game thinks we're still in nhood, so we check name of lot being loaded to determine state
    // If we are loading a valid lot, we manually get lot effect map and assign it to effect
    static void __declspec(naked) FixEffectMap()
    {
        __asm {
            cmp dword ptr [eax],0x12476B4 // Skip if already pointing to lot effect map vtable
            je LAB_Return
            call TS::GameStateController
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x24] // cTSGameStateController::CurrentLotInfo
            test eax,eax
            jz LAB_Return
            push [eax+0x1C] // Object var holding current lot name
            call IsInLot
            add esp,0x4
            test al,al
            jz LAB_Return
            mov eax,dword ptr ds:[0x1478F10] // nTSSG::TSSGSystem
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x9C] // cTSSGSystem::Terrain
            test eax,eax
            jz LAB_Return
            mov eax,[eax+0x40] // cTerrainGeometryData object
            add eax,0x8 // Object var holding vtable address for lot effect map
            mov [esp+0x10],eax
        LAB_Return:
            mov eax,[esp+0x10] // Restore stashed effect map vtable pointer
            ret
        }
    }

    // cTSParticlesEffect::Init
    void __declspec(naked) FixParticleEffectMap()
    {
        __asm {
            mov ecx,eax
            call [edx+0x1C] // cTSEffectsMap::EffectMap
            mov [esp+0xC],eax // Stash returned effect map vtable pointer
            call FixEffectMap
            jmp Particles_Init_Exit
        }
    }

    // cTSMetaParticlesEffect::Init
    void __declspec(naked) FixMetaParticleEffectMap()
    {
        __asm {
            mov ecx,eax
            call [edx+0x1C] // cTSEffectsMap::EffectMap
            mov [esp+0xC],eax // Stash returned effect map vtable pointer
            call FixEffectMap
            jmp MetaParticles_Init_Exit_2
        }
    }

    // cTSMetaParticlesEffect::Init
    // Handles creation of water sparkles in lot view based on user config
    void __declspec(naked) EnableWaterSparkles()
    {
        __asm {
            cmp [inLot],0x0 // Will be initialised by the time water sparkles are processed
            je LAB_ContinueInit
            cmp [Config::waterSparkles],0x0
            jne LAB_ContinueInit
            mov eax,[esp+0x28]
            push ecx
            push [eax]
            push offset waterSparkleFX
            call Common::IsSameString
            add esp,0x8
            pop ecx
            test al,al
            jnz LAB_Exit
        LAB_ContinueInit:
            mov esi,ecx
            mov al,[esi+0xC]
            jmp MetaParticles_Init_Exit_1
        LAB_Exit:
            jmp MetaParticles_Init_Exit_3
        }
    }

    // cTerrainGeometryData::GetBoundingRect
    // Effects can only exist within the bounds of their assigned effect map
    // Nhood effect map bounds cover entire nhood, lot effect map only covers size of current lot
    // This increases bounds of lot effect map so effects can appear anywhere in world
    void __declspec(naked) IncreaseLotBoundingRect()
    {
        __asm {
            fld [Floats::nhoodSize]
            fst [edx]
            jmp GetBoundingRect_Exit
        }
    }

    // cTerrainGeometryData::WaterLevel
    // Lot effect map always returns 0 for water level, regardless of lot height above sea level
    // Game calculates sea level relative to lot z in cLotSkirt::ComputeLotSkirtParameters
    // This returns the value of cLotSkirt var the result of said calculation was stored in
    // Ensures water effects spawn at the correct height
    float GetRelativeWaterLevel()
    {
        nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);
        nTSSG::cLotSkirt *lotSkirt = system->LotSkirt();

        if (!lotSkirt)
            return 0.0f;

        return lotSkirt->SeaLevel();
    }

    // This is a custom version of cLotSkirt::TranslateLotCoordsToNhoodCoords
    static void TranslateEffectCoordsToNhoodCoords(std::array<float, 2> &coords)
    {
        nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);
        nTSSG::cLotSkirt *lotSkirt = system->LotSkirt();

        if (!lotSkirt)
            return;

        const UINT rotation = lotSkirt->GetRotation();
        const float offsetX = static_cast<float>(lotSkirt->GetOffsetX() * 10);
        const float offsetY = static_cast<float>(lotSkirt->GetOffsetY() * 10);
        float currX = coords[0];
        float currY = coords[1];

        switch (rotation)
        {
        case 1:
            currX = coords[1];
            currY = -coords[0];
            break;
        case 2:
            currX = -coords[0];
            currY = -coords[1];
            break;
        case 3:
            currX = -coords[1];
            currY = coords[0];
        }

        coords[0] = offsetX + currX;
        coords[1] = offsetY + currY;
    }

    static float GetHeightAtPosition(const float x, const float y)
    {
        cTSGlobals *globals = TS::Globals();
        cTSGameStateController *gameState = TS::GameStateController();
        cTSNHoodTerrain *nhoodTerrain = globals->NeighborhoodTerrain();
        cTSLotInfo *lotInfo = gameState->CurrentLotInfo();

        if (!nhoodTerrain || !lotInfo)
            return Floats::nhoodSize; // Return large value so later checks fail

        std::array<float, 2> coords = {x, y};

        TranslateEffectCoordsToNhoodCoords(coords);

        float terrainHeight = nhoodTerrain->GetTerrainHeight(coords[0], coords[1]);
        float offsetZ = lotInfo->NHoodToLotHeightOffset();

        if (std::isnan(terrainHeight) || std::isinf(terrainHeight) || terrainHeight == 0)
            return Floats::nhoodSize;

        // Game will subtract water level from this value
        // Because lot water level is negative, calculation becomes height + lotWaterLvl
        // Equivalent to terrainHeight - nhoodWaterLvl, which is what nhood uses to get height
        return terrainHeight - offsetZ;
    }

    // cTerrainGeometryData::Height
    // Lot effect map only has heightmap data for terrain within bounds of current lot
    // Height at coords outside of lot bounds will always be 0, breaking collision detection
    // We completely replace the method with our own function to get nhood heightmap data instead
    // Method prologue and epilogue is recreated to preserve stack layout
    static void __declspec(naked) ReplaceHeightChecks()
    {
        __asm {
            push ebx
            push ebp
            push esi
            push edi
            push [esp+0x18] // y
            push [esp+0x18] // x
            call GetHeightAtPosition
            add esp,0x8
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x8
        }
    }

    static bool IsValidPosition(const float x, const float y)
    {
        if (std::isnan(x) || std::isnan(y))
            return false;

        float currX = x;
        float currY = y;

        if (inLot)
        {
            // Convert effect map coords (-640-640) to lot skirt coords (0-1280)
            currX += (Floats::nhoodSize / 2);
            currY += (Floats::nhoodSize / 2);
        }

        if (currX <= 0 || currY <= 0 || currX >= Floats::nhoodSize || currY >= Floats::nhoodSize)
            return false;

        return true;
    }

    // cTSMetaParticlesEffect::DoHeightBasedEmit
    static void __declspec(naked) ReplacePositionChecks1()
    {
        __asm {
            fstp st(0)
            pushad
            push [esp+0x34] // y
            push [esp+0x34] // x
            call IsValidPosition
            add esp,0x8
            test al,al
            popad
            jz LAB_InvalidPosition
            jmp DoHeightBasedEmit_Exit_1
        LAB_InvalidPosition:
            jmp DoHeightBasedEmit_Exit_2
        }
    }

    // cTSMetaParticlesEffect::ApplyTerrainForces
    static void __declspec(naked) ReplacePositionChecks2()
    {
        __asm {
            fstp st(0)
            push [esp+0x14] // y
            push [esp+0x14] // x
            call IsValidPosition
            add esp,0x8
            test al,al
            jz LAB_InvalidPosition
            jmp ApplyTerrainForces_Exit_1
        LAB_InvalidPosition:
            jmp ApplyTerrainForces_Exit_2
        }
    }

    // cTSMetaParticlesEffect::UpdateParticlePhysicsTimeStep
    static void __declspec(naked) ReplacePositionChecks3()
    {
        __asm {
            fstp st(0)
            push [esp+0x4C] // y
            push [esp+0x4C] // x
            call IsValidPosition
            add esp,0x8
            test al,al
            jz LAB_InvalidPosition
            jmp UpdateParticlePhysicsTimeStep_Exit_1
        LAB_InvalidPosition:
            jmp UpdateParticlePhysicsTimeStep_Exit_2
        }
    }

    // Lot meta particle effect coords are relative to lot terrain origin
    // Because we expanded effect map bounds, meta particle effect coords can be negative
    // The methods patched here don't consider this to be valid and destroy the effects
    void HandleNegativeValues()
    {
        // cTerrainGeometryData::Height
        Hooking::MakeJMP((BYTE *)0xB7F1B0, (DWORD)ReplaceHeightChecks, 6);

        // cTSMetaParticlesEffect::DoHeightBasedEmit
        Hooking::MakeJMP((BYTE *)0x101F4C7, (DWORD)ReplacePositionChecks1, 6);

        // cTSMetaParticlesEffect::ApplyTerrainForces
        Hooking::MakeJMP((BYTE *)0x102175B, (DWORD)ReplacePositionChecks2, 6);

        // cTSMetaParticlesEffect::UpdateParticlePhysicsTimeStep
        Hooking::MakeJMP((BYTE *)0x1022458, (DWORD)ReplacePositionChecks3, 6);
    }
}