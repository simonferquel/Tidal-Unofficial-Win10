#pragma once

ref class StreamWrapper sealed : public Windows::Storage::Streams::IRandomAccessStream {
private:
	Windows::Storage::Streams::IRandomAccessStream^ _innerStream;
public:
	StreamWrapper(Windows::Storage::Streams::IRandomAccessStream^ innerStream)  : _innerStream(innerStream){

	}

	virtual ~StreamWrapper() {
		OutputDebugString(L"StreamWrapper dtor\n");
	}

	virtual Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IBuffer ^, unsigned int> ^ ReadAsync(Windows::Storage::Streams::IBuffer ^buffer, unsigned int count, Windows::Storage::Streams::InputStreamOptions options) {
		return _innerStream->ReadAsync(buffer, count, options);
	}
	virtual Windows::Foundation::IAsyncOperationWithProgress<unsigned int, unsigned int> ^ WriteAsync(Windows::Storage::Streams::IBuffer ^buffer) {
		return _innerStream->WriteAsync(buffer);
	}
	virtual Windows::Foundation::IAsyncOperation<bool> ^ FlushAsync() {
		return _innerStream->FlushAsync();
	}
	virtual property bool CanRead {bool get() { return _innerStream->CanRead; }}
	virtual property bool CanWrite {bool get() { return _innerStream->CanWrite; }}
	virtual property unsigned long long Position {unsigned long long get() { return _innerStream->Position; }}
	virtual property unsigned long long Size {unsigned long long get() { return _innerStream->Size; } void set(unsigned long long value) { _innerStream->Size = value; }}
	virtual Windows::Storage::Streams::IInputStream ^ GetInputStreamAt(unsigned long long position) {
		throw ref new Platform::NotImplementedException();
	}
	virtual Windows::Storage::Streams::IOutputStream ^ GetOutputStreamAt(unsigned long long position) {

		throw ref new Platform::NotImplementedException();
	}
	virtual void Seek(unsigned long long position) { _innerStream->Seek(position); }
	virtual Windows::Storage::Streams::IRandomAccessStream ^ CloneStream() {
		return ref new StreamWrapper(_innerStream->CloneStream());
	}
};

ref class ObfuscateStream sealed : public Windows::Storage::Streams::IRandomAccessStream {
private:
	Windows::Storage::Streams::IRandomAccessStream^ _innerStream;
	std::uint64_t _obfuscateKey;
public:
	ObfuscateStream(Windows::Storage::Streams::IRandomAccessStream^ innerStream, std::uint64_t obfuscateKey) : _innerStream(ref new StreamWrapper(innerStream)), _obfuscateKey(obfuscateKey) {}
	virtual ~ObfuscateStream()  {
		_innerStream = nullptr;
	}
	// Inherited via IRandomAccessStream
	virtual Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IBuffer ^, unsigned int> ^ ReadAsync(Windows::Storage::Streams::IBuffer ^buffer, unsigned int count, Windows::Storage::Streams::InputStreamOptions options);
	virtual Windows::Foundation::IAsyncOperationWithProgress<unsigned int, unsigned int> ^ WriteAsync(Windows::Storage::Streams::IBuffer ^buffer);
	virtual Windows::Foundation::IAsyncOperation<bool> ^ FlushAsync() {
		return _innerStream->FlushAsync();
	}
	virtual property bool CanRead {bool get() { return _innerStream->CanRead; }}
	virtual property bool CanWrite {bool get() { return _innerStream->CanWrite; }}
	virtual property unsigned long long Position {unsigned long long get() { return _innerStream->Position; }}
	virtual property unsigned long long Size {unsigned long long get() { return _innerStream->Size; } void set(unsigned long long value) { _innerStream->Size = value; }}
	virtual Windows::Storage::Streams::IInputStream ^ GetInputStreamAt(unsigned long long position) {
		throw ref new Platform::NotImplementedException();
	}
	virtual Windows::Storage::Streams::IOutputStream ^ GetOutputStreamAt(unsigned long long position) {

		throw ref new Platform::NotImplementedException();
	}
	virtual void Seek(unsigned long long position) { _innerStream->Seek(position); }
	virtual Windows::Storage::Streams::IRandomAccessStream ^ CloneStream() {
		return ref new ObfuscateStream(_innerStream->CloneStream(), _obfuscateKey);
	}
};