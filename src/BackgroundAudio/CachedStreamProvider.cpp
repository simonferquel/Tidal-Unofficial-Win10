#include "pch.h"
#include "CachedStreamProvider.h"
#include <SoundQuality.h>
#include <CachedTrackManager.h>
#include <localdata/db.h>
#include <Api/GetTrackStreamUrlQuery.h>
#include <tools/StringUtils.h>
#include <Api/UrlInfo.h>
#include <memory>
#include <mutex>
#include <tools/TraceUtils.h>
#include <localdata/GetCachedTrackInfoQuery.h>
#include <tools/AsyncHelpers.h>
#include <tools/TimeUtils.h>
#include <Api/ApiErrors.h>
#include "WebStream.h"
#include <ObfuscateStream.h>
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;

class HttpCacheStateMachine : public std::enable_shared_from_this<HttpCacheStateMachine> {
private:
	enum class StreamState {
		Idle,
		Reading,
		Writing
	};
	Concurrency::task<void> _downloadTask;
	std::mutex _mutex;
	StreamState _streamState = StreamState::Idle;
	localdata::cached_track _track;
	std::int64_t _trackId;
	SoundQuality _soundQuality;
	IRandomAccessStream^ _fileStream;
	WebStream^ _webStream;
	Concurrency::cancellation_token_source _cts;
	std::vector<std::tuple<concurrency::task_completion_event<void>, unsigned long long>> _pendingReaders;
	std::vector<concurrency::task_completion_event<void>> _pendingWriters;
	struct Ownership {
		std::weak_ptr<HttpCacheStateMachine> _stateMachine;
		Ownership(const std::shared_ptr<HttpCacheStateMachine>& machine) : _stateMachine(machine) {}
		~Ownership() {
			auto sm = _stateMachine.lock();
			if (!sm) {
				return;
			}
			std::lock_guard<std::mutex> lg(sm->_mutex);
			auto readerIt = std::find_if(sm->_pendingReaders.begin(), sm->_pendingReaders.end(), [sm](auto& item) {
				return std::get<unsigned long long>(item) <= sm->_track.local_size;
			});
			if (readerIt != sm->_pendingReaders.end()) {
				sm->_streamState = StreamState::Reading;
				std::get<concurrency::task_completion_event<void>>(*readerIt).set();

				sm->_pendingReaders.erase(readerIt);
				return;
			}
			if (sm->_pendingWriters.size() != 0) {

				sm->_streamState = StreamState::Writing;
				sm->_pendingWriters[0].set();
				sm->_pendingWriters.erase(sm->_pendingWriters.begin());
				return;
			}
			sm->_streamState = StreamState::Idle;

		}
	};
	concurrency::task<std::shared_ptr<Ownership>> TakeWriteOwnershipAsync() {
		concurrency::task_completion_event<void> tce;
		{
			std::lock_guard<std::mutex> lg(_mutex);
			if (_streamState == StreamState::Idle) {
				_streamState = StreamState::Writing;
				tce.set();
			}
			else {
				_pendingWriters.push_back(tce);
			}
		}
		std::weak_ptr<HttpCacheStateMachine> weakThis = shared_from_this();
		await concurrency::create_task(tce);
		auto that = weakThis.lock();
		if (!that) {
			concurrency::cancel_current_task();
		}

		return std::make_shared<Ownership>(that);

	}

	concurrency::task<std::shared_ptr<Ownership>> TakeReadOwnershipAsync(unsigned long long requiredLocalSize) {
		concurrency::task_completion_event<void> tce;
		{
			std::lock_guard<std::mutex> lg(_mutex);
			if (_streamState == StreamState::Idle && _track.local_size >= requiredLocalSize) {
				_streamState = StreamState::Reading;
				tce.set();
			}
			else {
				_pendingReaders.push_back(std::make_tuple(tce, requiredLocalSize));
			}
		}
		std::weak_ptr<HttpCacheStateMachine> weakThis = shared_from_this();
		await concurrency::create_task(tce);
		auto that = weakThis.lock();
		if (!that) {
			concurrency::cancel_current_task();
		}

		return std::make_shared<Ownership>(that);

	}
	concurrency::task<void> continueDownloadingAsync(std::weak_ptr<HttpCacheStateMachine> weakThis) {
		auto trackId = _trackId;
		auto soundQuality = _soundQuality;

		auto track = _track;
		while (track.local_size != track.server_size) {
			try {
				auto that = weakThis.lock();
				if (!that) {
					await concurrency::task_from_result();
					return;
				}
				auto cancelToken = _cts.get_token();
				if (!_webStream) {
					await initializeAsync(cancelToken);
				}


				if (_webStream->Position != track.local_size) {
					_webStream->Seek(track.local_size);
				}
				auto buffer = ref new Buffer(128 * 1024);

				that = nullptr;
				IBuffer^ resultBuffer = nullptr;
				try {

					auto timeoutProvider = std::make_shared<tools::async::TimeoutCancelTokenProvider>(tools::time::ToWindowsTimeSpan(std::chrono::seconds(5)));

					resultBuffer = await concurrency::create_task(_webStream->ReadAsync(buffer, 128*1024, InputStreamOptions::ReadAhead), tools::async::combineCancelTokens(cancelToken, timeoutProvider->get_token()));
				}
				catch (concurrency::task_canceled&) {
					if (cancelToken.is_canceled()) {
						throw;
					}
					else {
						throw track_download_timeout();
					}
				}
				catch (...)
				{
					_webStream = nullptr;
					throw;
				}
				if (!resultBuffer) {
					continue;
				}
				that = weakThis.lock();
				if (!that) {
					return;
				}
				{
					auto ownershipTask = TakeWriteOwnershipAsync();
					that = nullptr;

					auto ownership = await ownershipTask;
					that = weakThis.lock();
					if (!that) {
						return;
					}
					if (_fileStream->Position != track.local_size) {
						_fileStream->Seek(track.local_size);
					}

					auto written = await concurrency::create_task(_fileStream->WriteAsync(resultBuffer));
					await concurrency::create_task(_fileStream->FlushAsync());
					track.local_size += written;
					_track = track;
					await cache::updateCachedTrackInfoAsync(localdata::getDb(), track, concurrency::cancellation_token::none());
				}

			}
			catch (concurrency::task_canceled&) {

			}



		}
	}

public:
	~HttpCacheStateMachine() {
		_cts.cancel();
	}
	unsigned long long size() { return _track.server_size; }

	HttpCacheStateMachine(std::int64_t trackId, SoundQuality soundQuality, IRandomAccessStream^ fileStream, const localdata::cached_track& track, concurrency::cancellation_token cancelToken) :
		_track(track), _trackId(trackId), _soundQuality(soundQuality), _fileStream(track.obuscated ==0 ? fileStream: ref new ObfuscateStream(fileStream, trackId))
	{
		if (cancelToken.is_cancelable()) {
			_cts = concurrency::cancellation_token_source::create_linked_source(cancelToken);
		}
	}

	concurrency::task<IBuffer^> ReadBlockAsync(IBuffer^ buffer, unsigned long long position, unsigned int count) {
		tools::debug_stream << L"begin read. Position : " << position << " count : " << count << std::endl;
		if (count + position > _track.server_size) {
			count = _track.server_size - position;
		}
		std::weak_ptr<HttpCacheStateMachine> weakThis = shared_from_this();
		auto ownership = await TakeReadOwnershipAsync(count + position);
		auto that = weakThis.lock();
		if (!that) {
			concurrency::cancel_current_task();
		}
		if (position != _fileStream->Position) {
			_fileStream->Seek(position);
		}
		auto result = await concurrency::create_task(_fileStream->ReadAsync(buffer, count, InputStreamOptions::None));
		tools::debug_stream << L"end read" << std::endl;
		return result;
	}
	concurrency::task<void> initializeAsync(concurrency::cancellation_token cancelToken) {
		auto that = shared_from_this();
		auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
		auto timeoutProvider = std::make_shared<tools::async::TimeoutCancelTokenProvider>(tools::time::ToWindowsTimeSpan(std::chrono::seconds(3)));


		api::GetTrackStreamUrlQuery q(dynamic_cast<Platform::String^>(settingsValues->Lookup(L"SessionId")), dynamic_cast<Platform::String^>(settingsValues->Lookup(L"CountryCode")), _track.id, toString(_soundQuality));
		try {
			auto urlInfo = await q.executeAsync(cancelToken);
			auto webStream = await WebStream::CreateWebStreamAsync(tools::strings::toWindowsString(urlInfo->url), tools::async::combineCancelTokens(cancelToken, timeoutProvider->get_token()));
			if (_track.server_size != webStream->Size || _track.quality != static_cast<std::uint32_t>(_soundQuality) || _track.server_timestamp != webStream->Modified.UniversalTime) {
				_track.server_size = webStream->Size;
				_track.local_size = 0;
				_track.server_timestamp = webStream->Modified.UniversalTime;
				_track.quality = static_cast<std::uint32_t>(_soundQuality);

			}
			_webStream = webStream;
		}
		catch (concurrency::task_canceled&) {
			if (cancelToken.is_canceled()) {
				throw;
			}
			else {
				throw track_download_timeout();
			}
		}
		catch (api::statuscode_exception& ex) {
			if (ex.getStatusCode() == Windows::Web::Http::HttpStatusCode::Unauthorized) {
				throw track_unavailable_for_streaming();
			}
			throw;
		}

	}
	void Start() {
		std::weak_ptr<HttpCacheStateMachine> weakThis(shared_from_this());
		_downloadTask =
			tools::async::retryWithDelays([weakThis]() {
			auto that = weakThis.lock();
			if (that) {
				return that->continueDownloadingAsync(weakThis);
			}
			else {
				return concurrency::task_from_result();
			}

		}, tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), _cts.get_token()).then([](concurrency::task<void>t) {
			try {
				t.get();
			}
			catch (concurrency::task_canceled&) {

			}
			catch (Platform::OperationCanceledException^) {}
		});

		/*_downloadTask = continueDownloadingAsync(weakThis).then([](concurrency::task<void>t) {
			try {
				t.get();
			}
			catch (concurrency::task_canceled&) {

			}
			catch (Platform::OperationCanceledException^) {}
		});*/

	}
};
ref class RandomAccessStreamOverHttpCacheStateMachine sealed : public IRandomAccessStream {
private:
	std::shared_ptr<HttpCacheStateMachine> _stateMachine;
	unsigned long long _position;
internal:
	RandomAccessStreamOverHttpCacheStateMachine(const std::shared_ptr<HttpCacheStateMachine>& sm) : _stateMachine(sm), _position(0) {

		OutputDebugString(L"RandomAccessStreamOverHttpCacheStateMachine()\n");
	}
public:
	virtual ~RandomAccessStreamOverHttpCacheStateMachine() {
		OutputDebugString(L"~RandomAccessStreamOverHttpCacheStateMachine()\n");
	}

	// Inherited via IRandomAccessStream
	virtual Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IBuffer ^, unsigned int> ^ ReadAsync(Windows::Storage::Streams::IBuffer ^buffer, unsigned int count, Windows::Storage::Streams::InputStreamOptions options)
	{
		auto weakThis = Platform::WeakReference(this);
		return concurrency::create_async([weakThis, buffer, count, pos = _position](concurrency::progress_reporter<unsigned int>) {
			auto that = weakThis.Resolve<RandomAccessStreamOverHttpCacheStateMachine>();
			if (that) {
				return that->_stateMachine->ReadBlockAsync(buffer, pos, count)
					.then([weakThis](IBuffer^ result) {
					auto that = weakThis.Resolve<RandomAccessStreamOverHttpCacheStateMachine>();
					if (that) {
						that->_position += result->Length;
					}

					return result;
				});
			}
		});
	}
	virtual Windows::Foundation::IAsyncOperationWithProgress<unsigned int, unsigned int> ^ WriteAsync(Windows::Storage::Streams::IBuffer ^buffer)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	virtual Windows::Foundation::IAsyncOperation<bool> ^ FlushAsync()
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	virtual property bool CanRead {bool get() { return true; }}
	virtual property bool CanWrite { bool get() { return false; }}
	virtual property unsigned long long Position {unsigned long long get() { return _position; }}
	virtual property unsigned long long Size {unsigned long long get() { return _stateMachine->size(); }void set(unsigned long long) {}}
	virtual Windows::Storage::Streams::IInputStream ^ GetInputStreamAt(unsigned long long position)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	virtual Windows::Storage::Streams::IOutputStream ^ GetOutputStreamAt(unsigned long long position)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	virtual void Seek(unsigned long long position)
	{
		_position = position;
	}
	virtual Windows::Storage::Streams::IRandomAccessStream ^ CloneStream()
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
};

concurrency::task<cached_stream_info> getCompleteStreamAsync(localdata::cached_track track, concurrency::cancellation_token cancelToken) {
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	auto folder = await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFolderAsync(L"track_cache", Windows::Storage::CreationCollisionOption::OpenIfExists));
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	auto file = await concurrency::create_task(folder->GetFileAsync(track.id.ToString()));
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	IRandomAccessStream^ stream = await concurrency::create_task(file->OpenReadAsync());
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	if (track.obuscated == 1) {
		stream = ref new ObfuscateStream(stream, track.id);
	}

	track.last_playpack_time = std::chrono::system_clock::now().time_since_epoch().count();
	await cache::updateCachedTrackInfoAsync(localdata::getDb(), track, cancelToken);
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	cached_stream_info result;
	result.stream = stream;
	result.isImport = false;
	result.soundQuality = static_cast<SoundQuality>(track.quality);
	result.contentType = track.quality >= static_cast<std::int32_t>(SoundQuality::Lossless) ? L"audio/flac" : L"audio/mp4";
	return result;
}

concurrency::task<cached_stream_info> getPartialStreamAsync(localdata::cached_track track, SoundQuality quality, concurrency::cancellation_token cancelToken) {
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	auto folder = await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFolderAsync(L"track_cache", Windows::Storage::CreationCollisionOption::OpenIfExists));
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	auto file = await concurrency::create_task(folder->CreateFileAsync(track.id.ToString(), Windows::Storage::CreationCollisionOption::OpenIfExists));
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	auto stream = await concurrency::create_task(file->OpenAsync(Windows::Storage::FileAccessMode::ReadWrite));
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;


	track.last_playpack_time = std::chrono::system_clock::now().time_since_epoch().count();
	await cache::updateCachedTrackInfoAsync(localdata::getDb(), track, cancelToken);
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	auto sm = std::make_shared< HttpCacheStateMachine>(track.id, quality, stream, track, cancelToken);
	await sm->initializeAsync(cancelToken);
	if (cancelToken.is_canceled()) {
		concurrency::cancel_current_task();
	}
	sm->Start();
	cached_stream_info result;

	result.isImport = false;
	result.soundQuality = static_cast<SoundQuality>(track.quality);
	result.stream = ref new RandomAccessStreamOverHttpCacheStateMachine(sm);
	result.contentType = track.quality >= static_cast<std::int32_t>(SoundQuality::Lossless) ? L"audio/flac" : L"audio/mp4";
	return result;

}
concurrency::task<cached_stream_info> resolveCachedStreamAsync(std::int64_t id, concurrency::cancellation_token cancelToken)
{
	auto importState = await LocalDB::executeAsyncWithCancel<localdata::GetImportedTrackInfoQuery>(localdata::getDb(), cancelToken, id);
	if (importState->size() > 0) {
		auto state = importState->at(0);
		state.last_playpack_time = std::chrono::system_clock::now().time_since_epoch().count();
		await LocalDB::executeAsyncWithCancel<localdata::imported_trackUpdateDbQuery>(localdata::getDb(), cancelToken, state);
		auto uri = ref new Windows::Foundation::Uri(L"ms-appdata:///local/imports/" + id.ToString());
		auto streamRef = RandomAccessStreamReference::CreateFromUri(uri);
		IRandomAccessStream^ stream = await concurrency::create_task(streamRef->OpenReadAsync(), cancelToken);
		if (state.obuscated == 1) {
			stream = ref new ObfuscateStream(stream, state.id);
		}
		cached_stream_info result;
		result.stream = stream;
		result.contentType = importState->at(0).quality >= static_cast<int32>(SoundQuality::Lossless) ?
			L"audio/flac" : L"audio/mp4";
		result.isImport = true;
		result.soundQuality = static_cast<SoundQuality>(importState->at(0).quality);
		return result;
	}

	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	auto streamingQualityTxt = dynamic_cast<Platform::String^>(settingsValues->Lookup(L"streamingQuality"));
	if (!streamingQualityTxt) {
		streamingQualityTxt = L"HIGH";
	}
	auto streamingQuality = parseSoundQuality(streamingQualityTxt);

	auto currentCacheState = await cache::getOrCreateCachedTrackInfoAsync(localdata::getDb(), id, cancelToken);
	if (currentCacheState.quality == static_cast<std::int32_t>(streamingQuality)) {
		if (currentCacheState.local_size == currentCacheState.server_size && currentCacheState.server_size != 0) {
			return await getCompleteStreamAsync(currentCacheState, cancelToken);
		}
	}
	return await getPartialStreamAsync(currentCacheState, streamingQuality, cancelToken);
}
