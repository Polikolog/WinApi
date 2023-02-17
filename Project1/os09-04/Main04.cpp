
//#include "Project1/Main.h"
#include "Lab1.h"

int main(int argc, char* argv[])
{
	wchar_t fileName[] = L"T:\\C3S2\\OS\\Project1\\Ex1.txt";

	if (!Lab1::printWathRowFileTxt(fileName, 20000))
		return -1;

	system("pause");

}