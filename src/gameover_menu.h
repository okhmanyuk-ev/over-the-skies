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
		std::shared_ptr<Scene::Label> mScoreLabel;
		std::shared_ptr<Scene::Label> mHighScoreLabel;
		std::shared_ptr<Scene::Label> mRubyScoreLabel;
		std::shared_ptr<Scene::Rectangle> mHighscoresRect;
	};
}