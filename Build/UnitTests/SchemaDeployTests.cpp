#include "pch.h"
#include "CppUnitTest.h"
#include <DBContext.h>
#include <SchemaBase.h>
#include <SchemaBuilder.h>
#include <DBQuery.h>
#include <string>

#include <SqliteException.h>
#include <Sqlite3Extensions.h>
using namespace std;
using namespace LocalDB;
using namespace LocalDB::SchemaDefinition;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#pragma warning(disable:4499)
namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework
		{
			template <> static std::wstring ToString(const std::int64_t& t) {
				return std::to_wstring(t);
			}
		}
	}
}
#pragma warning(default:4499)
TEST_CLASS(SchemaDeployTests)
{
public:
	TEST_METHOD(SimpleSchemaDeploy)
	{
		class TestSchema : public LocalDB::SchemaBase {
		protected:

			virtual void BuildSchema(SchemaBuilder& builder) override {
				builder.table("people")
						.column("id", SqliteType::Int64)
							.isPrimaryKey(true)
							.nullable(false)
						.endColumn()
						.column("firstName", SqliteType::Text)
							.nullable(false)
						.endColumn()
						.column("lastName", SqliteType::Text)
							.nullable(false)
						.endColumn()
						.column("managerId", SqliteType::Int64)
							.nullable(true)
						.endColumn()
						.foreignKey("people")
							.mapColumn("managerId", "id")
							.onDelete(ForeignKeyDeleteBehavior::SetNull)
						.endForeignKey()
					.endTable()
					.index("uk_people_lastName_firstName", "people")
						.addColumn("lastName")
						.addColumn("firstName")
						.unique(true)
					.endIndex();
			}
		public:
			TestSchema(const LocalDB::DBContext& dbContext) : SchemaBase(dbContext) {}
		};

		class TestSchemaV2 : public LocalDB::SchemaBase {
		protected:

			virtual void BuildSchema(SchemaBuilder& builder) override {
				builder.table("people")
						.column("id", SqliteType::Int64)
							.isPrimaryKey(true)
							.nullable(false)
						.endColumn()
						.column("firstName", SqliteType::Text)
							.nullable(false)
						.endColumn()
						.column("lastName", SqliteType::Text)
							.nullable(false)
						.endColumn()
						.column("managerId", SqliteType::Int64)
							.nullable(true)
						.endColumn()
						.column("phone", SqliteType::Int64, 2)
							.nullable(true)
						.endColumn()
						.foreignKey("people")
							.mapColumn("managerId", "id")
							.onDelete(ForeignKeyDeleteBehavior::SetNull)
						.endForeignKey()
					.endTable()
					.index("uk_people_lastName_firstName", "people")
						.addColumn("lastName")
						.addColumn("firstName")
						.unique(true)
					.endIndex();
			}
		public:
			TestSchemaV2(const LocalDB::DBContext& dbContext) : SchemaBase(dbContext) {}
		};

		class InsertPersonCommand : public DBQuery {
		private:
			std::int64_t _id;
			std::wstring _firstName;
			std::wstring _lastName;
		protected:
			virtual std::string identifier() override {
				return "InsertPersonCommand";
			}
			virtual std::string sql(int /*statementIndex*/) override {
				return "insert into people(id, firstName, lastName) values(@id, @firstName,@lastName)";
			}

			virtual void bindParameters(int /*statementIndex*/, sqlite3* /*db*/, sqlite3_stmt* statement) {
				sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _id);
				LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@firstName"), _firstName);
				LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@lastName"), _lastName);
			};
		public:
			InsertPersonCommand(const DBContext& ctx, std::int64_t id, const std::wstring& firstName, const std::wstring& lastName):DBQuery(ctx),
			_id(id), _firstName(firstName), _lastName(lastName){}
			concurrency::task<void> ExecuteAsync(concurrency::cancellation_token cancelToken) {
				return executeAsync(nullptr, nullptr, cancelToken);
			}

			void ExecuteSynchronously(sqlite3*  db, concurrency::cancellation_token cancelToken) {
				return executeSynchronously(db, nullptr, nullptr, cancelToken);
			}
		};

		class CountQueryInt : public LocalDB::IntegerScalarDBQuery {
		protected:
			virtual std::string identifier() override {
				return "CountQueryInt";
			}
			virtual std::string sql(int /*statementIndex*/) override {
				return "select count(Id) from people";
			}
		public:
			CountQueryInt(const DBContext& ctx) : IntegerScalarDBQuery(ctx) {}
		};

		class CountQueryDouble : public LocalDB::RealScalarDBQuery {
		protected:
			virtual std::string identifier() override {
				return "CountQueryDouble";
			}
			virtual std::string sql(int /*statementIndex*/) override {
				return "select count(*) from people";
			}
		public:
			CountQueryDouble(const DBContext& ctx) : RealScalarDBQuery(ctx) {}
		};

		class CountQueryString : public LocalDB::TextScalarDBQuery {
		protected:
			virtual std::string identifier() override {
				return "CountQueryString";
			}
			virtual std::string sql(int /*statementIndex*/) override {
				return "select 'count is ' || CAST(count(*) as TEXT) from people";
			}
		public:
			CountQueryString(const DBContext& ctx) : TextScalarDBQuery(ctx) {}
		};

		struct Person {
			std::int64_t id;
			std::wstring firstName;
			std::wstring lastName;
			Person() = default;
			Person(std::int64_t id, const std::wstring& firstName, const std::wstring& lastName) : id(id), firstName(firstName), lastName(lastName) {}
		};

		class GetPeopleQuery : public ResultSetDBQuery<Person> {
		protected:
			virtual std::string identifier() override {
				return "GetPeopleQuery";
			}
			virtual std::string sql(int /*statementIndex*/) override {
				return "select id, firstName, lastName from people order by id";
			}
			virtual Person materialize(sqlite3* /*db*/, sqlite3_stmt* statement) override {
				return Person(sqlite3_column_int64(statement, 0), static_cast<const wchar_t*>(sqlite3_column_text16(statement, 1)), static_cast<const wchar_t*>(sqlite3_column_text16(statement, 2)));
			}
			public:
				GetPeopleQuery(const DBContext& ctx) : ResultSetDBQuery<Person>(ctx) {}
		};


		DBContext ctx(L"testdb.dat");
		{
			std::shared_ptr<TestSchema> schema = std::make_shared<TestSchema>(ctx);

			schema->initializeAsync(concurrency::cancellation_token::none()).get();
		}
		{
			std::shared_ptr<TestSchemaV2> schema = std::make_shared<TestSchemaV2>(ctx);

			schema->initializeAsync(concurrency::cancellation_token::none()).get();
		}
		Assert::AreEqual(0ll, executeAsync<CountQueryInt>(ctx).get());
		Assert::AreEqual(0.0, executeAsync<CountQueryDouble>(ctx).get());
		Assert::AreEqual(L"count is 0"s, executeAsync<CountQueryString>(ctx).get());

		executeAsync<InsertPersonCommand>(ctx, 1, L"Simon", L"Ferquel").get();

		

		Assert::AreEqual(1ll, executeAsync<CountQueryInt>(ctx).get());
		Assert::AreEqual(1.0, executeAsync<CountQueryDouble>(ctx).get());
		Assert::AreEqual(L"count is 1"s, executeAsync<CountQueryString>(ctx).get());
		auto people = executeAsync<GetPeopleQuery>(ctx).get();
		Assert::AreEqual((size_t)1, people->size());
		Assert::AreEqual(1ll, people->at(0).id);
		Assert::AreEqual(L"Simon"s, people->at(0).firstName);
		Assert::AreEqual(L"Ferquel"s, people->at(0).lastName);

		// test synchronous overloads

		ctx.executeAsync([&ctx](sqlite3* db) {
			Assert::AreEqual(1ll, executeSynchronously<CountQueryInt>(ctx, db));
			Assert::AreEqual(1.0, executeSynchronously<CountQueryDouble>(ctx,db));
			Assert::AreEqual(L"count is 1"s, executeSynchronously<CountQueryString>(ctx, db));
			auto people = executeSynchronously<GetPeopleQuery>(ctx, db);
			Assert::AreEqual((size_t)1, people->size());
			Assert::AreEqual(1ll, people->at(0).id);
			Assert::AreEqual(L"Simon"s, people->at(0).firstName);
			Assert::AreEqual(L"Ferquel"s, people->at(0).lastName);


			executeSynchronously<InsertPersonCommand>(ctx, db, 2, L"Toto", L"SurSonBateau");

			Assert::AreEqual(2ll, executeSynchronously<CountQueryInt>(ctx, db));
			Assert::AreEqual(2.0, executeSynchronously<CountQueryDouble>(ctx, db));
			Assert::AreEqual(L"count is 2"s, executeSynchronously<CountQueryString>(ctx, db));
			people = executeSynchronously<GetPeopleQuery>(ctx, db);
			Assert::AreEqual((size_t)2, people->size());
			Assert::AreEqual(1ll, people->at(0).id);
			Assert::AreEqual(L"Simon"s, people->at(0).firstName);
			Assert::AreEqual(L"Ferquel"s, people->at(0).lastName);
			Assert::AreEqual(2ll, people->at(1).id);
			Assert::AreEqual(L"Toto"s, people->at(1).firstName);
			Assert::AreEqual(L"SurSonBateau"s, people->at(1).lastName);
		}).get();

	}
};