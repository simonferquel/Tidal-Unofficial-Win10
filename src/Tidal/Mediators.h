#pragma once
#include "AuthenticationState.h"
#include "Mediator.h"
#include <api/TrackInfo.h>
#include <vector>

Mediator<AuthenticationState>& getAuthenticationStateMediator();
Mediator<std::int64_t>& getCurrentPlaybackTrackIdMediator();

AsyncMediator<bool>& getAppSuspendingMediator();
Mediator<bool>& getAppResumingMediator();

Mediator<std::int64_t>& getTrackImportComplete();
Mediator<std::vector<api::TrackInfo>>& getCurrentPlaylistMediator();

struct ImportProgress {
	std::int64_t trackId;
	std::int64_t localSize;
	std::int64_t serverSize;
};


Mediator<ImportProgress>& getTrackImportProgress();



Mediator<bool>& getTrackImportLaunchedMediator();

struct ItemRemovedFromPlaylist {
	std::wstring playlistId;
	std::int64_t trackId;

};

Mediator<ItemRemovedFromPlaylist>& getItemRemovedFromPlaylistMediator();