#pragma once
#include <ppltasks.h>
ref class WebStream sealed : public Windows::Storage::Streams::IRandomAccessStream {
private:
	Windows::Foundation::Uri ^ _uri;
	unsigned long long _position = 0;
	unsigned long long _size;
	Windows::Foundation::DateTime _modified;
	Windows::Storage::Streams::IInputStream^ _currentStream;
	WebStream(Windows::Foundation::Uri^ uri, unsigned long long size, Windows::Foundation::DateTime modified) :
	_uri(uri), _size(size), _modified(modified){

	}
	concurrency::task<Windows::Storage::Streams::IInputStream^> EnsureInputStreamAsync(concurrency::cancellation_token cancelToken);
	concurrency::task<Windows::Storage::Streams::IBuffer^> DoReadAsync(Windows::Storage::Streams::IBuffer ^buffer, unsigned int count, Windows::Storage::Streams::InputStreamOptions options, concurrency::progress_reporter<unsigned int> reporter, concurrency::cancellation_token cancelToken);
public:
	property Windows::Foundation::DateTime Modified { Windows::Foundation::DateTime get(){ return _modified; }}
	// Inherited via IRandomAccessStream
	virtual ~WebStream() {}
	virtual Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IBuffer ^, unsigned int> ^ ReadAsync(Windows::Storage::Streams::IBuffer ^buffer, unsigned int count, Windows::Storage::Streams::InputStreamOptions options);
	virtual Windows::Foundation::IAsyncOperationWithProgress<unsigned int, unsigned int> ^ WriteAsync(Windows::Storage::Streams::IBuffer ^buffer);
	virtual Windows::Foundation::IAsyncOperation<bool> ^ FlushAsync();
	virtual property bool CanRead { bool get() { return true; }}
	virtual property bool CanWrite {bool get() { return false; }}
	virtual property unsigned long long Position { unsigned long long get() { return _position; }}
	virtual property unsigned long long Size { unsigned long long get() { return _size; }
	void set(unsigned long long) { throw ref new Platform::NotImplementedException(); }
	}
	virtual Windows::Storage::Streams::IInputStream ^ GetInputStreamAt(unsigned long long position);
	virtual Windows::Storage::Streams::IOutputStream ^ GetOutputStreamAt(unsigned long long position);
	virtual void Seek(unsigned long long position);
	virtual Windows::Storage::Streams::IRandomAccessStream ^ CloneStream();
internal:
	static concurrency::task<WebStream^> CreateWebStreamAsync(Platform::String^ url, concurrency::cancellation_token cancelToken);

};