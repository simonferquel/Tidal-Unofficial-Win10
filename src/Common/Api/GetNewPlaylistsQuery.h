#pragma once
#include "QueryBase.h"
#include "PlaylistResume.h"
#include "PaginatedList.h"
namespace api {
	class GetNewPlaylistsQuery : public QueryBase {
	private:
		Platform::String^ _list = L"featured";
		Platform::String^ _group = L"new";
	public:
		GetNewPlaylistsQuery(int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode);
		GetNewPlaylistsQuery(int limit, int offset, Platform::String^ countryCode);
		void withCustomListGroup(Platform::String^ list, Platform::String^ group) {
			_list = list;
			_group = group;
		}
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<PlaylistResume>>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}