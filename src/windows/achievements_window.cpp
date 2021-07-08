#include "achievements_window.h"
#include "achievements.h"
#include "profile.h"

using namespace hcg001;

AchievementsWindow::AchievementsWindow()
{
	getBackground()->setSize({ 314.0f, 512.0f - 64.0f });
	getTitle()->setText(LOCALIZE("ACHIEVEMENTS_WINDOW_TITLE"));

	auto ok_button = std::make_shared<Helpers::Button>();
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

	int num = 0;

	for (auto achievement : ACHIEVEMENTS->getItems())
	{
		num += 1;

		auto item = std::make_shared<Item>(num, achievement.name);
		item->setAnchor(0.5f);
		item->setPivot(0.5f);
		items.push_back(item);
	}

	const glm::vec2 ItemSize = { 314.0f, 64.0f };

	auto grid = Shared::SceneHelpers::MakeVerticalGrid(ItemSize, items);
	grid->setY(Item::VerticalMargin / 2.0f);
	grid->setHeight(grid->getHeight() + Item::VerticalMargin);

	auto scrollbox = std::make_shared<Scene::ClippableScissor<Scene::Scrollbox>>();
	scrollbox->setStretch(1.0f);
	scrollbox->getBounding()->setStretch(1.0f);
	scrollbox->getContent()->setSize(grid->getSize());
	scrollbox->getContent()->attach(grid);
	scrollbox->setVerticalMargin(48.0f);
	scrollbox->setSensitivity({ 0.0f, 1.0f });
	getBody()->attach(scrollbox);
    
    auto scrollbar = std::make_shared<Shared::SceneHelpers::VerticalScrollbar>();
	scrollbar->setX(-4.0f);
    scrollbar->setScrollbox(scrollbox);
    scrollbox->attach(scrollbar);

	auto top_gradient = std::make_shared<Scene::Rectangle>();
	top_gradient->setStretch({ 1.0f, 0.0f });
	top_gradient->setHeight(Item::VerticalMargin);
	top_gradient->setAnchor({ 0.5f, 0.0f });
	top_gradient->setPivot({ 0.5f, 0.0f });
	top_gradient->setVerticalGradient({ Helpers::Pallete::WindowBody, 1.0f }, { Helpers::Pallete::WindowBody, 0.0f });
	scrollbox->attach(top_gradient);

	auto bottom_gradient = std::make_shared<Scene::Rectangle>();
	bottom_gradient->setStretch({ 1.0f, 0.0f });
	bottom_gradient->setHeight(Item::VerticalMargin);
	bottom_gradient->setAnchor({ 0.5f, 1.0f });
	bottom_gradient->setPivot({ 0.5f, 1.0f });
	bottom_gradient->setVerticalGradient({ Helpers::Pallete::WindowBody, 0.0f }, { Helpers::Pallete::WindowBody, 1.0f });
	scrollbox->attach(bottom_gradient);
}

AchievementsWindow::Item::Item(int num, const std::string& name)
{
	setStretch({ 1.0f, 0.0f });
	setMargin({ 16.0f, VerticalMargin });
	setHeight(64.0f);
	setRounding(8.0f);
	setAbsoluteRounding(true);
	setColor(Helpers::Pallete::WindowItem);

	auto num_label = std::make_shared<Helpers::Label>();
	num_label->setFontSize(24.0f);
	num_label->setPosition({ 24.0f, 0.0f });
	num_label->setPivot(0.5f);
	num_label->setAnchor({ 0.0f, 0.5f });
	num_label->setText(std::to_string(num));
	attach(num_label);

	auto title = std::make_shared<Helpers::Label>();
	title->setPosition({ 48.0f, 8.0f });
	title->setFontSize(16.0f);
	title->setText(LOCALIZE("ACHIEVEMENT_" + name));
	title->setColor(Helpers::Pallete::YellowLabel);
	attach(title);

	auto progress = ACHIEVEMENTS->getProgress(name);
	auto required = ACHIEVEMENTS->getRequired(name);

	auto progress_label = std::make_shared<Helpers::Label>();
	progress_label->setPosition({ 48.0f, 28.0f });
	progress_label->setFontSize(12.0f);
	progress_label->setText(fmt::format("{}/{}", progress, required));
	attach(progress_label);

	auto progressbar = std::make_shared<Shared::SceneHelpers::Progressbar>();
	progressbar->setSize({ 148.0f, 4.0f });
	progressbar->setPosition({ 48.0f, 42.0f });
	progressbar->setProgress((float)progress / (float)required);
	attach(progressbar);

	auto claim_button = std::make_shared<Helpers::Button>();
	claim_button->setTouchMask(1 << 1);
	claim_button->setColor(glm::rgbColor(glm::vec3(150.0f, 0.5f, 0.25f + 0.125f)));
	claim_button->getLabel()->setText(LOCALIZE("ACHIEVEMENTS_WINDOW_CLAIM"));
	claim_button->setClickCallback([] {
		//
	});
	claim_button->setAnchor({ 1.0f, 0.5f });
	claim_button->setPivot({ 1.0f, 0.5f });
	claim_button->setSize({ 72.0f, 24.0f });
	claim_button->setX(-8.0f);
	attach(claim_button);
}
