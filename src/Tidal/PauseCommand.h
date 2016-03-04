#pragma once
namespace Tidal {
	public ref class PauseCommand sealed : Windows::UI::Xaml::Input::ICommand
	{
	public:

		// Inherited via ICommand
		virtual event Windows::Foundation::EventHandler<Platform::Object ^> ^ CanExecuteChanged;
		virtual bool CanExecute(Platform::Object ^parameter);
		virtual void Execute(Platform::Object ^parameter);
	};
}

