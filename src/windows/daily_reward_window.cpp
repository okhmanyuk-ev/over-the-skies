#include "daily_reward_window.h"

using namespace hcg001;

DailyRewardWindow::DailyRewardWindow(int current_day)
{
	getBackground()->setSize({ 314.0f, 286.0f });
	getTitle()->setText(LOCALIZE("DAILYREWARD_TITLE"));

	setCloseOnMissclick(false);

	const glm::vec2 PlashkaSize = { 74.0f, 96.0f };

	auto makePlashka = [this, PlashkaSize, current_day](int day) {
		auto rect = std::make_shared<Scene::Rectangle>();
		rect->setBatchGroup(fmt::format("plashka_rect_{}", (size_t)this));
		rect->setRounding(4.0f);
		rect->setAbsoluteRounding(true);
		rect->setStretch(1.0f);
		rect->setMargin(4.0f);
		rect->setAnchor(0.5f);
		rect->setPivot(0.5f);
		rect->setAlpha(0.66f);

		auto color = glm::vec3(Graphics::Color::Hsv::HueGreen, 0.0f, 0.5f);

		if (day <= current_day)
			color.y = 0.33f;

		rect->setColor(glm::rgbColor(color));
		
		if (day == current_day)
		{
			rect->runAction(Actions::Collection::RepeatInfinite([rect] {
				const auto Color1 = glm::rgbColor(glm::vec3(Graphics::Color::Hsv::HueGreen, 0.0f, 0.5f));
				const auto Color2 = glm::rgbColor(glm::vec3(Graphics::Color::Hsv::HueGreen, 0.5f, 0.5f));
				const float Duration = 0.5f;
				const auto Easing = Easing::QuadraticInOut;

				return Actions::Collection::MakeSequence(
					Actions::Collection::ChangeColor(rect, Color1, Duration, Easing),
					Actions::Collection::ChangeColor(rect, Color2, Duration, Easing)
				);
			}));
		}

		auto title = std::make_shared<Scene::Label>();
		title->setFont(FONT("default"));
		title->setFontSize(16.0f);
		title->setText(LOCALIZE_FMT("DAILYREWARD_DAY", day));
		title->setAnchor({ 0.5f, 0.0f });
		title->setPivot({ 0.5f, 0.0f });
		title->setY(4.0f);
		rect->attach(title);

		auto img = std::make_shared<Scene::Sprite>();
		img->setBatchGroup(fmt::format("plashka_img_{}", (size_t)this));
		img->setAnchor(0.5f);
		img->setPivot(0.5f);
		img->setTexture(TEXTURE("textures/dailyreward_rubies.png"));
		img->setSize(36.0f);
		rect->attach(img);

		auto value = std::make_shared<Scene::Label>();
		value->setFont(FONT("default"));
		value->setFontSize(16.0f);
		value->setText(std::to_string(DailyRewardMap.at(day)));
		value->setAnchor({ 0.5f, 1.0f });
		value->setPivot({ 0.5f, 1.0f });
		value->setY(-4.0f);
		rect->attach(value);

		return rect;
	};

	auto sub_grid1 = Shared::SceneHelpers::MakeHorizontalGrid(PlashkaSize, {
		makePlashka(1),
		makePlashka(2),
		makePlashka(3),
		makePlashka(4),
	});
	sub_grid1->setAnchor(0.5f);
	sub_grid1->setPivot(0.5f);

	auto sub_grid2 = Shared::SceneHelpers::MakeHorizontalGrid(PlashkaSize, {
		makePlashka(5),
		makePlashka(6),
		makePlashka(7)
	});
	sub_grid2->setAnchor(0.5f);
	sub_grid2->setPivot(0.5f);

	auto grid = Shared::SceneHelpers::MakeVerticalGrid(sub_grid1->getSize(), {
		sub_grid1,
		sub_grid2
	});
	grid->setAnchor({ 0.5f, 0.0f });
	grid->setPivot({ 0.5f, 0.0f });
	grid->setY(8.0f);
	getBody()->attach(grid);

	auto ok_button = std::make_shared<Helpers::RectangleButton>();
	ok_button->setColor(Helpers::BaseWindowColor);
	ok_button->getLabel()->setText(LOCALIZE("DAILYREWARD_CLAIM"));
	ok_button->getLabel()->setFontSize(18.0f);
	ok_button->setClickCallback([this] {
		SCENE_MANAGER->popWindow(mClaimCallback);
	});
	ok_button->setAnchor({ 0.5f, 1.0f });
	ok_button->setPivot(0.5f);
	ok_button->setSize({ 128.0f, 32.0f });
	ok_button->setY(-24.0f);
	getBody()->attach(ok_button);
}