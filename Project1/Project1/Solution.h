#pragma once

#include <windows.h>
#include <iostream>
#include <vector>
#include "General.h"

#define BUFSIZE 484

class Solution
{
public:
	static HANDLE OpenFile(LPWSTR);

	static std::string GetFileName(LPWSTR);

	static BOOL GetSystemTimeFromFile(FILETIME&, SYSTEMTIME&);

	static BOOL ReadFile(HANDLE, char*, LPOVERLAPPED = NULL);

	static BOOL GetFileSize(HANDLE, LARGE_INTEGER&);
	
	static BOOL GetFileInfo(HANDLE, BY_HANDLE_FILE_INFORMATION&);

	static BOOL CloseHandle(HANDLE);

	static BOOL WriteFile(HANDLE, char*, LPOVERLAPPED = NULL);

	static BOOL SetFilePointerToBegin(HANDLE);

};
