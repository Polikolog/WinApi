#pragma once
#include <objbase.h>

// {0DCAAC49-B932-4EB7-9E69-68309E74BA4A}
static const IID IID_IAdder = { 0xdcaac49, 0xb932, 0x4eb7, { 0x9e, 0x69, 0x68, 0x30, 0x9e, 0x74, 0xba, 0x4a } };

interface IAdder : public IUnknown 
{
	STDMETHOD(Add(const double, const double, double&)) PURE;
	STDMETHOD(Sub(const double, const double, double&))PURE;
};
