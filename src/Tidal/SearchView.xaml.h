//
// SearchView.xaml.h
// Declaration of the SearchView class
//

#pragma once
#include "SearchResultGroup.h"
#include "SearchResultTemplateSelector.h"
#include "SearchView.g.h"

namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SearchView sealed
	{
	public:
		SearchView();
	private:
		concurrency::cancellation_token_source _currentSearchCts;
		concurrency::task<void> DoSearchAsync();
		void OnTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void OnBackToSearchResults(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args);
		void OnLVHeaderTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnSearchResultClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
