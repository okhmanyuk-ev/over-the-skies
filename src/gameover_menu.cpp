#include "gameover_menu.h"
#include "helpers.h"
#include "main_menu.h"

using namespace hcg001;

GameoverMenu::GameoverMenu(int score)
{
	// score label

	mScoreLabel = std::make_shared<Scene::Label>();
	mScoreLabel->setFont(FONT("default"));
	mScoreLabel->setFontSize(56.0f);
	mScoreLabel->setAnchor({ 0.5f, 0.25f });
	mScoreLabel->setPivot({ 0.5f, 0.5f });
	mScoreLabel->setText(std::to_wstring(score));
	getContent()->attach(mScoreLabel);

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

	mHighScoreLabel = std::make_shared<Scene::Label>();
	mHighScoreLabel->setFont(FONT("default"));
	mHighScoreLabel->setFontSize(28.0f);
	mHighScoreLabel->setAnchor({ 1.0f, 0.5f });
	mHighScoreLabel->setPivot({ 0.0f, 0.5f });
	mHighScoreLabel->setHorizontalPosition(8.0f);
	mHighScoreLabel->setText(std::to_wstring(PROFILE->getHighScore()));
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

	mRubyScoreLabel = std::make_shared<Scene::Label>();
	mRubyScoreLabel->setFont(FONT("default"));
	mRubyScoreLabel->setFontSize(28.0f);
	mRubyScoreLabel->setAnchor({ 1.0f, 0.5f });
	mRubyScoreLabel->setPivot({ 0.0f, 0.5f });
	mRubyScoreLabel->setHorizontalPosition(8.0f);
	mRubyScoreLabel->setText(std::to_wstring(PROFILE->getRubies()));
	ruby->attach(mRubyScoreLabel);

	auto tap_label = std::make_shared<Scene::Label>();
	tap_label->setFont(FONT("default"));
	tap_label->setFontSize(24.0f);
	tap_label->setAnchor({ 0.5f, 0.75f });
	tap_label->setPivot({ 0.5f, 0.5f });
	tap_label->setText(LOCALIZE("GAMEOVER_MENU_TAP"));
	tap_label->setAlpha(0.0f);
	getContent()->attach(tap_label);

	Actions::Run(
		Actions::Collection::RepeatInfinite([this, tap_label]() -> std::unique_ptr<Actions::Action> {
			if (getState() != State::Entered)
				return nullptr;

			return Actions::Collection::MakeSequence(
				Actions::Collection::Show(tap_label, 0.75f),
				Actions::Collection::Hide(tap_label, 0.75f)
			);
		})
	);

	setClickCallback([] {
		SCENE_MANAGER->switchScreen(Helpers::gMainMenu);
	});
}

