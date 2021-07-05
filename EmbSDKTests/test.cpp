#include "pch.h"

using namespace PulseEmbSDK;

class MyEnvironment : public ::testing::Environment {
public:
	MyEnvironment() : bInit(false){ }

	// Depending on the value of failure_in_set_up_, SetUp() will
	// generate a non-fatal failure, generate a fatal failure, or
	// succeed.
	virtual void SetUp() override {
		char* dgDataPath = "C:\\ProgramData\\Tajima\\DGML By Pulse\\16";
		bInit = EMBSDK_Init(dgDataPath, (int)strlen(dgDataPath), NULL, 0);
	}

	virtual void TearDown() override {
		
	}

	bool bInit;
};

int main(int argc, char **argv) 
{
	testing::InitGoogleTest(&argc, argv);
	MyEnvironment *env = new MyEnvironment;
	::testing::AddGlobalTestEnvironment(env);
	return RUN_ALL_TESTS();
}

std::string _testCase;
int funcErrHandler(int type, const char* mes, uint len)
{
	std::cout << "[" << _testCase << "] ERROR-" << type << ": " << mes << std::endl;
	EXPECT_TRUE(false); // Fail the case (ASSERT_TRUE returns void)
	return 0;
}

EmbDesign* OpenDesign()
{
	char* designFile = "d:\\tmp\\Design1.pxf";
	char* format = "tajima";
	EmbDesign* pDes = EmbDesign_Open(
		designFile, (uint)strlen(designFile), PulseEmbSDK::FileTypes::ftAuto, PulseEmbSDK::OpenTypes::otStitches, format, (uint)strlen(format),
		funcErrHandler
	);
	return pDes;
}

class TestEmbDesign : public ::testing::Test {
public:
	TestEmbDesign() : _pDes(NULL){}
	~TestEmbDesign(){}

	EmbDesign* _pDes;

protected:
	void SetUp() override {
		_pDes = OpenDesign();
	}

	void TearDown() override {
		EmbDesign_Delete(_pDes);
	}
};

bool SaveAsDesign(EmbDesign* pDes, PulseEmbSDK::FileTypes ft)
{
	char pcfFile[L_tmpnam] = { 0 };
	if (0 != tmpnam_s(pcfFile, L_tmpnam))
		return false;

	// Add extension
	switch (ft)
	{
	case PulseEmbSDK::ftPXF:
	case PulseEmbSDK::ftAuto:
		sprintf_s(pcfFile, L_tmpnam, "%s.PXF", pcfFile);
		break;
	case PulseEmbSDK::ftPCF:
		sprintf_s(pcfFile, L_tmpnam, "%s.PCF", pcfFile);
		break;
	case PulseEmbSDK::ftDST:
		sprintf_s(pcfFile, L_tmpnam, "%s.DST", pcfFile);
		break;
	default:
		// Do not support
		return false;
	}

	EmbDesign_Save(pDes, pcfFile, L_tmpnam, ft, funcErrHandler);

	char* format = "tajima";
	EmbDesign* pNewDes = EmbDesign_Open(
		pcfFile, (uint)L_tmpnam, PulseEmbSDK::FileTypes::ftAuto, PulseEmbSDK::OpenTypes::otDefault, format, (uint)strlen(format),
		funcErrHandler
	);
	if (pNewDes == NULL)
		return false;
	EmbDesign_Delete(pNewDes);

	return true;
}

bool CheckFile(const char* fileName)
{
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesA(fileName) && GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		return false;
	}
	return true;
}

#ifdef TEST_DESIGN
TEST_F(TestEmbDesign, TestDesign) {
	ASSERT_TRUE(_pDes);


	int w = (int)EmbDesign_GetWidth(_pDes);
	int h = (int)EmbDesign_GetHeight(_pDes);
	ASSERT_GT(w, 0);
	ASSERT_GT(h, 0);

	// =========== Save as design
	_testCase = "Save design as PCF";
	ASSERT_TRUE(SaveAsDesign(_pDes, ftPCF));
	_testCase = "Save design as Auto";
	ASSERT_TRUE(SaveAsDesign(_pDes, ftAuto));
	_testCase = "Save design as DST";
	ASSERT_TRUE(SaveAsDesign(_pDes, ftDST));

	// =========== Save as image
	_testCase = "Save design as a image";
	BitmapImage* pBmp = BitmapImage_New(200, 200);
	ASSERT_TRUE(pBmp);
	BitmapImage_Fill(pBmp, 0, 0, 0, 0);
	RenderOptions* pOpt = RenderOptions_New();
	RenderOptions_setArtworkMinMax(pOpt, ammfOutlines);
	EmbDesign_Render2(_pDes, pBmp, 0, 0, 200, 200, pOpt, funcErrHandler);

	char pngFile[L_tmpnam] = { 0 };
	EXPECT_TRUE(0 == tmpnam_s(pngFile, L_tmpnam));
	sprintf_s(pngFile, L_tmpnam, "%s.PNG", pngFile);
	_testCase = "Save image as PNG";
	// NOTICE: Saving png will no work when using debug version
	BitmapImage_Save(pBmp, pngFile, L_tmpnam, itPNG, funcErrHandler);
	EXPECT_TRUE(CheckFile(pngFile));

	RenderOptions_Delete(pOpt);
	BitmapImage_Destroy(pBmp);

	// Test RenderOutline
	_testCase = "Test RenderOutline";
	pBmp = BitmapImage_New(200, 200);
	ASSERT_TRUE(pBmp);
	BitmapImage_Fill(pBmp, 0, 0, 0, 0);

	EmbDesign_RenderOutlines(_pDes, pBmp, 0, 0, 200, 200);
	char* bmpFile = "Outlines.bmp";
	BitmapImage_Save(pBmp, bmpFile, 12, itBMP, funcErrHandler);
	ASSERT_TRUE(CheckFile(bmpFile));

	// Test Export, Export2
	char* pltFile = "export.plt";
	char* pdfFile = "export.pdf";
	EmbDesign_Export(_pDes, pltFile, ExportFileTypes::eftAuto);
	ASSERT_TRUE(CheckFile(pltFile));
	EmbDesign_Export(_pDes, pdfFile, ExportFileTypes::eftAuto);
	ASSERT_TRUE(CheckFile(pltFile));

	char* pltFile2 = "export_header.hpgl";
	EmbDesign_Export2(_pDes, pltFile2, ExportFileTypes::eftHPGL, "Sample Header 1.0", false);
	ASSERT_TRUE(CheckFile(pltFile));

	std::cout << "EmbDesign: ALL PASSED\n";
}
#endif // TEST_DESIGN

#ifdef TEST_NEEDLESEQ
TEST_F(TestEmbDesign, NeedleSequence) {
	ASSERT_TRUE(_pDes);

	unsigned int oldNeedleSeq[128] = { 0 };
	unsigned int *needleSeq = NULL;
	auto cnt = EmbDesign_GetNeedleSequence(_pDes, &needleSeq);
	ASSERT_TRUE(cnt > 0);
	ASSERT_TRUE(needleSeq != NULL);

	std::cout << "Needles: \n";
	for (uint i = 0; i < cnt; i++)
	{
		std::cout << needleSeq[i] << " ";
	}
	std::cout << std::endl;
	
	// Reverse the needle sequence
	unsigned int newNeedleSeq[128] = { 0 };
	for (uint i = 0; i < cnt; i++)
	{
		newNeedleSeq[i] = needleSeq[cnt - i - 1];
	}

	// Release 
	memcpy(oldNeedleSeq, needleSeq, cnt * sizeof(unsigned int));
	delete[] needleSeq;
	needleSeq = NULL;

	ASSERT_TRUE(EmbDesign_SetNeedleSequence(_pDes, newNeedleSeq, cnt));

	SaveAsDesign(_pDes, ftPXF);

	auto newCnt = EmbDesign_GetNeedleSequence(_pDes, &needleSeq);
	// Compare
	ASSERT_TRUE(cnt == newCnt);
	ASSERT_TRUE(needleSeq != NULL);

	// Release again
	memcpy(oldNeedleSeq, needleSeq, cnt * sizeof(unsigned int));
	delete[] needleSeq;

	for (uint i = 0; i < cnt; i++)
	{
		ASSERT_TRUE(newNeedleSeq[i] == oldNeedleSeq[i]);
	}
	
	std::cout << "EmbDesign: NeedleSequence PASSED!\n";
}

#endif // TEST_NEEDLESEQ
