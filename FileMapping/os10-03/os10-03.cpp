// OS10_03.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "../os10-HTAPI/HT.h"

using namespace HT;


void main()
{
    LPWSTR FileName1 = (LPWSTR)L"..\\Ex.ht";
    LPWSTR FileName2 = (LPWSTR)L"..\\Ex2.ht";


    std::string key1 = "key1";
    std::string data1 = "data1";
    std::string key2 = "key2";
    std::string data2 = "data2";

    Element* element1 = new Element(key1.c_str(), key1.length(), data1.c_str(), data1.length());
    Element* element2 = new Element(key2.c_str(), key2.length(), data2.c_str(), data2.length());

    print(element1);
	print(element2);

    HTHANDLE* hashTable1 = Create(20, 5, 20, 20, FileName1);
    HTHANDLE* hashTable2 = Create(20, 5, 20, 20, FileName2);

    if (!Insert(hashTable1, element1)) {
        std::cout << hashTable1->LastErrorMessage;
        return;
    }
    if (!Insert(hashTable1, element2)) {
        std::cout << hashTable1->LastErrorMessage;
        return;
    }
    if(!Insert(hashTable2, element2))
    {
        std::cout << hashTable2->LastErrorMessage;
        return;
    }
    if(!Insert(hashTable2, element1))
    {
        std::cout << hashTable2->LastErrorMessage;
        return;
    }

    if (Get(hashTable1, element1) != NULL)
    {
        std::cout << "Element 1 in hashTable1" << std::endl;
    }
    if (Get(hashTable1, element2) != NULL)
    {
        std::cout << "Element 2 in hashTable1" << std::endl;
    }

    if (Get(hashTable2, element1) != NULL)
    {
        std::cout << "Element 1 in hashTable2" << std::endl;
    }
    if (Get(hashTable2, element2) != NULL)
    {
        std::cout << "Element 2 in hashTable2" << std::endl;
    }

    if (!Snap(hashTable1))
        std::cout << hashTable1->LastErrorMessage << std::endl;
    if (!Snap(hashTable2))
        std::cout << hashTable2->LastErrorMessage << std::endl;

    if (Close(hashTable1))
        std::cout << "Close 1 success" << std::endl;
    if (Close(hashTable2))
        std::cout << "Close 2 success";
}