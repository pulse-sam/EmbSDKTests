#include "pch.h"

using namespace PulseEmbSDK;

#ifdef TEST_DESIGN

extern std::string _testCase;
extern int funcErrHandler(int type, const char* mes, uint len);
extern bool SaveAsDesign(EmbDesign* pDes, PulseEmbSDK::FileTypes ft);

class NewEmbDesign : public ::testing::Test {
public:
	NewEmbDesign() : _pDes(NULL) {}
	~NewEmbDesign() {}

	EmbDesign* _pDes;
	TextProperties* _propText;
protected:
	void SetUp() override {

		_testCase = "Creating a new design";
		_pDes = EmbDesign_Create("Normal", 6, "Tajima", 6, funcErrHandler);

		_propText = TextProp_Create();
		// TODO: change text properties?
	}

	void TearDown() override {
		EmbDesign_Delete(_pDes);
		TextProp_Delete(_propText);
	}
};

TEST_F(NewEmbDesign, AddElements)
{
	char text1[] = "fit-center";
	char font1[] = "Bean Bag";
	int ret = EmbDesign_AddLineTextAsElement(
		_pDes, text1, (uint)strlen(text1), font1, (uint)strlen(font1), 100,
		0, 0, 0, 0, jtCenter, etRectangle, 0, _propText, "element1", 8);

	ASSERT_TRUE(ret == 0);

	char text2[] = "fit-curve";
	char font2[] = "Bridal Path";
	ret = EmbDesign_AddLineText(
		_pDes, text2, (uint)strlen(text2), font2, (uint)strlen(font2), 150,
		0, 100, 50, 100, jtFitToCurve, etRectangle, 0, _propText);
	ASSERT_TRUE(ret == 0);

	char text3[] = "arc";
	char font3[] = "Morocco";
	ret = EmbDesign_AddArcText(
		_pDes, text3, (uint)strlen(text3), font3, (uint)strlen(font3), 200,
		0, 250, 380, 178, 760, 250, jtCenter, 1, _propText);

	ASSERT_TRUE(ret == 0);

	char text4[] = "3letter";
	char font4[] = "Billboard";
	char deco[] = "MPM-BDE002";
	ret = EmbDesign_AddThreeLetterMonogram(
		_pDes, text4, (uint)strlen(text4),
		deco, (uint)strlen(deco),
		font4, (uint)strlen(font4),
		100,
		0, 500, 0, 500, 0, _propText);

	ASSERT_TRUE(ret == 0);

	ASSERT_TRUE(SaveAsDesign(_pDes, ftAuto));

	std::cout << "NewEmbDesign: ALL PASSED\n";
}


#endif