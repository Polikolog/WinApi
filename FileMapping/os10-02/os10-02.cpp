#include <iostream>
#include "../os10-HTAPI/HT.h"

using namespace HT;

int main()
{
	const wchar_t* FileName = L"T:\\C3S2\\OS\\FileMapping\\Ex.ht";

	std::string key1 = "key1";
	std::string data1 = "data1";
	std::string dataX = "XXXXX";
	std::string key2 = "key2";
	std::string data2 = "data2";

	HTHANDLE* hashTable = Create(20, 5, 20, 20, FileName);
	//HTHANDLE* hashTable = Open(FileName);
	if (!hashTable) {
		return 1;
	}
	Element* element1 = new Element(key1.c_str(), key1.length(), data1.c_str(), data1.length());
	Element* element2 = new Element(key2.c_str(), key2.length(), data2.c_str(), data2.length());
	if (!Insert(hashTable, element1)) {
		std::cout << hashTable->LastErrorMessage;
	}
	if (!Insert(hashTable, element2)) {
		std::cout << hashTable->LastErrorMessage;
	}
	Element* finded1 = Get(hashTable, element1);
	if (finded1 != NULL)
	{
		std::cout << "Finded1: " << (char*)finded1->_key << " " << (char*)finded1->_payload << std::endl;
	}



	Element* finded2 = Get(hashTable, element2);
	if (finded2 != NULL)
	{
		std::cout << "Finded2: " << (char*)finded2->_key << " " << (char*)finded2->_payload << std::endl;
	}
	Delete(hashTable, finded2);
	if (Get(hashTable, finded2) == NULL)
	{
		std::cout << "Deleted successfuly!" << std::endl;
	}
	Update(hashTable, finded1, dataX.c_str(), dataX.length());
	Element* updated1 = Get(hashTable, element2);
	print(updated1);
	if(Snap(hashTable))
		std::cout << "Snap success";
	if(Close(hashTable))
		std::cout << "Close success";
	Sleep(15000);
}