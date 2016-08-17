//
// XboxHomeCatalogWidget.xaml.cpp
// Implementation of the XboxHomeCatalogWidget class
//

#include "pch.h"
#include "XboxHomeCatalogWidget.xaml.h"
#include "../IncrementalDataSources.h"
#include "XboxArticleListPage.xaml.h"
#include "XboxArticleListPageParameter.h"
#include "XboxShell.xaml.h"
#include "XboxMoodsPage.xaml.h"
#include "XboxGenresPage.xaml.h"
#include "XboxVideosPage.xaml.h"
#include "XboxSearchPage.xaml.h"
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
DependencyProperty^ XboxHomeCatalogWidget_SelectionProperty = DependencyProperty::Register(
	L"Selection",
	Windows::Foundation::Collections::IVector<PromotionItemVM^>::typeid,
	XboxHomeCatalogWidget::typeid, ref new PropertyMetadata(nullptr));

DependencyProperty^ XboxHomeCatalogWidget::SelectionProperty::get() {
	return XboxHomeCatalogWidget_SelectionProperty;
}
XboxHomeCatalogWidget::XboxHomeCatalogWidget()
{
	InitializeComponent();
}

void Tidal::XboxHomeCatalogWidget::GoToNews()
{
	auto param = ref new XboxArticleListPageParameter();
	param->Title = L"What's new";
	param->PromotionListName = L"NEWS";
	param->ListName = L"featured";
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(XboxArticleListPage::typeid, param);
	}
}

void Tidal::XboxHomeCatalogWidget::GoToRising()
{
	auto param = ref new XboxArticleListPageParameter();
	param->Title = L"Rising";
	param->PromotionListName = L"RISING";
	param->ListName = L"rising";
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(XboxArticleListPage::typeid, param);
	}
}

void Tidal::XboxHomeCatalogWidget::GoToDiscovery()
{
	auto param = ref new XboxArticleListPageParameter();
	param->Title = L"Discovery";
	param->PromotionListName = L"DISCOVERY";
	param->ListName = L"discovery";
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(XboxArticleListPage::typeid, param);
	}
}

void Tidal::XboxHomeCatalogWidget::GoToPlaylists()
{
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(XboxMoodsPage::typeid);
	}
}

void Tidal::XboxHomeCatalogWidget::GoToGenres()
{
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(XboxGenresPage::typeid);
	}
}

void Tidal::XboxHomeCatalogWidget::GoToVideos()
{
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(XboxVideosPage::typeid);
	}
}

void Tidal::XboxHomeCatalogWidget::GoToSearch()
{
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(XboxSearchPage::typeid);
	}
}

concurrency::task<void> Tidal::XboxHomeCatalogWidget::LoadAsync()
{
	auto selectionSource = getNewsPromotionsDataSource();
	co_await concurrency::create_task(selectionSource->LoadMoreItemsAsync(4));
	auto selection = ref new Platform::Collections::Vector<PromotionItemVM^>();
	for (auto ix = 0; ix < 4 && ix < selectionSource->Size; ++ix) {
		selection->Append(dynamic_cast<PromotionItemVM^>(selectionSource->GetAt(ix)));
	}
	while (selection->Size<4)
	{
		selection->Append(nullptr);
	}
	Selection = selection;
	this->Bindings->Update();
	loadingView->LoadingState = LoadingState::Loaded;
}


void Tidal::XboxHomeCatalogWidget::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	LoadAsync();
}
