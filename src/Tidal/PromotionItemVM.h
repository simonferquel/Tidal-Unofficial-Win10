#pragma once
#include "IGo.h"
namespace api {
	struct PromotionArticle;
}
namespace Tidal {
	public ref class PromotionItemVM sealed : public IGo {
	public:
		property Platform::String^ ArtifactId;
		property Platform::String^ ImageUrl;
		property Platform::String^ ShortHeader;
		property Platform::String^ ShortSubHeader;
		property Platform::String^ Text;
		property Platform::String^ Type;
		property Platform::String^ TypeGlyph;
		PromotionItemVM() {}
		virtual void Go();
	internal:
		PromotionItemVM(const api::PromotionArticle& article);
		
	};
}