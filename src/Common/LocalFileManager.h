#pragma once
#include <ppltasks.h>

enum class DeleteFileStatus {
	Deleted,
	FileNotFound,
	FileAccessProblem
};

class LocalFileManager : public std::enable_shared_from_this<LocalFileManager>
{
private:
	Windows::Storage::StorageFolder^ _fileFolder;
	LocalFileManager(Windows::Storage::StorageFolder^ fileFolder) : _fileFolder(fileFolder) {}

public:
	static concurrency::task<std::shared_ptr<LocalFileManager>> MakeLocalFileManager(bool forImportFolder);
	concurrency::task<DeleteFileStatus> TryDeleteTrackFile(std::int64_t id);
};

