#pragma once
#include <memory>
#include <ppltasks.h>
#include <mutex>
class MusicPlayer : public std::enable_shared_from_this<MusicPlayer>
{
private:
	std::recursive_mutex _mutex;
	concurrency::cancellation_token_source _currentCts;
	concurrency::task<void> _currentPlaybackTask = concurrency::task_from_result();
	Windows::Media::Playback::MediaPlayer^ _wmp;
	Windows::Media::Playback::MediaPlaybackList^ _currentPlayList;
	Windows::Foundation::EventRegistrationToken _wmpStateChangedToken;
	Windows::Foundation::EventRegistrationToken _wmpMediaOpenedToken;
	Windows::Foundation::EventRegistrationToken _wmpMediaEndedToken;
	Windows::Foundation::EventRegistrationToken _wmpMediaFailedToken;
	void onWmpStateChanged();
	void onWmpMediaOpened();
	void onWmpMediaEnded();
	void onWmpMediaFailed(Windows::Media::Playback::MediaPlayerFailedEventArgs^ args);
public:
	void initialize();
	void resetPlayqueueAndPlay(int index);
	void resetPlayqueue(int index);

	void playAtIndex(int index);
	void shutdown();
	void moveNext();
	void movePrevious();
	void resume();
	void pause();
	~MusicPlayer();
};