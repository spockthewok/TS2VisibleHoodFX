#include "fx/maps.h"
#include "hooking.h"
#include "config.h"
#include "common.h"
#include "TS2/cTSSGSystem.h"
#include "TS2/cNHoodTerrain.h"
#include "TS2/cTerrainGeometryData.h"
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
}

// Fixes issues with effects tied to an effect map
namespace Maps
{
    // Many lot effects are started before game sets lot state and switches to lot effect map
    // This causes them to be assigned to nhood effect map and therefore never appear in lot view
    static void *GetLotEffectMap(void *cITSEffectMap)
    {
        // Skip if already pointing to lot effect map vtable
        if (*reinterpret_cast<DWORD *>(cITSEffectMap) == 0x12476B4 || !Common::IsInLot())
            return cITSEffectMap;

        nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);
        uintptr_t cTerrain = reinterpret_cast<uintptr_t>(system->Terrain());

        if (!cTerrain)
            return cITSEffectMap;

        uintptr_t cTerrainGeometryData = *reinterpret_cast<uintptr_t *>(cTerrain + 0x40);

        return reinterpret_cast<void *>(cTerrainGeometryData + 0x8);
    }

    // cTSParticlesEffect::Init
    void __declspec(naked) FixParticleEffectMap()
    {
        __asm {
            mov ecx,eax
            call [edx+0x1C] // cTSEffectsMap::EffectMap
            push eax
            call GetLotEffectMap
            add esp,0x4
            jmp Particles_Init_Exit
        }
    }

    // cTSMetaParticlesEffect::Init
    // Handles creation of water sparkles in lot view based on user config
    void __declspec(naked) EnableWaterSparkles()
    {
        __asm {
            mov esi,ecx
            cmp [Common::inLot],0x0 // Particles init first, so var will already be set
            je LAB_ContinueInit
            cmp [Config::waterSparkles],0x0
            jne LAB_ContinueInit
            mov eax,[esp+0x28]
            push [eax]
            push offset waterSparkleFX
            call _stricmp
            add esp,0x8
            test eax,eax
            jz LAB_Exit
        LAB_ContinueInit:
            mov al,[esi+0xC]
            jmp MetaParticles_Init_Exit_1
        LAB_Exit:
            jmp MetaParticles_Init_Exit_3
        }
    }

    // cTSMetaParticlesEffect::Init
    void __declspec(naked) FixMetaParticleEffectMap()
    {
        __asm {
            mov ecx,eax
            call [edx+0x1C] // cTSEffectsMap::EffectMap
            push eax
            call GetLotEffectMap
            add esp,0x4
            jmp MetaParticles_Init_Exit_2
        }
    }

    // cTerrainGeometryData::GetBoundingRect
    // Effects can only exist within the bounds of their assigned effect map
    // Nhood effect map bounds cover entire nhood, lot effect map only covers size of current lot
    // This increases bounds of lot effect map so effects can appear anywhere in world
    void __declspec(naked) IncreaseLotBoundingRect()
    {
        __asm {
            call Common::GetNHoodSizeX
            mov edx,[esp+0xC]
            fstp [edx]
            call Common::GetNHoodSizeY
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

    // cTerrainGeometryData::Height
    // Lot effect map only has heightmap data for terrain within bounds of current lot
    // Height at coords outside of lot bounds will always be 0, breaking collision detection
    // We completely replace the method with our own which gets nhood heightmap data instead
    // Only purpose of the dummy param is to preserve stack layout
    static float __fastcall HeightHook(nTSSG::cTerrainGeometryData *terrainGeomData, void *dummy,
                                       float x, float y)
    {
        return terrainGeomData->Height(x, y);
    }

    static void CopyNormalData(const cSlice *first, const cSlice *last)
    {
        nTSSG::nhoodTerrain.normals.assign(first, last);
    }

    // cNHoodTerrain::UpdateTerrainNormals
    // Preserves a copy of the nhood's normal data for later use
    // Would love to get this from the object directly, but it's destroyed when loading a lot :/
    void __declspec(naked) GetNHoodNormalData()
    {
        __asm {
            lea ebx,[edi+0x88]
            push [ebx+0x4]
            push [ebx]
            call CopyNormalData
            add esp,0x8
            pop edi
            pop ebx
            ret 0x4
        }
    }

    // cTerrainGeometryData::Normal
    // Like with Height, we replace this method with our own that uses nhood normal data
    static cSlice *__fastcall NormalHook(nTSSG::cTerrainGeometryData *terrainGeomData, void *dummy,
                                         cSlice *normal, float x, float y)
    {
        return terrainGeomData->Normal(normal, x, y);
    }

    static bool IsValidPosition(const float x, const float y)
    {
        if (std::isnan(x) || std::isnan(y))
            return false;

        float currX = x, currY = y;
        float minX = 0.0f, minY = 0.0f;
        float maxX = Common::GetNHoodSizeX();
        float maxY = Common::GetNHoodSizeY();

        if (Common::inLot)
        {
            // Convert effect map coords (-640-640) to lot skirt coords (0-1280)
            currX += (maxX / 2);
            currY += (maxY / 2);

            nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);
            nTSSG::cLotSkirt *lotSkirt = system->LotSkirt();

            if (lotSkirt)
            {
                // Adjust min/max bounds according to in-game view distance settings
                const int *bounds = lotSkirt->GetBounds();
                minX = static_cast<float>(bounds[0] * 10);
                minY = static_cast<float>(bounds[1] * 10);
                maxX = static_cast<float>(bounds[2] * 10);
                maxY = static_cast<float>(bounds[3] * 10);
            }
        }

        return (currX > minX && currX < maxX) && (currY > minY && currY < maxY);
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
        Hooking::MakeJMP((0x77F1B0 + Module::base), (DWORD)HeightHook, 6);

        // cTerrainGeometryData::Normal
        Hooking::MakeJMP((0x77FA90 + Module::base), (DWORD)NormalHook, 9);

        // cTSMetaParticlesEffect::DoHeightBasedEmit
        Hooking::MakeJMP((0xC1F4C7 + Module::base), (DWORD)ReplacePositionChecks1, 6);

        // cTSMetaParticlesEffect::ApplyTerrainForces
        Hooking::MakeJMP((0xC2175B + Module::base), (DWORD)ReplacePositionChecks2, 6);

        // cTSMetaParticlesEffect::UpdateParticlePhysicsTimeStep
        Hooking::MakeJMP((0xC22458 + Module::base), (DWORD)ReplacePositionChecks3, 6);
    }
}