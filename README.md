# UWP-LocalDB-CPP
Helper library for manipulating a local sqlite DB in a UWP application

## Features
- Schema definition using Fluent code style
- Schema migrations (upgrading database schema without losing existing data)
- Sqlite statement cache (avoiding unnecesary recompilation of sql statements)
- Helpers for common C++ types parameter bindings / column extractions
- Base class for creating queries / commands leveraging sqlite statement caching
- Asynchronous execution
- Transactions

## Dependencies
SQLite for Universal App Platform version 3.8.11.1
Windows 10 SDK version 10.0.10240.0

## Examples

### Schema definition
- Version 1:
```C++
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
```

deployment : 

```C++
DBContext ctx(L"testdb.dat");
{
	std::shared_ptr<TestSchema> schema = std::make_shared<TestSchema>(ctx);
	auto task = schema->initializeAsync(concurrency::cancellation_token::none());
	// do something while task is running
	task.get();
}
```

migration : 
```C++
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
				.column("phone", SqliteType::Int64, 2) // 2 makes the column appear in "version 2" of the schema (migrating from v1 will generate the correct alter table statement)
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
```

### Query definition and execution
Definition : 

```C++
class InsertPersonCommand : public NoResultDBQuery {
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
	InsertPersonCommand(const DBContext& ctx, std::int64_t id, const std::wstring& firstName, const std::wstring& lastName):NoResultDBQuery(ctx),
	_id(id), _firstName(firstName), _lastName(lastName){}
			
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
		return Person(sqlite3_column_int64(statement, 0), sqlite3_column_wstring(statement, 1), sqlite3_column_wstring(statement, 2));
	}
public:
	GetPeopleQuery(const DBContext& ctx) : ResultSetDBQuery<Person>(ctx) {}
};

```

Execution of a single query : 
```C++
auto peopleTask = executeAsync<GetPeopleQuery>(ctx);
propleTask.then([](std::shared_ptr<std::vector<Person>> people){
// do something with people
});
```

Execution of n queries inside a transaction : 

```C++
ctx.executeAsync([ctx](sqlite3* db) {
	auto ctxNonConstCopy = ctx;
	SynchronousTransactionScope trans(ctxNonConstCopy, db);
	executeSynchronously<InsertPersonCommand>(ctxNonConstCopy, db, 2, L"Simon", L"Ferquel");
	trans.commit();
});
```
