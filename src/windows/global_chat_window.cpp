#include "global_chat_window.h"
#include "helpers.h"
#include "input_window.h"

using namespace hcg001;

GlobalChatWindow::GlobalChatWindow()
{
	getBackground()->setSize({ 314.0f, 386.0f });
	getTitle()->setText(LOCALIZE("GLOBAL_CHAT_WINDOW_TITLE"));

	auto ok_button = std::make_shared<Helpers::RectangleButton>();
	ok_button->setColor(Helpers::BaseWindowColor);
	ok_button->setClickCallback([this] {
		auto window = std::make_shared<InputWindow>("awdawd", [this](auto str) {
			addItem(str);
		});
		SCENE_MANAGER->pushWindow(window);
	});
	ok_button->setAnchor(1.0f);
	ok_button->setPivot({ 1.0f, 0.5f });
	ok_button->setPosition({ -16.0f, -24.0f });
	ok_button->setSize({ 96.0f, 28.0f });
	getBody()->attach(ok_button);

	auto chat_message_img = std::make_shared<Shared::SceneHelpers::Adaptive<Scene::Sprite>>();
	chat_message_img->setTexture(TEXTURE("textures/chat_message.png"));
	chat_message_img->setAdaptSize({ 48.0f, 20.0f });
	chat_message_img->setAnchor(0.5f);
	chat_message_img->setPivot(0.5f);
	ok_button->attach(chat_message_img);

	auto scrollbox_holder = std::make_shared<Scene::ClippableScissor<Scene::Node>>();
	scrollbox_holder->setStretch(1.0f);
	scrollbox_holder->setVerticalMargin(48.0f);
	getBody()->attach(scrollbox_holder);

	mScrollbox = std::make_shared<Scene::Scrollbox>();
	mScrollbox->setStretch(1.0f);
	mScrollbox->getBounding()->setStretch(1.0f);
	mScrollbox->getContent()->setStretch({ 1.0f, 0.0f });
	mScrollbox->setSensitivity({ 0.0f, 1.0f });
	mScrollbox->setScrollOrigin({ 0.0f, 1.0f });
	mScrollbox->setScrollPosition({ 0.0f, 1.0f });
	scrollbox_holder->attach(mScrollbox);

	for (int i = 0; i < 10; i++)
	{
		addItem(std::to_string(i));
	}
}

void GlobalChatWindow::addItem(const utf8_string& text)
{
	auto item = std::make_shared<Scene::Node>();
	item->setStretch({ 1.0f, 0.0f });
	item->setHeight(64.0f);
	mScrollbox->getContent()->attach(item);

	auto rect = std::make_shared<Scene::Rectangle>();
	rect->setStretch(1.0f);
	rect->setMargin(8.0f);
	rect->setAlpha(0.25f);
	rect->setAnchor(0.5f);
	rect->setPivot(0.5f);
	rect->setRounding(8.0f);
	rect->setAbsoluteRounding(true);
	item->attach(rect);

	auto label = std::make_shared<Helpers::Label>();
	label->setText(text);
	label->setAnchor(0.5f);
	label->setPivot(0.5f);
	rect->attach(label);

	auto index = mItems.size();
	mItems.insert({ index, item });

	/*auto normalized_item_heght = item->getHeight() / mScrollbox->getVerticalScrollSpaceSize();
	auto scroll_pos_y = mScrollbox->getVerticalScrollPosition();
	mScrollbox->setVerticalScrollPosition(scroll_pos_y + (normalized_item_heght));*/

	refreshScrollContent();


}

void GlobalChatWindow::refreshScrollContent()
{
	float height = 0.0f;

	for (auto [index, item] : mItems)
	{
		item->setY(height);
		height += item->getHeight();
	}

	mScrollbox->getContent()->setHeight(height);
}
