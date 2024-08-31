#include "daily_reward_window.h"

using namespace hcg001;

DailyRewardWindow::DailyRewardWindow(int current_day)
{
	getBackground()->setSize({ 314.0f, 286.0f });
	getTitle()->setText(LOCALIZE("DAILYREWARD_TITLE"));
	setCloseOnMissclick(false);

	auto makePlashka = [this, current_day](int day) {
		auto holder = std::make_shared<Scene::Node>();
		holder->setSize({ 74.0f, 96.0f });

		auto rect = std::make_shared<Scene::Rectangle>();
		rect->setBatchGroup(fmt::format("plashka_rect_{}", (size_t)this));
		rect->setRounding(4.0f);
		rect->setAbsoluteRounding(true);
		rect->setStretch(1.0f);
		rect->setMargin(4.0f);
		rect->setAnchor(0.5f);
		rect->setPivot(0.5f);
		rect->setAlpha(0.66f);
		holder->attach(rect);

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
		title->setText(fmt::format(LOCALIZE("DAILYREWARD_DAY").c_str(), day));
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
		value->setText(std::to_wstring(DailyRewardMap.at(day)));
		value->setAnchor({ 0.5f, 1.0f });
		value->setPivot({ 0.5f, 1.0f });
		value->setY(-4.0f);
		rect->attach(value);

		return holder;
	};

	auto grid = std::make_shared<Scene::AutoSized<Scene::Grid>>();
	grid->setMaxItemsInRow(4);
	grid->setDirection(Scene::Grid::Direction::RightDown);
	for (int i = 0; i < 7; i++)
	{
		grid->attach(makePlashka(i + 1));
	}
	grid->setAnchor({ 0.5f, 0.0f });
	grid->setPivot({ 0.5f, 0.0f });
	grid->setY(8.0f);
	grid->setAlign(0.5f);
	getBody()->attach(grid);

	auto ok_button = std::make_shared<Helpers::Button>();
	ok_button->getLabel()->setText(LOCALIZE("DAILYREWARD_CLAIM"));
	ok_button->setClickCallback([this] {
		SCENE_MANAGER->popWindow(mClaimCallback);
	});
	ok_button->setAnchor({ 0.5f, 1.0f });
	ok_button->setPivot(0.5f);
	ok_button->setSize({ 128.0f, 32.0f });
	ok_button->setY(-24.0f);
	getBody()->attach(ok_button);
}