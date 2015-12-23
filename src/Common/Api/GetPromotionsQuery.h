#pragma once
#include "QueryBase.h"
#include "PaginatedList.h"
namespace api {
	struct PromotionArticle;
	class GetPromotionsQuery : public QueryBase {
	public:
		GetPromotionsQuery(int limit, int offset, Platform::String^ group, Platform::String^ subscribtionType, std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);
		GetPromotionsQuery(int limit, int offset, Platform::String^ group, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override;
		concurrency::task<std::shared_ptr<PaginatedList<PromotionArticle>>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}