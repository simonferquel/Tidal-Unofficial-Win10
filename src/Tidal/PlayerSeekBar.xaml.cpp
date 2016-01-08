//
// PlayerSeekBar.xaml.cpp
// Implementation of the PlayerSeekBar class
//

#include "pch.h"
#include "PlayerSeekBar.xaml.h"
#include "AudioService.h"
#include <tools/TimeUtils.h>
#include <tools/StringUtils.h>

using namespace std::chrono;
using namespace Tidal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

PlayerSeekBar::PlayerSeekBar()
{
	InitializeComponent();
}


void Tidal::PlayerSeekBar::OnSliderPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	auto player = Windows::Media::Playback::BackgroundMediaPlayer::Current;
	sliderBG->CapturePointer(e->Pointer);
	player->Pause();
	auto pos = e->GetCurrentPoint(sliderBG);
	auto progress = pos->Position.X / sliderBG->ActualWidth;
	if (progress < 0) {
		progress = 0;
	}
	if (progress > 1) {
		progress = 1;
	}
	sliderScale->ScaleX = progress;
}


void Tidal::PlayerSeekBar::OnSliderReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	auto player = Windows::Media::Playback::BackgroundMediaPlayer::Current;
	sliderBG->ReleasePointerCapture(e->Pointer);
	auto pos = e->GetCurrentPoint(sliderBG);
	auto progress = pos->Position.X / sliderBG->ActualWidth;
	if (progress < 0) {
		progress = 0;
	}
	if (progress > 1) {
		progress = 1;
	}
	if (player->CurrentState == Windows::Media::Playback::MediaPlayerState::Paused) {
		Windows::Foundation::TimeSpan ts;
		ts.Duration = static_cast<std::int64_t>(progress * player->NaturalDuration.Duration);
		player->Position = ts;
	}
	player->Play();
}


void Tidal::PlayerSeekBar::OnSliderMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	if (sliderBG->PointerCaptures && sliderBG->PointerCaptures->Size > 0) {
		auto pos = e->GetCurrentPoint(sliderBG);
		auto progress = pos->Position.X / sliderBG->ActualWidth;
		if (progress < 0) {
			progress = 0;
		}
		if (progress > 1) {
			progress = 1;
		}
		sliderScale->ScaleX = progress;
	}
}


void Tidal::PlayerSeekBar::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	_timer = ref new DispatcherTimer();
	_timer->Interval = tools::time::ToWindowsTimeSpan(100ms);
	_timer->Tick += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &Tidal::PlayerSeekBar::OnTick);
	_timer->Start();
}


void Tidal::PlayerSeekBar::OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_timer) {
		_timer->Stop();
		_timer = nullptr;
	}
}


void Tidal::PlayerSeekBar::OnTick(Platform::Object ^sender, Platform::Object ^args)
{
	auto player = Windows::Media::Playback::BackgroundMediaPlayer::Current;
	auto duration = std::chrono::duration_cast<seconds>(tools::time::WindowsToChrono(player->NaturalDuration));
	auto position = std::chrono::duration_cast<seconds>(tools::time::WindowsToChrono(player->Position));
	
	if (duration != _lastDuration || position != _lastPosition) {
		_lastDuration = duration;
		_lastPosition = position;
		auto maxval = duration.count();
		if (maxval <= 0) {
			maxval = 1;
			duration = 0s;
		}

		if (!sliderBG->PointerCaptures || sliderBG->PointerCaptures->Size == 0) {
			sliderScale->ScaleX = static_cast<double>(position.count()) / static_cast<double>(maxval);
		}
		std::wstringstream durationBuilder;
		durationBuilder << tools::time::toStringMMSS(position) << L" / " << tools::time::toStringMMSS(duration);
		durationText->Text = tools::strings::toWindowsString(durationBuilder.str());
	}
}
