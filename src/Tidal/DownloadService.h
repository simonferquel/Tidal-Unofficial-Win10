#pragma once
#include <localdata/Entities.h>
class DownloadService {
private:
	DownloadService();
public:
	concurrency::task<void> StartDownloadAlbumAsync(std::int64_t id);
	concurrency::task<void> StartDownloadPlaylistAsync(const std::wstring& id);
	concurrency::task<void> StartDownloadTracksAsync(const std::vector<std::int64_t>& id);
	concurrency::task<std::shared_ptr<std::vector<localdata::track_import_job>>> getPendingJobsAsync();
	concurrency::task<std::shared_ptr<std::vector<localdata::imported_album>>> getImportedAlbumsAsync();
	concurrency::task<std::shared_ptr<std::vector<localdata::imported_playlist>>> getImportedPlaylistsAsync();
	concurrency::task<std::shared_ptr<std::vector<localdata::imported_track>>> getImportedTracksAsync();
	friend DownloadService& getDownloadService();
};