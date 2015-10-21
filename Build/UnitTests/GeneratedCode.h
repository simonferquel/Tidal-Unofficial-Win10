#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <sqlite3.h>
#include <DBQuery.h>
#include <Sqlite3Extensions.h>
namespace test {
	namespace entities {
		struct people {
			std::int64_t id;
			std::wstring firstName;
			std::wstring lastName;
			std::int64_t managerId;
			std::int64_t phone;
			static std::string getOrderedColumnsForSelect() {
				return "id, firstName, lastName, managerId, phone";
			}
			static people createFromSqlRecord(sqlite3_stmt* stmt) {
				people data;
				data.id = sqlite3_column_int64(stmt, 0);
				data.firstName = LocalDB::sqlite3_column_wstring(stmt, 1);
				data.lastName = LocalDB::sqlite3_column_wstring(stmt, 2);
				data.managerId = sqlite3_column_int64(stmt, 3);
				data.phone = sqlite3_column_int64(stmt, 4);
				return data;
			}
		};
		class peopleInsertDbQuery : public LocalDB::NoResultDBQuery {
		private:
			people _entity;
		protected:
			virtual std::string identifier() override { return "gen-people-insert"; }
			virtual std::string sql(int) override {
				return "insert into people(id, firstName, lastName, managerId, phone) values (@id, @firstName, @lastName, @managerId, @phone)";
			}
			virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
				sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
				LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@firstName"), _entity.firstName);
				LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@lastName"), _entity.lastName);
				sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@managerId"), _entity.managerId);
				sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@phone"), _entity.phone);
			}
		public:
			peopleInsertDbQuery(const LocalDB::DBContext& ctx, const people& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
		};
		class peopleUpdateDbQuery : public LocalDB::NoResultDBQuery {
		private:
			people _entity;
		protected:
			virtual std::string identifier() override { return "gen-people-update"; }
			virtual std::string sql(int) override {
				return "update people SET firstName = @firstName, lastName = @lastName, managerId = @managerId, phone = @phone WHERE id = @id";
			}
			virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
				sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
				LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@firstName"), _entity.firstName);
				LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@lastName"), _entity.lastName);
				sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@managerId"), _entity.managerId);
				sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@phone"), _entity.phone);
			}
		public:
			peopleUpdateDbQuery(const LocalDB::DBContext& ctx, const people& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
		};
	}
}