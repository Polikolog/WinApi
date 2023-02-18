#include "Solution.h"
#include <fstream>

HANDLE Solution::OpenFile(LPWSTR fileName)
{
	HANDLE file = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		0, OPEN_EXISTING, NULL, NULL);

	if (file == INVALID_HANDLE_VALUE)
	{
		std::cout << "File error(create/open): " << GetLastError() << std::endl;
		return INVALID_HANDLE_VALUE;
	}

	return file;
}

BOOL Solution::GetSystemTimeFromFile(FILETIME& fileTime, SYSTEMTIME& sysTime)
{
	if (!::FileTimeToSystemTime(&fileTime, &sysTime))
	{
		std::cout << "File error(system_time): " << GetLastError() << std::endl;
		return FALSE;
	}
	std::cout << sysTime.wYear << "." << sysTime.wMonth << "." << sysTime.wDay << "/" << sysTime.wHour << ":" << sysTime.wMinute << std::endl;
	return TRUE;
}


BOOL Solution::ReadFile(HANDLE file, char* buffer, LPOVERLAPPED ovl)
{
	DWORD bufRead;

	if (!::ReadFile(file, buffer, size, &bufRead, ovl))
	{
		std::cout << "File error(read): " << GetLastError() << std::endl;
		return FALSE;
	}
	return TRUE;
}


BOOL Solution::GetFileSize(HANDLE file, LARGE_INTEGER& fileSize)
{

	if (!GetFileSizeEx(file, &fileSize))
	{
		std::cout << "File error(size): " << GetLastError() << std::endl;
		return FALSE;
	}
	std::cout << "File size: " << fileSize.QuadPart << "kb" << std::endl;
	return TRUE;
}

BOOL Solution::GetFileInfo(HANDLE file, BY_HANDLE_FILE_INFORMATION& fileInfo)
{
	if (!GetFileInformationByHandle(file, &fileInfo))
	{
		std::cout << "File error(file_time): " << GetLastError() << std::endl;
		return FALSE;
	}
	return TRUE;
}

BOOL Solution::CloseHandle(HANDLE file)
{
	if (!::CloseHandle(file))
	{
		std::cout << "File error(close): " << GetLastError() << std::endl;
		return FALSE;
	}
	return TRUE;
}

BOOL Solution::WriteFile(HANDLE file, char* buffer, LPOVERLAPPED ovl)
{
	DWORD bufRead;
	if (!::WriteFile(file, buffer, BUFFER_SIZE, &bufRead, NULL))
	{
		std::cout << "File error(write): " << GetLastError() << std::endl;
		return FALSE;
	}
	return TRUE;
}

BOOL Solution::SetFilePointerToBegin(HANDLE file)
{
	LARGE_INTEGER from{ 0 };
	LARGE_INTEGER to;
	if (!SetFilePointerEx(file, from, &to, FILE_BEGIN))
	{
		std::cout << "File error(set pointer): " << GetLastError() << std::endl
			<< "Old Pointer: " << to.QuadPart;
		return FALSE;
	}
	return TRUE;
}

