#include "pch.h"
#include "CppUnitTest.h"
#include <DBContext.h>

#include <SqliteException.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#pragma warning (disable:4100)
TEST_CLASS(DbConstructorTests)
{
public:
	TEST_METHOD(NormalStatement)
	{
		LocalDB::DBContext ctx(L"NormalStatement.db");
		SleepEx(100, FALSE);
		auto t1 = ctx.executeAsync([](sqlite3* db) {
			if (SQLITE_OK != sqlite3_exec(db, "create table test(data TEXT not null);", nullptr, nullptr, nullptr)) {
				throw LocalDB::SqliteException(db);
			}
			if (SQLITE_OK != sqlite3_exec(db, "insert into test(data)values('hello');", nullptr, nullptr, nullptr)) {
				throw LocalDB::SqliteException(db);
			}
		});
		auto t2 = ctx.executeAsync([](sqlite3* db) {
			if (SQLITE_OK != sqlite3_exec(db, "select * from test;", [](void*, int count, char** values, char** names) {
				for (auto ix = 0; ix < count; ++ix) {
					OutputDebugStringA(names[ix]);
					OutputDebugString(L" : ");
					OutputDebugStringA(values[ix]);

					OutputDebugString(L" | ");
				}

				OutputDebugString(L"\n");
				return 0;
			}, nullptr, nullptr)) {
				throw LocalDB::SqliteException(db);
			}
		});
		t1.get();
		t2.get();

	}
	class test_ex : public std::exception {
	public:
		test_ex() : std::exception("test_ex") {}
	};

	struct nostdex {
		int v;
		nostdex() = default;
		nostdex(int a) :v(a) {}
	};

	TEST_METHOD(ExceptionPropagation)
	{
		LocalDB::DBContext ctx(L"ExceptionPropagation.db");
		SleepEx(100, FALSE);
		auto t1 = ctx.executeAsync([](sqlite3* db) {

			throw test_ex();
		});
		auto t2 = ctx.executeAsync([](sqlite3* db) {
			throw ref new Platform::COMException(-7);
		});
		auto t3 = ctx.executeAsync([](sqlite3* db) {
			throw nostdex(12);
		});

		bool stdexcatched = false;
		bool winrtexcatched = false;
		bool nostdcatched = false;
		try {
			t1.get();
		}
		catch (test_ex&) {
			stdexcatched = true;
		}
		try {
			t2.get();
		}
		catch (Platform::COMException^) {
			winrtexcatched = true;
		}
		try {
			t3.get();
		}
		catch (nostdex&) {
			nostdcatched = true;
		}
		Assert::IsTrue(stdexcatched, L"standard ex not catched");
		Assert::IsTrue(winrtexcatched, L"winrt ex not catched");
		Assert::IsTrue(nostdcatched, L"not standard ex not catched");

	}


};
#pragma warning (default:4100)