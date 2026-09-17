#include "daily_reward_window.h"

using namespace hcg001;

DailyRewardWindow::DailyRewardWindow(int current_day)
{
	getBackground()->setSize({ 314.0f, 286.0f });
	getTitle()->setText(sky::Localize("DAILYREWARD_TITLE"));
	setCloseOnMissclick(false);

	auto makePlashka = [this, current_day](int day) {
		auto [holder, collection] = Shared::SceneHelpers::CreateNodesFromXml(fmt::format(R"(
			<Node size="74,96">
				<Rectangle id="rect" stretch="1" anchor="0.5" pivot="0.5" margin="4" rounding="4" absolute_rounding="true" alpha="0.66" batch_group="daily_reward_item">
					<Label font_size="16" anchor="0.5,0" pivot="0.5,0" y="4" text="{title_text}"/>
					<Sprite anchor="0.5" pivot="0.5" size="36" texture="textures/dailyreward_rubies.png" batch_group="daily_reward_img"/>
					<Label font_size="16" anchor="0.5,1" pivot="0.5,1" y="-4" text="{value_text}"/>
				</Rectangle>
			</Node>
		)",
			fmt::arg("title_text", sky::to_string(sky::format(sky::Localize("DAILYREWARD_DAY"), day))),
			fmt::arg("value_text", DailyRewardMap.at(day))
		));

		auto rect = std::static_pointer_cast<Scene::Rectangle>(collection.at("rect"));
		rect->setColor(glm::rgbColor(glm::vec3(sky::HsvColors::HueGreen, day <= current_day ? 0.33f : 0.0f, 0.5f)));

		if (day == current_day)
		{
			rect->runAction(sky::Actions::RepeatInfinite([rect] {
				const auto Color1 = glm::rgbColor(glm::vec3(sky::HsvColors::HueGreen, 0.0f, 0.5f));
				const auto Color2 = glm::rgbColor(glm::vec3(sky::HsvColors::HueGreen, 0.5f, 0.5f));
				const float Duration = 0.5f;
				const auto Easing = Easing::QuadraticInOut;

				return sky::Actions::Sequence(
					sky::Actions::ChangeColorRgb(rect, Color1, Duration, Easing),
					sky::Actions::ChangeColorRgb(rect, Color2, Duration, Easing)
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
	ok_button->getLabel()->setText(sky::Localize("DAILYREWARD_CLAIM"));
	ok_button->setClickCallback([this] {
		SCENE_MANAGER->popWindow(mClaimCallback);
	});
	ok_button->setAnchor({ 0.5f, 1.0f });
	ok_button->setPivot(0.5f);
	ok_button->setSize({ 128.0f, 32.0f });
	ok_button->setY(-24.0f);
	getBody()->attach(ok_button);
}