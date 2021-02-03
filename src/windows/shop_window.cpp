#include "shop_window.h"

using namespace hcg001;

ShopWindow::ShopWindow()
{	
	getBackground()->setSize({ 314.0f, 286.0f });
	getTitle()->setText(LOCALIZE("SHOP_WINDOW_TITLE"));

	auto buy_button = std::make_shared<Helpers::RectangleButton>();
	buy_button->setColor(Helpers::BaseWindowColor);
	buy_button->getLabel()->setText(LOCALIZE("SHOP_WINDOW_BUY"));
	buy_button->getLabel()->setFontSize(18.0f);
	buy_button->setClickCallback([this] {
		PLATFORM->purchase("rubies.001");
	});
	buy_button->setAnchor({ 0.5f, 1.0f });
	buy_button->setPivot(0.5f);
	buy_button->setSize({ 128.0f, 32.0f });
	buy_button->setY(-24.0f);
	getBody()->attach(buy_button);
}