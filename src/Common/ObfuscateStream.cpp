#include "pch.h"
#include "ObfuscateStream.h"
#include <ppltasks.h>
#include <experimental/coroutine>
#include <pplawait.h>
#include <wrl.h>
#include <robuffer.h>
#include <windows.storage.streams.h>

using namespace concurrency;
using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;
struct ObfuscateKey {
	union {
		std::uint64_t numeric;
		std::uint8_t bytes[8];
	};
};
//
//concurrency::task<IBuffer^> ReadObfuscatedAsync(IRandomAccessStream^ stream, std::uint64_t obfuscateKey, IBuffer^ buffer, unsigned int count, InputStreamOptions options) {
//	auto globalPos = stream->Position;
//	auto outBuffer = co_await create_task(stream->ReadAsync(buffer, count, options));
//	ComPtr<IBufferByteAccess> bufferBa;
//	byte* rawBuff;
//	reinterpret_cast<IInspectable*>(outBuffer)->QueryInterface<IBufferByteAccess>(&bufferBa);
//	bufferBa->Buffer(&rawBuff);
//
//	
//	ObfuscateKey key;
//	key.numeric = obfuscateKey;
//	for (std::uint32_t i = 0; i < outBuffer->Length; ++i) {
//		rawBuff[i] = rawBuff[i] ^ key.bytes[(i + globalPos) % 8];
//	}
//	return outBuffer;
//
//}
Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IBuffer^, unsigned int>^ ObfuscateStream::ReadAsync(Windows::Storage::Streams::IBuffer ^ buffer, unsigned int count, Windows::Storage::Streams::InputStreamOptions options)
{
	//auto t = ReadObfuscatedAsync(_innerStream, _obfuscateKey, buffer, count, options);
	return create_async([stream = _innerStream, obfuscateKey = _obfuscateKey, buffer, count, options](concurrency::progress_reporter<unsigned int> progress, concurrency::cancellation_token cancelToken) {
		auto globalPos = stream->Position;
		return create_task(stream->ReadAsync(buffer, count, options))
			.then([globalPos, obfuscateKey](IBuffer^ outBuffer) {
			ComPtr<IBufferByteAccess> bufferBa;
			byte* rawBuff;
			reinterpret_cast<IInspectable*>(outBuffer)->QueryInterface<IBufferByteAccess>(&bufferBa);
			bufferBa->Buffer(&rawBuff);


			ObfuscateKey key;
			key.numeric = obfuscateKey;
			for (std::uint32_t i = 0; i < outBuffer->Length; ++i) {
				rawBuff[i] = rawBuff[i] ^ key.bytes[(i + globalPos) % 8];
			}
			return outBuffer;
		});
	});
	//return _innerStream->ReadAsync(buffer, count, options);
}

Windows::Foundation::IAsyncOperationWithProgress<unsigned int, unsigned int>^ ObfuscateStream::WriteAsync(Windows::Storage::Streams::IBuffer ^ buffer)
{
	auto globalPos = _innerStream->Position;
	ComPtr<IBufferByteAccess> bufferBa;
	byte* rawBuff;
	reinterpret_cast<IInspectable*>(buffer)->QueryInterface<IBufferByteAccess>(&bufferBa);
	bufferBa->Buffer(&rawBuff);


	ObfuscateKey key;
	key.numeric = _obfuscateKey;
	for (std::uint32_t i = 0; i < buffer->Length; ++i) {
		rawBuff[i] = rawBuff[i] ^ key.bytes[(i + globalPos) % 8];
	}
	return _innerStream->WriteAsync(buffer);

	//return _innerStream->WriteAsync(buffer);
}
