#pragma once

#include "Lab1.h"

namespace exs
{
	void ex04()
	{
		wchar_t fileName[] = L"T:\\C3S2\\OS\\Project1\\Ex1.txt";

		if (!Lab1::printWathRowFileTxt(fileName, 20000))
			return;

		std::cout << "Succes!";
	}

	void ex0103()
	{
		setlocale(LC_ALL, "Rus");
		//("Write: %s\nT:\\C3S2\\OS\\Project1\\Ex1.txt: %s\n", argv[0], argv[1]);
		//Solution::printFileInfo(argv[1]);
		wchar_t fileName[] = L"T:\\C3S2\\OS\\Project1\\Ex1.txt";
		wchar_t str[] = L"Im Ivan Shayuk";
		if (!Lab1::printFileInfo(fileName))
			return;

		if (!Lab1::printFileTxt(fileName))
			return;

		if (!Lab1::delRowFileTxt(fileName, 1))
			return;

		if (!Lab1::printFileTxt(fileName))
			return;

		if (!Lab1::insRowFileTxt(fileName, str, 1))
			return;

		if (!Lab1::printFileTxt(fileName))
			return;

		std::cout << "Succes!";
	}
}
