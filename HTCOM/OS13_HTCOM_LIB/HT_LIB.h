#pragma once
#include "../OS13_HTCOM/HT.h"

class IHT;

#define OS12HANDEL void*

namespace HT_LIB
{
	OS12HANDEL Init();
	BOOL Create();
	void Dispose(OS12HANDEL);
	void OutputErrorHandle(int);

	namespace HT
	{
		HTHANDLE Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const std::wstring FileName);
		HTHANDLE Open(const std::wstring FileName);
		HTHANDLE OpenExist();
		BOOL Insert(HTHANDLE*, Element*);
		BOOL Get(HTHANDLE*, Element*);
		BOOL Delete(HTHANDLE*, Element*);
		BOOL Update(HTHANDLE* hthandle, Element* oldelement, void* newpayload, int newpayloadlenth);
		char* GetLastError(HTHANDLE*);
		void print(const Element*);
	}

	IHT* p_Ht = nullptr;
}