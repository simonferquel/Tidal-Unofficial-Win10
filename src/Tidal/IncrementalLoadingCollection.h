#pragma once
#include <functional>
#include <ppltasks.h>

namespace Tidal {
	ref class IncrementalLoadingCollection  sealed:
		Windows::UI::Xaml::Interop::IBindableObservableVector,
		Windows::UI::Xaml::Data::ISupportIncrementalLoading {
	public:
		using LoadMoreCallback = std::function<concurrency::task<bool>(Platform::Collections::Vector<Platform::Object^>^, unsigned int, concurrency::cancellation_token)>;
	private:
		std::vector<concurrency::task_completion_event<void>> _nextLoadTces;
		Platform::Collections::Vector<Platform::Object^>^ _innerVector;
		event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ _privateVectorChanged;

		void _storageVectorChanged(Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ e)
		{

			VectorChanged(this, e);

		}
		LoadMoreCallback _loadMoreItemsCallback;
		concurrency::task<Windows::UI::Xaml::Data::LoadMoreItemsResult> LoadMoreItemsAsync(unsigned int count, concurrency::cancellation_token cancelToken);
	internal:
		IncrementalLoadingCollection(const LoadMoreCallback& callback);
		concurrency::task<void> waitForNextLoadAsync() {
			concurrency::task_completion_event<void> tce;
			_nextLoadTces.push_back(tce);
			return concurrency::create_task(tce);
		}

	public:
		// Inherited via IBindableVector
		virtual Windows::UI::Xaml::Interop::IBindableIterator ^ First() { return dynamic_cast<Windows::UI::Xaml::Interop::IBindableIterator^>(_innerVector->First()); }

		// Inherited via IBindableObservableVector
		virtual property unsigned int Size { unsigned int get() { return _innerVector->Size; }}
		virtual Platform::Object ^ GetAt(unsigned int index) { return _innerVector->GetAt(index); }
		virtual Windows::UI::Xaml::Interop::IBindableVectorView ^ GetView() {
			return safe_cast<Windows::UI::Xaml::Interop::IBindableVectorView^>(_innerVector->GetView());
		}
		virtual bool IndexOf(Platform::Object ^value, unsigned int *index) { return _innerVector->IndexOf(value, index); }
		virtual void SetAt(unsigned int index, Platform::Object ^value) { _innerVector->SetAt(index, value); }
		virtual void InsertAt(unsigned int index, Platform::Object ^value) { _innerVector->InsertAt(index, value); }
		virtual void RemoveAt(unsigned int index) { _innerVector->RemoveAt(index); }
		virtual void Append(Platform::Object ^value) { _innerVector->Append(value); }
		virtual void RemoveAtEnd() { _innerVector->RemoveAtEnd(); }
		virtual void Clear() { _innerVector->Clear(); }
		virtual event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ VectorChanged
		{
			virtual Windows::Foundation::EventRegistrationToken add(Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ e)
			{
				return _privateVectorChanged += e;
			}

			virtual void remove(Windows::Foundation::EventRegistrationToken t)
			{
				_privateVectorChanged -= t;
			}
			virtual void raise(Windows::UI::Xaml::Interop::IBindableObservableVector^ vector, Platform::Object^ e)
			{

				_privateVectorChanged(vector, e);

			}
		}

		// Inherited via ISupportIncrementalLoading
		virtual property bool HasMoreItems;
		virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ LoadMoreItemsAsync(unsigned int count);
	};

}