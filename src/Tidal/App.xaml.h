//
// App.xaml.h
// Declaration of the App class.
//

#pragma once
#include "PauseCommand.h"
#include "App.g.h"
#include "SmtcService.h"
#include <memory>
#include "LoadingView.h"

namespace Tidal
{
	/// <summary>
	/// Provides application-specific behavior to supplement the default Application class.
	/// </summary>
	ref class App sealed
	{
	private:
		std::unique_ptr<SmtcService> _smtcService;
		Windows::Foundation::EventRegistrationToken _backMouseToken;
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;
		virtual void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs^ e) override;

	internal:
		App();

	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
		void OnResuming(Platform::Object ^sender, Platform::Object ^args);
	};
}
