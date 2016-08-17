//
// XboxVideosPage.xaml.cpp
// Implementation of the XboxVideosPage class
//

#include "pch.h"
#include "XboxVideosPage.xaml.h"
#include "../IncrementalDataSources.h"
#include "../XboxOneHub.h"
#include <tools/StringUtils.h>
#include "../IGo.h"
#include "XboxShell.xaml.h"
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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

concurrency::task<void> Tidal::XboxVideosPage::LoadAsync()
{
	loader->LoadingState = LoadingState::Loading;
	try {

		auto allLists = co_await getSublistsAsync(concurrency::cancellation_token::none());
		for (auto&& info : *allLists) {
			if (info.hasVideos) {
				auto section = ref new XboxOneHubSection();
				section->Header = tools::strings::toWindowsString(info.name);
				section->Content = getNewsVideosDataSource(L"featured", tools::strings::toWindowsString(info.path));
				section->ContentTemplate = dynamic_cast<DataTemplate^>( this->Resources->Lookup(L"sectionContentTemplate"));
				hub->Items->Append(section);
			}
		}
		hub->SelectedIndex = 0;
		loader->LoadingState = LoadingState::Loaded;
		auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
		if (shell)
		{
			auto anim = hub->CompositionPropertySet->Compositor->CreateExpressionAnimation(L"hub.NormalizedOffsetX");
			anim->SetReferenceParameter(L"hub", hub->CompositionPropertySet);
			shell->AnimatedBackground->SetParallaxAmountBinding(anim);
		}
	}
	catch (...) {
		loader->LoadingState = LoadingState::Error;
	}
}

XboxVideosPage::XboxVideosPage()
{
	InitializeComponent();
}


void Tidal::XboxVideosPage::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	LoadAsync();
}


void Tidal::XboxVideosPage::OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto go = dynamic_cast<IGo^>(e->ClickedItem);
	if (go) {
		go->Go();
	}
}
