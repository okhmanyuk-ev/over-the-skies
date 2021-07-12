#include "gameover_menu.h"

using namespace hcg001;

GameoverMenu::GameoverMenu(int score)
{
	// score label

	mScoreLabel = std::make_shared<Scene::Label>();
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

	mHighScoreLabel = std::make_shared<Scene::Label>();
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

	mRubyScoreLabel = std::make_shared<Scene::Label>();
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
		Actions::Collection::RepeatInfinite([this, tap_label]() -> Actions::Collection::UAction {
			if (getState() != State::Entered)
				return nullptr;

			return Actions::Collection::MakeSequence(
				Actions::Collection::Show(tap_label, 0.75f),
				Actions::Collection::Hide(tap_label, 0.75f)
			);
		})
	);

	/*mHighscoresRect = std::make_shared<Scene::Rectangle>();
	mHighscoresRect->setAbsoluteRounding(true);
	mHighscoresRect->setRounding(16.0f);
	mHighscoresRect->setSize({ 256.0f, 128.0f });
	mHighscoresRect->setAnchor(0.5f);
	mHighscoresRect->setPivot(0.5f);
	mHighscoresRect->setAlpha(0.25f);
	mHighscoresRect->setY(64.0f);
	attach(mHighscoresRect);*/
}

void GameoverMenu::onEvent(const NetEvents::HighscoresEvent& e)
{
	/*std::vector<std::shared_ptr<Scene::Node>> items;

	for (int i = 0; i < e.uids.size(); i++)
	{
		auto uid = e.uids.at(i);

		CLIENT->requestProfile(uid);

		auto label = std::make_shared<Helpers::Label>();
		label->setText(std::to_string(i) + ") " + std::to_string(uid));
		label->setAnchor(0.5f);
		label->setPivot(0.5f);
		items.push_back(label);

		auto wait_func = [uid] { 
			return Client::Profiles.count(uid) == 0; 
		};

		label->runAction(Actions::Collection::Delayed(wait_func, Actions::Collection::Execute([uid, label] {
			auto name_chars = Client::Profiles.at(uid).at("nickname");
			auto name = utf8_string(name_chars.begin(), name_chars.end());
			label->setText(name);
		})));
	}

	glm::vec2 cell_size = { mHighscoresRect->getWidth(), 32.0f };
	auto grid = Shared::SceneHelpers::MakeVerticalGrid(cell_size, items);

	mHighscoresRect->attach(grid);*/
}

void GameoverMenu::onEnterBegin()
{
	Scene::Clickable<Screen>::onEnterBegin();

	//CLIENT->requestHighscores();
}
