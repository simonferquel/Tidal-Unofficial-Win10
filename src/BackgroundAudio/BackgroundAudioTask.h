#pragma once
#include <ppltasks.h>
#include <Api/TrackInfo.h>
#include <map>
#include "MusicPlayer.h"
#include "BackgroundDownloader.h"
namespace BackgroundAudio {
	public ref class BackgroundAudioTask sealed : public Windows::ApplicationModel::Background::IBackgroundTask{
	private:
		Platform::Agile<Windows::ApplicationModel::Background::BackgroundTaskDeferral^> _taskDeferral;
		void AttachToEvents();
		void DettachFromEvents();
		std::shared_ptr<MusicPlayer> _musicPlayer;
		std::shared_ptr<BackgroundDownloader> _backgroundDownloader;
	public:
		// Inherited via IBackgroundTask
		virtual void Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance ^taskInstance);
		void OnCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance ^sender, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason);
		void OnMessageReceivedFromForeground(Platform::Object ^sender, Windows::Media::Playback::MediaPlayerDataReceivedEventArgs ^args);
		void OnMediaTransportButtonPressed(Windows::Media::SystemMediaTransportControls ^sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs ^args);
	};
}