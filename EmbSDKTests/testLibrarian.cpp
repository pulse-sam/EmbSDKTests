#include "pch.h"

using namespace PulseEmbSDK;

#ifdef TEST_LIBRARIAN

extern std::string _testCase;
extern int funcErrHandler(int type, const char* mes, uint len);
bool SaveAsDesign(EmbDesign* pDes, PulseEmbSDK::FileTypes ft);

class TestLibrarian : public ::testing::Test {
public:
	TestLibrarian() : _pLib(NULL) {}
	~TestLibrarian() {}

	Librarian* _pLib;
	bool _bConnect;
protected:
	void SetUp() override {
		_pLib = Librarian_Create();
		_bConnect = Librarian_Connect(_pLib, "librarian.pulseoffice.pulsemicro.com", 9000, "Administrator", "dg50TFGN5", Librarian::lctPdlClient);
	}

	void TearDown() override {
		Librarian_Disconnect(_pLib);
		Librarian_Delete(_pLib);
	}
};

TEST_F(TestLibrarian, OpenDesign)
{
	ASSERT_TRUE(_bConnect);
	ASSERT_TRUE(Librarian_OpenDB(_pLib, "DAKOTA"));

	EmbDesign* pDes = Librarian_OpenDesign(_pLib, "CF0225");
	ASSERT_TRUE(pDes);

	_testCase = "Save Librarian design as a local design";
	auto ret = SaveAsDesign(pDes, ftAuto);
	EmbDesign_Delete(pDes);
	ASSERT_TRUE(ret);

	std::cout << "TestLibrarian: ALL PASSED\n";
}

#endif // TEST_LIBRARIAN