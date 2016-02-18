#include "pch.h"
#include "CppUnitTest.h"
#include <SchemaBuilder.h>
#include <string>


using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace LocalDB::SchemaDefinition;
#pragma warning(disable:4499)
namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework
		{
			template <> static std::wstring ToString(const SqliteType& t) {
				switch (t)
				{
				case SqliteType::Blob:
					return L"Blob";
				case SqliteType::Double:
					return L"Double";
				case SqliteType::Int64:
					return L"Int64";
				case SqliteType::Text:
					return L"Text";

				default:
					return L"Unknown";
				}
			}

			template <> static std::wstring ToString(const ForeignKeyDeleteBehavior& t) {
				switch (t)
				{
				case ForeignKeyDeleteBehavior::Cascade:
					return L"Cascade";
				case ForeignKeyDeleteBehavior::NoAction:
					return L"NoAction";
				case ForeignKeyDeleteBehavior::SetDefault:
					return L"SetDefault";
				case ForeignKeyDeleteBehavior::SetNull:
					return L"SetNull";

				default:
					return L"Unknown";
				}
			}template <> static std::wstring ToString(const IndexDirection& t) {
				switch (t)
				{
				case IndexDirection::Ascending:
					return L"Ascending";
				case IndexDirection::Descending:
					return L"Descending";

				default:
					return L"Unknown";
				}
			}
		}
	}
}

#pragma warning(default:4499)
TEST_CLASS(SchemaBuilderTests)
{
public:
	TEST_METHOD(SimpleSchema)
	{
		SchemaBuilder builder;
		builder
			.table("people")
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
		SchemaDefinition def = builder.definition();
		Assert::AreEqual((size_t)1, def.tables.size());
		Assert::AreEqual((size_t)1, def.indices.size());
		Assert::AreEqual("people"s, def.tables[0].name);
		Assert::AreEqual((size_t)4, def.tables[0].columns.size());

		Assert::AreEqual("id"s, def.tables[0].columns[0].name);
		Assert::AreEqual(""s, def.tables[0].columns[0].defaultValue);
		Assert::AreEqual(false, def.tables[0].columns[0].nullable);
		Assert::AreEqual(true, def.tables[0].columns[0].primaryKey);
		Assert::AreEqual(SqliteType::Int64, def.tables[0].columns[0].type);
		Assert::AreEqual(false, def.tables[0].columns[0].unique);
		Assert::AreEqual(1, def.tables[0].columns[0].version);

		Assert::AreEqual("firstName"s, def.tables[0].columns[1].name);
		Assert::AreEqual(""s, def.tables[0].columns[1].defaultValue);
		Assert::AreEqual(false, def.tables[0].columns[1].nullable);
		Assert::AreEqual(false, def.tables[0].columns[1].primaryKey);
		Assert::AreEqual(SqliteType::Text, def.tables[0].columns[1].type);
		Assert::AreEqual(false, def.tables[0].columns[1].unique);
		Assert::AreEqual(1, def.tables[0].columns[1].version);

		Assert::AreEqual("lastName"s, def.tables[0].columns[2].name);
		Assert::AreEqual(""s, def.tables[0].columns[2].defaultValue);
		Assert::AreEqual(false, def.tables[0].columns[2].nullable);
		Assert::AreEqual(false, def.tables[0].columns[2].primaryKey);
		Assert::AreEqual(SqliteType::Text, def.tables[0].columns[2].type);
		Assert::AreEqual(false, def.tables[0].columns[2].unique);
		Assert::AreEqual(1, def.tables[0].columns[2].version);

		Assert::AreEqual("managerId"s, def.tables[0].columns[3].name);
		Assert::AreEqual(""s, def.tables[0].columns[3].defaultValue);
		Assert::AreEqual(true, def.tables[0].columns[3].nullable);
		Assert::AreEqual(false, def.tables[0].columns[3].primaryKey);
		Assert::AreEqual(SqliteType::Int64, def.tables[0].columns[3].type);
		Assert::AreEqual(false, def.tables[0].columns[3].unique);
		Assert::AreEqual(1, def.tables[0].columns[3].version);


		Assert::AreEqual((size_t)1, def.tables[0].foreignKeys.size());
		Assert::AreEqual("people"s, def.tables[0].foreignKeys[0].foreignTable);
		Assert::AreEqual(ForeignKeyDeleteBehavior::SetNull, def.tables[0].foreignKeys[0].deleteBehavior);
		Assert::AreEqual((size_t)1, def.tables[0].foreignKeys[0].localToForeignColumnMappings.size());
		Assert::AreEqual("managerId"s, get<0>(def.tables[0].foreignKeys[0].localToForeignColumnMappings[0]));
		Assert::AreEqual("id"s, get<1>(def.tables[0].foreignKeys[0].localToForeignColumnMappings[0]));

		Assert::AreEqual("uk_people_lastName_firstName"s, def.indices[0].name);
		Assert::AreEqual("people"s, def.indices[0].tableName);
		Assert::AreEqual(true, def.indices[0].unique);
		Assert::AreEqual((size_t)2, def.indices[0].columns.size());
		Assert::AreEqual("lastName"s, get<string>(def.indices[0].columns[0]));
		Assert::AreEqual(IndexDirection::Ascending, get<IndexDirection>(def.indices[0].columns[0]));
		Assert::AreEqual("firstName"s, get<string>(def.indices[0].columns[1]));
		Assert::AreEqual(IndexDirection::Ascending, get<IndexDirection>(def.indices[0].columns[1]));

		Assert::AreEqual(1, maxVersion(def));

		auto expectedSql = "CREATE TABLE people(\n\tid INTEGER NOT NULL ,\n\tfirstName TEXT NOT NULL ,\n\tlastName TEXT NOT NULL ,\n\tmanagerId INTEGER ,\n\tPRIMARY KEY (id),\n\tFOREIGN KEY (managerId) REFERENCES people (id) ON DELETE SET NULL\n);\n"s;
		auto expectedSql2 = "CREATE UNIQUE INDEX uk_people_lastName_firstName ON people (lastName ASC, firstName ASC);\n"s;
		std::string tables, indices;
		buildSql(tables, indices, def, 0);
		Assert::AreEqual(expectedSql, tables);
		Assert::AreEqual(expectedSql2, indices);
	}


	TEST_METHOD(AddColumn)
	{
		SchemaBuilder builder;
		builder
			.table("people")
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
				.column("phone", SqliteType::Text, 2)
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
		SchemaDefinition def = builder.definition();
		
		Assert::AreEqual(2, maxVersion(def));

		auto expectedSql = "CREATE TABLE people(\n\tid INTEGER NOT NULL ,\n\tfirstName TEXT NOT NULL ,\n\tlastName TEXT NOT NULL ,\n\tmanagerId INTEGER ,\n\tphone TEXT ,\n\tPRIMARY KEY (id),\n\tFOREIGN KEY (managerId) REFERENCES people (id) ON DELETE SET NULL\n);\nCREATE UNIQUE INDEX uk_people_lastName_firstName ON people (lastName ASC, firstName ASC);\n"s;
		std::string tables, indices;
		buildSql(tables, indices, def, 1);
		Assert::AreEqual("ALTER TABLE people ADD \tphone TEXT ;\n"s, tables);
	}
};