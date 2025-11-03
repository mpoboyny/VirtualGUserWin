#include "pch.h"
#include "CppUnitTest.h"

#include "../include/mp.hxx"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace frameMPuTest
{
	TEST_CLASS(frameMPstrTest)
	{
	public:
        TEST_METHOD(Test01_StrToStrA)
        {
            frameMP::Str testStr = L"Test string 12345!@#$%";
            frameMP::StrA convertedStr = frameMP::StrToStrA(testStr);
            Assert::AreEqual(convertedStr.c_str(), "Test string 12345!@#$%");
        }

        TEST_METHOD(Test02_StrToStrW)
        {
            frameMP::Str testStr = _T("Another test string 67890^&*()");
            frameMP::StrW convertedStr = frameMP::StrToStrW(testStr);
            Assert::AreEqual(convertedStr.c_str(), L"Another test string 67890^&*()");
        }
	};
}
