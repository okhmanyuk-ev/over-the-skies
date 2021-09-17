#pragma once

#include <shared/all.h>
#include "helpers.h"

namespace hcg001
{
	class Window : public Shared::SceneHelpers::BackblurredStandardWindow
	{
	public:
		Window(bool has_close_button = true);

	public:
		auto getBackground() const { return mBackground; }
		auto getBody() const { return mBody; }
		auto getTitle() const { return mTitle; }

	private:
		std::shared_ptr<Scene::ClippableStencil<Scene::Rectangle>> mBackground;
		std::shared_ptr<Scene::Node> mBody;
		std::shared_ptr<Helpers::Label> mTitle;
	};
}
