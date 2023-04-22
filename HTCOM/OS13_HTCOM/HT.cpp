#include "pch.h"
#include "HT.h"
#include <Windows.h>
#include <algorithm>
#include<iomanip>
#include <iostream>

long g_lLocks{ 0 };
long g_lObjs{ 0 };

LPCWSTR  FriendlyName = L"HT.COM";
LPCWSTR  VerIndProg = L"HT";
LPCWSTR  ProgID = L"HT.1.0";

// {1479DCD4-C2B8-4CB4-B9FD-BC70701AFD38}
DEFINE_GUID(IID_IHT,
	0x1479dcd4, 0xc2b8, 0x4cb4, 0xb9, 0xfd, 0xbc, 0x70, 0x70, 0x1a, 0xfd, 0x38);

// {871A3D0A-5F35-4452-BEF3-5BD8C0FBDECB}
DEFINE_GUID(CLSID_HT,
	0x871a3d0a, 0x5f35, 0x4452, 0xbe, 0xf3, 0x5b, 0xd8, 0xc0, 0xfb, 0xde, 0xcb);

HANDLE Addr;

DWORD WINAPI SnapShotCycle(HTHANDLE* ht);

HT::HT()
	:m_refCount{0}
{
	InterlockedIncrement(&g_lObjs);
}

HT::~HT()
{
	InterlockedDecrement(&g_lObjs);
}

HRESULT HT::QueryInterface(const IID& riid, void** ppvObject)
{
	if(riid == IID_IUnknown || riid == IID_IHT)
	{
		*ppvObject = static_cast<HT*>(this);
		AddRef();
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

ULONG HT::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}

ULONG HT::Release()
{
	return InterlockedDecrement(&m_refCount);
}

HTHANDLE* HT::Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const std::wstring FileName)
{
	HANDLE hf = CreateFile(
		FileName.c_str(),//path
		GENERIC_WRITE | GENERIC_READ,
		NULL,//режим совместного пользования
		NULL,// атрибуты защиты
		CREATE_NEW,// создание 
		FILE_ATTRIBUTE_NORMAL,//атрибуты и флаги
		NULL//файл атрибутов
	);
	if (hf == INVALID_HANDLE_VALUE) return NULL;
	DWORD HTsize = sizeof(HTHANDLE) + Capacity * (MaxKeyLength + MaxPayloadLength + 2 * sizeof(int));
	HANDLE hm = CreateFileMapping(
		hf,
		NULL,
		PAGE_READWRITE,
		0, HTsize, NULL);
	if (!hm)return NULL;
	std::cout << "Open FileMapping: " << hm << std::endl;

	LPVOID lp = MapViewOfFile(
		hm,
		FILE_MAP_ALL_ACCESS | FILE_MAP_READ | FILE_MAP_WRITE,
		0, 0, 0);
	if (!lp)return NULL;
	std::cout << "Open MapViewOfFile: " << lp << std::endl;

	ZeroMemory(lp, HTsize);
	HTHANDLE* ht = new(lp) HTHANDLE(
		Capacity,
		SecSnapshotInterval,
		MaxKeyLength,
		MaxPayloadLength,
		FileName.c_str());

	ht->File = hf;
	ht->FileMapping = hm;
	ht->Addr = lp;
	ht->lastsnaptime = time(NULL);
	ht->Mutex = CreateMutex(
		NULL,
		FALSE,
		GenerateMutexName(FileName.c_str()));//MAX_PATH LIMIT

	DWORD SnapShotThread = NULL;
	if (!(ht->SnapshotThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SnapShotCycle, ht, 0, &SnapShotThread))) return NULL;
	return ht;
}

HTHANDLE* HT::Open(const std::wstring FileName)
{
	HANDLE hf = CreateFile(FileName.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hf == INVALID_HANDLE_VALUE)
	{
		std::cout << _Post_equals_last_error_::GetLastError();
		return NULL;
	}

	HANDLE hm = CreateFileMapping(hf, NULL, PAGE_READWRITE, 0, 0, L"name");
	if (!hm)
		return NULL;

	std::cout << "Open FileMapping: " << hm << std::endl;

	LPVOID lp = MapViewOfFile(hm, FILE_MAP_ALL_ACCESS | FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	Addr = lp;
	if (!lp)
	{
		return NULL;
	}
	std::cout << "Open MapViewOfFile: " << lp << std::endl;

	HTHANDLE* ht = (HTHANDLE*)lp;
	ht->File = hf;
	ht->FileMapping = hm;
	ht->Addr = lp;
	ht->Mutex = CreateMutex(NULL, FALSE, L"mutex");

	DWORD SnapShotThread = NULL;
	if (!(ht->SnapshotThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SnapShotCycle, ht, 0, &SnapShotThread))) return NULL;
	return ht;
}

HTHANDLE* HT::OpenExist(const std::wstring FileName)
{
	HANDLE hm;
	HANDLE mutex = CreateMutex(NULL, FALSE, L"mutex");
	WaitForSingleObject(mutex, INFINITE);
	std::cout << _Post_equals_last_error_::GetLastError();

	hm = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"name");
	std::cout << _Post_equals_last_error_::GetLastError();
	if (!hm)
		return NULL;

	LPVOID lp = MapViewOfFile(hm, FILE_MAP_ALL_ACCESS | FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	Addr = lp;
	std::cout << _Post_equals_last_error_::GetLastError();
	if (!lp)
		return NULL;

	ReleaseMutex(mutex);
	HTHANDLE* ht = (HTHANDLE*)lp;
	ht->FileMapping = hm;
	ht->Addr = lp;
	ht->Mutex = mutex;
	return ht;
}

BOOL HT::Snap(HTHANDLE* hthandle) {
	std::cout << "-----SNAPSHOT WAIT-----" << std::endl;
	WaitForSingleObject(hthandle->Mutex, INFINITE);
	if (!FlushViewOfFile(hthandle->Addr, NULL)) {
		SetErrorMessage(hthandle, "Snapshot error", 15);
		ReleaseMutex(hthandle->Mutex);
		return FALSE;
	}
	std::cout << "-----SNAPSHOT-----" << std::endl;
	hthandle->lastsnaptime = time(NULL);
	ReleaseMutex(hthandle->Mutex);
	return TRUE;
}

BOOL HT::Close(const HTHANDLE* hthandle)
{
	HANDLE mapping, file, mutex;

	memcpy(&mapping, &hthandle->FileMapping, sizeof(HANDLE));
	memcpy(&file, &hthandle->File, sizeof(HANDLE));
	memcpy(&mutex, &hthandle->Mutex, sizeof(HANDLE));

	TerminateThread(hthandle->SnapshotThread, 0);

	if (!CloseHandle(mapping))return FALSE;
	if (!CloseHandle(file))return FALSE;
	if (!CloseHandle(mutex))return FALSE;
	return TRUE;
}

BOOL HT::CloseExist(const HTHANDLE* hthandle) {
	if (!UnmapViewOfFile(hthandle->Addr))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL HT::Insert(HTHANDLE* hthandle, Element* element)
{
	if (!CheckParmLength(hthandle, element))
		return FALSE;
	WaitForSingleObject(hthandle->Mutex, INFINITE);
	bool inserted = false;

	if (hthandle->N != hthandle->Capacity)
	{
		for (int i = 0, j = HashFunction(element, hthandle->Capacity, 0);
			i != hthandle->Capacity && !inserted;  j = Next_hash(j, hthandle->Capacity, ++i)) {
			Element* elFromHT = GetElementFromHT(hthandle, j);
			if (elFromHT != NULL && !IsDeleted(elFromHT)) {
				if (EqualElementKeys(elFromHT, element)) {
					SetErrorMessage(hthandle, "Key exists\n", 12);
					ReleaseMutex(hthandle->Mutex);
					return FALSE;
				}
			}
			SetElementToHT(hthandle, element, j);
			hthandle->N++;
			inserted = true;

		}
	}
	ReleaseMutex(hthandle->Mutex);
	return inserted;
}

Element* HT::Get(HTHANDLE* hthandle, Element* element)
{
	if (!CheckParmLength(hthandle, element))
		return NULL;
	WaitForSingleObject(hthandle->Mutex, INFINITE);

	int indexInHT = -1;
	bool found = false;
	if (hthandle->N != 0)
		for (int i = 0, j = HashFunction(element, hthandle->Capacity, 0);
			GetElementFromHT(hthandle, j) != NULL && i != hthandle->Capacity && !found;
			j = Next_hash(j, hthandle->Capacity, ++i)) {
		Element* elFromHT = GetElementFromHT(hthandle, j);
		if (!IsDeleted(elFromHT)) {
			if (EqualElementKeys(elFromHT, element)) {
				indexInHT = j; found = true;
			}
		}
	}
	if (indexInHT < 0) {
		SetErrorMessage(hthandle, "Not found key\n", 15);
		ReleaseMutex(hthandle->Mutex);
		return NULL;
	}
	ReleaseMutex(hthandle->Mutex);
	return GetElementFromHT(hthandle, indexInHT);
}

BOOL HT::Delete(HTHANDLE* hthandle, Element* element)
{
	if (!CheckParmLength(hthandle, element))
		return FALSE;
	WaitForSingleObject(hthandle->Mutex, INFINITE);
	int indexInHT = -1;
	bool deleted = false;
	if (hthandle->N != 0)
		for (int i = 0, j = HashFunction(element, hthandle->Capacity, 0);
			GetElementFromHT(hthandle, j) != NULL && i != hthandle->Capacity && !deleted;
			j = Next_hash(j, hthandle->Capacity, ++i)) {
		Element* elFromHT = GetElementFromHT(hthandle, j);
		if (!IsDeleted(elFromHT)) {
			if (EqualElementKeys(elFromHT, element)) {
				indexInHT = j; deleted = true;
			}
		}
	}
	if (indexInHT < 0) {
		SetErrorMessage(hthandle, "Not found key\n", 15);
		ReleaseMutex(hthandle->Mutex);
		return FALSE;
	}

	SetDeletedFlag(GetElementFromHT(hthandle, indexInHT));
	hthandle->N--;
	ReleaseMutex(hthandle->Mutex);
	return TRUE;
}

BOOL HT::Update(HTHANDLE* hthandle, Element* oldelement, void* newpayload, int newpayloadlength)
{
	if (!CheckParmLength(hthandle, oldelement) || !CheckParmLength(hthandle, newpayloadlength))
		return FALSE;
	WaitForSingleObject(hthandle->Mutex, INFINITE);
	int indexInHT = -1;
	bool updated = false;
	if (hthandle->N != 0)
		for (int i = 0, j = HashFunction(oldelement, hthandle->Capacity, 0);
			GetElementFromHT(hthandle, j) != NULL && i != hthandle->Capacity && !updated;
			j = Next_hash(j, hthandle->Capacity, ++i)) {
		Element* elFromHT = GetElementFromHT(hthandle, j);
		if (!IsDeleted(elFromHT)) {
			if (EqualElementKeys(elFromHT, oldelement)) {
				indexInHT = j; updated = true;
			}
		}
	}
	if (indexInHT < 0) {
		SetErrorMessage(hthandle, "Not found key\n", 15);
		ReleaseMutex(hthandle->Mutex);
		return FALSE;
	}

	UpdateElement(hthandle, GetElementFromHT(hthandle, indexInHT), newpayload, newpayloadlength);
	ReleaseMutex(hthandle->Mutex);
	return TRUE;
}

char* HT::GetLastErrorProg(HTHANDLE* ht) {
	return ht->LastErrorMessage;
}

void HT::print(const Element* element) {
	char* key = (char*)element->key;
	std::cout << std::right << std::setfill('=') << std::setw(30) << '|' << std::endl;
	std::cout << std::left << std::setfill(' ') << std::setw(10) << "KEY";
	for (int i = 0; i < element->keylength; i++) {
		std::cout << key[i];
	}
	std::cout << std::left << std::setfill(' ') << std::setw(10) << "\nVALUE";
	char* payload = (char*)element->payload;
	for (int i = 0; i < element->payloadlength; i++) {
		std::cout << payload[i];
	}
	std::cout << std::endl;
	std::cout << std::right << std::setfill('=') << std::setw(30) << '|' << std::endl;
}

Element* HT::CreateNewFullElement(const void* key, int keylength, const void* payload, int payloadlength)
{
	return new Element(key, keylength, payload, payloadlength);
}

Element* HT::CreateNewKeyElement(const void* key, int keylength)
{
	return new Element(key, keylength);
}

Element* HT::GetElementFromHT(HTHANDLE* ht, int n) {
	void* elementsAddr = ht + 1;
	int defaultElementSize = ht->MaxKeyLength + ht->MaxPayloadLength + 2 * sizeof(int);
	void* elementAddr = (char*)elementsAddr + defaultElementSize * n;

	Element* element = new Element();
	element->key = elementAddr;
	element->keylength = *(int*)((char*)elementAddr + ht->MaxKeyLength);
	element->payload = ((char*)elementAddr + ht->MaxKeyLength + sizeof(int));
	element->payloadlength = *(int*)((char*)elementAddr + ht->MaxKeyLength + sizeof(int) + ht->MaxPayloadLength);
	if (element->keylength == 0) {
		delete element;
		return NULL;
	}
	return element;
}

BOOL HT::SetElementToHT(HTHANDLE* ht, Element* element, int n) {
	void* elementsAddr = ht + 1;
	int defaultElementSize = ht->MaxKeyLength + ht->MaxPayloadLength + 2 * sizeof(int);
	void* elementAddr = (char*)elementsAddr + defaultElementSize * n;

	memcpy(elementAddr, element->key, element->keylength);
	memcpy(((char*)elementAddr + ht->MaxKeyLength), &element->keylength, sizeof(int));
	memcpy(((char*)elementAddr + ht->MaxKeyLength + sizeof(int)), element->payload, element->payloadlength);
	memcpy(((char*)elementAddr + ht->MaxKeyLength + sizeof(int) + ht->MaxPayloadLength), &element->payloadlength, sizeof(int));
	return TRUE;
}

BOOL HT::IsDeleted(Element* ptr) {
	if (*(int*)ptr->key == -1) {
		return TRUE;
	}
	return FALSE;
}

BOOL HT::EqualElementKeys(Element* el1, Element* el2) {
	return !memcmp(el1->key, el2->key, el2->keylength) ? TRUE : FALSE;
}

void HT::SetErrorMessage(HTHANDLE* ht, const char* message, int messageLen) {
	char* err = (char*)message;
	memcpy(ht->LastErrorMessage, err, messageLen);
}

void HT::SetDeletedFlag(Element* ptr) {
	memcpy(ptr->key, &DELETED, sizeof(DELETED));
}

void HT::UpdateElement(HTHANDLE* ht, Element* el, void* newpayload, int newpayloadlength) {
	ZeroMemory(el->payload, ht->MaxPayloadLength + sizeof(int));
	memcpy(el->payload, newpayload, newpayloadlength);
	memcpy((char*)el->payload + ht->MaxPayloadLength, &newpayloadlength, sizeof(int));
}

int HT::HashFunction(const Element* element, int size, int p)    //Хеш-функция
{
	char* arrKeyBytes = new char[element->keylength];
	memcpy(arrKeyBytes, element->key, element->keylength);
	int sumBytes = 0;
	for (int i = 0; i < element->keylength; i++) {
		sumBytes += arrKeyBytes[i];
	}
	delete[] arrKeyBytes;
	return (p + sumBytes) % size;
}
int HT::Next_hash(int hash, int size, int p)
{
	return (hash + 5 * p + 3 * p * p) % size;
}

DWORD WINAPI SnapShotCycle(HTHANDLE* ht)
{
	while (true)
	{
		if (ht) {
			if (time(NULL) >= ht->lastsnaptime + ht->SecSnapshotInterval)
			{
				WaitForSingleObject(ht->Mutex, INFINITE);
				if (!FlushViewOfFile(Addr, NULL)) {
					HT::SetErrorMessage(ht, "Snapshot error", 15);
					return FALSE;
				}
				ht->lastsnaptime = time(NULL);
				std::cout << "----SNAPSHOT in Thread----" << std::endl;

				ReleaseMutex(ht->Mutex);
			}
		}
		else
			break;
	}
	return TRUE;
}

wchar_t* HT::GenerateMutexName(const wchar_t* pathToHT) {
	std::wstring s(pathToHT);
	std::wstring mutexName;
	s.erase(std::remove(s.begin(), s.end(), '\\'), s.end());
	std::wcout << s;
	return (wchar_t*)s.c_str();
}
wchar_t* HT::GenerateViewName(const wchar_t* pathToHT) {
	std::wstring s(pathToHT);
	std::wstring mutexName;
	s.erase(std::remove(s.begin(), s.end(), '\\'), s.end());
	std::wcout << s;
	return (wchar_t*)s.c_str();
}

BOOL HT::CheckParmLength(HTHANDLE* ht, Element* element) {
	if (element->keylength > ht->MaxKeyLength) {
		SetErrorMessage(ht, "KeyLength > maxKeyLength\n", strlen("KeyLength > maxKeyLength\n") + 1);
		return FALSE;
	}
	return TRUE;
}

BOOL HT::CheckParmLength(HTHANDLE* ht, int payloadLength) {
	if (payloadLength > ht->MaxPayloadLength) {
		HT::SetErrorMessage(ht, "PayloadLength > maxPayloadLength\n", strlen("PayloadLength > maxPayloadLength\n") + 1);
		return FALSE;
	}
	return TRUE;
}
