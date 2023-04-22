#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "OS11_HTAPI.h"
#include <wchar.h>
#include <string>
#include <sstream>
#include <ctime>
#include <chrono>

HANDLE mutex;

DWORD WINAPI RecordingWork(LPVOID point)
{
	try {
		auto pr1 = reinterpret_cast<std::pair<std::pair<HT::Element, HT::Element>, std::wstring>*>(point);
		auto *pr2 = &std::get<0>(*pr1);
		HT::Element* el = &pr2->first;
		HT::Element* el2 = &pr2->second;
		std::wstring fileName = pr1->second;

		std::cout << "Recording...." << std::endl;

		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		std::tm* now_tm = std::localtime(&now_c);

		std::wstringstream ss;
		ss << std::put_time(now_tm, L"%Y-%m-%d_%H-%M-%S") << L".txt";
		std::wstring date = ss.str();
		std::wstring filePath(L"..\\..\\Records\\Update\\" + date);

		auto file = CreateFile(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
			throw "Error create file record\t" + GetLastError();

		const char* key_str = static_cast<const char*>(el->key);
		const char* payload_str = static_cast<const char*>(el->payload);

		std::string key;
		std::string payload;

		for (int i{}; i < el->keylength; i++)
		{
			if (key_str[i] != '\n')
				key.push_back(key_str[i]);
		}
		for (int i{}; i < el->payloadlength; i++)
		{
			if (payload_str[i] != '\n')
			{
				if (payload_str[i] == '0')
					payload.push_back('0');
				else
					payload.push_back(payload_str[i]);
			}
		}
		
		std::string str = "Update element:\tKey: " + key + "\tOLD-Payload: " + payload;

		const char* npayload_str = static_cast<const char*>(el2->payload);
		std::string npayload;
		for(int i{}; i < el2->payloadlength; i++)
		{
			if (npayload_str[i] != '\n')
				npayload.push_back(npayload_str[i]);
		}

		str += "\tNEW-Payload: " + npayload + "\tFile name:\t" + std::string(fileName.begin(), fileName.end());

		if (!WriteFile(file, str.c_str(), static_cast<DWORD>(str.size() + 1), NULL, NULL))
			throw "Error write file record";

		if (!CloseHandle(file))
			throw "Error close file record";

		std::cout << "Record success" << std::endl;

		return 0;
	}
	catch (char* err)
	{
		std::cout << err << std::endl;
		return 1;
	}
}

wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

void printStr(char* str) {
	std::cout << "ERROR:\t";
	int i = 0;
	while (str[i]) {
		std::cout << str[i];
		i++;
	}
	std::cout << std::endl;
}


int main(int argc, char* argv[])
{
	mutex = CreateMutex(NULL, FALSE, L"Record");
	const HINSTANCE hDLL = LoadLibrary(L"..\\x64\\Debug\\OS11_HTAPI.dll");
	if (hDLL != NULL)
	{
		const auto createFullElement = (HT::Element * (*)(const void*, int, const void*, int))GetProcAddress(hDLL, "CreateNewFullElement");
		if (createFullElement == NULL)
			throw "Error createElement";

		const auto createKeyElement = (HT::Element * (*)(const void*, int))GetProcAddress(hDLL, "CreateNewFullElement");
		if (createKeyElement == NULL)
			throw "Error createElement";

		const auto insert = (BOOL(*)(HT::HTHANDLE*, HT::Element*))GetProcAddress(hDLL, "Insert");
		if (insert == NULL)
			throw "Error insert";

		const auto update = (BOOL(*)(HT::HTHANDLE*, HT::Element*, void*, int))GetProcAddress(hDLL, "Update");
		if (update == NULL)
			throw "Error update";

		const auto get = (HT::Element * (*)(HT::HTHANDLE*, HT::Element*))GetProcAddress(hDLL, "Get");
		if (get == NULL)
			throw "Error get";

		const auto openExist = (HT::HTHANDLE * (*)(const wchar_t*))GetProcAddress(hDLL, "OpenExist");
		if (openExist == NULL)
			throw "Error openExist";

		const auto getLastError = (char* (*)(HT::HTHANDLE*))GetProcAddress(hDLL, "GetLastErrorProg");
		if (getLastError == NULL)
			throw "Error getLastError";

		const auto print = (void(*)(HT::Element*))GetProcAddress(hDLL, "print");
		if (print == NULL)
			throw "Error print";

		srand(time(0));

		wchar_t* fileName;
		const wchar_t* directoryPath = L"..\\..\\HT";
		std::wstring filePath(L"..\\..\\HT\\test.ht");

		try {

			if (argv[1]) {
				fileName = GetWC(argv[1]);
				std::wstring s(directoryPath);
				s += L"\\" + std::wstring(fileName);
				filePath = s;
			}

			HT::HTHANDLE* HT;
			HT = openExist(filePath.c_str());
			if (HT == NULL)
				throw "Invalid handle";

			while(true)
			{
				int numberKey = rand() % 50 + 1;
				std::string key;
				std::stringstream convert;

				convert << numberKey;
				key = convert.str();
				std::cout << std::endl << key << ' ' << key.length() << std::endl;

				HT::Element* el;
				HT::Element* el1 = createKeyElement(key.c_str(), key.length() + 1);
				HT::Element* el2;

				if ((el = get(HT, el1)) == NULL)
					printStr(getLastError(HT));
				else {
					print(el);
					HT::Element oel = *el;
					std::stringstream ss;
					int num;
					ss << *(char*)el->payload;
					ss >> num;
					int newNum = num + 1;

					std::string payload;
					std::stringstream payloadConvert;
					payloadConvert << newNum;
					payload = payloadConvert.str();

					el2 = createFullElement(key.c_str(), key.length() + 1, payload.c_str(), payload.length() + 1);

					std::pair<std::pair<HT::Element, HT::Element>, std::wstring> pr{std::make_pair(oel, *el2), fileName};

					HANDLE hThread{NULL};
					if (WaitForSingleObject(mutex, INFINITE) == WAIT_OBJECT_0)
						hThread = CreateThread(NULL, 0, RecordingWork, &pr, 0, NULL);

					WaitForSingleObject(hThread, INFINITE);

					ReleaseMutex(mutex);

					if (!update(HT, el1, el2->payload, el2->payloadlength)) {
						printStr(getLastError(HT));
					}
					else
					{
						std::cout << "UPDATE SUCCESS" << std::endl;
					}
					if ((el = get(HT, el1)) == NULL)
						printStr(getLastError(HT));
				}
				Sleep(1000);
			}
		}
		catch (const char* err) {
			FreeLibrary(hDLL);
			std::cout << err << std::endl;
			return 1;
		}
	}
	else
	{
		std::cout << "Не удалось загрузить DLL!" << std::endl;
		return 1;
	}
}
