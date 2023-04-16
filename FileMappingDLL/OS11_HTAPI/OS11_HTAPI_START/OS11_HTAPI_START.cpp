#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "OS11_HTAPI.h"
#include <wchar.h>

//typedef HT::HTHANDLE* (*Open)(const wchar_t[512]);
typedef BOOL (*Close)(HT::HTHANDLE*);
typedef char* (*GetLastErrorProg)(HT::HTHANDLE*);

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
		GetLastErrorProg err = (GetLastErrorProg)GetProcAddress(hDLL, "GetLastErrorProg");
		if (err == NULL)
			throw "Error get proc address for func GETLASTERRORPROG";

		wchar_t* fileName{nullptr};
		const std::wstring directoryPath = L"..\\..\\HT\\";
		std::wstring filePath(L"..\\..\\HT\\test.ht");

		try {
			if (argv[1]) {
				/*fileName = GetWC(argv[1]);
				std::wstring s(directoryPath);
				s += std::wstring(fileName);
				filePath = s;*/filePath = GetWC(argv[1]);
			}
			else
				std::cout << "Using default file path" << std::endl;

			

			HT::HTHANDLE* HT{nullptr};

			auto open = (HT::HTHANDLE*(*)(const wchar_t*))GetProcAddress(hDLL, "Open");
			if (open == NULL)
				throw "Error get proc address for func open";
				
			HT = open(filePath.c_str());

			if (HT == NULL)
			{
				std::cout <<" Error open " << err(HT) << std::endl;
				throw "Invalid handle";
			}

			std::cout << "HT-Storage Created " << std::endl;
			std::wcout << "filename: " << filePath << std::endl;
			std::cout << "snapshotIntervalSec: " << HT->SecSnapshotInterval << std::endl;
			std::cout << "capacity: " << HT->Capacity << std::endl;
			std::cout << "maxKeyLength: " << HT->MaxKeyLength << std::endl;
			std::cout << "maxPayloadLength: " << HT->MaxPayloadLength << std::endl;

			int key;
			std::cin >> key;

			Close close = (Close)GetProcAddress(hDLL, "Close");

			if (!close(HT))
				throw "Error close";
		}
		catch (const char* err) 
		{
			std::cout << err << std::endl;
			FreeLibrary(hDLL);
			return 1;
		}
		FreeLibrary(hDLL);
		return 0;
	}
	else
	{
		std::cout << "Cant start DLL!" << std::endl;
		return 1;
	}
}
