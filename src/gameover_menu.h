#pragma once

#include "screen.h"

namespace hcg001
{
	class GameoverMenu : public Scene::Clickable<Screen>
	{
	public:
		GameoverMenu(int score);

	private:
		std::shared_ptr<Scene::Label> mScoreLabel;
		std::shared_ptr<Scene::Label> mHighScoreLabel;
		std::shared_ptr<Scene::Label> mRubyScoreLabel;
		std::shared_ptr<Scene::Rectangle> mHighscoresRect;
	};
}
