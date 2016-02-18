#include "pch.h"
#include "CppUnitTest.h"
#include <DBContext.h>

#include <SqliteException.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(DbConstructorTests)
{
public:
	TEST_METHOD(ValidFileName)
	{
		LocalDB::DBContext ctx(L"testü.db");
	}

	TEST_METHOD(InvalidFileName)
	{
		bool rightException = false;
		try {
			LocalDB::DBContext ctx(L"test/.db");
		}
		catch (LocalDB::SqliteException& ex) {
			rightException = ex.errorCode() == SQLITE_CANTOPEN;
		}
		Assert::AreEqual(true, rightException, L"Incorrect or no exception catched");
	}
};