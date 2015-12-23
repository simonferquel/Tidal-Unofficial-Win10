#pragma once
#include <ppltasks.h>
inline concurrency::task<void> AnimateTo(Windows::UI::Xaml::DependencyObject^ obj, Platform::String^ prop, double to, Windows::Foundation::TimeSpan duration) {
	auto anim = ref new Windows::UI::Xaml::Media::Animation::DoubleAnimation();
	anim->To = to;
	anim->Duration = Windows::UI::Xaml::Duration(duration);
	Windows::UI::Xaml::Media::Animation::Storyboard::SetTarget(anim, obj);
	Windows::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(anim, prop);
	auto storyBoard = ref new Windows::UI::Xaml::Media::Animation::Storyboard();
	storyBoard->Children->Append(anim);
	concurrency::task_completion_event<void> tce;
	storyBoard->Completed += ref new Windows::Foundation::EventHandler<Platform::Object ^>([tce](Platform::Object^, Platform::Object^) {
		tce.set();
	});
	storyBoard->Begin();
	return concurrency::create_task(tce);
}