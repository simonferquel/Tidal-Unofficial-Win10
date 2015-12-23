#include "pch.h"
#include "SearchResultGroup.h"
#include "IncrementalDataSources.h"

Windows::UI::Xaml::Interop::IBindableObservableVector ^ Tidal::SearchResultGroup::Drilldown()
{

	

	
	return getFilteredSearchSource(Query, Filter);
}
