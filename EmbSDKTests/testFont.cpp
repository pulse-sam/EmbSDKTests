#include "pch.h"

using namespace PulseEmbSDK;

#ifdef TEST_FONT

TEST(TestFont, GetSet)
{
	EmbFont_InitFonts();

	EmbFont* pFont = EmbFont_GetFont("Avante New");
	ASSERT_TRUE(pFont);

	ASSERT_FALSE(EmbFont_IsMultiColor(pFont));
	ASSERT_FALSE(EmbFont_IsMonogram(pFont));

	int cnt = EmbFont_GetCharNum(pFont);
	ASSERT_TRUE(cnt > 0);

	wchar_t c = EmbFont_GetChar(pFont, 12);
	ASSERT_TRUE(c == 0x0010d);

	std::cout << "TestFont: ALL PASSED\n";
}

#endif //TEST_FONT