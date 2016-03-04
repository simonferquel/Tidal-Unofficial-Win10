#include "pch.h"
#include "UnauthenticatedDialog.h"

void showUnauthenticatedDialog()
{
	auto dlg = ref new Windows::UI::Popups::MessageDialog(L"This action requires a Tidal account, please log in first.", L"Not logged in");
	dlg->ShowAsync();
}
