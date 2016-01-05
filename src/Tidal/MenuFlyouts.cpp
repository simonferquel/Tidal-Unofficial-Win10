#include "pch.h"
#include "MenuFlyouts.h"
#include "DownloadService.h"
using namespace Windows::UI::Xaml::Controls;
Windows::UI::Xaml::Controls::MenuFlyout ^ getAlbumMenuFlyout(std::int64_t id)
{
	MenuFlyout^ result = ref new MenuFlyout();
	auto importItem = ref new MenuFlyoutItem();
	importItem->Text = L"import";
	importItem->Click += ref new Windows::UI::Xaml::RoutedEventHandler([id](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
		getDownloadService().StartDownloadAlbumAsync(id);
	});
	result->Items->Append(importItem);
	return result;
}

Windows::UI::Xaml::Controls::MenuFlyout ^ getPlaylistMenuFlyout(const std::wstring & id)
{
	MenuFlyout^ result = ref new MenuFlyout();
	auto importItem = ref new MenuFlyoutItem();
	importItem->Text = L"import";
	importItem->Click += ref new Windows::UI::Xaml::RoutedEventHandler([id](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
		getDownloadService().StartDownloadPlaylistAsync(id);
	});
	result->Items->Append(importItem);
	return result;
}
