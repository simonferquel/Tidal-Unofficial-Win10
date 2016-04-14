//
// LocalMusic.xaml.h
// Declaration of the LocalMusic class
//

#pragma once

#include "LocalMusic.g.h"
#include <localdata/Entities.h>
#include "Mediators.h"
#include "IPageWithPreservedState.h"

namespace Tidal
{

	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class DownloadItemVM sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged {
	private:
		Windows::UI::Xaml::Visibility _progressVisibility = Windows::UI::Xaml::Visibility::Collapsed;
		double _localSize = 0;
		double _serverSize = 0;
	public:
		property Platform::String^ Title;
		property Platform::String^ ImageUrl;
		property Platform::String^ Artist;
		property std::int64_t Id;
		property Windows::UI::Xaml::Visibility ProgressVisibility { Windows::UI::Xaml::Visibility get() { return _progressVisibility; }
		void set(Windows::UI::Xaml::Visibility value) {
			if (value != _progressVisibility) {
				_progressVisibility = value;
				PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"ProgressVisibility"));
			}
		}}
		property double LocalSize {
			double get() {
				return _localSize;
			}
			void set(double value) {
				_localSize = value;
				PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"LocalSize"));
			}
		}
		property double ServerSize {
			double get() {
				return _serverSize;
			}
			void set(double value) {
				_serverSize = value;
				PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"ServerSize"));
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	};

	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class LocalMusic sealed : public IPageWithPreservedState
	{
	private:
		std::vector<RegistrationToken> _mediatorTokens;
		Platform::Collections::Vector<DownloadItemVM^>^ _downloadQueue;
	protected:

		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	public:
		LocalMusic();
		property Windows::Foundation::Collections::IVector<DownloadItemVM^>^ DownloadQueue {Windows::Foundation::Collections::IVector<DownloadItemVM^>^ get() { return _downloadQueue; } }

		// Inherited via IPageWithPreservedState
		virtual Platform::Object ^ GetStateToPreserve();
	private:
		concurrency::task<void> LoadAsync();
		void OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void PlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPlaylistClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnViewLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void OnTrackImportComplete(const std::int64_t& id);
		void OnTrackImportProgress(const ImportProgress& progress);

	};
}
