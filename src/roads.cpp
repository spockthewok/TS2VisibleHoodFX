#include "roads.h"

namespace
{
    const DWORD CreateSceneGraphNodeForRoadOccupantWithModel_Exit_1 = 0xAD2113;
    const DWORD CreateSceneGraphNodeForRoadOccupantWithModel_Exit_2 = 0xAD2151;
    const DWORD CreateMergedRoadGeometry_Exit_1 = 0xAD55EE;
    const DWORD CreateMergedRoadGeometry_Exit_2 = 0xAD55F3;
    // Base address of (anonymous_namespace)::OffsetPositionToLot
    const DWORD OffsetPositionToLot = 0xAD0460;

    const BYTE ecxReg[] = {0x4E, 0x51};
    const BYTE roadSkip[] = {0x55,                    // PUSH EBP
                             0x57,                    // PUSH EDI
                             0x74, 0x4A,              // JZ 0xAD54F4
                             0x90, 0x90, 0x90, 0x90}; // NOP
}

namespace Roads
{
    // This does something similar to nGZNumerics::EulerToQuaternion used by props and flora
    // We need our own function as bridge orientation is already in quaternion form
    static void SetOrientation(float *cQuaternion, int orientation)
    {
        float degrees = orientation * 90.0;
        float radians = degrees * (M_PI / 180.0);

        // These vars are a bit unnecessary, but they help to reduce line length :/
        float currX = cQuaternion[0];
        float currY = cQuaternion[1];
        float currZ = cQuaternion[2];
        float currW = cQuaternion[3];

        float rotX = 0.0;
        float rotY = 0.0;
        float rotZ = sinf(radians * 0.5);
        float rotW = cosf(radians * 0.5);

        float x = (currX * rotW) + (currW * rotX) + (currZ * rotY) - (currY * rotZ);
        float y = (currY * rotW) - (currZ * rotX) + (currW * rotY) + (currX * rotZ);
        float z = (currZ * rotW) + (currY * rotX) - (currX * rotY) + (currW * rotZ);
        float w = (currW * rotW) - (currX * rotX) - (currY * rotY) - (currZ * rotZ);

        cQuaternion[0] = x;
        cQuaternion[1] = y;
        cQuaternion[2] = z;
        cQuaternion[3] = w;
    }

    // cNHoodOccupantManager::CreateMergedRoadGeometry
    // Changes jump location for lots from early exit to bridge setup
    // We skip road occupant creation as lot skirt has its own road variants
    // Two push instructions moved before jump to account for this (preserves stack layout)
    void AllowBridgesInLot()
    {
        Hooking::WriteToMemory((DWORD)0xAD54A6, roadSkip, sizeof(roadSkip));
    }

    // cNHoodOccupantManager::CreateMergedRoadGeometry
    // Prevents register storing occupant manager object being overwritten
    // Important as we need to check one of its vars for SkipRoadsDestructor()
    void PreserveOccupantManager()
    {
        // MOV ESI,[ESI+0x64] -> MOV ECX,[ESI+0x64]
        Hooking::WriteToMemory((DWORD)0xAD559F, &ecxReg[0], sizeof(ecxReg[0]));
        // PUSH ESI -> PUSH ECX
        Hooking::WriteToMemory((DWORD)0xAD55A6, &ecxReg[1], sizeof(ecxReg[1]));
    }

    // cNHoodOccupantManager::CreateMergedRoadGeometry
    // Skips road occupant vector destructor when loading a lot
    // Game would crash here without this as our lot path skips constructor
    void __declspec(naked) SkipRoadsDestructor()
    {
        __asm {
            mov al,[esi+0xC1] // Is 0 if not in neighbourhood
            test al,al
            jz LAB_Skip
            lea ecx,[esp+0x1C]
            mov [esp+0x30],-0x1
            jmp CreateMergedRoadGeometry_Exit_1
        LAB_Skip:
            jmp CreateMergedRoadGeometry_Exit_2
        }
    }

    // cNHoodOccupantManager::CreateSceneGraphNodeForRoadOccupantWithModel
    // Translates bridge positioning to match world coordinates in lot view
    // Game uses same approach for props and flora
    void __declspec(naked) TranslateBridgesToLot()
    {
        __asm {
            mov [esp+0x54],eax
            pushad
            mov edx,[esp+0x24] // cNHoodOccupantManager object
            mov al,[edx+0xC1]
            test al,al
            jnz LAB_Exit
            lea eax,[esp+0x6C]
            push eax
            call OffsetPositionToLot
            add esp,0x4
        LAB_Exit:
            popad
            mov edx,[ecx]
            jmp CreateSceneGraphNodeForRoadOccupantWithModel_Exit_1
        }
    }

    // cNHoodOccupantManager::CreateSceneGraphNodeForRoadOccupantWithModel
    // Rotates bridges to match current world orientation
    void __declspec(naked) RotateBridges()
    {
        __asm {
            pushad
            mov edx,[esp+0x24]
            push [edx+0xC4] // Scene orientation (0/1/2/3)
            lea edx,[esp+0x7C] // Bridge cQuaternion
            push edx
            call SetOrientation
            add esp,0x8
            popad
            mov ecx,[esp+0x8C]
            jmp CreateSceneGraphNodeForRoadOccupantWithModel_Exit_2
        }
    }
}