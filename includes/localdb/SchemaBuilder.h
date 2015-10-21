#pragma once
#include <vector>
#include <string>
namespace LocalDB {
	namespace SchemaDefinition {
		// SQLite column types
		enum class SqliteType {
			Text,
			Int64,
			Double,
			Blob
		};

		// Delete behavior for a foreign key definition
		enum class ForeignKeyDeleteBehavior {
			NoAction,
			SetNull,
			SetDefault,
			Cascade
		};

		// direction of an index definition
		enum class IndexDirection {
			Ascending, 
			Descending
		};
		struct SchemaDefinition;
		struct TableDefinition;
		struct ColumnDefinition;
		struct IndexDefinition;
		struct ForeignKeyDefinition;

		// schema structure
		struct SchemaDefinition {
			// tables of the schema
			std::vector<TableDefinition> tables;
			// indices of the schema
			std::vector<IndexDefinition> indices;
		};
		// definition of a table
		struct TableDefinition {
			// name of the table
			std::string name;
			// version of the schema in which the table appeared
			int version;
			// columns of the table
			std::vector<ColumnDefinition> columns;
			// foreign keys of the table
			std::vector<ForeignKeyDefinition> foreignKeys;
		};
		// difinition of a foreign key
		struct ForeignKeyDefinition {
			// version of the schema in which the FK appeared
			int version;
			// parent table
			std::string foreignTable;
			// delete behavior of the foreign key
			ForeignKeyDeleteBehavior deleteBehavior = ForeignKeyDeleteBehavior::NoAction;
			// column mapping (child=>parent)
			std::vector<std::tuple<std::string, std::string>> localToForeignColumnMappings;
		};
		// definition of a table column
		struct ColumnDefinition {
			// name of the column
			std::string name;

			// version of the schema in which the column appeared
			int version;
			// type of the column
			SqliteType type;
			// is member of the primary key
			bool primaryKey = false;
			// is unique
			bool unique = false;
			// is nullable
			bool nullable = true;
			// default value (sql literral)
			std::string defaultValue;
		};

		// definition of an index
		struct IndexDefinition {
			// name of the index
			std::string name;
			// version of the schema in which the index appeared
			int version;
			// name of the table on which the index is created
			std::string tableName;
			// uniqueness of the index
			bool unique = false;
			// columns (and directions) indexed
			std::vector<std::tuple<std::string, IndexDirection>> columns;
		};

		class SchemaBuilder;
		class TableBuilder;
		class ColumnBuilder;
		class IndexBuilder;
		class ForeignKeyBuilder;

		// helper to build a schema definition in a fluent manner
		class SchemaBuilder {
		private:
			SchemaDefinition _built;
		public:
			SchemaDefinition& definition()  {
				return _built;
			}
			// create a table defintion
			// name : name of the table
			// version : version of the schema in which the table appears
			TableBuilder table(const char* name, int schemaVersion = 1);
			// create an index defintion
			// name : name of the index
			// tableName : name of the table
			// version : version of the schema in which the index appears
			IndexBuilder index(const char* name, const char* tableName, int schemaVersion = 1);

		};

		// helper to build a table definition
		class TableBuilder {
			TableDefinition _built;
			SchemaBuilder& _parent;
		public:
			TableDefinition& definition() {
				return _built;
			}
			TableBuilder(SchemaBuilder& parent, const char* name, int schemaVersion) : _parent(parent) {
				_built.name = name;
				_built.version = schemaVersion == -1 ? 1 : schemaVersion;
			}
			// end the table definition and put it in the owning schema definition (returns the owning SchemaBuilder)
			SchemaBuilder& endTable() {
				_parent.definition().tables.push_back(std::move(_built));
				return _parent;
			}
			// add a column to the table
			// name : name of the column
			// type : type of the column
			// schemaVersion : -1 = same as the owning table, else version in which the column appears
			ColumnBuilder column(const char* name, SqliteType type, int schemaVersion = -1);

			// add a foreign key to the table
			// otherTableName : name of the parent table
			// schemaVersion : -1 = same as the owning table, else version in which the FK appears
			ForeignKeyBuilder foreignKey(const char* otherTableName, int schemaVersion = -1);
		};

		// helper to build a column definition
		class ColumnBuilder {
			ColumnDefinition _built;
			TableBuilder& _parent;
		public:
			ColumnBuilder(TableBuilder& parent, const char* name, SqliteType type, int schemaVersion) : _parent(parent) {
				_built.name = name;
				_built.type = type;
				_built.version = schemaVersion == -1 ? parent.definition().version : schemaVersion;
			}
			// end the column definition and put it in the owning table definition (returns the parent TableBuilder)
			TableBuilder& endColumn() {
				_parent.definition().columns.push_back(std::move(_built));
				return _parent;
			}
			// nullable flag (default is true)
			ColumnBuilder& nullable(bool value) {
				_built.nullable = value;
				return *this;
			}
			// is member of the primary key (default is false)
			ColumnBuilder& isPrimaryKey(bool value) {

				_built.primaryKey = value;
				return *this;
			}
			// is unique (default is false)
			ColumnBuilder& unique(bool value) {

				_built.unique = value;
				return *this;
			}
			// default value (sql litteral - can be an sql function call)
			ColumnBuilder& defaultValue(const char* litteralValue) {
				_built.defaultValue = litteralValue;
				return *this;
			}
			
		};

		// helper to build a FK definition
		class ForeignKeyBuilder {
			ForeignKeyDefinition _built;
			TableBuilder& _parent;
		public:
			ForeignKeyBuilder(TableBuilder& parent, const char* otherTableName, int schemaVersion) : _parent(parent) {
				_built.foreignTable = otherTableName;
				_built.version = schemaVersion == -1 ? parent.definition().version : schemaVersion;
			}
			// end the foreign key definition and put it in the owning table (returns the owning table definition)
			TableBuilder& endForeignKey() {
				_parent.definition().foreignKeys.push_back(std::move(_built));
				return _parent;
			}
			// specify delete behavior
			ForeignKeyBuilder& onDelete(ForeignKeyDeleteBehavior behavior) {
				_built.deleteBehavior = behavior;
				return *this;
			}
			// map a column to the parent table column
			ForeignKeyBuilder& mapColumn(const char* localColumn, const char* otherTableColumn) {
				_built.localToForeignColumnMappings.emplace_back(localColumn, otherTableColumn);
				return *this;
			}

		};

		// helper to build an index definition
		class IndexBuilder {
			IndexDefinition _built;
			SchemaBuilder& _parent;
		public:
			IndexBuilder(SchemaBuilder& parent, const char* name, const char* tableName, int schemaVersion) : _parent(parent) {
				_built.name = name;
				_built.tableName = tableName;
				_built.version = schemaVersion == -1 ? 1 : schemaVersion;
			}
			// end the index definition and put it in the owning schema definition
			SchemaBuilder& endIndex() {
				_parent.definition().indices.push_back(std::move(_built));
				return _parent;
			}
			// unique flag (default is false)
			IndexBuilder& unique(bool value ) {
				_built.unique = value;
				return *this;
			}
			// add a column to the index
			IndexBuilder& addColumn(const char* column, IndexDirection direction = IndexDirection::Ascending) {
				_built.columns.emplace_back(column, direction);
				return *this;
			}
		};



		inline TableBuilder SchemaBuilder::table(const char * name, int schemaVersion) {
			return TableBuilder(*this, name, schemaVersion);
		}

		inline IndexBuilder SchemaBuilder::index(const char * name, const char * tableName, int schemaVersion) {
			return IndexBuilder(*this, name, tableName, schemaVersion);
		}

		inline ColumnBuilder TableBuilder::column(const char * name, SqliteType type, int schemaVersion) {
			return ColumnBuilder(*this, name, type, schemaVersion);
		}

		inline ForeignKeyBuilder TableBuilder::foreignKey(const char * otherTableName, int schemaVersion)
		{
			return ForeignKeyBuilder(*this, otherTableName, schemaVersion);
		}

		// returns the maximum version number of the SchemaDefinition (used to know if the database schema must be updated)
		int maxVersion(const SchemaDefinition& definition);

		// builds sql statement for creating tables and indices for the given schema definition or updating the schema
		void buildSql(std::string& tableDefinitions, std::string& indicesDefinitions, const SchemaDefinition& definition, int currentVersion, int targetVersion = -1);
		std::string generateEntitiesClasses(const SchemaDefinition& definition, const std::vector<std::string>& nameSpace);
	}
}