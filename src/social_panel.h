#pragma once

#include <shared/all.h>
#include "helpers.h"

namespace hcg001
{
	class SocialPanel : public Scene::Rectangle,
		public Common::Event::Listenable<Helpers::HighscoresEvent>
	{
	public:
		SocialPanel();

	private:
		void onEvent(const Helpers::HighscoresEvent& e) override;

	private:
		std::shared_ptr<Scene::Node> mGrid;
	};
}