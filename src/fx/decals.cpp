#define NOMINMAX
#include "fx/decals.h"
#include "common.h"
#include "TS2/base.h"
#include "TS2/cTSSGSystem.h"
#include <algorithm>

namespace
{
    const DWORD TurnOnDisplay_Exit = 0xA84A3B;
    const DWORD CreateSceneGraphNodesAndGeometryBuilders_Exit = 0xADA618;
    const DWORD NodeBoundingBox_Exit = 0xB66AC4;
    const DWORD SetMaterialState_Exit_1 = 0xB677BB;
    const DWORD SetMaterialState_Exit_2 = 0xB67B28;
    const DWORD SetMaterialState_Exit_3 = 0xB67C9D;
    const DWORD AddSelfToDisplayList_Exit_1 = 0xB69D80;
    const DWORD AddSelfToDisplayList_Exit_2 = 0xB69DA6;
    const DWORD SetVertexDataStateAndDrawPart_Exit = 0xB6A201;
    const DWORD Init_Exit = 0x101C31D;

    const char decalMaterial[] = "overlaydecalmaterial";
    void *overlayMgr = nullptr;
    char *matName = nullptr;
}

// Fixes quirks/bugs with decal effects in lot view
namespace Decals
{
    // cLotSkirt::TurnOnDisplay
    // Stashes pointer to overlay manager allocated to lot skirt
    void __declspec(naked) GetLotSkirtOverlayManager()
    {
        __asm {
            call [eax+0xB4]
            mov [overlayMgr],eax
            jmp TurnOnDisplay_Exit
        }
    }

    // cTSDecalEffect::Init
    // Decals get assigned to the wrong overlay manager in lot view for some reason
    // This assigns them to the lot skirt overlay manager so they can actually be drawn
    void __declspec(naked) FixDecalOverlayManager()
    {
        __asm {
            mov ebx,[esi+0xC0]
            cmp [overlayMgr],0x0
            je LAB_Exit
            mov eax,[overlayMgr]
        LAB_Exit:
            jmp Init_Exit
        }
    }

    // cNHoodTerrain::CreateSceneGraphNodesAndGeometryBuilders
    // Nullifies pointer to stashed lot skirt overlay manager when transitioning to neighbourhood
    // Necessary or decals will still be attached to lot skirt manager and won't be drawn in nhood
    void __declspec(naked) ResetOverlayManager()
    {
        __asm {
            mov [overlayMgr],0x0
            call [eax+0xB4]
            jmp CreateSceneGraphNodesAndGeometryBuilders_Exit
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
            cmp [overlayMgr],0x0
            je LAB_CheckCull
            mov edx,[ebp+0x88]
            cmp [overlayMgr],edx
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
    // Checks material name and that call came from cLotSkirtOverlayGeometry to avoid affecting lot shadows
    // Does similar test for nhood to replace constant _strnicmp check game was performing even in lot view
    void __declspec(naked) ColourDecals()
    {
        __asm {
            mov ecx,[esp+0x4]
            cmp [overlayMgr],0x0
            jne LAB_InLot
            cmp dword ptr [ecx],0x123E8C4 // cNHoodTerrainOverlayGeometry vtable address
            je LAB_ColourDecal
        LAB_InLot:
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

        int minX = *reinterpret_cast<int *>(lotSkirt + 0xD0);
        int minY = *reinterpret_cast<int *>(lotSkirt + 0xD4);
        int maxX = *reinterpret_cast<int *>(lotSkirt + 0xD8);
        int maxY = *reinterpret_cast<int *>(lotSkirt + 0xDC);

        currBounds[2] = std::min(currBounds[2], maxX - minX);
        currBounds[3] = std::min(currBounds[3], maxY - minY);
    }

    // cLotSkirtOverlayGeometry::SetVertexDataStateAndDrawPart
    // Lot skirt vertex bounds are set to max + 1
    // Presumably no height data for this region as z for overlay verts overlapping bounds defaults to 0
    // This causes the affected verts to stretch into the sky
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
