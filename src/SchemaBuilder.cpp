#include "pch.h"
#include "SchemaBuilder.h"
#undef max
#include <algorithm>
#include <exception>
#include <tuple>
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
