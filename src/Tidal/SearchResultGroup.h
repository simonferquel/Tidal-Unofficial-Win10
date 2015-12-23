#pragma once
#include "IncrementalLoadingCollection.h"
namespace Tidal {
	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class SearchResultGroup sealed {
	public:
		property Platform::String^ Name;
		property Platform::String^ Icon;
		property Platform::String^ Filter;
		property Platform::String^ Query;

		property Windows::Foundation::Collections::IVector<Platform::Object^>^ Items;

		Windows::UI::Xaml::Interop::IBindableObservableVector^ Drilldown();
	};


	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class DrillDownSearchResultGroup sealed {
	public:
		property Platform::String^ Name;
		property Platform::String^ Icon;
		property Platform::String^ Filter;

		property Windows::UI::Xaml::Interop::IBindableObservableVector^ Items;
	};
}