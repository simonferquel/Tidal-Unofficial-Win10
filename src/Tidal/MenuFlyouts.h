#pragma once
#include <cstdint>
Windows::UI::Xaml::Controls::MenuFlyout^ getAlbumMenuFlyout(std::int64_t id);
Windows::UI::Xaml::Controls::MenuFlyout^ getPlaylistMenuFlyout(const std::wstring& id);