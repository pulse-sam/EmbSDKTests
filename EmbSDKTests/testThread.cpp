#include "pch.h"

using namespace PulseEmbSDK;

#ifdef TEST_THREADPROP

const int MAX_LEN = 100;
struct _ThreadProperty
{
	char manu[MAX_LEN];
	char name[MAX_LEN];
	char code[MAX_LEN];
	PulseEmbSDK::ThreadTypes type;
	double thick;
	byte r;
	byte g;
	byte b;
} ;

void SetThreadProperty(PulseEmbSDK::ThreadProp* pProp, _ThreadProperty& tp)
{
	ThreadProp_SetManufacturer(pProp, tp.manu, MAX_LEN);
	ThreadProp_SetCode(pProp, tp.code, MAX_LEN);
	ThreadProp_SetName(pProp, tp.name, MAX_LEN);
	ThreadProp_SetThickness(pProp, tp.thick);
	ThreadProp_SetType(pProp, tp.type);
	ThreadProp_SetRed(pProp, tp.r);
	ThreadProp_SetGreen(pProp, tp.g);
	ThreadProp_SetBlue(pProp, tp.b);
}

bool CheckThreadProperty(PulseEmbSDK::ThreadProp* pProp, _ThreadProperty& tp)
{
	char str[MAX_LEN] = { 0 };
	int realSize = MAX_LEN;
	ThreadProp_GetManufacturer(pProp, str, MAX_LEN, &realSize);
	if(realSize <= 0 || 0 != strcmp(str, tp.manu))
		return false;

	ThreadProp_GetCode(pProp, str, MAX_LEN, &realSize);
	if (realSize <= 0 || 0 != strcmp(str, tp.code))
		return false;

	ThreadProp_GetName(pProp, str, MAX_LEN, &realSize);
	if (realSize <= 0 || 0 != strcmp(str, tp.name))
		return false;

	if (tp.thick != ThreadProp_GetThickness(pProp)
		|| tp.type != ThreadProp_GetType(pProp)
		|| tp.r != ThreadProp_GetRed(pProp)
		|| tp.g != ThreadProp_GetGreen(pProp)
		|| tp.b != ThreadProp_GetBlue(pProp)
		)
		return false;
	
	return true;
}

TEST(TestThreadProperty, ThreadInfo)
{
	_ThreadProperty tp0, tp1;
	strcpy_s(tp0.manu, "AutoCAD");
	strcpy_s(tp0.code, "1");
	strcpy_s(tp0.name, "Unknown");
	tp0.type = PulseEmbSDK::ThreadTypes::ttCotton;
	tp0.thick = 3.0;
	tp0.g = tp0.b = 0;
	tp0.r = 255;

	// After 'Set'
	strcpy_s(tp1.manu, "Pulse");
	strcpy_s(tp1.code, "PG1");
	strcpy_s(tp1.name, "Pulse Green");
	tp1.type = PulseEmbSDK::ThreadTypes::ttMetallic;
	tp1.thick = 1.5;
	tp1.r = tp1.b = 128;
	tp1.g = 255;

	int cnt = EMBSDK_ThreadTableCount();
	ASSERT_TRUE(cnt > 0);

	for (int i = 0; i < cnt; i++)
	{
		const int nameSize = 100;
		char str[nameSize] = { 0 };
		int realSize = nameSize;
		EMBSDK_GetThreadTableName(i, str, nameSize, &realSize);
		ASSERT_TRUE(realSize > 0);

		// Find 'AutoCAD' and compare values
		if (0 == strcmp(str, "AutoCAD"))
		{
			// Get the first ThreadProperty from each thread table
			int propcnt = EMBSDK_ThreadPropertyCount(i);
			ASSERT_TRUE(propcnt > 0);
			PulseEmbSDK::ThreadProp* pProp = EMBSDK_GetThreadProperty(i, 0);
			ASSERT_TRUE(pProp);
			ASSERT_TRUE(CheckThreadProperty(pProp, tp0));

			// Change thread property
			SetThreadProperty(pProp, tp1);
			ASSERT_TRUE(CheckThreadProperty(pProp, tp1));
			return;
		}
	}
}

#endif //TEST_THREADPROP