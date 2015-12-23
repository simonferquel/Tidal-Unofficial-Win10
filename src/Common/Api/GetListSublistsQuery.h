#pragma once
#include "QueryBase.h"
#include "SublistInfo.h"
namespace api {
	class GetListSublistsQuery : QueryBase {
	private:
		Platform::String^ _list;
	public:
		GetListSublistsQuery(Platform::String^ list, Platform::String^ countryCode) :
			_list(list) {
			addQueryStringParameter(L"countryCode", countryCode);
		}
		GetListSublistsQuery(Platform::String^ list, Platform::String^ sessionId, Platform::String^ countryCode):
			QueryBase(sessionId, countryCode),
			_list(list)
		{}

		// Inherited via QueryBase
		virtual std::wstring url() const override;
		concurrency::task<std::shared_ptr<std::vector<api::SublistInfo>>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}