#define NOMINMAX
#include "fx/decals.h"
#include "common.h"
#include "TS2/base.h"
#include "TS2/cTSSGSystem.h"
#include <algorithm>

namespace
{
    const DWORD NodeBoundingBox_Exit = 0xB66AC4;
    const DWORD SetMaterialState_Exit_1 = 0xB677BB;
    const DWORD SetMaterialState_Exit_2 = 0xB67B28;
    const DWORD SetMaterialState_Exit_3 = 0xB67C9D;
    const DWORD AddSelfToDisplayList_Exit_1 = 0xB69D80;
    const DWORD AddSelfToDisplayList_Exit_2 = 0xB69DA6;
    const DWORD SetVertexDataStateAndDrawPart_Exit = 0xB6A201;
    const DWORD Init_Exit = 0x101C31D;

    const char decalMaterial[] = "overlaydecalmaterial";
    char *matName = nullptr;
}

// Fixes quirks/bugs with decal effects in lot view
namespace Decals
{
    static void *GetLotSkirtOverlayManager(void *cITSOverlayManager)
    {
        nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);

        // Avoid reassigning if decal is assigned to lot terrain overlay manager
        if (cITSOverlayManager == system->OverlayManager(0))
            return cITSOverlayManager;

        return system->OverlayManager(8);
    }

    // cTSDecalEffect::Init
    // Game still tries to assign nhood decals to nhood overlay manager in lot view
    // This assigns them to the lot skirt overlay manager so they can actually be drawn
    void __declspec(naked) FixDecalOverlayManager()
    {
        __asm {
            mov ebx,[esi+0xC0]
            cmp [Common::inLot],0x0
            je LAB_Exit
            push eax
            call GetLotSkirtOverlayManager
            add esp,0x4
        LAB_Exit:
            jmp Init_Exit
        }
    }

    // cOverlayNode::NodeBoundingBox
    // Keeps overlay node bounding box consistent between nhood and lot view
    void __declspec(naked) IncreaseOverlayBoundingBox()
    {
        __asm {
            call Common::GetNHoodSizeX
            mov ecx,[esp+0x1C]
            fstp [ecx+0xC]
            call Common::GetNHoodSizeY
            mov ecx,[esp+0x1C]
            fstp [ecx+0x10]
            jmp NodeBoundingBox_Exit
        }
    }

    // cOverlayNode::AddSelfToDisplayList
    // Stops nodes assigned to lot skirt overlay manager being culled when camera leaves lot bounds
    void __declspec(naked) PreventCullingDecals()
    {
        __asm {
            cmp [Common::inLot],0x0
            je LAB_CheckCull
            push eax
            push 0x0 // Push nullptr because we don't care about checking for lot terrain manager
            call GetLotSkirtOverlayManager
            add esp,0x4
            cmp eax,[ebp+0x88]
            pop eax
            je LAB_AddToDisplayList
        LAB_CheckCull:
            test eax,eax
            jg LAB_Cull
        LAB_AddToDisplayList:
            jmp AddSelfToDisplayList_Exit_1
        LAB_Cull:
            jmp AddSelfToDisplayList_Exit_2
        }
    }

    // cTerrainOverlayGeometryBase::SetMaterialState
    // Stashes name of material currently being processed
    // We need this as offset material name is stored at varies when game executes ColourDecals()
    void __declspec(naked) GetCurrentMaterial()
    {
        __asm {
            mov eax,[esp+0x1C] // Pointer to material name stored on stack
            mov eax,[eax]
            mov [matName],eax
            mov ecx,[ecx+0x90]
            jmp SetMaterialState_Exit_1
        }
    }

    // cTerrainOverlayGeometryBase::SetMaterialState
    // Allows decals to receive proper colouring in lot view so they're not overly bright at night
    // Checks call location and material name to avoid affecting shadows
    // Does similar test for nhood to replace constant _strnicmp check game was performing
    void __declspec(naked) ColourDecals()
    {
        __asm {
            mov ecx,[esp+0x4]
            cmp dword ptr [ecx],0x123E8C4 // cNHoodTerrainOverlayGeometry vtable address
            je LAB_ColourDecal
            cmp dword ptr [ecx],0x1245754 // cLotSkirtOverlayGeometry vtable address
            jne LAB_Skip
            pushad
            push [matName]
            push offset decalMaterial
            call _stricmp
            add esp,0x8
            test eax,eax
            popad
            jnz LAB_Skip
        LAB_ColourDecal:
            jmp SetMaterialState_Exit_2
        LAB_Skip:
            jmp SetMaterialState_Exit_3
        }
    }

    static void ClampBounds(int *currBounds)
    {
        nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);
        nTSSG::cLotSkirt *lotSkirt = system->LotSkirt();

        if (!lotSkirt)
            return;

        // Same maths used in cLotSkirt::GetLotSkirtVertexHeights without incrementing max bounds
        // We don't patch GetLotSkirtVertexHeights directly as we would break overlay positioning
        int minX = *reinterpret_cast<int *>(lotSkirt + 0xD0);
        int minY = *reinterpret_cast<int *>(lotSkirt + 0xD4);
        int maxX = *reinterpret_cast<int *>(lotSkirt + 0xD8);
        int maxY = *reinterpret_cast<int *>(lotSkirt + 0xDC);

        currBounds[2] = std::min(currBounds[2], maxX - minX);
        currBounds[3] = std::min(currBounds[3], maxY - minY);
    }

    // cLotSkirtOverlayGeometry::SetVertexDataStateAndDrawPart
    // Lot skirt vertex bounds are set to max + 1
    // Presumably no height data for this region as z for any overlay verts here defaults to 0
    // This causes verts at positions between max and max + 1 to stretch into the sky
    void __declspec(naked) CheckOverlayBounds()
    {
        __asm {
            call [eax+0x2C] // cOverlayNode::OverlayInfo
            mov edi,eax
            push edi
            call ClampBounds
            add esp,0x4
            jmp SetVertexDataStateAndDrawPart_Exit
        }
    }
}