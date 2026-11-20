#include "fx/maps.h"
#include "hooking.h"
#include "config.h"
#include "common.h"
#include "TS2/base.h"
#include "TS2/cTSSGSystem.h"
#include "TS2/cNHoodTerrain.h"
#include <cmath>

namespace
{
    constexpr DWORD GetBoundingRect_Exit = 0xB7EBDE;
    constexpr DWORD Particles_Init_Exit = 0xFE2EBA;
    constexpr DWORD DoHeightBasedEmit_Exit_1 = 0x101F517;
    constexpr DWORD DoHeightBasedEmit_Exit_2 = 0x101F599;
    constexpr DWORD MetaParticles_Init_Exit_1 = 0x102129A;
    constexpr DWORD MetaParticles_Init_Exit_2 = 0x1021369;
    constexpr DWORD MetaParticles_Init_Exit_3 = 0x10213E3;
    constexpr DWORD ApplyTerrainForces_Exit_1 = 0x10217AB;
    constexpr DWORD ApplyTerrainForces_Exit_2 = 0x1021C3C;
    constexpr DWORD UpdateParticlePhysicsTimeStep_Exit_1 = 0x1022498;
    constexpr DWORD UpdateParticlePhysicsTimeStep_Exit_2 = 0x10224DA;

    const char waterSparkleFX[] = "watersparklecontrol";

    // An effect map is essentially a pointer to a section of the terrain data classes' vtables
    // Only 3 classes have an effect map: cNHoodTerrain, cTerrainGeometryData, and cTSTestMap
    typedef uintptr_t *cITSEffectMap;
}

// Fixes issues with effects tied to an effect map
namespace Maps
{
    // Many lot effects are started before game sets lot state and switches to lot effect map
    // This causes them to be assigned to nhood effect map and therefore never appear in lot view
    static cITSEffectMap *GetLotEffectMap(cITSEffectMap *effectMap)
    {
        // Skip if already pointing to lot effect map vtable
        if (*reinterpret_cast<DWORD *>(effectMap) == 0x12476B4 || !Common::IsInLot())
            return effectMap;

        nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);
        uintptr_t cTerrain = reinterpret_cast<uintptr_t>(system->Terrain());

        if (!cTerrain)
            return effectMap;

        nTSSG::cTerrainGeometryData *terrainGeomData =
            *reinterpret_cast<nTSSG::cTerrainGeometryData **>(cTerrain + 0x40);

        return reinterpret_cast<cITSEffectMap *>(terrainGeomData + 0x8);
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

    static void SetNHoodHeightAndNormalData(const cVector *hFirst, const cVector *hLast,
                                            const cSlice *nFirst, const cSlice *nLast)
    {
        nTSSG::cNHoodTerrain &nhoodTerrain = nTSSG::cNHoodTerrain::NHoodTerrain();

        nhoodTerrain.UpdateTerrainHeightmap(hFirst, hLast);
        nhoodTerrain.UpdateTerrainNormals(nFirst, nLast);
    }

    // cNHoodTerrain::UpdateTerrainNormals
    // Preserves copies of game's cNHoodTerrain heightmap and normal data for our custom object
    // Would love to just use the original object, but it's destroyed when loading a lot :/
    void __declspec(naked) GetNHoodHeightAndNormalData()
    {
        __asm {
            lea ebx,[edi+0x88]
            push [ebx+0x4]
            push [ebx]
            lea ebx,[edi+0x7C]
            push [ebx+0x4]
            push [ebx]
            call SetNHoodHeightAndNormalData
            add esp,0x10
            pop edi
            pop ebx
            ret 0x4
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

    // cTerrainGeometryData::Height
    // Lot effect map only has heightmap data for terrain within bounds of current lot
    // Height at coords outside of lot bounds will always be 0, breaking collision detection
    // We completely replace the method with our own which uses nhood heightmap data instead
    // Only purpose of the dummy param is to preserve stack layout
    static float __fastcall HeightHook(nTSSG::cTerrainGeometryData *terrainGeomData, void *dummy,
                                       float x, float y)
    {
        return terrainGeomData->Height(x, y);
    }

    // cTerrainGeometryData::Normal
    // Like with Height, we replace this method with our own that uses nhood normal data
    static cSlice *__fastcall NormalHook(nTSSG::cTerrainGeometryData *terrainGeomData, void *dummy,
                                         cSlice *normal, float x, float y)
    {
        return terrainGeomData->Normal(normal, x, y);
    }

    // cTerrainGeometryData::WaterLevel
    float __fastcall WaterLevelHook(nTSSG::cTerrainGeometryData *terrainGeomData)
    {
        return terrainGeomData->WaterLevel();
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
            Common::TranslateEffectCoordsToNHoodCoords(currX, currY);

            nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);
            nTSSG::cLotSkirt *lotSkirt = system->LotSkirt();

            if (lotSkirt)
            {
                // Adjust min/max bounds according to in-game view distance settings
                const cRZRect *bounds = lotSkirt->GetBounds();
                minX = static_cast<float>(bounds->minX * 10);
                minY = static_cast<float>(bounds->minY * 10);
                maxX = static_cast<float>(bounds->maxX * 10);
                maxY = static_cast<float>(bounds->maxY * 10);
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