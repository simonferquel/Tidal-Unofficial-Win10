#include "pch.h"
#include "DBQuery.h"

concurrency::task<void> LocalDB::DBQuery::executeAsync(void(*rowCallback)(void *param, sqlite3 *db,int statementIndex, sqlite3_stmt *statement), void* param, concurrency::cancellation_token cancelToken)
{
	auto that = shared_from_this();
	return _context.executeAsync([that, rowCallback, param, cancelToken](sqlite3 * db) {
		that->executeSynchronously(db, rowCallback, param, cancelToken);
	});
}

void LocalDB::DBQuery::executeSynchronously(sqlite3 * db, void(*row_callback)(void *param, sqlite3 *db, int statementIndex, sqlite3_stmt *statement), void* param, concurrency::cancellation_token cancelToken)
{

	for (int i = 0; i < statementCount(); ++i) {
		if (cancelToken.is_canceled()) {
			throw concurrency::task_canceled();
		}
		auto fullId = identifier() + std::to_string(i);
		auto cachedStatement = _context.getCachedStatement(fullId);
		if (!cachedStatement.get()) {
			auto statement = sqlite3_prepare_v2_shared(db, sql(i).c_str(), -1, nullptr);
			_context.registerCachedStatement(fullId, statement);
			cachedStatement = auto_reset_statement_ptr(statement);
		}
		if (cancelToken.is_canceled()) {
			throw concurrency::task_canceled();
		}
		bindParameters(i, db, cachedStatement.get());
		for (;;) {

			if (cancelToken.is_canceled()) {
				throw concurrency::task_canceled();
			}

			auto result = sqlite3_step(cachedStatement.get());
			if (result == SQLITE_DONE) {
				break;
			}
			else if (result == SQLITE_ROW) {
				if (row_callback) {
					row_callback(param, db, i, cachedStatement.get());
				}
			}
			else {
				throw SqliteException(db);
			}
		}
	}
}

std::int64_t LocalDB::IntegerScalarDBQuery::ExecuteSynchronously(sqlite3 * db, concurrency::cancellation_token ct)
{
	std::int64_t value;
	executeSynchronously(db, [](void* param, sqlite3* , int, sqlite3_stmt* statement) {
		std::int64_t* pValue = static_cast<std::int64_t*>(param);
		*pValue = sqlite3_column_int64(statement, 0);
	}, &value, ct);
	return value;
}

concurrency::task<std::int64_t> LocalDB::IntegerScalarDBQuery::ExecuteAsync(concurrency::cancellation_token ct)
{
	
	struct shared_state{
		std::shared_ptr<IntegerScalarDBQuery> that;
		std::int64_t result;
		shared_state() = default;
		shared_state(std::shared_ptr<IntegerScalarDBQuery>&& pthat) : that(pthat) {}
	};
	auto state = std::make_shared<shared_state>(std::dynamic_pointer_cast<IntegerScalarDBQuery>(shared_from_this()));
	return context().executeAsync([state, ct](sqlite3* db) {state->result = state->that->ExecuteSynchronously(db, ct); })
		.then([state]() {
		return state->result;
	});
}

double LocalDB::RealScalarDBQuery::ExecuteSynchronously(sqlite3 * db, concurrency::cancellation_token ct)
{
	double value;
	executeSynchronously(db, [](void* param, sqlite3* , int, sqlite3_stmt* statement) {
		double* pValue = static_cast<double*>(param);
		*pValue = sqlite3_column_double(statement, 0);
	}, &value, ct);
	return value;
}

concurrency::task<double> LocalDB::RealScalarDBQuery::ExecuteAsync(concurrency::cancellation_token ct)
{
	struct shared_state {
		std::shared_ptr<RealScalarDBQuery> that;
		double result;
		shared_state() = default;
		shared_state(std::shared_ptr<RealScalarDBQuery>&& pthat) : that(pthat) {}
	};
	auto state = std::make_shared<shared_state>(std::dynamic_pointer_cast<RealScalarDBQuery>(shared_from_this()));
	return context().executeAsync([state, ct](sqlite3* db) {state->result = state->that->ExecuteSynchronously(db, ct); })
		.then([state]() {
		return state->result;
	});
}

std::wstring LocalDB::TextScalarDBQuery::ExecuteSynchronously(sqlite3 * db, concurrency::cancellation_token ct)
{
	std::wstring value;
	executeSynchronously(db, [](void* param, sqlite3* , int, sqlite3_stmt* statement) {
		std::wstring* pValue = static_cast<std::wstring*>(param);
		*pValue = static_cast<const wchar_t*>(sqlite3_column_text16(statement, 0));
	}, &value, ct);
	return value;
}

concurrency::task<std::wstring> LocalDB::TextScalarDBQuery::ExecuteAsync(concurrency::cancellation_token ct)
{
	struct shared_state {
		std::shared_ptr<TextScalarDBQuery> that;
		std::wstring result;
		shared_state() = default;
		shared_state(std::shared_ptr<TextScalarDBQuery>&& pthat) : that(pthat) {}
	};
	auto state = std::make_shared<shared_state>(std::dynamic_pointer_cast<TextScalarDBQuery>(shared_from_this()));
	return context().executeAsync([state, ct](sqlite3* db) {state->result = state->that->ExecuteSynchronously(db, ct); })
		.then([state]() {
		return state->result;
	});
}
