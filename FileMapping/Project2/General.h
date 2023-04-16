#pragma once
#include <vector>
#include <iostream>
#include <windows.h>
#include <string>
#include <algorithm>
#include <sstream>

namespace General
{
	extern DWORD SizeLow;

	std::vector<std::string> split(const std::string& s);

	std::string merge_strings(std::vector<std::string>& s);

	BOOL insertRow(DWORD row, LPWSTR wstr, std::vector<std::string>& s);

	void string_to_wstring(const std::string& src, std::wstring& dest);

	void wstring_to_string(const std::wstring& src, std::string& dest);

	std::string GetFileName(LPWSTR filePath);

};