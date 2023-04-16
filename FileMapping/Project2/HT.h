#pragma once

#include "windows.h"
#include <iostream>


namespace HT
{
	struct HTHANDLE
	{
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const std::wstring FileName)
			:_capacity{ Capacity }, _currentCapacity{0}, _secSnapshotInterval {
			SecSnapshotInterval
		}, _maxKeyLength{ MaxKeyLength },
			_maxPayloadLength{ MaxKeyLength }, _fileName{ FileName } {}
		~HTHANDLE();
		int _capacity;
		int _currentCapacity;
		int _secSnapshotInterval;
		int _maxKeyLength;
		int _maxPayloadLength;
		std::wstring _fileName;
		HANDLE _file;
		HANDLE _fileMapping;
		LPVOID _addr;
		char LastErrorMessage[512];
		time_t _lastsnaptime;

	};
	struct Element
	{
		Element();
		Element(const void* key, int keylength)
			:_key{ (void*)key }, _keyLength{ keylength } {}
		Element(const void* key, int keylength, const void* payload, int payloadlength)
			:_key{ (void*)key }, _keyLength{ keylength }, _payload{(void*)payload }, _payloadLength{ payloadlength } {}
		Element(Element* oldelement, const void* newpayload, int newpayloadlength);
		void* _key;
		int _keyLength;
		void* _payload;
		int _payloadLength;
	};
	HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const std::wstring FileName);
	HTHANDLE* Open(const std::wstring FileName);
	BOOL Snap(HTHANDLE* hthandle);
	BOOL Insert(HTHANDLE* hthandle, const Element* element);
	BOOL Delete(HTHANDLE* hthandle, const Element* element);
	Element* Get(const HTHANDLE* hthandle, const Element* element);
	BOOL Update(const HTHANDLE* hthandle, Element* oldelement, const void* newpayload, int newpayloadlenth);
	BOOL Close(const HTHANDLE* htHandle);
	char* GetLasrError(HTHANDLE* ht);
	void print(const Element* element);
}
