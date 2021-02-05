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
			CLIENT->sendChatMessage(str.cpp_str());
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

	auto scrollbar = std::make_shared<Shared::SceneHelpers::VerticalScrollbar>();
	scrollbar->setScrollbox(mScrollbox);
	mScrollbox->attach(scrollbar);

	if (CLIENT->isConnected())
	{
		/*const auto& messages = CLIENT->getGlobalChatMessages();
		for (auto [index, msg] : messages)
		{
			addMessage(index, msg);
		}*/
	}

	runAction(Actions::Factory::ExecuteInfinite([this] {
		if (getState() != Window::State::Opened)
			return;

		refreshMessages();
	}));
}

void GlobalChatWindow::onEvent(const Channel::GlobalChatMessageEvent& e)
{
	/*const auto& messages = CLIENT->getGlobalChatMessages();
	auto msg = messages.at(e.msgid);
	addMessage(e.msgid, msg);*/
}

void GlobalChatWindow::refreshMessages() // TODO: highly refactor names, and body of this function !!
{
	if (!CLIENT->isConnected())
	{
		// TODO: clear chat here
		return;
	}
	const auto& history = CLIENT->getGlobalChatMessages();

	if (history.empty())
		return;

	STATS_INDICATE_GROUP("chat", "chat items size", std::to_string(mItems.size()));
	STATS_INDICATE_GROUP("chat", "chat history size", std::to_string(history.size()));

	if (mScrollbox->isTouching())
		return;

	// TODO: return if no visible messages here ?

	const int TopVisibleInHistory = history.begin()->first;
	const int BottomVisibleInHistory = history.rbegin()->first; // TODO: rename to PascalCase

	// TODO: try to remove mWatchIndex because we doesnt need it

	if (mItems.empty())
	{
		mWatchIndex = BottomVisibleInHistory;
		addMessages(mWatchIndex, mWatchIndex);
		return; // TODO: return maybe can be removed
	}

	//if (mItems.size() > 7)
	//if (mItems.size() > 1)
	//	return;

	int topVisibleIndex = mWatchIndex;
	int bottomVisibleIndex = mWatchIndex;

	int topInvisibleCount = 0;
	int bottomInvisibleCount = 0;

	const int itemsTopIndex = mItems.begin()->first; // TODO: rename to PascalCase
	const int itemsBottomIndex = mItems.rbegin()->first;

	for (int i = itemsBottomIndex; i >= itemsTopIndex; i--) {
		if (mItems.count(i) == 0) {
			continue;
		}
		bottomInvisibleCount++;
		if (!mItems.at(i)->isVisible()) {
			continue;
		}
		bottomVisibleIndex = i;
		break;
	}

	for (int i = itemsTopIndex; i <= itemsBottomIndex; i++) {
		if (mItems.count(i) == 0) {
			continue;
		}
		topInvisibleCount++;
		if (!mItems.at(i)->isVisible()) {
			continue;
		}
		topVisibleIndex = i;
		break;
	}

	mWatchIndex = bottomVisibleIndex;

	const int VisibleTolerance = 5;
	const int AddPayload = 5;
	const int RemoveTolerance = 10;
	const int RemovePayload = 5;

	bool enoughTopMessages = topInvisibleCount - 1 >= VisibleTolerance;
	bool enoughBottomMessages = bottomInvisibleCount - 1 >= VisibleTolerance;

	if (!enoughTopMessages && itemsTopIndex <= TopVisibleInHistory) {
		enoughTopMessages = true;
	}
	if (!enoughBottomMessages && itemsBottomIndex >= BottomVisibleInHistory) {
		enoughBottomMessages = true;
	}

	if (!enoughTopMessages) {
		int bottom = itemsTopIndex - 1;
		int top = bottom - AddPayload;
		for (int i = bottom; i >= top; i--) {
			if (history.count(i) == 0) {
				break;
			}
			//if (!GET_CHAT_SYSTEM()->isDummyMessage(history.at(i))) {
			//	continue;
			//}
			//top--;
		}
		//addMessages(top, bottom, onMessagesAdded);
		addMessages(top, bottom);
	}
	/*else if (!enoughBottomMessages) {
		int top = itemsBottomIndex + 1;
		int bottom = top + AddPayload;
		for (int i = top; i <= bottom; i++) {
			if (history.count(i) == 0) {
				break;
			}
			//if (!GET_CHAT_SYSTEM()->isDummyMessage(history.at(i))) {
			//	continue;
			//}
			//bottom++;
		}
		//addMessages(top, bottom, onMessagesAdded);
		addMessages(top, bottom);
	}*/
	else {
		// messagesLoading = false;

		bool tooManyTopMessages = topInvisibleCount > RemoveTolerance;
		bool tooManyBottomMessages = bottomInvisibleCount > RemoveTolerance;

		assert(RemoveTolerance > RemovePayload);

		/*if (tooManyTopMessages) {
			auto [index, item] = *std::next(mItems.cbegin(), RemovePayload);
			removeMessages(itemsTopIndex, index);
		}
		else if (tooManyBottomMessages) {
			auto [index, item] = *std::next(mItems.crbegin(), RemovePayload);
			removeMessages(index, itemsBottomIndex);
		}*/
	}
}

void GlobalChatWindow::addMessage(int msgid, std::shared_ptr<Channel::ChatMessage> message)
{
	auto item = std::make_shared<Scene::Cullable<Scene::Node>>();
	item->setStretch({ 1.0f, 0.0f });
	item->setHeight(64.0f);
	item->setCullTarget(mScrollbox);
	item->setVisible(false);
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
	
	auto label = std::make_shared<Helpers::ProfileListenable<Helpers::Label>>();
	label->setAnchor(0.5f);
	label->setPivot(0.5f);
	label->setProfileUID(message->getUID());
	label->setProfileCallback([label, message, msgid](Channel::ProfilePtr profile) {
		label->setText(utf8_string(std::to_string(msgid)) + ") " + profile->getNickName() + ": " + message->getText());
	});
	rect->attach(label);

	fixScrollPosition(item->getHeight());

	/*if (!mItems.empty())
	{
		auto [index, item] = *mItems.rbegin();
		if (item->isVisible())
		{
			scrollToBack();
		}
	}*/

	mItems.insert({ msgid, item });

	refreshScrollContent(); // TODO: executes to many times (we use ranges)
}

void GlobalChatWindow::removeMessage(int msgid)
{
	assert(mItems.count(msgid) > 0);
	auto item = mItems.at(msgid);

	if (msgid <= mWatchIndex) 
		fixScrollPosition(-item->getHeight());

	mScrollbox->getContent()->detach(item);
	mItems.erase(msgid);

	refreshScrollContent(); // TODO: executes to many times (we use ranges)
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

void GlobalChatWindow::scrollToBack(bool animated)
{
	if (animated)
		runAction(Actions::Factory::ChangeVerticalScrollPosition(mScrollbox, 1.0f, 0.25f, Easing::CubicOut));
	else
		mScrollbox->setVerticalScrollPosition(1.0f);
}

void GlobalChatWindow::addMessages(int topIndex, int bottomIndex)
{
	assert(CLIENT->isConnected());
	assert(topIndex <= bottomIndex);

	const auto& history = CLIENT->getGlobalChatMessages();

	int count = 0;

	for (int index = topIndex; index <= bottomIndex; index++) 
	{
		if (history.count(index) == 0)
			continue;

		//if (!CLIENT->hasProfile(history.at(index)->getUID()))
		//	return;

		addMessage(index, history.at(index));
		count += 1;
	}
	LOG((std::to_string(count) + " message(s) added").c_str());
}

void GlobalChatWindow::removeMessages(int topIndex, int bottomIndex)
{
	assert(topIndex <= bottomIndex);

	int count = 0;
	for (int index = topIndex; index <= bottomIndex; index++) 
	{
		removeMessage(index);
		count += 1;
	}

	LOG((std::to_string(count) + " message(s) removed").c_str());
}

void GlobalChatWindow::fixScrollPosition(float height)
{
	auto old_space = mScrollbox->getVerticalScrollSpaceSize();
	auto new_space = old_space + height;

	auto scroll_pos_y = mScrollbox->getVerticalScrollPosition();
	scroll_pos_y /= new_space / old_space;
	mScrollbox->setVerticalScrollPosition(scroll_pos_y);
}
