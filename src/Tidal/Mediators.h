#pragma once
#include "AuthenticationState.h"
#include "Mediator.h"

Mediator<AuthenticationState>& getAuthenticationStateMediator();
Mediator<std::int64_t>& getCurrentPlaybackTrackIdMediator();

AsyncMediator<bool>& getAppSuspendingMediator();
Mediator<bool>& getAppResumingMediator();

Mediator<std::int64_t>& getTrackImportComplete();

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