#pragma once

#include "Solution.h"
#include "General.h"
#include <string.h>
#include <locale>
#include <codecvt>

namespace Lab1
{
	BOOL printFileInfo(LPWSTR);

	BOOL printFileTxt(LPWSTR);

	BOOL delRowFileTxt(LPWSTR, DWORD);

	BOOL insRowFileTxt(LPWSTR, LPWSTR, DWORD);

	BOOL printWathRowFileTxt(LPWSTR, DWORD);

};