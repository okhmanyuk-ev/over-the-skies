#include "achievements_window.h"
#include "achievements.h"
#include "profile.h"

using namespace hcg001;

AchievementsWindow::AchievementsWindow()
{
	getBackground()->setSize({ 314.0f, 386.0f });
	getTitle()->setText(LOCALIZE("ACHIEVEMENTS_WINDOW_TITLE"));

	auto ok_button = std::make_shared<Helpers::RectangleButton>();
	ok_button->setColor(Helpers::BaseWindowColor);
	ok_button->getLabel()->setText(LOCALIZE("WINDOW_OK"));
	ok_button->getLabel()->setFontSize(18.0f);
	ok_button->setClickCallback([] {
		SCENE_MANAGER->popWindow();
	});
	ok_button->setAnchor({ 0.5f, 1.0f });
	ok_button->setPivot(0.5f);
	ok_button->setSize({ 128.0f, 28.0f });
	ok_button->setY(-24.0f);
	getBody()->attach(ok_button);

	std::vector<std::shared_ptr<Scene::Node>> items;

	for (auto achievement : ACHIEVEMENTS->getItems())
	{
		auto item = std::make_shared<Item>(achievement.name);
		item->setAnchor(0.5f);
		item->setPivot(0.5f);
		items.push_back(item);
	}

	const glm::vec2 ItemSize = { 314.0f, 64.0f };

	auto grid = Shared::SceneHelpers::MakeVerticalGrid(ItemSize, items);

	auto scrollbox = std::make_shared<Scene::ClippableScissor<Scene::Scrollbox>>();
	scrollbox->setStretch(1.0f);
	scrollbox->getBounding()->setStretch(1.0f);
	scrollbox->getContent()->setSize(grid->getSize());
	scrollbox->getContent()->attach(grid);
	scrollbox->setVerticalMargin(48.0f);
	scrollbox->setSensitivity({ 0.0f, 1.0f });

	getBody()->attach(scrollbox);
}

AchievementsWindow::Item::Item(const std::string& name)
{
	setStretch({ 1.0f, 0.0f });
	setMargin(8.0f);
	setHeight(64.0f);
	setRounding(8.0f);
	setAbsoluteRounding(true);
	setAlpha(0.125f);

	auto title = std::make_shared<Helpers::Label>();
	title->setPosition({ 8.0f, 8.0f });
	title->setFontSize(15.0f);
	title->setText(name);
	attach(title);

	auto progress_label = std::make_shared<Helpers::Label>();
	progress_label->setPosition({ 8.0f, 24.0f });
	progress_label->setFontSize(12.0f);

	auto progress = ACHIEVEMENTS->getProgress(name);
	auto required = ACHIEVEMENTS->getRequired(name);

	progress_label->setText(fmt::format("{}/{}", progress, required));
	attach(progress_label);

	/*auto claim_button = std::make_shared<Helpers::Button>();
	claim_button->setTouchMask(1 << 1);
	claim_button->setColor(Helpers::ButtonColor);
	claim_button->getLabel()->setText(LOCALIZE("CLAIM"));
	claim_button->setClickCallback([] {
		//
	});
	claim_button->setAnchor({ 1.0f, 0.5f });
	claim_button->setPivot({ 1.0f, 0.5f });
	claim_button->setSize({ 64.0f, 24.0f });
	claim_button->setX(-8.0f);
	attach(claim_button);*/
}