/*****************************************************************************/
/*!
\file   Dll.cpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\par    GAM400
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#include "Precompiled.hpp"

#include <BWAPI.h>
#include <Windows.h>

#include "HoldAIModule.hpp"

extern "C" __declspec(dllexport) void gameInit(BWAPI::Game* game) { BWAPI::BroodwarPtr = game; }
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

extern "C" __declspec(dllexport) BWAPI::AIModule* newAIModule()
{
  return new HoldAIModule();
}
