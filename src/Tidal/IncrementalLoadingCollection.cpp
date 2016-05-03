#include "pch.h"
#include "IncrementalLoadingCollection.h"
#include <experimental/coroutine>
#include <pplawait2.h>

concurrency::task<Windows::UI::Xaml::Data::LoadMoreItemsResult> Tidal::IncrementalLoadingCollection::LoadMoreItemsAsync(unsigned int count, concurrency::cancellation_token cancelToken)
{
	auto sizeBefore = _innerVector->Size;
	return _loadMoreItemsCallback(_innerVector, count, cancelToken).then([this, sizeBefore](bool hasMoreItems) {
		this->HasMoreItems = hasMoreItems;
		auto sizeAfter = _innerVector->Size;
		Windows::UI::Xaml::Data::LoadMoreItemsResult result;
		result.Count = sizeAfter - sizeBefore;
		for (auto& tce : _nextLoadTces) {
			tce.set();
		}
		_nextLoadTces.clear();
		return result;
	}, concurrency::task_continuation_context::get_current_winrt_context());
	
	
}


Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult>^ Tidal::IncrementalLoadingCollection::LoadMoreItemsAsync(unsigned int count)
{
	return concurrency::create_async([this, count](concurrency::cancellation_token cancelToken)-> concurrency::task<Windows::UI::Xaml::Data::LoadMoreItemsResult>{return LoadMoreItemsAsync(count, cancelToken); });
}


Tidal::IncrementalLoadingCollection::IncrementalLoadingCollection(const LoadMoreCallback& callback)
	:_loadMoreItemsCallback(callback)
{
	_innerVector = ref new Platform::Collections::Vector<Platform::Object^>();
	_innerVector->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object ^>(this, &Tidal::IncrementalLoadingCollection::_storageVectorChanged);
	HasMoreItems = true;
}

//Tidal::IncrementalLoadingCollection::IncrementalLoadingCollection(const LoadMoreCallbackWithHat & callback)
//	: _loadMoreItemsCallback([callback](Platform::Collections::Vector<Platform::Object^>^ p1, unsigned int p2, concurrency::cancellation_token p3) {
//	return callback(p1, p2, p3);
//})
//{
//	_innerVector = ref new Platform::Collections::Vector<Platform::Object^>();
//	_innerVector->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object ^>(this, &Tidal::IncrementalLoadingCollection::_storageVectorChanged);
//	HasMoreItems = true;
//}
