#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "OS11_HTAPI.h"
#include <wchar.h>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <locale>
#include <codecvt>

typedef HT::HTHANDLE* (__stdcall *Create)(int, int, int, int, const wchar_t*);
typedef BOOL(__stdcall *Close)(HT::HTHANDLE*);

wchar_t* GetWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}


int main(int argc, char* argv[])
{
	HINSTANCE hDLL = LoadLibrary(L"..\\x64\\Debug\\OS11_HTAPI.dll");
	if (hDLL != NULL)
	{
		std::cout << "DLL!" << std::endl;
		try {

			int capacity = 2000;
			int snapshotIntervalSec = 3;
			int maxKeyLength = 4;
			int maxPayloadLength = 4;
			wchar_t* fileName{nullptr};
			const wchar_t* directoryPath = L"..\\..\\HT";
			std::wstring filePath(L"..\\..\\HT\\test.ht");

			if (argc == 6)
			{
				capacity = std::stoi(argv[1]);
				snapshotIntervalSec = std::stoi(argv[2]);
				maxKeyLength = std::stoi(argv[3]);
				maxPayloadLength = std::stoi(argv[4]);
				std::cout << GetWC(argv[5]) << std::endl;
				std::cout << argv[5];
				fileName = GetWC(argv[5]);
				std::wstring s(directoryPath);
				s += L"\\" + std::wstring(fileName);
				filePath = s;
			}
			else {
				std::cout << "Using default values" << std::endl;
			}

			HT::HTHANDLE* HT = nullptr;

			Create create = (Create)GetProcAddress(hDLL, "Create");
			if (create != NULL)
				HT = create(capacity, snapshotIntervalSec, maxKeyLength, maxPayloadLength, filePath.c_str());

			if (HT == NULL)
				throw "Invalid handle";

			std::cout << "HT-Storage Created " << std::endl;
			std::wcout << "filename: " << filePath << std::endl;
			std::cout << "snapshotIntervalSec: " << snapshotIntervalSec << std::endl;
			std::cout << "capacity: " << capacity << std::endl;
			std::cout << "maxKeyLength: " << maxKeyLength << std::endl;
			std::cout << "maxPayloadLength: " << maxPayloadLength << std::endl;

			FreeLibrary(hDLL); // выгружаем DLL
			return 0;
		}
		catch (const char* err)
		{
			std::cout << std::endl << err << std::endl;
			FreeLibrary(hDLL);
			return 1;
		}
	}
	else
	{
		std::cout << "Не удалось загрузить DLL!" << std::endl;
		return 1;
	}
}
