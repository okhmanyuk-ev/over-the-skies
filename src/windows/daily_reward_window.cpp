#include "daily_reward_window.h"

using namespace hcg001;

DailyRewardWindow::DailyRewardWindow(int current_day)
{
	getBackground()->setSize({ 314.0f, 286.0f });
	getTitle()->setText(LOCALIZE("DAILYREWARD_TITLE"));
	setCloseOnMissclick(false);

	auto makePlashka = [this, current_day](int day) {
		auto [holder, collection] = Shared::SceneHelpers::CreateNodesFromXml(R"(
			<Node size="74, 96">
				<Rectangle id="rect" stretch="1.0" anchor="0.5" pivot="0.5" margin="4" rounding="4" absolute_rounding="true" alpha="0.66">
					<Label id="title" font_size="16" anchor="0.5,0" pivot="0.5,0" y="4"/>
					<Sprite id="img" anchor="0.5" pivot="0.5" size="36" texture="textures/dailyreward_rubies.png"/>
					<Label id="value" font_size="16" anchor="0.5,1" pivot="0.5,1" y="-4"/>
				</Rectangle>
			</Node>
		)");

		auto rect = std::static_pointer_cast<Scene::Rectangle>(collection.at("rect"));
		rect->setBatchGroup(fmt::format("plashka_rect_{}", (size_t)this));
		rect->setColor(glm::rgbColor(glm::vec3(Graphics::Color::Hsv::HueGreen, day <= current_day ? 0.33f : 0.0f, 0.5f)));

		auto title = std::static_pointer_cast<Scene::Label>(collection.at("title"));
		title->setText(fmt::format(LOCALIZE("DAILYREWARD_DAY").c_str(), day));

		auto img = std::static_pointer_cast<Scene::Sprite>(collection.at("img"));
		img->setBatchGroup(fmt::format("plashka_img_{}", (size_t)this));

		auto value = std::static_pointer_cast<Scene::Label>(collection.at("value"));
		value->setText(std::to_wstring(DailyRewardMap.at(day)));

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

		return holder;
	};

	auto grid = std::make_shared<Scene::AutoSized<Scene::Grid>>();
	grid->setMaxItemsInRow(4);
	grid->setBreakToFitSize(false);
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