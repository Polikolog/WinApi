#pragma once
#include <objbase.h>

// {4762408A-0D91-48ED-B3F8-1E095E7274F0}
static const IID IID_IMultiplier = { 0x4762408a, 0xd91, 0x48ed, { 0xb3, 0xf8, 0x1e, 0x9, 0x5e, 0x72, 0x74, 0xf0 } };

interface IMultiplier : public IUnknown
{
	STDMETHOD(Mul(const double, const double, double&)) PURE;
	STDMETHOD(Div(const double, const double, double&)) PURE;
};