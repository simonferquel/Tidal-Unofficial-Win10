#pragma once
#include "QueryBase.h"
#include "PlaylistResume.h"
#include <vector>
namespace api {
	class CreatePlaylistQuery : public QueryBase {
	private:

		std::int64_t _userId;
	public:
		CreatePlaylistQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, Platform::String^ title, Platform::String^ description);


		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PlaylistResume>> executeAsync(concurrency::cancellation_token cancelToken);
	};

	class AddTracksToPlaylistQuery : public QueryBase {
	private:
		std::wstring _uuid;
	public:
		AddTracksToPlaylistQuery(const std::wstring& playlistUuid, Platform::String^ etag, Platform::String^ sessionId, Platform::String^ countryCode, const std::vector<std::int64_t>& trackIds, int toIndex = 0);

		virtual std::wstring url() const override;
		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);
	};
}