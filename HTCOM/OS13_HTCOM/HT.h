#pragma once
#include "IHT.h"
#include <initguid.h>

extern long g_lObjs;
extern long g_lLocks;

extern LPCWSTR  FriendlyName;
extern LPCWSTR  VerIndProg;
extern LPCWSTR  ProgID;

extern const GUID IID_IHT;
extern const GUID CLSID_HT;

class HT : public IHT
{
public:
	HT();
	~HT();

	STDMETHOD(QueryInterface(const IID& riid, void** ppvObject));
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());

	STDMETHOD_(HTHANDLE*, Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const std::wstring FileName));
	STDMETHOD_(HTHANDLE*, Open(const std::wstring FileName));
	STDMETHOD_(HTHANDLE*, OpenExist(const std::wstring FileName));
	STDMETHOD_(BOOL, CloseExist(const HTHANDLE* hthandle));
	STDMETHOD_(BOOL, Snap(HTHANDLE* hthandle));
	STDMETHOD_(BOOL, Insert(HTHANDLE* hthandle, Element* element));
	STDMETHOD_(BOOL, Update(HTHANDLE* hthandle, Element* oldelement, void* newpayload, int newpayloadlenth));
	STDMETHOD_(BOOL, Delete(HTHANDLE* hthandle, Element* element));
	STDMETHOD_(BOOL, Close(const HTHANDLE* htHandle));
	STDMETHOD_(Element*, Get(HTHANDLE* hthandle, Element* element));
	STDMETHOD_(char*, GetLastErrorProg(HTHANDLE* ht));
	STDMETHOD_(void, print(const Element* element));

	static void SetErrorMessage(HTHANDLE* ht, const char* message, int messageLen);

private:
	Element* CreateNewFullElement(const void* key, int keylength, const void* payload, int payloadlength);
	Element* CreateNewKeyElement(const void* key, int keylength);
	int HashFunction(const Element* element, int size, int p);
	int Next_hash(int hash, int size, int p);
	wchar_t* GenerateMutexName(const wchar_t* pathToHT);
	wchar_t* GenerateViewName(const wchar_t* pathToHT);
	BOOL CheckParmLength(HTHANDLE* ht, Element* element);
	BOOL CheckParmLength(HTHANDLE* ht, int payloadLength);

	const int DELETED = -1;
	Element* GetElementFromHT(HTHANDLE* ht, int n);
	BOOL SetElementToHT(HTHANDLE* ht, Element* element, int n);
	BOOL IsDeleted(Element* ptr);
	void SetDeletedFlag(Element* ptr);
	BOOL EqualElementKeys(Element* el1, Element* el2);
	void UpdateElement(HTHANDLE* ht, Element* el, void* newpayload, int newpayloadlength);

	LONG m_refCount;
};