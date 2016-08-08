#include "pch.h"
#include "PauseCommand.h"
#include "AudioService.h"


bool Tidal::PauseCommand::CanExecute(Platform::Object ^parameter)
{
	return true;
}

void Tidal::PauseCommand::Execute(Platform::Object ^parameter)
{
	try {
		getAudioService().player()->Pause();
	}
	catch (...) {}
}
