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
## Threading model
Sqlite provides a somewhat limited support for multi-threading. It allows multiple db* instances to be open concurrently (even from multiple processes), and handles concurrent transactions quite well (even from multiple processes), but each db* instance should be considered as single threaded, at least when multi-statement transactions are needed. Indeed, there is no kind of transaction context in the sqlite library, so the user must very carefull when using it in a multi-threaded way so that statements belonging to different transactions do not interleave.
There is 2 ways to safely achieve concurrent transactions with sqlite:
- make sure the data access code is single threaded
- open / close a db* connection for each transaction
The problem with the second solution is that opening/closing a connection is a non-trivial operation, and that prepared statements belongs to one and only one sqlite connection (so we cannot reuse statements prepared on one connection to execute code on another). So performance-wise, this approach did not fit with the goals I had with this library.
So I did take the first approach : each DBContext instance creates a worker thread and a job queue. The sqlite connection is accessible to other components only from within a submited job (using DBContext::executeAsync()). So, multi-statement transactions should always be coded with this kind of code snippet:
```C++
ctx.executeAsync([ctx](sqlite3* db) {
	auto ctxNonConstCopy = ctx;
	SynchronousTransactionScope trans(ctxNonConstCopy, db);
	executeSynchronously<Query1>(ctxNonConstCopy, db, ...);
	executeSynchronously<Query2>(ctxNonConstCopy, db, ...);
	...
	trans.commit();
});
```
This way, 2 transactions cannot have there statements interleaved, and sqlite processing is always done in a single background thread. Results are exposed to the caller trough ppl tasks (so user code can wait for the result using the "get()" method, or submit a continuation task to work in a purely asynchronous and non-blocking style).
