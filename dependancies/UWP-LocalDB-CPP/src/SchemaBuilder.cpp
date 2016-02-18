#include "pch.h"
#include "SchemaBuilder.h"
#undef max
#include <algorithm>
#include <exception>
#include <tuple>
#include <sstream>
using namespace LocalDB::SchemaDefinition;
using namespace std;

int LocalDB::SchemaDefinition::maxVersion(const SchemaDefinition & definition)
{
	int version = 0;
	for (auto& table : definition.tables) {
		version = std::max(version, table.version);
		for (auto& col : table.columns) {
			version = std::max(version, col.version);
		}
		for (auto& fk : table.foreignKeys) {
			version = std::max(version, fk.version);
		}
	}
	for (auto& index : definition.indices) {
		version = std::max(version, index.version);
	}
	return version;
}

std::string toString(ForeignKeyDeleteBehavior behavior) {
	switch (behavior)
	{
	case LocalDB::SchemaDefinition::ForeignKeyDeleteBehavior::NoAction:
		return "NO ACTION";
		break;
	case LocalDB::SchemaDefinition::ForeignKeyDeleteBehavior::SetNull:
		return "SET NULL";
		break;
	case LocalDB::SchemaDefinition::ForeignKeyDeleteBehavior::SetDefault:
		return "SET DEFAULT";
		break;
	case LocalDB::SchemaDefinition::ForeignKeyDeleteBehavior::Cascade:
		return "CASCADE";
		break;
	default:
		return "";
		break;
	}
}

std::string toString(SqliteType type) {
	switch (type)
	{
	case LocalDB::SchemaDefinition::SqliteType::Text:
		return "TEXT";
	case LocalDB::SchemaDefinition::SqliteType::Int64:
		return "INTEGER";
	case LocalDB::SchemaDefinition::SqliteType::Double:
		return "REAL";
	case LocalDB::SchemaDefinition::SqliteType::Blob:
		return "BLOB";
	default:
		throw std::exception("sqlite type unknown");
	}
}

void buildSql(std::string& output, const ColumnDefinition& column, bool isFirstCol) {
	if(!isFirstCol) {
		output.append(",\n");
	}
	output.append("\t");
	
	output.append(column.name);
	output.append(" ");
	output.append(toString(column.type));
	output.append(" ");
	if (!column.nullable) {
		output.append("NOT NULL ");
	}
	if (column.unique) {
		output.append("UNIQUE ");
	}
	if (column.defaultValue.size() != 0) {
		output.append("DEFAULT (");
		output.append(column.defaultValue);
		output.append(") ");
	}
}

void buildSql(std::string& output, const TableDefinition& definition, int currentVersion, int targetVersion) {
	if (targetVersion < definition.version) {
		return;
	}
	std::vector<ColumnDefinition> filteredColumns;
	std::vector<ForeignKeyDefinition> filteredFKs;
	for (auto& column : definition.columns) {
		if (column.version>currentVersion && column.version <= targetVersion) {
			filteredColumns.push_back(column);
		}
	}
	for (auto& fk : definition.foreignKeys) {
		if (fk.version>currentVersion && fk.version <= targetVersion) {
			filteredFKs.push_back(fk);
		}
	}
	bool isCreate = currentVersion < definition.version;

	if (isCreate) {
		output.append("CREATE TABLE ");
		output.append(definition.name);
		output.append("(\n");

		bool firstCol = true;
		// columns
		for (auto& column : filteredColumns) {
			buildSql(output, column, firstCol);
			firstCol = false;
		}
		// primary key
		std::vector<std::string> pk;
		for (auto& col : filteredColumns) {
			if (col.primaryKey) {
				pk.push_back(col.name);
			}
		}
		if (pk.size() != 0) {
			output.append(",\n\tPRIMARY KEY (");
			firstCol = true;
			for (auto& pkCol : pk) {
				if (firstCol) {
					firstCol = false;
				}
				else {
					output.append(", ");
				}
				output.append(pkCol);
			}
			output.append(")");
		}
		for (auto& fk : filteredFKs) {
			output.append(",\n\tFOREIGN KEY (");
			firstCol = true;
			for (auto& mapping : fk.localToForeignColumnMappings) {
				if (firstCol) {
					firstCol = false;
				}
				else {
					output.append(", ");
				}
				output.append(get<0>(mapping));
			}
			output.append(") REFERENCES ");
			output.append(fk.foreignTable);
			output.append(" (");
			firstCol = true;
			for (auto& mapping : fk.localToForeignColumnMappings) {
				if (firstCol) {
					firstCol = false;
				}
				else {
					output.append(", ");
				}
				output.append(get<1>(mapping));
			}

			output.append(") ON DELETE ");
			output.append(toString(fk.deleteBehavior));
		}

		output.append("\n);\n");
	}
	else if(filteredColumns.size() != 0 || filteredFKs.size() != 0) {
		

		// columns
		for (auto& column : filteredColumns) {
			output.append("ALTER TABLE ");
			output.append(definition.name);
			output.append(" ADD ");
			buildSql(output, column, true);
			output.append(";\n");
		}
		
		for (auto& fk : filteredFKs) {
			output.append("ALTER TABLE ");
			output.append(definition.name);
			output.append(" ADD FOREIGN KEY (");
			
			bool firstCol = true;
			for (auto& mapping : fk.localToForeignColumnMappings) {
				if (firstCol) {
					firstCol = false;
				}
				else {
					output.append(", ");
				}
				output.append(get<0>(mapping));
			}
			output.append(") REFERENCES ");
			output.append(fk.foreignTable);
			output.append(" (");
			firstCol = true;
			for (auto& mapping : fk.localToForeignColumnMappings) {
				if (firstCol) {
					firstCol = false;
				}
				else {
					output.append(", ");
				}
				output.append(get<1>(mapping));
			}

			output.append(") ON DELETE ");
			output.append(toString(fk.deleteBehavior));
			output.append(";\n");
		}

	}
}
void buildSql(std::string& output, const IndexDefinition& definition, int currentVersion, int targetVersion) {
	if (targetVersion < definition.version) {
		return;
	}
	if (currentVersion >= definition.version) {
		return;
	}

	output.append("CREATE ");
	if (definition.unique) {
		output.append("UNIQUE ");
	}
	output.append("INDEX ");
	output.append(definition.name);
	output.append(" ON ");
	output.append(definition.tableName);
	output.append(" (");
	bool isFirstCol = true;
	for (auto& col : definition.columns) {
		if (isFirstCol) {
			isFirstCol = false;
		}
		else {
			output.append(", ");
		}
		output.append(get<string>(col));
		auto dir = get<IndexDirection>(col);
		if (dir == IndexDirection::Ascending) {
			output.append(" ASC");
		}
		else {
			output.append("DESC");
		}
	}
	output.append(");\n");
}
void LocalDB::SchemaDefinition::buildSql(std::string& tableDefinitions, std::string& indicesDefinitions, const SchemaDefinition & definition, int currentVersion, int targetVersion)
{
	if (targetVersion == -1) {
		targetVersion = maxVersion(definition);
	}
	for (auto& table : definition.tables) {
		::buildSql(tableDefinitions, table, currentVersion, targetVersion);
		
	}
	for (auto& index : definition.indices) {
		::buildSql(indicesDefinitions, index, currentVersion, targetVersion);
	}
}


std::string sqliteToCppType(SqliteType t) {
	switch (t)
	{
	case LocalDB::SchemaDefinition::SqliteType::Text:
		return "std::wstring";
	case LocalDB::SchemaDefinition::SqliteType::Int64:
		return "std::int64_t";
	case LocalDB::SchemaDefinition::SqliteType::Double:
		return "double";
	case LocalDB::SchemaDefinition::SqliteType::Blob:
		return "std::vector<unsigned char>";
	default:
		break;
	}
	return "";
}

std::string sqliteToColumnFunction(SqliteType t) {
	switch (t)
	{
	case LocalDB::SchemaDefinition::SqliteType::Text:
		return "LocalDB::sqlite3_column_wstring";
	case LocalDB::SchemaDefinition::SqliteType::Int64:
		return "sqlite3_column_int64";
	case LocalDB::SchemaDefinition::SqliteType::Double:
		return "sqlite3_column_double";
	case LocalDB::SchemaDefinition::SqliteType::Blob:
		return "sqlite3_column_int64";
	default:
		break;
	}
	return "";
}

std::string sqliteToBindFunction(SqliteType t) {
	switch (t)
	{
	case LocalDB::SchemaDefinition::SqliteType::Text:
		return "LocalDB::sqlite3_bind_string";
	case LocalDB::SchemaDefinition::SqliteType::Int64:
		return "sqlite3_bind_int64";
	case LocalDB::SchemaDefinition::SqliteType::Double:
		return "sqlite3_bind_double";
	case LocalDB::SchemaDefinition::SqliteType::Blob:
		return "sqlite3_bind_int64";
	default:
		break;
	}
	return "";
}
template <typename Tit, typename TWrite, typename TPred>
void stringJoin(ostream& stream, Tit rangeBegin, Tit rangeEnd, const std::string& separator, TWrite&& writeCallback, TPred&& predicate) {
	bool first = true;
	for (auto it = rangeBegin; it != rangeEnd; ++it) {
		if (predicate(*it)) {
			if (first) {
				first = false;
			}
			else {
				stream << separator;
			}
			writeCallback(stream, *it);
		}
	}
}
template <typename Tit, typename TWrite>
void stringJoin(ostream& stream, Tit rangeBegin, Tit rangeEnd, const std::string& separator, TWrite&& writeCallback) {
	return stringJoin(stream, rangeBegin, rangeEnd, separator, writeCallback, [](const auto&) {return true; });
}


std::string LocalDB::SchemaDefinition::generateEntitiesClasses(const SchemaDefinition & definition, const std::vector<std::string>& nameSpace)
{
	std::stringstream result;
	result << "#pragma once" << std::endl;
	result << "#include <string>" << std::endl;
	result << "#include <vector>" << std::endl;
	result << "#include <cstdint>" << std::endl;
	result << "#include <sqlite3.h>" << std::endl;
	result << "#include <LocalDB/DBQuery.h>" << std::endl;
	result << "#include <LocalDB/Sqlite3Extensions.h>" << std::endl;
	for (auto& ns : nameSpace) {
		result << "namespace " << ns << "{" << std::endl;
	}
	for (auto& table : definition.tables) {
		result << "struct " << table.name << "{" << std::endl;

		for (auto& column : table.columns) {
			result << sqliteToCppType(column.type) << " " << column.name << ";" << std::endl;
		}

		result << "static std::string getOrderedColumnsForSelect() {" << std::endl;
		result << "return \"";

		stringJoin(result, table.columns.begin(), table.columns.end(), ", ", [](auto& stream, const auto& col) {stream << col.name; });

		result << "\";" << std::endl;
		result << "}" << std::endl;

		result << "static " << table.name << " createFromSqlRecord(sqlite3_stmt* stmt) {" << std::endl;
		result << table.name << " data;" << std::endl;
		int index = 0;
		for (auto& column : table.columns) {
			result << "data." << column.name << " = " << sqliteToColumnFunction(column.type) << "(stmt, " << (index++) << ");" << std::endl;
		}
		result << "return data;" << std::endl;
		result << "}" << std::endl;

		result << "};" << std::endl;

		// insert command
		result << "class " << table.name << "InsertDbQuery : public LocalDB::NoResultDBQuery{" << std::endl;
		result << "private:" << std::endl;
		result << table.name << " _entity;" << std::endl;
		result << "protected:" << std::endl;
		result << "virtual std::string identifier() override { return \"gen-" << table.name << "-insert\";}" << std::endl;
		result << "virtual std::string sql(int ) override {" << std::endl;

		result << "return \"insert into " << table.name << "(";
		stringJoin(result, table.columns.begin(), table.columns.end(), ", ", [](auto& stream, const auto& col) {stream << col.name; });
		result << ") values (";
		stringJoin(result, table.columns.begin(), table.columns.end(), ", ", [](auto& stream, const auto& col) {stream << "@" << col.name; });
		result << ")\";" << std::endl;
		result << "}" << std::endl;

		result << "virtual  void bindParameters(int, sqlite3* , sqlite3_stmt* statement) override {" << std::endl;
		stringJoin(result, table.columns.begin(), table.columns.end(), "", [](auto& stream, const auto& col) {
			stream << sqliteToBindFunction(col.type) << "(statement, sqlite3_bind_parameter_index(statement, \"@" << col.name << "\"), _entity." << col.name << ");" << std::endl;
		});
		result << "}" << std::endl;
		result << "public:" << std::endl;
		result << table.name << "InsertDbQuery(const LocalDB::DBContext& ctx, const " << table.name << "& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity){}" << std::endl;
		result << "};" << std::endl;


		// insert or replace command
		result << "class " << table.name << "InsertOrReplaceDbQuery : public LocalDB::NoResultDBQuery{" << std::endl;
		result << "private:" << std::endl;
		result << table.name << " _entity;" << std::endl;
		result << "protected:" << std::endl;
		result << "virtual std::string identifier() override { return \"gen-" << table.name << "-insertorreplace\";}" << std::endl;
		result << "virtual std::string sql(int ) override {" << std::endl;

		result << "return \"insert or replace into " << table.name << "(";
		stringJoin(result, table.columns.begin(), table.columns.end(), ", ", [](auto& stream, const auto& col) {stream << col.name; });
		result << ") values (";
		stringJoin(result, table.columns.begin(), table.columns.end(), ", ", [](auto& stream, const auto& col) {stream << "@" << col.name; });
		result << ")\";" << std::endl;
		result << "}" << std::endl;

		result << "virtual  void bindParameters(int, sqlite3* , sqlite3_stmt* statement) override {" << std::endl;
		stringJoin(result, table.columns.begin(), table.columns.end(), "", [](auto& stream, const auto& col) {
			stream << sqliteToBindFunction(col.type) << "(statement, sqlite3_bind_parameter_index(statement, \"@" << col.name << "\"), _entity." << col.name << ");" << std::endl;
		});
		result << "}" << std::endl;
		result << "public:" << std::endl;
		result << table.name << "InsertOrReplaceDbQuery(const LocalDB::DBContext& ctx, const " << table.name << "& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity){}" << std::endl;
		result << "};" << std::endl;

		// update query

		result << "class " << table.name << "UpdateDbQuery : public LocalDB::NoResultDBQuery{" << std::endl;
		result << "private:" << std::endl;
		result << table.name << " _entity;" << std::endl;
		result << "protected:" << std::endl;
		result << "virtual std::string identifier() override { return \"gen-" << table.name << "-update\";}" << std::endl;
		result << "virtual std::string sql(int ) override {" << std::endl;

		result << "return \"update " << table.name << " SET ";
		stringJoin(result, table.columns.begin(), table.columns.end(), ", ", [](auto& stream, const auto& col) {stream << col.name << " = @" << col.name; }, [](auto& col) {return !col.primaryKey; });
		result << " WHERE ";
		stringJoin(result, table.columns.begin(), table.columns.end(), " AND ", [](auto& stream, const auto& col) {stream << col.name << " = @" << col.name; }, [](auto& col) {return col.primaryKey; });
		result << "\";" << std::endl;
		result << "}" << std::endl;

		result << "virtual  void bindParameters(int, sqlite3* , sqlite3_stmt* statement) override {" << std::endl;
		stringJoin(result, table.columns.begin(), table.columns.end(), "", [](auto& stream, const auto& col) {
			stream << sqliteToBindFunction(col.type) << "(statement, sqlite3_bind_parameter_index(statement, \"@" << col.name << "\"), _entity." << col.name << ");" << std::endl;
		});
		result << "}" << std::endl;
		result << "public:" << std::endl;
		result << table.name << "UpdateDbQuery(const LocalDB::DBContext& ctx, const " << table.name << "& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity){}" << std::endl;
		result << "};" << std::endl;

	}
	for (auto ix = 0ULL; ix < nameSpace.size(); ++ix) {
		result << "}" << std::endl;
	}
	return result.str();
}

