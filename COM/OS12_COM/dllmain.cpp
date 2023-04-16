#include "pch.h"
#include "OS12.h"

long    g_lObjs = 0;
long    g_lLocks = 0;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

STDAPI DLLGetClassObject(const CLSID& clsid, const IID& iid, void** ppvObject)
{
    OS12* po{ nullptr };
    if (clsid != CLSID_OS12)
        return CLASS_E_CLASSNOTAVAILABLE;
    if ((po = new OS12()) == NULL)
        return E_OUTOFMEMORY;
    HRESULT rc = po->QueryInterface(iid, ppvObject);
    po->Release();
    return rc;
}

STDAPI DLLCanUnloadNow(void)
{
    if (g_lObjs || g_lLocks)
        return S_OK;
    else
        return S_FALSE;
}


