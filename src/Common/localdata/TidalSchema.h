#pragma once
#include <localdb/SchemaBase.h>
#include <localdb/SchemaBuilder.h>
namespace localdata{
	class TidalSchema : public LocalDB::SchemaBase {
	protected:
		// Inherited via SchemaBase
		virtual void BuildSchema(LocalDB::SchemaDefinition::SchemaBuilder & builder) override;
	public:
		TidalSchema(const LocalDB::DBContext& dbContext) : LocalDB::SchemaBase(dbContext) {}
	};

}