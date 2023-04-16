
#include "pch.h"
#include "framework.h"

#include "HT.h"

namespace HT
{

	HTHANDLE::~HTHANDLE()
	{
		if (!_file)
			CloseHandle(_file);
		if (!_fileMapping)
			CloseHandle(_fileMapping);
		if (!_addr)
			UnmapViewOfFile(_addr);
	}


	HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const std::wstring FileName)
	{
		auto ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
		DWORD size = sizeof(HTHANDLE) + Capacity * (sizeof(Element) + MaxKeyLength + MaxPayloadLength);
		try
		{
			ht->_file = CreateFile(FileName.c_str(), GENERIC_ALL, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (ht->_file == INVALID_HANDLE_VALUE)
			{
				throw "Error open file";
			}

			ht->_fileMapping = CreateFileMapping(ht->_file, NULL, PAGE_READWRITE, 0, size, L"Polu");
			if (!ht->_fileMapping)
			{
				throw "Error create mapping";
			}

			ht->_addr = MapViewOfFile(ht->_fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, size);
			if (!ht->_addr)
			{
				throw "Error map view";
			}

			return ht;
		}
		catch (char* e)
		{
			std::cout << e << std::endl;
			delete ht;
			return NULL;
		}
	}

	HTHANDLE* Open(const std::wstring FileName)
	{
		HTHANDLE* ht;
		HANDLE file = NULL;
		HANDLE fileMapping = NULL;
		HANDLE viewMap = NULL;
		try
		{
			/*file = CreateFile(FileName.c_str(), GENERIC_ALL, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (!file)
			{
				throw "Error open file";
			}*/

			fileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, L"Polu");
			if (!fileMapping)
			{
				throw "Error open mapping";
			}

			viewMap = MapViewOfFile(fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (!viewMap)
			{
				throw "Error open map view";
			}

			ht = (HTHANDLE*)viewMap;

			ht->_addr = viewMap;
			return ht;
		}
		catch (char* e)
		{
			std::cout << e << std::endl;
			CloseHandle(file);
			CloseHandle(fileMapping);
			CloseHandle(viewMap);
			return NULL;
		}
	}


	BOOL Insert(HTHANDLE* hthandle, const Element* element)
	{
		if (element->_keyLength > hthandle->_maxKeyLength)
			return FALSE;
		if (element->_payloadLength > hthandle->_maxPayloadLength)
			return FALSE;
		if (hthandle->_currentCapacity == hthandle->_capacity)
			return FALSE;

		auto pointer = (BYTE*)hthandle->_addr + (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength) * hthandle->_currentCapacity;

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
		auto pointer = (BYTE*)hthandle->_addr;

		for (int i = 0; i < hthandle->_currentCapacity; i++)
		{
			BYTE* currentPointer = pointer + (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength) * i;
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
		auto deletedElement = Get(hthandle, element);
		if (deletedElement)
			return FALSE;

		BYTE* deletedElementPtr = (BYTE*)deletedElement;
		BYTE* firstElementPtr = (BYTE*)hthandle->_addr;
		int deletedElementIndex = (deletedElementPtr - firstElementPtr) / (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength);

		// Zero deletedElement memory
		ZeroMemory(deletedElementPtr, (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength));

		// We have to replace elements after deletedElement
		BYTE* currentElementPtr = deletedElementPtr;
		for (int i = deletedElementIndex; i < hthandle->_capacity; i++)
		{
			// When the last element
			if (i == hthandle->_capacity - 1)
			{
				ZeroMemory(currentElementPtr, (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength));
				hthandle->_currentCapacity--;
				return TRUE;
			}

			BYTE* nextElementPtr = currentElementPtr + (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength);
			CopyMemory(currentElementPtr, nextElementPtr, (sizeof(Element) + hthandle->_maxKeyLength + hthandle->_maxPayloadLength));
			currentElementPtr = nextElementPtr;
		}
		return TRUE;
	}

	BOOL Update(const HTHANDLE* hthandle, const Element* oldelement, const void* newpayload, int newpayloadlength)
	{
		auto newElem = Get(hthandle, oldelement);
		if (newElem)
			return FALSE;

		newElem->_payloadLength = newpayloadlength;

		CopyMemory(newElem->_payload, newpayload, newpayloadlength);

		return TRUE;
	}

	BOOL Snap(HTHANDLE* htHandle)
	{
		DWORD htSize = sizeof(HTHANDLE) + htHandle->_capacity * (sizeof(Element) + htHandle->_maxKeyLength + htHandle->_maxPayloadLength);

		if (htHandle->_file == INVALID_HANDLE_VALUE)
		{
			std::string str = "Snap file creation error";
			strcpy_s(htHandle->LastErrorMessage, str.length() + 1, str.c_str());
			return FALSE;
		}

		WriteFile(htHandle->_file, htHandle, htSize, NULL, NULL);

		std::cout << "Snap success" << std::endl;

		return TRUE;
	}

	BOOL Close(const HTHANDLE* htHandle)
	{
		if (htHandle->_file)
			CloseHandle(htHandle->_file);
		else
			return FALSE;
		if (htHandle->_fileMapping)
			CloseHandle(htHandle->_fileMapping);
		else
			return FALSE;
		if (htHandle->_addr)
			UnmapViewOfFile(htHandle->_addr);
		else
			return FALSE;

		return TRUE;
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
