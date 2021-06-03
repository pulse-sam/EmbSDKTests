#include "pch.h"
#include <EmbSDKC.h>
#include <functional>

using namespace PulseEmbSDK;

class EmbApp : public ::testing::Test {
public:
	EmbApp() : appInstance(NULL){}
	~EmbApp(){}

protected:
	void SetUp() override {
		char* dgDataPath = "C:\\ProgramData\\Tajima\\DGML By Pulse\\16";
		appInstance = EMBSDK_Create(dgDataPath, (int)strlen(dgDataPath), NULL, 0);
	}

	void TearDown() override {
		EMBSDK_Delete(appInstance);
	}

private:
	EmbSDKCApp* appInstance;
};

static EmbDesign* OpenDesign()
{
	char* designFile = "d:\\tmp\\Design1.pxf";
	char* format = "tajima";
	EmbDesign* pDes = EmbDesign_Open(
		designFile, (uint)strlen(designFile), PulseEmbSDK::FileTypes::ftAuto, PulseEmbSDK::OpenTypes::otDefault, format, (uint)strlen(format),
		[](int type, const char* mes, uint len)->int {
			std::cout << "[Design] ERROR-" << type << ": " << mes << std::endl;
			EXPECT_TRUE(false); // Fail the case
			return 0;
		}
	);
	return pDes;
}

TEST_F(EmbApp, TestDesign) {
	auto pDes = OpenDesign();
	ASSERT_TRUE(pDes != NULL);

	int w = (int)EmbDesign_GetWidth(pDes);
	int h = (int)EmbDesign_GetHeight(pDes);
	ASSERT_GT(w, 0);
	ASSERT_GT(h, 0);

	EmbDesign_Delete(pDes);
}