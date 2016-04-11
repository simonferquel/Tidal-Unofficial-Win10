#include "pch.h"
#include "LocalFileManager.h"
#include <experimental/coroutine>
#include <pplawait.h>
using namespace Platform;
using namespace Windows::Storage;
concurrency::task<DeleteFileStatus> LocalFileManager::TryDeleteTrackFile(std::int64_t id)
{
	auto fileItem = co_await _fileFolder->TryGetItemAsync(id.ToString());
	auto file = dynamic_cast<StorageFile^>(fileItem);
	if (file == nullptr) {
		co_return DeleteFileStatus::FileNotFound;
	}
	try {
		co_await file->DeleteAsync();
		co_return DeleteFileStatus::Deleted;
	}
	catch (Platform::AccessDeniedException^ ex) {
		co_return DeleteFileStatus::FileAccessProblem;
	}
}
concurrency::task<std::shared_ptr<LocalFileManager>> LocalFileManager::MakeLocalFileManager(bool forImportFolder)
{
	if (forImportFolder) {
		auto folder = co_await Windows::Storage::ApplicationData::Current->LocalFolder->CreateFolderAsync(L"imports", CreationCollisionOption::OpenIfExists);
		co_return std::shared_ptr<LocalFileManager>(new LocalFileManager(folder));
	}
	else {
		auto folder = co_await Windows::Storage::ApplicationData::Current->LocalFolder->CreateFolderAsync(L"track_cache", CreationCollisionOption::OpenIfExists);
		co_return std::shared_ptr<LocalFileManager>(new LocalFileManager(folder));
	}
}
