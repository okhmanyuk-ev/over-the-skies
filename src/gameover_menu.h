#pragma once

#include "screen.h"
#include "client.h"

namespace hcg001
{
	class GameoverMenu : public Scene::Clickable<Screen>,
		public Common::Event::Listenable<NetEvents::HighscoresEvent>
	{
	public:
		GameoverMenu(int score);

	private:
		void onEvent(const NetEvents::HighscoresEvent& e) override;

	protected:
		void onEnterBegin() override;

	private:
		std::shared_ptr<Scene::Label> mScoreLabel;
		std::shared_ptr<Scene::Label> mHighScoreLabel;
		std::shared_ptr<Scene::Label> mRubyScoreLabel;
		std::shared_ptr<Scene::Rectangle> mHighscoresRect;
	};
}