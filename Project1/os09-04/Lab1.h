#pragma once

#include "Solution.h"
#include "General.h"
#include <string.h>
#include <locale>
#include <codecvt>

struct Lab1
{
	static BOOL printFileInfo(LPWSTR);

	static BOOL printFileTxt(LPWSTR);

	static BOOL delRowFileTxt(LPWSTR, DWORD);

	static BOOL insRowFileTxt(LPWSTR, LPWSTR, DWORD);

	static BOOL printWathRowFileTxt(LPWSTR, DWORD);

};