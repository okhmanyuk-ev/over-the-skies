#include "achievements_window.h"

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

	auto json = Common::Helpers::LoadJsonFromAsset({ "achievements.json" });

	auto label = std::make_shared<Helpers::Label>();
	label->setStretch(1.0f);
	label->setMultiline(true);
	label->setText(json.dump(1));
	label->setMultilineAlign(Graphics::TextMesh::Align::Center);
	getBody()->attach(label);
}