// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include <iostream>

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::cout << "DLL Connect!" << std::endl;
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        std::cout << "DLL Disconnect!" << std::endl;
        break;
    }
    return TRUE;
}

