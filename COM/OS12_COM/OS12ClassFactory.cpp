#include "pch.h"
#include "OS12ClassFactory.h"

OS12ClassFactory::OS12ClassFactory()
{
	
}

OS12ClassFactory::~OS12ClassFactory()
{
	
}

STDMETHODIMP OS12ClassFactory::QueryInterface(const IID& riid, void** ppvObject)
{
	switch(riid)
	{
	case IID_IUnknown:
		*ppvObject = static_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	case IID_IClassFactory:
		*ppvObject = static_cast<IClassFactory*>(this);
		AddRef();
		return S_OK;
	}


}

