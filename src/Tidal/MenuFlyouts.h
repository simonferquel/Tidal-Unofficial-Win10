#pragma once
#include <cstdint>
#include <string>
#include <Api/TrackInfo.h>
Windows::UI::Xaml::Controls::MenuFlyout^ getAlbumMenuFlyout(std::int64_t id);
Windows::UI::Xaml::Controls::MenuFlyout^ getPlaylistMenuFlyout(const std::wstring& id);
Windows::UI::Xaml::Controls::MenuFlyout^ getTrackMenuFlyout(const api::TrackInfo& track, std::wstring ownedPlaylist = std::wstring());