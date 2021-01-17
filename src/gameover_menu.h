#pragma once

#include "screen.h"
#include "helpers.h"

namespace hcg001
{
	class GameoverMenu : public Scene::Clickable<Screen>,
		public Common::Event::Listenable<Helpers::HighscoresEvent>
	{
	public:
		GameoverMenu(int score);

	private:
		void onEvent(const Helpers::HighscoresEvent& e) override;

	protected:
		void onEnterBegin() override;

	private:
		std::shared_ptr<Scene::Label> mScoreLabel = std::make_shared<Scene::Label>();
		std::shared_ptr<Scene::Label> mHighScoreLabel = std::make_shared<Scene::Label>();
		std::shared_ptr<Scene::Label> mRubyScoreLabel = std::make_shared<Scene::Label>();
	};
}