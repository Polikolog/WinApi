#pragma once

#include <objbase.h>
#include "HTHandle.h"

interface IHT : public IUnknown
{
	STDMETHOD_(HTHANDLE*, Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const std::wstring FileName)) PURE;
	STDMETHOD_(HTHANDLE*, Open(const std::wstring FileName)) PURE;
	STDMETHOD_(HTHANDLE*, OpenExist(const std::wstring FileName)) PURE;
	STDMETHOD_(BOOL, Snap(HTHANDLE* hthandle)) PURE;
	STDMETHOD_(BOOL, Insert(HTHANDLE* hthandle, Element* element)) PURE;
	STDMETHOD_(BOOL, Delete(HTHANDLE* hthandle, Element* element)) PURE;
	STDMETHOD_(Element*, Get(HTHANDLE* hthandle, Element* element)) PURE;
	STDMETHOD_(BOOL, Update(HTHANDLE* hthandle, Element* oldelement, void* newpayload, int newpayloadlenth)) PURE;
	STDMETHOD_(BOOL, Close(const HTHANDLE* htHandle)) PURE;
	STDMETHOD_(BOOL, CloseExist(const HTHANDLE* hthandle));
	STDMETHOD_(char*, GetLastErrorProg(HTHANDLE* ht)) PURE;
	STDMETHOD_(void, print(const Element* element)) PURE;
};
