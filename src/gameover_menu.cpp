#include "gameover_menu.h"

using namespace hcg001;

GameoverMenu::GameoverMenu(int score)
{
	// score label

	mScoreLabel->setFont(FONT("default"));
	mScoreLabel->setFontSize(56.0f);
	mScoreLabel->setAnchor({ 0.5f, 0.25f });
	mScoreLabel->setPivot({ 0.5f, 0.5f });
	mScoreLabel->setText(std::to_string(score));
	attach(mScoreLabel);

	// crown

	auto crown = std::make_shared<Scene::Sprite>();
	crown->setTexture(TEXTURE("textures/crown2.png"));
	crown->setAnchor({ 0.5f, 1.0f });
	crown->setPivot({ 1.0f, 0.0f });
	crown->setSize({ 28.0f, 28.0f });
	crown->setHorizontalPosition(-8.0f);
	crown->setVerticalPosition(24.0f);
	mScoreLabel->attach(crown);

	// highscore label

	mHighScoreLabel->setFont(FONT("default"));
	mHighScoreLabel->setFontSize(28.0f);
	mHighScoreLabel->setAnchor({ 1.0f, 0.5f });
	mHighScoreLabel->setPivot({ 0.0f, 0.5f });
	mHighScoreLabel->setHorizontalPosition(8.0f);
	mHighScoreLabel->setText(std::to_string(PROFILE->getHighScore()));
	crown->attach(mHighScoreLabel);

	// ruby

	auto ruby = std::make_shared<Scene::Sprite>();
	ruby->setTexture(TEXTURE("textures/ruby.png"));
	ruby->setAnchor({ 1.0f, 1.0f });
	ruby->setPivot({ 1.0f, 0.0f });
	ruby->setSize({ 28.0f, 28.0f });
	ruby->setVerticalPosition(8.0f);
	crown->attach(ruby);

	// rubies label

	mRubyScoreLabel->setFont(FONT("default"));
	mRubyScoreLabel->setFontSize(28.0f);
	mRubyScoreLabel->setAnchor({ 1.0f, 0.5f });
	mRubyScoreLabel->setPivot({ 0.0f, 0.5f });
	mRubyScoreLabel->setHorizontalPosition(8.0f);
	mRubyScoreLabel->setText(std::to_string(PROFILE->getRubies()));
	ruby->attach(mRubyScoreLabel);

	auto tap_label = std::make_shared<Scene::Label>();
	tap_label->setFont(FONT("default"));
	tap_label->setFontSize(24.0f);
	tap_label->setAnchor({ 0.5f, 0.75f });
	tap_label->setPivot({ 0.5f, 0.5f });
	tap_label->setText(LOCALIZE("GAMEOVER_MENU_TAP"));
	tap_label->setAlpha(0.0f);
	attach(tap_label);

	Actions::Run(
		Actions::Factory::RepeatInfinite([this, tap_label]() -> Actions::Factory::UAction {
			if (getState() != State::Entered)
				return nullptr;

			return Actions::Factory::MakeSequence(
				Actions::Factory::Show(tap_label, 0.75f),
				Actions::Factory::Hide(tap_label, 0.75f)
			);
		})
	);
}

void GameoverMenu::onEvent(const Helpers::HighscoresEvent& e)
{
	LOG("highscores begin");
	for (int i = 0; i < e.uids.size(); i++)
	{
		LOG(std::to_string(i) + ") " + std::to_string(e.uids.at(i)));
	}
	LOG("highscores end");
}

void GameoverMenu::onEnterBegin()
{
	Scene::Clickable<Screen>::onEnterBegin();

	CLIENT->requestHighscores();
}
