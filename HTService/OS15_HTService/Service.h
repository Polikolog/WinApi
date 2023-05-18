﻿#pragma once
#include <iostream>
#include <windows.h>
#include <fstream>

#define SERVICENAME L"OS15Service"
#define TRACEPATH "D:\\Service.trace"

VOID WINAPI  ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
VOID WINAPI ServiceHandler(DWORD fdwControl);

void trace(const char* msg, int r = std::ofstream::app);