#pragma once

#include "screen.h"
#include "profile.h"

namespace hcg001
{
	class GameoverMenu : public Scene::Clickable<Screen>
	{
	public:
		GameoverMenu(int score);

	private:
		std::shared_ptr<Scene::Label> mScoreLabel = std::make_shared<Scene::Label>();
		std::shared_ptr<Scene::Label> mHighScoreLabel = std::make_shared<Scene::Label>();
		std::shared_ptr<Scene::Label> mRubyScoreLabel = std::make_shared<Scene::Label>();
	};
}