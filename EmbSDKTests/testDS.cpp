#include "pch.h"

using namespace PulseEmbSDK;

EmbDesign* OpenDesign();

#ifdef TEST_DESIGNSPOOLER

class TestDS : public ::testing::Test {
public:
	TestDS() : _pDS(NULL) {}
	~TestDS() {}

	DesignSpooler* _pDS;
	int _nErrCode;
protected:
	void SetUp() override {
		_pDS = DesignSpooler_Create("localhost", 9050, _nErrCode);
	}

	void TearDown() override {
		DesignSpooler_Delete(_pDS);
	}
};


TEST_F(TestDS, DesignQueue)
{
	ASSERT_TRUE(_pDS);

	const char* sMachineName = "TEST--0001--0002";

	int nMachineCnt = 0;
	Machine** pMachines = DesignSpooler_GetMachines(_pDS, nMachineCnt, _nErrCode);
	// Should be only 1 simulator
	ASSERT_TRUE(nMachineCnt >= 1);
	ASSERT_TRUE(pMachines);

	Machine* pMachine = DesignSpooler_GetMachine(_pDS, sMachineName, _nErrCode);
	ASSERT_TRUE(pMachine);
	if(nMachineCnt == 1)
		ASSERT_TRUE(pMachine == pMachines[0]);

	_nErrCode = DesignSpooler_EmptyQueue(pMachine);

	EmbDesign* pDes = OpenDesign();
	ASSERT_TRUE(pDes);
	const char* strDesignName = "SampleEnqueue";
	_nErrCode = DesignSpooler_PublishDesign(pMachine, (void*)pDes, strDesignName, 10);
	EmbDesign_Delete(pDes);

	int nCnt = 0;
	QueuedDesign** pDesigns = DesignSpooler_GetQueuedDesigns(pMachine, nCnt);
	ASSERT_TRUE(nCnt == 1);
	ASSERT_TRUE(pDesigns);

	pDes = (EmbDesign*)DesignSpooler_OpenDesign(pMachine, strDesignName, _nErrCode);
	ASSERT_TRUE(pDes);
	EmbDesign_Delete(pDes);

	_nErrCode = DesignSpooler_DeleteDesign(pDesigns[0]);
	ASSERT_TRUE(_nErrCode == 0);

	nCnt = 0;
	pDesigns = DesignSpooler_GetQueuedDesigns(pMachine, nCnt);
	ASSERT_TRUE(nCnt == 0);
	ASSERT_TRUE(pDesigns == NULL);
}

#endif //TEST_DESIGNSPOOLER