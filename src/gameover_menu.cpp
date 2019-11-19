#include "gameover_menu.h"
#include "defines.h"

using namespace hcg001;

GameoverMenu::GameoverMenu()
{
	// score label

	mScoreLabel->setFont(FONT("default"));
	mScoreLabel->setFontSize(56.0f);
	mScoreLabel->setAnchor({ 0.5f, 0.25f });
	mScoreLabel->setPivot({ 0.5f, 0.5f });
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
	ruby->attach(mRubyScoreLabel);

	auto tap_label = std::make_shared<Scene::Label>();
	tap_label->setFont(FONT("default"));
	tap_label->setFontSize(24.0f);
	tap_label->setAnchor({ 0.5f, 0.75f });
	tap_label->setPivot({ 0.5f, 0.5f });
	tap_label->setText(LOCALIZE("GAMEOVER_MENU_TAP"));
	tap_label->setAlpha(0.0f);
	attach(tap_label);

	Common::Actions::Run(Shared::ActionHelpers::RepeatInfinite([this, tap_label]() -> Shared::ActionHelpers::Action {
		if (getState() != State::Entered)
			return nullptr;

		return Shared::ActionHelpers::MakeSequence(
			Shared::ActionHelpers::Show(tap_label, 0.75f),
			Shared::ActionHelpers::Hide(tap_label, 0.75f)
		);
	}));
}

void GameoverMenu::updateScore(int score)
{
	mScoreLabel->setText(std::to_string(score));
	mHighScoreLabel->setText(std::to_string(PROFILE->getHighScore()));
	mRubyScoreLabel->setText(std::to_string(PROFILE->getRubies()));
}