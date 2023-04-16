#include <iostream>
#include <thread>
#include <Windows.h>
#include "Header.h"

using namespace TEST_API;

int main()
{
	BOOL ft1{ FALSE }, ft2{ FALSE }, ft3{ FALSE }, ft4{ FALSE }, ft5{ FALSE }, ft6{ FALSE };
	std::cout << " Create test " << std::endl;
	std::thread th1(createTest, &ft1);

	if (ft1)
		std::cout << " Create: success " << std::endl;
	else
		std::cout << " Create: failed " << std::endl;

	std::cout << " Open test " << std::endl;
	if (ft2)
		std::cout << " Open: success " << std::endl;
	else
		std::cout << " Open: failed " << std::endl;

	std::cout << " Insert test " << std::endl;
	insertTest(&ft3);
	if (ft3)
		std::cout << " Insert: success " << std::endl;
	else
		std::cout << " Insert: failed " << std::endl;

	std::cout << " Insert many test " << std::endl;
	insertManyTest(&ft4);
	if (ft4)
		std::cout << " Insert many: success " << std::endl;
	else
		std::cout << " Insert many: failed " << std::endl;

	std::cout << " Delete test " << std::endl;
	deleteTest(&ft5);
	if (ft5)
		std::cout << " Delete: success " << std::endl;
	else
		std::cout << " Delete: failed " << std::endl;

	std::cout << " Update test " << std::endl;
	updateTest(&ft6);
	if (ft6)
		std::cout << " Update: success " << std::endl;
	else
		std::cout << " Update: failed " << std::endl;

	th1.detach();
}
