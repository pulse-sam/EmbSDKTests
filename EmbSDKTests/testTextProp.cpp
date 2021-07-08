#include "pch.h"

using namespace PulseEmbSDK;

#ifdef TEST_TEXTPROP

struct _TextProp
{
	double _wc;
	TwoColorSewOrderTypes _tcsot;
	PullCompensationTypes _pullComp;
	double _pullCompPerc;
	unsigned short _pullCompMax;
	double _monoH;
	TextEffectTypes _effectType;
	Style* _pStyle;
};

void SetTextProperty(PulseEmbSDK::TextProperties* pProp, _TextProp& tp)
{
	TextProp_SetWidthCompress(pProp, tp._wc);
	TextProp_SetTwoClrOrder(pProp, tp._tcsot);
	TextProp_SetPullComp(pProp, tp._pullComp);
	TextProp_SetPullCompPercent(pProp, tp._pullCompPerc);
	TextProp_SetPullCompMax(pProp, tp._pullCompMax);
	TextProp_SetMonoHeight(pProp, tp._monoH);
	TextProp_SetTextEffect(pProp, tp._effectType);
	TextProp_SetStyle(pProp, tp._pStyle);
}

bool CheckTextProperty(PulseEmbSDK::TextProperties* pProp, _TextProp& tp)
{
	if (TextProp_GetStyle(pProp) != tp._pStyle
		|| TextProp_GetWidthCompress(pProp) != tp._wc
		|| TextProp_GetTwoClrOrder(pProp) != tp._tcsot
		|| TextProp_GetPullComp(pProp) != tp._pullComp
		|| TextProp_GetPullCompPercent(pProp) != tp._pullCompPerc
		|| TextProp_GetPullCompMax(pProp) != tp._pullCompMax
		|| TextProp_GetMonoHeight(pProp) != tp._monoH
		|| TextProp_GetTextEffect(pProp) != tp._effectType)
		return false;

	return true;
}

// test.cpp
extern std::string _testCase;
int funcErrHandler(int type, const char* mes, uint len);
bool SaveAsDesign(EmbDesign* pDes, PulseEmbSDK::FileTypes ft);

TEST(TestTextProperty, GetSet)
{
	_TextProp stp;
	stp._wc = 90;
	stp._tcsot = TwoColorSewOrderTypes::tcsotByColor;
	stp._effectType = TextEffectTypes::tetNone;
	stp._monoH = 80;
	stp._pullComp = PullCompensationTypes::pctPercentage;
	stp._pullCompPerc = 110;
	stp._pStyle = Style_Create();
	char s[] = "CANVAS";
	Style_Load(stp._pStyle, s, (uint)strlen(s));
	
	TextProperties* tp = TextProp_Create();
	ASSERT_TRUE(tp);

	SetTextProperty(tp, stp);
	EXPECT_TRUE(CheckTextProperty(tp, stp));

	_testCase = "Create a new design";
	// Create design with desired properties
	char recipe[] = "Normal";
	EmbDesign* pDes = EmbDesign_Create(recipe, (uint)strlen(recipe), "Tajima", 6, funcErrHandler);
	EXPECT_TRUE(pDes);

	char txt[] = "Test";
	char fnt[] = "Avante New";
	char elName[] = "1stCreated";
	_testCase = "Add a text segment";
	EmbDesign_AddLineTextAsElement(
		pDes, txt, (uint)strlen(txt), fnt, (uint)strlen(fnt),
		60, 0, 0, 100, 0,
		PulseEmbSDK::JustifyTypes::jtCenter, 
		PulseEmbSDK::EnvelopeTypes::etRectangle,
		2, tp,
		elName, (uint)strlen(elName));

	_testCase = "Save the design";
	bool ret = SaveAsDesign(pDes, PulseEmbSDK::FileTypes::ftAuto);
	// Open the temp design to check the properties

	Style_Delete(stp._pStyle);
	TextProp_Delete(tp);
	EmbDesign_Delete(pDes);
	ASSERT_TRUE(ret);
}


#endif //