#include "shop_window.h"
#include "helpers.h"

using namespace hcg001;

ShopWindow::ShopWindow()
{	
	auto rect = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	rect->setRounding(12.0f);
	rect->setAbsoluteRounding(true);
	rect->setSize({ 314.0f, 286.0f });
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
	header_bg->setColor(Helpers::BaseWindowColor);
	header_bg->setAlpha(0.25f);
	header->attach(header_bg);

	auto title = std::make_shared<Scene::Label>();
	title->setFont(FONT("default"));
	title->setFontSize(20.0f);
	title->setText(LOCALIZE("SHOP_WINDOW_TITLE"));
	title->setAnchor(0.5f);
	title->setPivot(0.5f);
	header_bg->attach(title);

	auto close_button = std::make_shared<Scene::Clickable<Scene::Rectangle>>();
	close_button->setSize(20.0f);
	close_button->setRounding(0.25f);
	close_button->setAnchor({ 1.0f, 0.5f });
	close_button->setPivot({ 0.5f, 0.5f });
	close_button->setX(-20.0f);
	close_button->setColor(Graphics::Color::Red);
	close_button->setAlpha(0.5f);
	close_button->setClickCallback([] {
		SCENE_MANAGER->popWindow();
	});
	header_bg->attach(close_button);

	auto buy_button = std::make_shared<Helpers::RectangleButton>();
	buy_button->setColor(Helpers::BaseWindowColor);
	buy_button->getLabel()->setText(LOCALIZE("SHOP_WINDOW_BUY"));
	buy_button->getLabel()->setFontSize(18.0f);
	buy_button->setClickCallback([this] {
		PLATFORM->purchase("rubies.001");
	});
	buy_button->setAnchor(0.5f);
	buy_button->setPivot(0.5f);
	buy_button->setSize({ 128.0f, 32.0f });
	footer->attach(buy_button);
}