#pragma once
#include <objbase.h>

class OS12ClassFactory : IClassFactory
{
	OS12ClassFactory();
	~OS12ClassFactory();

	STDMETHOD(QueryInterface(const IID& riid, void** ppvObject));
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());

	STDMETHOD(CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppvObject));
	STDMETHOD(LockServer(BOOL fLock));
};