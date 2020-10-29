#include "daily_reward_window.h"
#include "helpers.h"

using namespace hcg001;

DailyRewardWindow::DailyRewardWindow(int current_day)
{
	setCloseOnMissclick(false);

	const auto BaseColor = glm::rgbColor(glm::vec3(210.0f, 0.5f, 1.0f));

	auto rect = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	rect->setRounding(12.0f);
	rect->setAbsoluteRounding(true);
	rect->setSize({ 314.0f, 286.0f });
	rect->setAnchor(0.5f);
	rect->setPivot(0.5f);
	rect->setColor(BaseColor / 12.0f);
	rect->setTouchable(true);
	getContent()->attach(rect);

	auto header = std::make_shared<Scene::Node>();
	header->setHorizontalStretch(1.0f);
	header->setHeight(36.0f);
	rect->attach(header);

	auto footer = std::make_shared<Scene::Node>();
	footer->setHeight(48.0f);
	footer->setHorizontalStretch(1.0f);
	footer->setAnchor({ 0.5f, 1.0f });
	footer->setPivot({ 0.5f, 1.0f });
	rect->attach(footer);

	auto content = std::make_shared<Scene::Node>();
	content->setStretch(1.0f);
	content->setY(header->getHeight());
	content->setVerticalMargin(header->getHeight() + footer->getHeight());
	rect->attach(content);

	// --------

	auto header_bg = std::make_shared<Scene::Rectangle>();
	header_bg->setStretch(1.0f);
	header_bg->setColor(BaseColor);
	header_bg->setAlpha(0.25f);
	header->attach(header_bg);

	auto title = std::make_shared<Scene::Label>();
	title->setFont(FONT("default"));
	title->setFontSize(20.0f);
	title->setText(LOCALIZE("DAILYREWARD_TITLE"));
	title->setAnchor({ 0.5f, 0.5f });
	title->setPivot({ 0.5f, 0.5f });
	header_bg->attach(title);

	const glm::vec2 PlashkaSize = { 74.0f, 96.0f };

	auto makePlashka = [PlashkaSize, current_day](int day) {
		auto rect = std::make_shared<Scene::Actionable<Scene::Rectangle>>();
		rect->setBatchGroup("plashka_rect");
		rect->setRounding(4.0f);
		rect->setAbsoluteRounding(true);
		rect->setStretch(1.0f);
		rect->setMargin(4.0f);
		rect->setAnchor(0.5f);
		rect->setPivot(0.5f);

		auto color = glm::vec3(Graphics::Color::Hsv::HueGreen, 0.0f, 0.33f);

		if (day <= current_day)
			color.y = 0.33f;

		rect->setColor(glm::rgbColor(color));
		
		if (day == current_day)
		{
			rect->runAction(Shared::ActionHelpers::RepeatInfinite([rect] {
				const auto Color1 = glm::rgbColor(glm::vec3(Graphics::Color::Hsv::HueGreen, 0.0f, 0.33f));
				const auto Color2 = glm::rgbColor(glm::vec3(Graphics::Color::Hsv::HueGreen, 0.5f, 0.5f));
				const float Duration = 0.5f;
				const auto Easing = Common::Easing::QuadraticInOut;

				return Shared::ActionHelpers::MakeSequence(
					Shared::ActionHelpers::ChangeColor(rect, Color1, Duration, Easing),
					Shared::ActionHelpers::ChangeColor(rect, Color2, Duration, Easing)
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
		img->setBatchGroup("plashka_img");
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
	grid->setAnchor(0.5f);
	grid->setPivot(0.5f);
	grid->setY(4.0f);
	content->attach(grid);

	auto ok_button = std::make_shared<Shared::SceneHelpers::FastButton>();
	ok_button->setRounding(6.0f);
	ok_button->setAbsoluteRounding(true);
	ok_button->setColor(BaseColor);
	ok_button->getLabel()->setText(LOCALIZE("DAILYREWARD_CLAIM"));
	ok_button->getLabel()->setFontSize(18.0f);
	ok_button->setClickCallback([this] {
		SCENE_MANAGER->popWindow(mClaimCallback);
	});
	ok_button->setAnchor(0.5f);
	ok_button->setPivot(0.5f);
	ok_button->setSize({ 128.0f, 32.0f });
	footer->attach(ok_button);
}