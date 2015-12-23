#include "pch.h"
#include "Mediators.h"

Mediator<AuthenticationState>& getAuthenticationStateMediator()
{
	static std::shared_ptr<Mediator<AuthenticationState>> m = std::make_shared<Mediator<AuthenticationState>>();
	return *m;
}

Mediator<std::int64_t>& getCurrentPlaybackTrackIdMediator()
{
	static std::shared_ptr<Mediator<std::int64_t>> m = std::make_shared<Mediator<std::int64_t>>();
	return *m;
}

AsyncMediator<bool>& getAppSuspendingMediator()
{
	static std::shared_ptr<AsyncMediator<bool>> m = std::make_shared<AsyncMediator<bool>>();
	return *m;
}

Mediator<bool>& getAppResumingMediator()
{
	static std::shared_ptr<Mediator<bool>> m = std::make_shared<Mediator<bool>>();
	return *m;
}

Mediator<std::int64_t>& getTrackImportComplete()
{
	static std::shared_ptr<Mediator<std::int64_t>> m = std::make_shared<Mediator<std::int64_t >>();
	return *m;
}

Mediator<ImportProgress>& getTrackImportProgress()
{
	static std::shared_ptr<Mediator<ImportProgress>> m = std::make_shared<Mediator<ImportProgress>>();
	return *m;
}

Mediator<bool>& getTrackImportLaunchedMediator()
{
	static std::shared_ptr<Mediator<bool>> m = std::make_shared<Mediator<bool>>();
	return *m;
}
