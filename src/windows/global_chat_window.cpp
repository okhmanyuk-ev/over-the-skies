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
	ok_button->setClickCallback([] {
		auto window = std::make_shared<InputWindow>("awdawd", [](auto str) {
			//
		});
		SCENE_MANAGER->pushWindow(window);
	});
	ok_button->setAnchor(0.5f);
	ok_button->setPivot(0.5f);
	ok_button->setSize({ 128.0f, 28.0f });
	getBody()->attach(ok_button);

	auto chat_message_img = std::make_shared<Shared::SceneHelpers::Adaptive<Scene::Sprite>>();
	chat_message_img->setTexture(TEXTURE("textures/chat_message.png"));
	chat_message_img->setAdaptSize({ 48.0f, 20.0f });
	chat_message_img->setAnchor(0.5f);
	chat_message_img->setPivot(0.5f);
	ok_button->attach(chat_message_img);
}