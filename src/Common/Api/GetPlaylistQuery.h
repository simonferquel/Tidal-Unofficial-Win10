#pragma once
#include "QueryBase.h"
#include "PlaylistResume.h"
namespace api {
	class GetPlaylistQuery : public QueryBase {
	private:
		std::wstring _playlistId;
	public:
		GetPlaylistQuery(const std::wstring& playlistId, Platform::String^ sessionId, Platform::String^ countryCode);
		GetPlaylistQuery(const std::wstring& playlistId, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PlaylistResume>> executeAsync(concurrency::cancellation_token cancelToken);
	};
}