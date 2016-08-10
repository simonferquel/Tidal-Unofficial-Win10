#include "pch.h"
#include "Environment.h"

bool env::isRunningOnXbox()
{
	return true;
	return Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily == L"Windows.Xbox";
}
