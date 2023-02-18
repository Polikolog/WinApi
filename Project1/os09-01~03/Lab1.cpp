#include "Lab1.h"

//1
BOOL Lab1::printFileInfo(LPWSTR fileName)
{
	LARGE_INTEGER fileSize;
	BY_HANDLE_FILE_INFORMATION fileInfo;
	SYSTEMTIME createTime, modyTime;

	HANDLE file = Solution::OpenFile(fileName);
	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	std::cout << "File Name: " << General::GetFileName(fileName) << std::endl;

	switch (GetFileType(file))
	{
	case FILE_TYPE_CHAR:
		std::cout << "Fyle type: a character file, typically an LPT device or a console" << std::endl;
		break;
	case FILE_TYPE_DISK:
		std::cout << "File type: disk file" << std::endl;
		break;
	case FILE_TYPE_PIPE:
		std::cout << "File type: a socket, a named pipe, or an anonymous pipe" << std::endl;
		break;
	case FILE_TYPE_REMOTE:
		std::cout << "File type: unused" << std::endl;
		break;
	case FILE_TYPE_UNKNOWN:
		std::cout << "File type: file is unknown, or the function failed" << std::endl;
		break;
	default:
		std::cout << "Uknow" << std::endl;
		break;
	}

	if (!Solution::GetFileSize(file, fileSize))
		return FALSE;

	if (!Solution::GetFileInfo(file, fileInfo))
		return FALSE;

	std::cout << "Create time :\t";
	if (!Solution::GetSystemTimeFromFile(fileInfo.ftCreationTime, createTime))
		return FALSE;

	std::cout << "Modify time:\t";
	if (!Solution::GetSystemTimeFromFile(fileInfo.ftLastWriteTime, modyTime))
		return FALSE;

	if (!Solution::CloseHandle(file))
		return FALSE;

	return TRUE;
}

//2
BOOL Lab1::printFileTxt(LPWSTR fileName)
{
	HANDLE file = Solution::OpenFile(fileName);
	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD size = GetFileSize(file, NULL), bytes;
	std::string buffer("", size);
	if (!Solution::ReadFile(file, &buffer[0], buffer.size()))
		return FALSE;

	std::cout << buffer << std::endl;

	if (!Solution::CloseHandle(file))
		return FALSE;

	return TRUE;
}

//3
BOOL Lab1::delRowFileTxt(LPWSTR fileName, DWORD row)
{
	HANDLE file = Solution::OpenFile(fileName);
	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD size = GetFileSize(file, NULL), bytes;
	std::string buffer("", size);

	if (!Solution::ReadFile(file, &buffer[0], buffer.size()))
		return FALSE;

	CloseHandle(file);

	auto strings = General::split(buffer);

	strings.erase(strings.begin() + (row - 1));

	auto new_buffer = General::merge_strings(strings);

	file = CreateFile(fileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, TRUNCATE_EXISTING
		, NULL, NULL);

	if (!Solution::SetFilePointerToBegin(file))
		return FALSE;

	if (!Solution::WriteFile(file, &new_buffer[0], new_buffer.size()))
		return FALSE;

	if (!Solution::CloseHandle(file))
		return FALSE;

	return TRUE;
}

//4
BOOL Lab1::insRowFileTxt(LPWSTR fileName, LPWSTR str, DWORD row)
{
	HANDLE file = Solution::OpenFile(fileName);
	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD size = GetFileSize(file, NULL), bytes;
	std::string buffer("", size);

	if (!Solution::ReadFile(file, &buffer[0], buffer.size()))
		return FALSE;

	if (!Solution::CloseHandle(file))
		return FALSE;

	auto strings = General::split(buffer);

	General::insertRow(row, str, strings);
	auto new_buffer = General::merge_strings(strings);


	file = CreateFile(fileName, GENERIC_WRITE | GENERIC_READ
		,FILE_SHARE_READ | FILE_SHARE_WRITE
		,NULL, TRUNCATE_EXISTING
		,NULL, NULL);


	if (!Solution::SetFilePointerToBegin(file))
		return FALSE;

	if (!Solution::WriteFile(file, &new_buffer[0], new_buffer.size()))
		return FALSE;

	if (!CloseHandle(file))
		return FALSE;

	return TRUE;
}
