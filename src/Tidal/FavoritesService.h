#pragma once
#include <mutex>
#include <set>
#include "ArtistItemVM.h"
#include "AlbumResumeItemVM.h"
#include "TrackItemVM.h"
#include "PlaylistResumeItemVM.h"
class FavoritesService {
private:
	std::recursive_mutex _mutex;
	std::set<std::int64_t> _albumIds;
	std::set<std::int64_t> _artistIds;
	std::set<std::int64_t> _trackIds;
	std::set<std::wstring> _playlistIds;
	Platform::Collections::Vector<Tidal::AlbumResumeItemVM^>^ _albums;
	Platform::Collections::Vector<Tidal::ArtistItemVM^>^ _artists;
	Platform::Collections::Vector<Tidal::PlaylistResumeItemVM^>^ _playlists;
	Platform::Collections::Vector<Tidal::PlaylistResumeItemVM^>^ _myPlaylists;
	Platform::Collections::Vector<Tidal::TrackItemVM^>^ _tracks;
	FavoritesService() {
		_albums = ref new Platform::Collections::Vector<Tidal::AlbumResumeItemVM^>();
		_artists = ref new Platform::Collections::Vector<Tidal::ArtistItemVM^>();
		_playlists = ref new Platform::Collections::Vector<Tidal::PlaylistResumeItemVM^>();
		_myPlaylists = ref new Platform::Collections::Vector<Tidal::PlaylistResumeItemVM^>();
		_tracks = ref new Platform::Collections::Vector<Tidal::TrackItemVM^>();
	}
	concurrency::task<void> doRefreshAsync(concurrency::cancellation_token cancelToken);
public:
	friend FavoritesService& getFavoriteService();

	concurrency::task<void> refreshAsync(concurrency::cancellation_token cancelToken);
	Platform::Collections::Vector<Tidal::AlbumResumeItemVM^>^ Albums()const { return _albums; }
	Platform::Collections::Vector<Tidal::ArtistItemVM^>^ Artists()const { return _artists; }
	Platform::Collections::Vector<Tidal::PlaylistResumeItemVM^>^ Playlists()const { return _playlists; }
	Platform::Collections::Vector<Tidal::PlaylistResumeItemVM^>^ MyPlaylists()const { return _myPlaylists; }
	Platform::Collections::Vector<Tidal::TrackItemVM^>^ Tracks()const { return _tracks; }

	bool isAlbumFavorite(std::int64_t id);
	bool isArtistFavorite(std::int64_t id);
	bool isTrackFavorite(std::int64_t id);
	bool isPlaylistFavorite(const std::wstring& id);


	concurrency::task<void> addPlaylistAsync(const std::wstring& id);
	concurrency::task<void> removePlaylistAsync(const std::wstring& id);



	concurrency::task<void> addAlbumAsync(std::int64_t id);
	concurrency::task<void> removeAlbumAsync(std::int64_t id);

	concurrency::task<void> addArtistAsync(std::int64_t id);
	concurrency::task<void> removeArtistAsync(std::int64_t id);

	concurrency::task<void> addTrackAsync(std::int64_t id);
	concurrency::task<void> removeTrackAsync(std::int64_t id);

};