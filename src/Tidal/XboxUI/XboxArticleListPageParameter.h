#pragma once
ref class XboxArticleListPageParameter sealed
{
public:
	XboxArticleListPageParameter();
	property bool IsFavorites;
	property bool IsLocalMusic;
	property Platform::String^ Title;
	property Platform::String^ ListName;
	property Platform::String^ PromotionListName;
};

