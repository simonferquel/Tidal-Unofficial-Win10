#pragma once
#include <string>
#include "DBContext.h"
#include <mutex>
#include "SqliteSmartPtr.h"
#include <cstdint>
#include <vector>
#include <chrono>
namespace LocalDB {

	// base class for creating sqlite based queries and commands
	class DBQuery : public std::enable_shared_from_this<DBQuery>
	{
	private:
		DBContext _context;
	protected:
		DBContext& context() { return _context; }
		// get identifier of the query (for auto-statement caching)
		virtual std::string identifier() = 0;
		// get the sql statement for a given index (number of calls depends of statementCount())
		virtual std::string sql(int statementIndex) = 0;
		// get the number of sql statements composing the query (default : 1)
		virtual int statementCount() {
			return 1;
		};
#pragma warning (disable : 4100)
		// called when the sqlite statement must be bound to its parameters. Parameterless queries should not override this method
		virtual void bindParameters(int statementIndex, sqlite3* db, sqlite3_stmt* statement) {};
#pragma warning (default : 4100)
	
		// execute the query asynchronously
		concurrency::task<void> executeAsync(void(*row_callback)(void* param, sqlite3* db, int statementIndex, sqlite3_stmt* statement), void* param, concurrency::cancellation_token = concurrency::cancellation_token::none());
		// execute the query synchronously (within a DBContext:executeAsync call)
		void executeSynchronously(sqlite3* db, void(*row_callback)(void* param, sqlite3* db, int statementIndex, sqlite3_stmt* statement), void* param, concurrency::cancellation_token = concurrency::cancellation_token::none());
	public:
		// constructor of the query
		DBQuery(const DBContext& context) : _context(context) {}
		virtual ~DBQuery() {}

	};
	// Query executing a Begin Transaction Immediate statement
	class BeginImmediateTransactionQuery : public DBQuery {
	protected:
		virtual std::string identifier() {
			return 	"BeginImmediateTransactionQuery";
		}
		virtual std::string sql(int) {
			return "BEGIN TRANSACTION IMMEDIATE;";
		}
	public:

		BeginImmediateTransactionQuery(const DBContext& context) : DBQuery(context) {}
		void ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token cancelToken = concurrency::cancellation_token::none()) {
			return executeSynchronously(db, nullptr, nullptr, cancelToken);
		}
		concurrency::task<void> ExecuteAsync(concurrency::cancellation_token cancelToken = concurrency::cancellation_token::none()) {
			return executeAsync(nullptr, nullptr, cancelToken);
		}
	};

	// Query executing a Commit transaction statement
	class CommitTransactionQuery : public DBQuery {
	protected:
		virtual std::string identifier() {
			return 	"CommitTransactionQuery";
		}
		virtual std::string sql(int) {
			return "COMMIT TRANSACTION;";
		}
	public:

		CommitTransactionQuery(const DBContext& context) : DBQuery(context) {}
		void ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token cancelToken = concurrency::cancellation_token::none()) {
			return executeSynchronously(db, nullptr, nullptr, cancelToken);
		}
		concurrency::task<void> ExecuteAsync(concurrency::cancellation_token cancelToken = concurrency::cancellation_token::none()) {
			return executeAsync(nullptr, nullptr, cancelToken);
		}
	};

	// Query executing a rollback transaction statement
	class RollbackTransactionQuery : public DBQuery {
	protected:
		virtual std::string identifier() {
			return 	"RollbackTransactionQuery";
		}
		virtual std::string sql(int) {
			return "ROLLBACK TRANSACTION;";
		}
	public:

		RollbackTransactionQuery(const DBContext& context) : DBQuery(context) {}
		void ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token cancelToken = concurrency::cancellation_token::none()) {
			return executeSynchronously(db, nullptr, nullptr, cancelToken);
		}
		concurrency::task<void> ExecuteAsync(concurrency::cancellation_token cancelToken = concurrency::cancellation_token::none()) {
			return executeAsync(nullptr, nullptr, cancelToken);
		}
	};

	// base class for scalar queries returning an Int64
	class IntegerScalarDBQuery : public DBQuery {
	protected:
		IntegerScalarDBQuery(const DBContext& ctx) : DBQuery(ctx) {}
	public:
		std::int64_t ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token = concurrency::cancellation_token::none());
		concurrency::task<std::int64_t> ExecuteAsync(concurrency::cancellation_token = concurrency::cancellation_token::none());
	};

	// base class for scalar queries returning a double
	class RealScalarDBQuery : public DBQuery {
	protected:
		RealScalarDBQuery(const DBContext& ctx) : DBQuery(ctx) {}
	public:
		double ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token = concurrency::cancellation_token::none());
		concurrency::task<double> ExecuteAsync(concurrency::cancellation_token = concurrency::cancellation_token::none());
	};

	// base class for scalar queries returning a wstring
	class TextScalarDBQuery : public DBQuery {
	protected:
		TextScalarDBQuery(const DBContext& ctx) : DBQuery(ctx) {}
	public:
		std::wstring ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token = concurrency::cancellation_token::none());
		concurrency::task<std::wstring> ExecuteAsync(concurrency::cancellation_token = concurrency::cancellation_token::none());
	};

	// base class for queries with no results
	class NoResultDBQuery : public DBQuery {
	protected:
		NoResultDBQuery(const DBContext& ctx) : DBQuery(ctx) {}
	public:
		concurrency::task<void> ExecuteAsync(concurrency::cancellation_token cancelToken) {
			return executeAsync(nullptr, nullptr, cancelToken);
		}

		void ExecuteSynchronously(sqlite3*  db, concurrency::cancellation_token cancelToken) {
			return executeSynchronously(db, nullptr, nullptr, cancelToken);
		}
	};

	// base class for queries returning a set of entities
	// T: type of the entities to materialize
	template<typename T>
	class ResultSetDBQuery : public DBQuery {
		struct shared_state {
			std::shared_ptr<ResultSetDBQuery<T>> that;
			std::shared_ptr<std::vector<T>> result;
			shared_state() = default;
			shared_state(std::shared_ptr<ResultSetDBQuery<T>>&& pthat) : that(pthat) {}
		};

	protected:
		ResultSetDBQuery(const DBContext& ctx) : DBQuery(ctx) {}
		// implement this method to materialize an entity (called once per returned row)
		virtual T materialize(sqlite3* db, sqlite3_stmt* statement) = 0;
	public:
		std::shared_ptr<std::vector<T>> ExecuteSynchronously(sqlite3* db, concurrency::cancellation_token = concurrency::cancellation_token::none());
		concurrency::task<std::shared_ptr<std::vector<T>>> ExecuteAsync(concurrency::cancellation_token = concurrency::cancellation_token::none());
	};
	template<typename T>
	inline std::shared_ptr<std::vector<T>> ResultSetDBQuery<T>::ExecuteSynchronously(sqlite3 * db, concurrency::cancellation_token ct)
	{
		struct thisAndResult {
			ResultSetDBQuery<T>& that;
			std::shared_ptr<std::vector<T>> result;
			thisAndResult(ResultSetDBQuery<T>& that, const std::shared_ptr<std::vector<T>>& result) : that(that), result(result) {}
		};
		std::shared_ptr<std::vector<T>> result = std::make_shared<std::vector<T>>();
		thisAndResult state(*this, result);
		executeSynchronously(db, [](void* param, sqlite3* db, int /*statementIndex*/, sqlite3_stmt* statement) {
			thisAndResult* pState = static_cast<thisAndResult*>(param);
			pState->result->push_back(pState->that.materialize(db, statement));
		}, &state, ct);
		return result;
	}
	template<typename T>
	inline concurrency::task<std::shared_ptr<std::vector<T>>> ResultSetDBQuery<T>::ExecuteAsync(concurrency::cancellation_token ct)
	{

		auto state = std::make_shared<shared_state>(std::dynamic_pointer_cast<ResultSetDBQuery<T>>(shared_from_this()));
		return context().executeAsync([state, ct](sqlite3* db) {state->result = state->that->ExecuteSynchronously(db, ct); })
			.then([state]() {
			return  state->result;
		});
	}

	// execute a query synchronously (within a DBContext::executeAsync call)
	// TQuery : type of the query)
	// ctx : DBContext on which to call the query
	// db : sqlite3* parameter passed to the DBContext::executeAsync callback
	// cancelToken : cancellation token associated with the query
	// args : additional parameters passed to the constructor of TQuery
	template<typename TQuery, typename... TParams>
	auto executeSynchronouslyWithCancel(DBContext& ctx, sqlite3 * db, concurrency::cancellation_token cancelToken, TParams&&... args) {
		TQuery q(ctx, std::forward<TParams>(args)...);
		return q.ExecuteSynchronously(db, cancelToken);
	}

	// execute a query asynchronously
	// TQuery : type of the query)
	// ctx : DBContext on which to call the query
	// cancelToken : cancellation token associated with the query
	// args : additional parameters passed to the constructor of TQuery
	template<typename TQuery, typename... TParams>
	auto executeAsyncWithCancel(DBContext& ctx, concurrency::cancellation_token cancelToken, TParams&&... args) {
		std::shared_ptr<TQuery> q = std::make_shared<TQuery>(ctx, std::forward<TParams>(args)...);
		return q->ExecuteAsync(cancelToken);
	}

	// execute a query synchronously (within a DBContext::executeAsync call)
	// TQuery : type of the query)
	// ctx : DBContext on which to call the query
	// db : sqlite3* parameter passed to the DBContext::executeAsync callback
	// args : additional parameters passed to the constructor of TQuery
	template<typename TQuery, typename... TParams>
	auto executeSynchronously(DBContext& ctx, sqlite3 * db, TParams&&... args) {
		return executeSynchronouslyWithCancel<TQuery>(ctx, db, concurrency::cancellation_token::none(), std::forward<TParams>(args)...);
	}

	// execute a query asynchronously
	// TQuery : type of the query)
	// ctx : DBContext on which to call the query
	// args : additional parameters passed to the constructor of TQuery
	template<typename TQuery, typename... TParams>
	auto executeAsync(DBContext& ctx, TParams&&... args) {
		return executeAsyncWithCancel<TQuery>(ctx, concurrency::cancellation_token::none(), std::forward<TParams>(args)...);
	}


	// RAII style transaction scope usable within a DBContext::executeAsync call
	// if destroyed before calling the commit method, the transaction will be automatically rollback
	class SynchronousTransactionScope {
	private:
		DBContext& _context;
		sqlite3* _db;
		bool _commit = false;
	public:
		SynchronousTransactionScope(DBContext& dbcontext, sqlite3* db) :_context(dbcontext), _db(db) {
			executeSynchronously<BeginImmediateTransactionQuery>(_context, db);
		}
		// commit the transaction
		void commit() {
			_commit = true;
			executeSynchronously<CommitTransactionQuery>(_context, _db);
		}
		~SynchronousTransactionScope() {
			if (!_commit) {
				_commit = true;
				executeSynchronously<RollbackTransactionQuery>(_context, _db);
			}
		}
	};

}

