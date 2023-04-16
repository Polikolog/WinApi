#include <iostream>
#include "../OS12_COM/OS12.h"

#define IERR(s)	   std::cout << "error " << s << std::endl;
#define IRES(s, r) std::cout << s << r << std::endl;

IAdder *pIAdder = nullptr;
IMultiplier *pIMultiplier = nullptr;

void main()
{
	IUnknown* pIUnknown = NULL;
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_OS12, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
	if(SUCCEEDED(hr))
	{
		std::cout << "CoCreateInstance success" << std::endl;
		if(SUCCEEDED(pIUnknown->QueryInterface(IID_IAdder, (void**)&pIAdder)))
		{
			{
				double z = 0.0;
				if (!SUCCEEDED(pIAdder->Add(2.0, 3.0, z)))
					IERR("IAdder::Add")
				else
					IRES("IAdder::Add = ", z)
			}
			{
				double z = 0.0;
				if (!SUCCEEDED(pIAdder->Sub(2.0, 3.0, z)))
					IERR("IAdder::Sub")
				else
					IRES("IAdder::Sub = ", z)
			}
			if(pIAdder->QueryInterface(IID_IMultiplier, (void**)&pIMultiplier))
			{
				
			}
		}
		else
			IERR("IUnknown -> IAdder")
		if (SUCCEEDED(pIUnknown->QueryInterface(IID_IMultiplier, (void**)&pIMultiplier)))
		{
			{
				double z = 0.0;
				if (!SUCCEEDED(pIMultiplier->Mul(2.0, 3.0, z))) IERR("IMultiplier::Mul")
				else IRES("Multiplier::Mul = ", z);
			}
			{
				double z = 0.0;
				if (!SUCCEEDED(pIMultiplier->Div(2.0, 3.0, z))) IERR("IMultiplier::Div")
				else IRES("IMultiplier::Div = ", z);
			}
		}
		if (SUCCEEDED(pIMultiplier->QueryInterface(IID_IAdder, (void**)&pIAdder)))
		{
			double z = 0.0;
			if (!SUCCEEDED(pIAdder->Add(2.0, 3.0, z))) IERR("IAdder::Add")
			else IRES("IAdder::Add = ", z);
		}
		else  IERR("IMultiplier->IAdder");
		pIMultiplier->Release();
	}
}