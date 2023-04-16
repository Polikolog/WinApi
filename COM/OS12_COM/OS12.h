#pragma once
#include "IAdder.h"
#include "IMultiplier.h"

// {CB5C3100-6A0D-44A5-A1DC-9CA1A1B03DD1}
DEFINE_GUID(CLSID_OS12,	0xcb5c3100, 0x6a0d, 0x44a5, 0xa1, 0xdc, 0x9c, 0xa1, 0xa1, 0xb0, 0x3d, 0xd1);

class OS12 : public IAdder, public IMultiplier
{
public:
	OS12();
	~OS12();

	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG,Release());
	STDMETHOD(QueryInterface(const IID& riid, void** ppvObject));

	STDMETHOD(Add(const double x, const double y, double& z));
	STDMETHOD(Sub(const double x, const double y, double& z));
	STDMETHOD(Mul(const double x, const double y, double& z));
	STDMETHOD(Div(const double x, const double y, double& z));

private:
	LONG m_refCount;
};
