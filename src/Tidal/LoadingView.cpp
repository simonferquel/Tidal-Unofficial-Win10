//
// LoadingView.cpp
// Implementation of the LoadingView class.
//

#include "pch.h"
#include "LoadingView.h"

using namespace Tidal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

// The Templated Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234235

LoadingView::LoadingView()
{
	DefaultStyleKey = "Tidal.LoadingView";
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Tidal::LoadingView::OnLoaded);
}

DependencyProperty^ LoadingView::_contentProperty = DependencyProperty::Register(
	L"Content", 
	Object::typeid, 
	LoadingView::typeid, 
	PropertyMetadata::Create(static_cast<Platform::Object^>(nullptr), 
		ref new PropertyChangedCallback([](DependencyObject^ sender, DependencyPropertyChangedEventArgs^ args) {
})));


DependencyProperty^ LoadingView::_loadingStateProperty = DependencyProperty::Register(
	L"LoadingState",
	Tidal::LoadingState::typeid,
	LoadingView::typeid,
	PropertyMetadata::Create(Tidal::LoadingState::Loading,
		ref new PropertyChangedCallback([](DependencyObject^ sender, DependencyPropertyChangedEventArgs^ args) {
	auto state = dynamic_cast<IBox<Tidal::LoadingState>^>(args->NewValue)->Value;
	Platform::String^ stateName = L"Loading";
	switch (state)
	{
	case Tidal::LoadingState::Loaded:
		stateName = L"Loaded";
		break;
	case Tidal::LoadingState::Loading:
		stateName = L"Loading";
		break;
	case Tidal::LoadingState::Error:
		stateName = L"Error";
		break;
	default:
		break;
	}
	VisualStateManager::GoToState(dynamic_cast<Control^>(sender), stateName, true);
})));


DependencyProperty^ LoadingView::_loadingTextProperty = DependencyProperty::Register(
	L"LoadingText",
	String::typeid,
	LoadingView::typeid,
	PropertyMetadata::Create(L"Loading...",
		ref new PropertyChangedCallback([](DependencyObject^ sender, DependencyPropertyChangedEventArgs^ args) {
})));

DependencyProperty^ LoadingView::_errorTextProperty = DependencyProperty::Register(
	L"ErrorText",
	String::typeid,
	LoadingView::typeid,
	PropertyMetadata::Create(L"An error occured. Please try again later.",
		ref new PropertyChangedCallback([](DependencyObject^ sender, DependencyPropertyChangedEventArgs^ args) {
})));

void Tidal::LoadingView::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	Platform::String^ stateName = L"Loading";
	switch (this->LoadingState)
	{
	case Tidal::LoadingState::Loaded:
		stateName = L"Loaded";
		break;
	case Tidal::LoadingState::Loading:
		stateName = L"Loading";
		break;
	case Tidal::LoadingState::Error:
		stateName = L"Error";
		break;
	default:
		break;
	}
	if (Windows::ApplicationModel::DesignMode::DesignModeEnabled)
		stateName = L"Loaded";
	VisualStateManager::GoToState(dynamic_cast<Control^>(sender), stateName, false);
}
