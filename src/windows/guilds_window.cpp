#include "guilds_window.h"
#include "helpers.h"

using namespace hcg001;

GuildsWindow::GuildsWindow()
{
	auto rect = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	rect->setRounding(12.0f);
	rect->setAbsoluteRounding(true);
	rect->setSize({ 314.0f, 386.0f });
	rect->setAnchor(0.5f);
	rect->setPivot(0.5f);
	rect->setTouchable(true);
	rect->setColor(Helpers::BaseWindowColor / 12.0f);
	getContent()->attach(rect);

	auto header = std::make_shared<Scene::Node>();
	header->setHorizontalStretch(1.0f);
	header->setHeight(36.0f);
	rect->attach(header);

	auto footer = std::make_shared<Scene::Node>();
	footer->setHeight(44.0f);
	footer->setHorizontalStretch(1.0f);
	footer->setAnchor({ 0.5f, 1.0f });
	footer->setPivot({ 0.5f, 1.0f });
	rect->attach(footer);

	auto content = std::make_shared<Scene::Node>();
	content->setStretch(1.0f);
	content->setY(header->getHeight());
	content->setVerticalMargin(header->getHeight() + footer->getHeight());
	rect->attach(content);

	auto tabs_field = std::make_shared<Scene::Rectangle>();
	tabs_field->setStretch({ 1.0f, 0.0f });
	tabs_field->setHeight(32.0f);
	tabs_field->setColor(Graphics::Color::Black);
	content->attach(tabs_field);

	auto asdfsd = std::make_shared<Helpers::Label>();
	asdfsd->setText("TABS ARE HERE (score, rubies)");
	asdfsd->setAnchor(0.5f);
	asdfsd->setPivot(0.5f);
	tabs_field->attach(asdfsd);

	auto scrollbox = std::make_shared<Scene::Scrollbox>();
	scrollbox->setStretch(1.0f);
	scrollbox->setVerticalMargin(tabs_field->getHeight());
	scrollbox->setAnchor({ 0.5f, 1.0f });
	scrollbox->setPivot({ 0.5f, 1.0f });
	content->attach(scrollbox);

	auto header_bg = std::make_shared<Scene::Rectangle>();
	header_bg->setStretch(1.0f);
	header_bg->setColor(Helpers::BaseWindowColor);
	header_bg->setAlpha(0.25f);
	header->attach(header_bg);

	auto title = std::make_shared<Helpers::Label>();
	title->setFontSize(20.0f);
	title->setText(LOCALIZE("GUILDS_WINDOW_TITLE"));
	title->setAnchor(0.5f);
	title->setPivot(0.5f);
	header_bg->attach(title);

	auto ok_button = std::make_shared<Helpers::RectangleButton>();
	ok_button->setColor(Helpers::BaseWindowColor);
	ok_button->getLabel()->setText(LOCALIZE("WINDOW_OK"));
	ok_button->getLabel()->setFontSize(18.0f);
	ok_button->setClickCallback([] {
		SCENE_MANAGER->popWindow();
	});
	ok_button->setAnchor(0.5f);
	ok_button->setPivot(0.5f);
	ok_button->setSize({ 128.0f, 28.0f });
	footer->attach(ok_button);
}