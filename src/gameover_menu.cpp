#include "gameover_menu.h"
#include "helpers.h"
#include "main_menu.h"
#include "yandex.h"

using namespace hcg001;

GameoverMenu::GameoverMenu(int score)
{
	auto score_label = std::make_shared<Scene::Label>();
	score_label->setFont(FONT("default"));
	score_label->setFontSize(56.0f);
	score_label->setAnchor({ 0.5f, 0.25f });
	score_label->setPivot({ 0.5f, 0.5f });
	score_label->setText(std::to_wstring(score));
	getContent()->attach(score_label);

	auto highscore_label = std::make_shared<Shared::SceneHelpers::RichLabel>();
	highscore_label->setAnchor({ 0.5f, 1.0f });
	highscore_label->setPivot({ 0.5f, 0.0f });
	highscore_label->setY(24.0f);
	highscore_label->setFontSize(28.0f);
	highscore_label->setText(std::format(L"<icon=textures/crown2.png> {}", PROFILE->getHighScore()));
	score_label->attach(highscore_label);

	auto rubies_label = std::make_shared<Shared::SceneHelpers::RichLabel>();
	rubies_label->setAnchor({ 0.5f, 1.0f });
	rubies_label->setPivot({ 0.5f, 0.0f });
	rubies_label->setY(8.0f);
	rubies_label->setFontSize(28.0f);
	rubies_label->setText(std::format(L"<icon=textures/ruby.png> {}", PROFILE->getRubies()));
	highscore_label->attach(rubies_label);

	auto tap_label = std::make_shared<Scene::Label>();
	tap_label->setFont(FONT("default"));
	tap_label->setFontSize(24.0f);
	tap_label->setAnchor({ 0.5f, 0.75f });
	tap_label->setPivot({ 0.5f, 0.5f });
	tap_label->setText(LOCALIZE("GAMEOVER_MENU_TAP"));
	tap_label->setAlpha(0.0f);
	getContent()->attach(tap_label);

	runAction(Actions::Collection::Delayed([this] { return getState() != State::Entered; },
		Actions::Collection::RepeatInfinite([this, tap_label]() -> std::unique_ptr<Actions::Action> {
			return Actions::Collection::MakeSequence(
				Actions::Collection::Show(tap_label, 0.75f),
				Actions::Collection::Hide(tap_label, 0.75f)
			);
		})
	));

	setTapCallback([] {
		SCENE_MANAGER->switchScreen(Helpers::gMainMenu);
	});

	Yandex::SendHighScore(score);
}
