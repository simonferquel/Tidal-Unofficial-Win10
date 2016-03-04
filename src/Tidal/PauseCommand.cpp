#include "pch.h"
#include "PauseCommand.h"



bool Tidal::PauseCommand::CanExecute(Platform::Object ^parameter)
{
	return true;
}

void Tidal::PauseCommand::Execute(Platform::Object ^parameter)
{
	try {
		Windows::Media::Playback::BackgroundMediaPlayer::Current->Pause();
	}
	catch (...) {}
}
