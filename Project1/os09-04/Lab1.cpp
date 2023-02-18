#include "Lab1.h"

//5
BOOL Lab1::printWathRowFileTxt(LPWSTR fileName, DWORD wait_mlsec)
{
	TCHAR buffer[BUFFER_SIZE];
	OVERLAPPED ovl{ 0 };
	ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	std::string wfileName;

	General::wstring_to_string(fileName, wfileName);
	std::string filePath = wfileName.substr(0, wfileName.find_last_of("\\"));
	std::wstring wfilePath = std::wstring(fileName).substr(0, std::wstring(fileName).find_last_of(L"\\"));
	HANDLE file = CreateFileA(filePath.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		std::cout << "Directory open error: " << GetLastError() << std::endl;
		return FALSE;
	}

	HANDLE fl = Solution::OpenFile(fileName);

	DWORD size = GetFileSize(fl, NULL), bytes;

	std::string buf("", size);
	if (!Solution::ReadFile(fl,&buf[0], size))
		return FALSE;

	int countb = General::split(buf).size();
	CloseHandle(fl);

	if (!ReadDirectoryChangesW(file, buffer, BUFFER_SIZE, FALSE,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES,
		NULL, &ovl, NULL))
	{
		std::cout << "Directory read error: " << GetLastError() << std::endl;
		return FALSE;
	}

	for (DWORD i{}; i < wait_mlsec / 20; i++)
	{
		DWORD nbt;
		DWORD result = ::WaitForSingleObject(ovl.hEvent, wait_mlsec / 10);

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

			fl = Solution::OpenFile(fileName);

			if (fl == INVALID_HANDLE_VALUE)
				return FALSE;

			size = GetFileSize(fl, NULL), bytes;

			std::string nbuf("", size);

			if (!Solution::ReadFile(fl, &nbuf[0], nbuf.size()))
				return FALSE;
			CloseHandle(fl);

			int counta = General::split(nbuf).size();

			std::string name;
			General::wstring_to_string(some_event->FileName, name);

			switch (some_event->Action)
			{
			case FILE_ACTION_ADDED:
				std::wcout << "File added: " << std::wstring(some_event->FileName) << std::endl;
				break;
			case FILE_ACTION_MODIFIED:
			{
				if (counta > countb)
					std::cout << "File modified: " << name << "\n added " << counta - countb << " row(s)" << std::endl;
				if(counta < countb)
					std::cout << "File modified: " << name << "\n delete " << countb - counta << " row(s)" << std::endl;
				if(counta == countb)
					std::cout << "File modified: " << name << "\n rows dont added" << std::endl;
				countb = counta;
				break;
			}
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
