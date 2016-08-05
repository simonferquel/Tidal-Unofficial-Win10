//
// XboxHomeMyMusicWidgetxaml.xaml.cpp
// Implementation of the XboxHomeMyMusicWidgetxaml class
//

#include "pch.h"
#include "XboxHomeMyMusicWidget.xaml.h"
#include "AudioService.h"

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

Tidal::XboxHomeMyMusicWidget::XboxHomeMyMusicWidget()
{
	InitializeComponent();
}


void Tidal::XboxHomeMyMusicWidget::OnPlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getAudioService().playAllLocalMusicAsync();
}
