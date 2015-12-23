//
// LocalMusic.xaml.h
// Declaration of the LocalMusic class
//

#pragma once

#include "LocalMusic.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class LocalMusic sealed
	{
	public:
		LocalMusic();
	private:
		void OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
