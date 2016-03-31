#include "pch.h"
#include "IncrementalLoadingCollection.h"
#include <pplawait.h>

concurrency::task<Windows::UI::Xaml::Data::LoadMoreItemsResult> Tidal::IncrementalLoadingCollection::LoadMoreItemsAsync(unsigned int count, concurrency::cancellation_token cancelToken)
{
	auto sizeBefore = _innerVector->Size;
	HasMoreItems = co_await _loadMoreItemsCallback(_innerVector, count, cancelToken);
	
	auto sizeAfter = _innerVector->Size;
	Windows::UI::Xaml::Data::LoadMoreItemsResult result;
	result.Count = sizeAfter - sizeBefore;
	for (auto& tce : _nextLoadTces) {
		tce.set();
	}
	_nextLoadTces.clear();
	return result;
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
