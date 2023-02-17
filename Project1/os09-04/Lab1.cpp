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
	char buffer[BUFFER_SIZE]{};

	HANDLE file = Solution::OpenFile(fileName);
	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	if (!Solution::ReadFile(file, buffer))
		return FALSE;

	std::cout << buffer << std::endl;

	if (!Solution::CloseHandle(file))
		return FALSE;

	return TRUE;
}

//3
BOOL Lab1::delRowFileTxt(LPWSTR fileName, DWORD row)
{
	char buffer[BUFFER_SIZE]{};

	HANDLE file = Solution::OpenFile(fileName);
	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	if (!Solution::ReadFile(file, buffer))
		return FALSE;

	auto strings = General::split(buffer);
	strings.erase(strings.begin() + (row - 1));

	auto new_buffer = General::merge_strings(strings);

	if (!Solution::SetFilePointerToBegin(file))
		return FALSE;

	if (!Solution::WriteFile(file, &new_buffer[0]))
		return FALSE;

	if (!Solution::CloseHandle(file))
		return FALSE;

	return TRUE;
}

//4
BOOL Lab1::insRowFileTxt(LPWSTR fileName, LPWSTR str, DWORD row)
{
	char buffer[BUFFER_SIZE];

	HANDLE file = Solution::OpenFile(fileName);
	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	if (!Solution::ReadFile(file, buffer))
		return FALSE;

	auto strings = General::split(buffer);

	General::insertRow(row, str, strings);
	auto new_buffer = General::merge_strings(strings);

	if (!Solution::SetFilePointerToBegin(file))
		return FALSE;

	if (!Solution::WriteFile(file, &new_buffer[0]))
		return FALSE;

	if (!CloseHandle(file))
		return FALSE;

	return TRUE;
}

BOOL Lab1::printWathRowFileTxt(LPWSTR fileName, DWORD wait_mlsec)
{
	TCHAR buffer[BUFFER_SIZE];
	OVERLAPPED ovl{ 0 };
	ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	std::string wfileName;

	General::wstring_to_string(fileName, wfileName);
	std::string filePath = wfileName.substr(0, wfileName.find_last_of("\\"));

	HANDLE file = CreateFileA(filePath.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		std::cout << "Directory open error: " << GetLastError() << std::endl;
		return FALSE;
	}

	if (!ReadDirectoryChangesW(file, buffer, BUFFER_SIZE, FALSE,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES,
		NULL, &ovl, NULL))
	{
		std::cout << "Directory read error: " << GetLastError() << std::endl;
		return FALSE;
	}

	for (DWORD i{}; i < wait_mlsec / 200; i++)
	{
		DWORD nbt;
		DWORD result = ::WaitForSingleObject(ovl.hEvent, wait_mlsec / 100);

		switch (result)
		{
		case WAIT_TIMEOUT:
			std::cout << "Wait time out!" << std::endl;
			break;
		case WAIT_FAILED:
			std::cout << "Wait error: " << GetLastError() << std::endl;
			return FALSE;
		case WAIT_OBJECT_0:
		{
			if (!GetOverlappedResult(file, &ovl, &nbt, FALSE))
			{
				std::cout << "Error ger overlapped result: " << GetLastError() << std::endl;
				return FALSE;
			}

			FILE_NOTIFY_INFORMATION* some_event = (FILE_NOTIFY_INFORMATION*)buffer;


			switch (some_event->Action)
			{
			case FILE_ACTION_ADDED:
				std::wcout << "File added: " << std::wstring(some_event->FileName) << std::endl;
				break;
			case FILE_ACTION_MODIFIED:
				std::wcout << "File modified: " << std::wstring(some_event->FileName) << std::endl;
				break;
			case FILE_ACTION_REMOVED:
				std::wcout << "File removed: " << std::wstring(some_event->FileName) << std::endl;
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				std::wcout << "File rename from: " << std::wstring(some_event->FileName) << std::endl;
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				std::wcout << "File raname to: " << std::wstring(some_event->FileName) << std::endl;
				break;
			default:
				std::wcout << "Unknown action with file: " << std::wstring(some_event->FileName) << std::endl;
				break;
			}

			if (!ReadDirectoryChangesW(file, buffer, BUFFER_SIZE, FALSE,
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES,
				NULL, &ovl, NULL))
			{
				std::cout << "Directory read error: " << GetLastError() << std::endl;
				return FALSE;
			}

			break;
		}
		default:
			std::cout << "Failed wait: " << GetLastError() << std::endl;
			return FALSE;
		}

	}

	return TRUE;
}
