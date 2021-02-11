#include "guilds_window.h"

using namespace hcg001;

GuildsWindow::GuildsWindow()
{
	getBackground()->setSize({ 314.0f, 386.0f });
	getTitle()->setText(LOCALIZE("GUILDS_WINDOW_TITLE"));

	auto ok_button = std::make_shared<Helpers::Button>();
	ok_button->setColor(Helpers::ButtonColor);
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

	auto item = std::make_shared<Item>();
	item->setAnchor(0.5f);
	item->setPivot(0.5f);
	getBody()->attach(item);
}

GuildsWindow::Item::Item()
{
	setStretch({ 1.0f, 0.0f });
	setMargin(8.0f);
	setHeight(48.0f);
	setRounding(8.0f);
	setAbsoluteRounding(true);
	setAlpha(0.125f);

	auto title = std::make_shared<Helpers::Label>();
	title->setAnchor({ 0.0f, 0.5f });
	title->setPivot({ 0.0f, 0.5f });
	title->setX(8.0f);
	title->setText("awdawdawd");
	title->setFontSize(15.0f);
	attach(title);

	auto join_button = std::make_shared<Helpers::Button>();
	join_button->setColor(Helpers::ButtonColor);
	join_button->getLabel()->setText(LOCALIZE("JOIN"));
	join_button->getLabel()->setFontSize(15.0f);
	join_button->setClickCallback([] {
		LOG("join button");
	});
	join_button->setAnchor({ 1.0f, 0.5f });
	join_button->setPivot({ 1.0f, 0.5f });
	join_button->setSize({ 64.0f, 28.0f });
	join_button->setX(-8.0f);
	attach(join_button);

}
