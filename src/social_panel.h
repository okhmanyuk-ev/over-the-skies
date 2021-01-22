#pragma once

#include <shared/all.h>
#include "helpers.h"

namespace hcg001
{
	class SocialPanel : public Scene::Node,
		public Common::Event::Listenable<Helpers::HighscoresEvent>
	{
	public:
		SocialPanel();

	private:
		void refresh();

	private:
		void onEvent(const Helpers::HighscoresEvent& e) override;

	private:
		std::shared_ptr<Scene::ClippableStencil<Scene::Rectangle>> mRect;
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		std::shared_ptr<Scene::Node> mGrid;

		Helpers::HighscoresEvent mHighscores;
	};
}