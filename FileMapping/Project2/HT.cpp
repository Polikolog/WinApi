#include "HT.h"

namespace HT
{

	HTHANDLE::~HTHANDLE()
	{
		if(_file != INVALID_HANDLE_VALUE)
			CloseHandle(_file);
		if(_fileMapping != INVALID_HANDLE_VALUE)
			CloseHandle(_fileMapping);
		if(_addr != INVALID_HANDLE_VALUE)
			UnmapViewOfFile(_addr);
	}


	HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const std::wstring FileName)
	{
		auto ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
		DWORD size = sizeof(HTHANDLE) + Capacity * (sizeof(Element) + MaxKeyLength + MaxPayloadLength);
		try
		{
			ht->_file = CreateFile(FileName.c_str(), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (!ht->_file)
			{
				throw "Error open file";
			}

			ht->_fileMapping = CreateFileMapping(ht->_file, NULL, PAGE_READWRITE, 0, size, L"Polu");
			if(!ht->_fileMapping)
			{
				throw "Error create mapping";
			}

			ht->_addr = MapViewOfFile(ht->_fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, size);
			if(!ht->_addr)
			{
				throw "Error map view";
			}

			return ht;
		}
		catch(char* e)
		{
			std::cout << e << std::endl;
			delete ht;
			return NULL;
		}
	}

	HTHANDLE* Open(const std::wstring FileName)
	{
		HTHANDLE* ht;
		try
		{
			/*auto file = CreateFile(FileName.c_str(), GENERIC_ALL, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(!file)
			{
				throw "Error open file";
			}*/

			auto fileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"Polu");
			if(fileMapping == NULL)
			{
				throw "Error open mapping";
			}

			auto viewMap = MapViewOfFile(fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if(!viewMap)
			{
				throw "Error open map view";
			}

			ht = (HTHANDLE*)viewMap;
			return ht;
		}
		catch(char* e)
		{
			std::cout << e << std::endl;
			return NULL;
		}
	}


	BOOL Insert(HTHANDLE* hthandle,const Element* element)
	{
		if (element->_keyLength > hthandle->_maxKeyLength)
			return FALSE;
		if (element->_payloadLength > hthandle->_maxPayloadLength)
			return FALSE;
		if (hthandle->_currentCapacity == hthandle->_capacity)
			return FALSE;
		
		auto pointer = (byte*)hthandle->_addr + (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength) * hthandle->_currentCapacity;

		Element* elem = (Element*)pointer;

		elem->_key = pointer + sizeof(Element);
		elem->_payload = pointer + sizeof(Element) + hthandle->_maxKeyLength;
		elem->_keyLength = element->_keyLength;
		elem->_payloadLength = element->_payloadLength;
		CopyMemory(elem->_key, element->_key, element->_keyLength);
		CopyMemory(elem->_payload, element->_payload, element->_payloadLength);

		hthandle->_currentCapacity++;
		return TRUE;
	}

	Element* Get(const HTHANDLE* hthandle, const Element* element)
	{
		auto pointer = (byte*)hthandle->_addr;

		for (int i = 0; i < hthandle->_currentCapacity; i++)
		{
			byte* currentPointer = pointer + (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength) * i;
			Element* currentElement = (Element*)currentPointer;
			if (currentElement->_keyLength == element->_keyLength &&
				strcmp((char*)currentElement->_key, (char*)element->_key) == 0)
			{
				return currentElement;
			}
		}

		return NULL;
	}


	BOOL Delete(HTHANDLE* hthandle, const Element* element)
	{
		/*auto deletedElement = Get(hthandle, element);
		if (deletedElement)
			return FALSE;*/

		byte* deletedElementPtr = (byte*)element;//(byte*)deletedElement;
		byte* firstElementPtr = (byte*)hthandle->_addr;
		int deletedElementIndex = (deletedElementPtr - firstElementPtr) / (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength);

		// Zero deletedElement memory
		ZeroMemory(deletedElementPtr, (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength));

		// We have to replace elements after deletedElement
		byte* currentElementPtr = deletedElementPtr;
		for (int i = deletedElementIndex; i < hthandle->_capacity; i++)
		{
			// When the last element
			if (i == hthandle->_capacity - 1)
			{
				ZeroMemory(currentElementPtr, (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength));
				hthandle->_currentCapacity--;
				return TRUE;
			}

			byte* nextElementPtr = currentElementPtr + (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength);
			CopyMemory(currentElementPtr, nextElementPtr, (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength));
			currentElementPtr = nextElementPtr;
		}
		return TRUE;
	}

	BOOL Update(const HTHANDLE* hthandle, Element* oldelement, const void* newpayload, int newpayloadlength)
	{
		//auto newElem = Get(hthandle, oldelement);
		//if (newElem)
			//return FALSE;

		oldelement->_payloadLength = newpayloadlength;

		CopyMemory(oldelement->_payload, newpayload, newpayloadlength);

		return TRUE;
	}

	BOOL Snap(HTHANDLE* htHandle)
	{
		DWORD htSize = sizeof(HTHANDLE) + htHandle->_capacity * (sizeof(Element) + htHandle->_maxKeyLength + htHandle->_maxPayloadLength);
		if (htHandle->_file == INVALID_HANDLE_VALUE)
		{
			std::cout << ::GetLastError() << std::endl;
			std::string str = "Snap file creation error";
			strcpy_s(htHandle->LastErrorMessage, str.length() + 1, str.c_str());
			return FALSE;
		}

		WriteFile(htHandle->_file, htHandle, htSize, NULL, NULL);

		return TRUE;
	}

	BOOL Close(const HTHANDLE* htHandle)
	{
		if (CloseHandle(htHandle->_fileMapping) && CloseHandle(htHandle->_file) && UnmapViewOfFile(htHandle->_addr))
		{
			std::cout << "delete" << std::endl;

			delete htHandle;
			return TRUE;
		}
		delete htHandle;

		return FALSE;
	}

	char* GetLasrError(HTHANDLE* ht)
	{
		return ht->LastErrorMessage;
	}

	void print(const Element* element)
	{
		std::cout << "Elemet key:\t" << (char*)element->_key << "\nElement value:\t" << (char*)element->_payload << std::endl;
	}

}
