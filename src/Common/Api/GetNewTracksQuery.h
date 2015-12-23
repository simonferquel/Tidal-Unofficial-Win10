#pragma once
#include "TracksQueryBase.h"
#include "TrackInfo.h"
#include "PaginatedList.h"
namespace api {
	class GetNewTracksQuery : public GetTracksQueryBase {
	private:
		Platform::String^ _list = L"featured";
		Platform::String^ _group = L"new";
	public:
		GetNewTracksQuery(int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode)
		: GetTracksQueryBase(limit, offset, sessionId, countryCode){}
		GetNewTracksQuery(int limit, int offset, Platform::String^ countryCode)
		: GetTracksQueryBase(limit, offset, countryCode){}
		void withCustomListGroup(Platform::String^ list, Platform::String^ group) {
			_list = list;
			_group = group;
		}
		// Inherited via QueryBase
		virtual std::wstring url() const override {
			std::wstring result = _list->Data();
			result.push_back(L'/');
			result.append(_group->Data());
			result.append(L"/tracks");
			return result;
		}

	};
}