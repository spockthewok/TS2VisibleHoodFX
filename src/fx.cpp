#include "fx.h"

namespace
{
    const DWORD TurnOnDisplay_Exit = 0xA84A3B;
    const DWORD CreateSceneGraphNodeForPropOccupant_Exit_1 = 0xAD3B85;
    const DWORD CreateSceneGraphNodeForPropOccupant_Exit_2 = 0xAD3BAA;
    const DWORD CreateSceneGraphNodesAndGeometryBuilders_Exit = 0xADA618;
    const DWORD SetMaterialState_Exit_1 = 0xB677BB;
    const DWORD SetMaterialState_Exit_2 = 0xB67B17;
    const DWORD SetMaterialState_Exit_3 = 0xB67B28;
    const DWORD AddSelfToDisplayList_Exit_1 = 0xB69D80;
    const DWORD AddSelfToDisplayList_Exit_2 = 0xB69DA6;
    const DWORD FrustumQuery_Exit_1 = 0xFB00E9;
    const DWORD FrustumQuery_Exit_2 = 0xFB0194;
    const DWORD Init_Exit = 0x101C31D;

    void *overlayMgr = nullptr;
    char *matName = nullptr;
}

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
            mov al,[ebp+0xC1]
            test al,al
            mov byte ptr [esp+0xA0],0x5
            jz LAB_IsLot
        LAB_CreateEffect:
            jmp CreateSceneGraphNodeForPropOccupant_Exit_1
        LAB_IsLot:
            push [esp+0x30] // Name of current neighbourhood object stored on stack
            call IsBlacklistedEffect
            add esp,0x4
            test al,al
            jz LAB_CreateEffect
            jmp CreateSceneGraphNodeForPropOccupant_Exit_2
        }
    }

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
    // Necessary otherwise decals will still be attached to lot skirt manager and won't be drawn in hood
    void __declspec(naked) ResetOverlayManager()
    {
        __asm {
            mov [overlayMgr],0x0
            call [eax+0xB4]
            jmp CreateSceneGraphNodesAndGeometryBuilders_Exit
        }
    }

    // cLooseOctreePartitionNode::FrustumQuery
    // Prevents any cOverlayNode objects being culled based on visibility in lot view
    // Important as cLooseOctreePartitionNode::FrustumQuery calls cOverlayNode::AddSelfToDisplayList
    // Without this, PreventCullingDecals() would be skipped
    // Shouldn't affect performance as non-decal overlays will be culled by cOverlayNode::AddSelfToDisplayList
    void __declspec(naked) PreventCullingOverlays()
    {
        __asm {
            cmp [overlayMgr],0x0
            je LAB_CheckCull
            cmp [ebp+0x24],0x0
            je LAB_CheckCull
            mov edx,[ebp+0x24]
            mov edx,[edx]
            cmp [edx],0x12457E8 // cOverlayNode vtable address
            je LAB_ContinueQuery
        LAB_CheckCull:
            test eax,eax
            jg LAB_Cull
        LAB_ContinueQuery:
            jmp FrustumQuery_Exit_1
        LAB_Cull:
            jmp FrustumQuery_Exit_2
        }
    }

    // cOverlayNode::AddSelfToDisplayList
    // Prevents nodes belonging to lot skirt overlay manager being culled when camera leaves lot bounds
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

    static bool IsDecalMaterial(const char *currMaterial)
    {
        if (!currMaterial)
            return false;

        return _stricmp(currMaterial, "overlaydecalmaterial") == 0;
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
    // Checks material name and that call came from cLotSkirtOverlayGeometry to avoid affecting shadows
    void __declspec(naked) ColourDecals()
    {
        __asm {
            mov ecx,[esp+0x4]
            cmp dword ptr [ecx],0x1245754 // cLotSkirtOverlayGeometry vtable address
            jne LAB_CheckInHood
            pushad
            push [matName]
            call IsDecalMaterial
            add esp,0x4
            test al,al
            popad
            jnz LAB_ColourDecal
        LAB_CheckInHood:
            push 0x5
            push 0x12456F0 // "neigh"
            jmp SetMaterialState_Exit_2
        LAB_ColourDecal:
            jmp SetMaterialState_Exit_3
        }
    }
}