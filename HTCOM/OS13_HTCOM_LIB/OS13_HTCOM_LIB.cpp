#include "pch.h"
#include "framework.h"
#include <iostream>
#include "HT_LIB.h"

OS12HANDEL HT_LIB::Init()
{
	try
	{
		IUnknown* pIUnknown = NULL;
		CoInitialize(NULL);                        // инициализация библиотеки OLE32
		HRESULT hr = CoCreateInstance(CLSID_HT, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
		if (!SUCCEEDED(hr))
			throw hr;
		return pIUnknown;
	}
	catch (HRESULT rc)
	{
		OutputErrorHandle(rc);
		return NULL;
	}
}

void HT_LIB::Dispose(OS12HANDEL h)
{
	static_cast<IUnknown*>(h)->Release();
	CoFreeUnusedLibraries();
}

void HT_LIB::OutputErrorHandle(int rc)
{
	LPWSTR errorText = nullptr;
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		nullptr, rc, 0, (LPWSTR)&errorText, 0, nullptr);
	std::wcout << L"Error: " << rc << L" - " << errorText << std::endl;
	LocalFree(errorText);
}
