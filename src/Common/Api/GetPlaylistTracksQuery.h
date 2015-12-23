#pragma once
#include "TracksQueryBase.h"
#include "TrackInfo.h"
#include "PaginatedList.h"
namespace api {
	class GetPlaylistTracksQuery : public GetTracksQueryBase {
		std::wstring _playlistId;
	public:
		GetPlaylistTracksQuery(const std::wstring& id, int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode)
			: GetTracksQueryBase(limit, offset, sessionId, countryCode), _playlistId(id) {}
		GetPlaylistTracksQuery(const std::wstring& id, int limit, int offset, Platform::String^ countryCode)
			: GetTracksQueryBase(limit, offset, countryCode), _playlistId(id) {}

		// Inherited via QueryBase
		virtual std::wstring url() const override {
			std::wstring result = L"playlists/";
			result.append(_playlistId);
			result.append(L"/tracks");
			return result;
		}

	};
}