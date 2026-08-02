#include "fx.h"

namespace
{
    const DWORD TurnOnDisplay_Exit = 0xA84A3B;
    const DWORD CreateSceneGraphNodeForPropOccupant_Exit_1 = 0xAD3B85;
    const DWORD CreateSceneGraphNodeForPropOccupant_Exit_2 = 0xAD3BAA;
    const DWORD CreateSceneGraphNodesAndGeometryBuilders_Exit = 0xADA618;
    const DWORD AddSelfToDisplayList_Exit_1 = 0xB69D80;
    const DWORD AddSelfToDisplayList_Exit_2 = 0xB69DA6;
    const DWORD Init_Exit = 0x101C31D;

    void *overlayMgr = nullptr;
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
            push [esp+0x30] // Name of current effect stored on stack
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

    // cTerrainOverlayGeometryBase::SetMaterialState
    // Allows decals to receive proper colouring in lot view so they're not overly bright at night
    void ColourDecals()
    {
        Hooking::Nop((BYTE *)0xB67B10, 24);
    }
}