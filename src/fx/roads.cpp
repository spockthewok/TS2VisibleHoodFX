#define _USE_MATH_DEFINES
#include "fx/roads.h"
#include "hooking.h"
#include "TS2/base.h"
#include "TS2/cTSSGSystem.h"
#include <math.h>

namespace
{
    const DWORD CreateSceneGraphNodeForRoadOccupantWithModel_Exit_1 = 0xAD2113;
    const DWORD CreateSceneGraphNodeForRoadOccupantWithModel_Exit_2 = 0xAD2151;
    const DWORD CreateSceneGraphNodeForRoadOccupantWithModel_Exit_3 = 0xAD2179;
    const DWORD CreateMergedRoadGeometry_Exit_1 = 0xAD55EE;
    const DWORD CreateMergedRoadGeometry_Exit_2 = 0xAD55F3;

    const BYTE ecxReg[] = {0x4E, 0x51};
    const BYTE roadSkip[] = {0x55,                    // PUSH EBP
                             0x57,                    // PUSH EDI
                             0x74, 0x4A,              // JZ 0xAD54F4
                             0x90, 0x90, 0x90, 0x90}; // NOP
}

// Creates neighbourhood bridges in lot view
namespace Roads
{
    // cNHoodOccupantManager::CreateMergedRoadGeometry
    // Changes jump location for lots from early exit to bridge setup
    // We skip road occupant creation as lot skirt has its own road variants
    // Two push instructions moved before jump to account for this (preserves stack layout)
    void AllowBridgesInLot()
    {
        Hooking::WriteToMemory((DWORD)(0x6D54A6 + Module::base), roadSkip, sizeof(roadSkip));
    }

    // cNHoodOccupantManager::CreateMergedRoadGeometry
    // Prevents register storing occupant manager object being overwritten
    // Important as we need to check one of its vars for SkipRoadsDestructor()
    void PreserveOccupantManager()
    {
        // MOV ESI,[ESI+0x64] -> MOV ECX,[ESI+0x64]
        Hooking::WriteToMemory((DWORD)(0x6D559F + Module::base), &ecxReg[0], sizeof(ecxReg[0]));
        // PUSH ESI -> PUSH ECX
        Hooking::WriteToMemory((DWORD)(0x6D55A6 + Module::base), &ecxReg[1], sizeof(ecxReg[1]));
    }

    // cNHoodOccupantManager::CreateMergedRoadGeometry
    // Skips road occupant vector destructor when loading a lot
    // Game would crash here without this as our lot path skips constructor
    void __declspec(naked) SkipRoadsDestructor()
    {
        __asm {
            mov al,[esi+0xC1] // bool inNeighbourhood
            test al,al
            jz LAB_Skip
            lea ecx,[esp+0x1C]
            mov [esp+0x30],-0x1
            jmp CreateMergedRoadGeometry_Exit_1
        LAB_Skip:
            jmp CreateMergedRoadGeometry_Exit_2
        }
    }

    static bool IsBridgeInBounds(const float *coords)
    {
        nTSSG::cTSSGSystem *system = *reinterpret_cast<nTSSG::cTSSGSystem **>(0x1478F10);
        nTSSG::cLotSkirt *lotSkirt = system->LotSkirt();

        if (!lotSkirt)
            return true;

        const int *bounds = lotSkirt->GetBounds();

        float minX = static_cast<float>(bounds[0] * 10);
        float minY = static_cast<float>(bounds[1] * 10);
        float maxX = static_cast<float>(bounds[2] * 10);
        float maxY = static_cast<float>(bounds[3] * 10);
        float currX = coords[0];
        float currY = coords[1];

        if (currX < minX || currY < minY || currX > maxX || currY > maxY)
            return false;

        return true;
    }

    // cNHoodOccupantManager::CreateSceneGraphNodeForRoadOccupantWithModel
    // Translates bridge positioning to match world coordinates in lot view
    // Game uses same approach for props and flora
    // Skips adding bridge piece if it lies outside current view distance bounds
    // We check bounds before translation so coords are still in range of (0-nhoodSize)
    void __declspec(naked) TranslateBridgesToLot()
    {
        __asm {
            mov [esp+0x54],eax
            mov edx,[esp+0x4] // cNHoodOccupantManager object
            mov al,[edx+0xC1]
            test al,al
            jnz LAB_Continue
            pushad
            lea eax,[esp+0x6C]
            push eax
            call IsBridgeInBounds
            add esp,0x4
            test al,al
            popad
            jz LAB_Skip
            pushad
            lea eax,[esp+0x6C]
            push eax
            call OffsetPositionToLot
            add esp,0x4
            popad
        LAB_Continue:
            mov edx,[ecx]
            jmp CreateSceneGraphNodeForRoadOccupantWithModel_Exit_1
        LAB_Skip:
            jmp CreateSceneGraphNodeForRoadOccupantWithModel_Exit_3
        }
    }

    // This does something similar to nGZNumerics::EulerToQuaternion used by props and flora
    // We need our own function as bridge orientation is already in quaternion form
    static void SetRotation(float *cQuaternion, const UINT rotation)
    {
        float degrees = rotation * 90.0f;
        float radians = degrees * (M_PI / 180.0f);

        // These vars are a bit unnecessary, but they help to reduce line length :/
        float currX = cQuaternion[0];
        float currY = cQuaternion[1];
        float currZ = cQuaternion[2];
        float currW = cQuaternion[3];

        float rotX = 0.0f;
        float rotY = 0.0f;
        float rotZ = sinf(radians * 0.5f);
        float rotW = cosf(radians * 0.5f);

        float x = (currX * rotW) + (currW * rotX) + (currZ * rotY) - (currY * rotZ);
        float y = (currY * rotW) - (currZ * rotX) + (currW * rotY) + (currX * rotZ);
        float z = (currZ * rotW) + (currY * rotX) - (currX * rotY) + (currW * rotZ);
        float w = (currW * rotW) - (currX * rotX) - (currY * rotY) - (currZ * rotZ);

        cQuaternion[0] = x;
        cQuaternion[1] = y;
        cQuaternion[2] = z;
        cQuaternion[3] = w;
    }

    // cNHoodOccupantManager::CreateSceneGraphNodeForRoadOccupantWithModel
    // Orients bridges to match current world rotation
    void __declspec(naked) RotateBridges()
    {
        __asm {
            pushad
            mov edx,[esp+0x24]
            push [edx+0xC4] // Scene rotation (0/1/2/3)
            lea edx,[esp+0x7C] // Bridge cQuaternion
            push edx
            call SetRotation
            add esp,0x8
            popad
            mov ecx,[esp+0x8C]
            jmp CreateSceneGraphNodeForRoadOccupantWithModel_Exit_2
        }
    }
}