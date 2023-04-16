
#include "../os10-HTAPI/HT.h"
#include "Header.h"

namespace TEST_API {

	void createTest(BOOL* ft) {
		HT::HTHANDLE* ht = nullptr;
		ht = HT::Create(1000, 3, 10, 256, L"..\\Ex.ht");
		Sleep(1000);
		if (ht)
		{
			HT::Close(ht);
			*ft = TRUE;
		}
		else
		{
			HT::Close(ht);
			*ft = FALSE;
		}
	}

	void openTest(BOOL* ft) {

		HT::HTHANDLE* ht = nullptr;

		ht = HT::Open(L"..\\Ex.ht");

		if (ht)
		{
			HT::Insert(ht, new HT::Element("data", 4, "ndata", 5));
			HT::Close(ht);
			*ft = TRUE;
		}
		else
		{
			HT::Close(ht);
			*ft = FALSE;
		}
	}

	void insertTest(BOOL *ft) {
		HT::HTHANDLE* ht = nullptr;
		ht = HT::Create(1000, 3, 10, 2, L"T:\\C3S2\\OS\\FileMapping\\test_case_htapi\\HT.ht");

		HT::Insert(ht, new HT::Element("data", 4, "ndata", 5));
		HT::Element* hte = HT::Get(ht, new HT::Element("data", 4));

		if (hte)
		{
			HT::Close(ht);
			*ft = TRUE;
		}
		else
		{
			std::cout << HT::GetLasrError(ht);
			HT::Close(ht);
			*ft = FALSE;
		}
	}

	void insertManyTest(BOOL *ft) {
		HT::HTHANDLE* ht = nullptr;
		ht = HT::Create(1000, 3, 2, 256, L"T:\\C3S2\\OS\\FileMapping\\test_case_htapi\\HT.ht");

		HT::Insert(ht, new HT::Element("data", 4, "ndata", 5));
		HT::Insert(ht, new HT::Element("data2", 5, "data2", 5));

		HT::Element* hte1 = HT::Get(ht, new HT::Element("data", 4));
		HT::Element* hte2 = HT::Get(ht, new HT::Element("data2", 5));

		if (hte1 && hte2)
		{
			HT::Close(ht);
			*ft = TRUE;
		}
		else
		{
			std::cout << HT::GetLasrError(ht) << std::endl;

			HT::Close(ht);
			*ft = FALSE;
		}
	}

	void deleteTest(BOOL *ft) {
		HT::HTHANDLE* ht = nullptr;
		ht = HT::Create(1000, 3, 10, 256, L"T:\\C3S2\\OS\\FileMapping\\test_case_htapi\\HT.ht");

		HT::Insert(ht, new HT::Element("data", 4, "ndata", 5));
		HT::Element* hte = HT::Get(ht, new HT::Element("data", 4));
		HT::Delete(ht, hte);
		hte = HT::Get(ht, new HT::Element("data", 4));

		if (hte)
		{
			HT::Close(ht);
			*ft = TRUE;
		}
		else
		{
			std::cout << HT::GetLasrError(ht) << std::endl;

			HT::Close(ht);
			*ft = FALSE;
		}
	}

	void updateTest(BOOL *ft) {
		HT::HTHANDLE* ht = nullptr;
		ht = HT::Create(1000, 3, 10, 256, L"T:\\C3S2\\OS\\FileMapping\\test_case_htapi\\HT.ht");

		HT::Insert(ht, new HT::Element("data", 4, "ndata", 5));
		HT::Element* hte = HT::Get(ht, new HT::Element("data", 4));
		HT::Update(ht, hte, "data2", 5);
		hte = HT::Get(ht, new HT::Element("data", 4));

		if (memcmp(hte->_payload, "data2", hte->_payloadLength) == NULL)
		{
			HT::Close(ht);
			*ft = TRUE;
		}
		else
		{
			std::cout << HT::GetLasrError(ht) << std::endl;

			HT::Close(ht);
			*ft = FALSE;
		}
	};
}