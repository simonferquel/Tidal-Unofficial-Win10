//
// Videos.xaml.h
// Declaration of the Videos class
//

#pragma once

#include "Videos.g.h"
#include "SublistItemVM.h"
#include "IPageWithPreservedState.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Videos sealed : IPageWithPreservedState
	{
	private:
		concurrency::cancellation_token_source _cts;

	protected:
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			_cts.cancel();
			_cts = concurrency::cancellation_token_source();
		}
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	public:
		Videos();
		// Inherited via IPageWithPreservedState
		virtual Platform::Object ^ GetStateToPreserve();
	private:
		concurrency::task<void> LoadAsync(bool reloadPreservedState);
		void OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnVideoFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e);
		void OnVideoClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);

	};
}
