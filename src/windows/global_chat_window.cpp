#include "global_chat_window.h"
#include "helpers.h"
#include "input_window.h"
#include "cheats.h"

using namespace hcg001;

GlobalChatWindow::GlobalChatWindow()
{
	getBackground()->setSize({ 314.0f, 386.0f });
	getTitle()->setText(LOCALIZE("GLOBAL_CHAT_WINDOW_TITLE"));

	auto ok_button = std::make_shared<Helpers::RectangleButton>();
	ok_button->setColor(Helpers::BaseWindowColor);
	ok_button->setClickCallback([] {
		auto window = std::make_shared<InputWindow>("", [](auto str) {
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

	runAction(Actions::Factory::ExecuteInfinite([this] {
		if (getState() != Window::State::Opened)
			return;

		mScrollVerticalSpace = mScrollbox->getVerticalScrollSpace();
		refreshMessages();
	}));
}

void GlobalChatWindow::onEvent(const Channel::GlobalChatMessageEvent& e)
{
	if (mItems.empty())
		return;

	if (mItems.count(e.msgid) != 0) 
	{
		// some message was updated, remove it now
		// refreshMessages later will add it again
		removeMessage(e.msgid);
		return;
	}

	if (mScrollbox->isTouching())
		return;

	if (!mItems.rbegin()->second->isVisible()) // return if we not at the end of list
		return;

	const auto& history = CLIENT->getGlobalChatMessages();
	
	if (e.msgid != history.rbegin()->first) // return if not last message (last message means we receive new message)
		return;

	// if we are here this means that we receive new message
	// wait until refreshMessages will add this message
	// and scroll down with animation

	runAction(Actions::Factory::Delayed([this, e] { return mItems.count(e.msgid) == 0; }, 
		Actions::Factory::Execute([this] {
			scrollToBack();
		})
	));
}

void GlobalChatWindow::refreshMessages()
{
	if (!CLIENT->isConnected())
		return;
	
	const auto& history = CLIENT->getGlobalChatMessages();

	STATS_INDICATE_GROUP("chat", "chat items size", mItems.size());
	STATS_INDICATE_GROUP("chat", "chat history size", history.size());
	STATS_INDICATE_GROUP("chat", "chat watch position", mWatchIndex);

	if (history.empty())
		return;

	if (mScrollbox->isTouching())
		return;

	const int TopVisibleInHistory = history.begin()->first;
	const int BottomVisibleInHistory = history.rbegin()->first;

	// add first message

	if (mItems.empty())
	{
		mWatchIndex = BottomVisibleInHistory;
		addMessages(mWatchIndex, mWatchIndex);
		return;
	}

	const int ItemsTopIndex = mItems.begin()->first;
	const int ItemsBottomIndex = mItems.rbegin()->first;

	// add removed messages from center

	for (int i = ItemsTopIndex; i <= ItemsBottomIndex; i++)
	{
		if (mItems.count(i) > 0)
			continue;

		if (history.count(i) == 0)
			continue;

		addMessages(i, i);
		return;
	}

	int topVisibleIndex = mWatchIndex;
	int bottomVisibleIndex = mWatchIndex;

	int topInvisibleCount = 0;
	int bottomInvisibleCount = 0;

	for (int i = ItemsBottomIndex; i >= ItemsTopIndex; i--) {
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

	for (int i = ItemsTopIndex; i <= ItemsBottomIndex; i++) {
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

	const int VisibleTolerance = 10;
	const int AddCount = 20;
	const int RemoveTolerance = 80;
	const int RemoveCount = 40;

	assert(AddCount > 0);
	assert(RemoveCount > 0);

	bool enoughTopMessages = topInvisibleCount - 1 >= VisibleTolerance;
	bool enoughBottomMessages = bottomInvisibleCount - 1 >= VisibleTolerance;

	if (!enoughTopMessages && ItemsTopIndex <= TopVisibleInHistory) {
		enoughTopMessages = true;
	}
	if (!enoughBottomMessages && ItemsBottomIndex >= BottomVisibleInHistory) {
		enoughBottomMessages = true;
	}

	if (!enoughTopMessages) {
		int bottom = ItemsTopIndex - 1;
		int top = bottom - AddCount + 1;
		for (int i = bottom; i >= top; i--) {
			if (history.count(i) == 0) {
				break;
			}
			//if (!isDummyMessage(history.at(i))) {
			//	continue;
			//}
			//top--;
		}
		addMessages(top, bottom);
	}
	else if (!enoughBottomMessages) {
		int top = ItemsBottomIndex + 1;
		int bottom = top + AddCount - 1;
		for (int i = top; i <= bottom; i++) {
			if (history.count(i) == 0) {
				break;
			}
			//if (!isDummyMessage(history.at(i))) {
			//	continue;
			//}
			//bottom++;
		}
		addMessages(top, bottom);
	}
	else {
		bool tooManyTopMessages = topInvisibleCount > RemoveTolerance;
		bool tooManyBottomMessages = bottomInvisibleCount > RemoveTolerance;

		assert(RemoveTolerance > RemoveCount - 1);

		if (tooManyTopMessages) {
			auto [index, item] = *std::next(mItems.cbegin(), RemoveCount - 1);
			removeMessages(ItemsTopIndex, index);
		}
		else if (tooManyBottomMessages) {
			auto [index, item] = *std::next(mItems.crbegin(), RemoveCount - 1);
			removeMessages(index, ItemsBottomIndex);
		}
	}
}

void GlobalChatWindow::addMessage(int msgid, std::shared_ptr<Channel::ChatMessage> message)
{
	auto item = createTextMessage(message, msgid);

	fixScrollPosition(item->getHeight(), mWatchIndex >= msgid);
	
	assert(mItems.count(msgid) == 0);
	mItems.insert({ msgid, item });

	refreshScrollContent(); // TODO: executes to many times (we use ranges)
}

void GlobalChatWindow::removeMessage(int msgid)
{
	assert(mItems.count(msgid) > 0);
	auto item = mItems.at(msgid);

	fixScrollPosition(-item->getHeight(), mWatchIndex >= msgid);
	
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

	// check range for profiles

	for (int index = topIndex; index <= bottomIndex; index++)
	{
		if (history.count(index) == 0)
			continue;

		if (CLIENT->hasProfile(history.at(index)->getUID()))
			continue;

		return; // discard whole range if no profile in some message
	}

	int count = 0;

	for (int index = topIndex; index <= bottomIndex; index++) 
	{
		if (history.count(index) == 0)
			continue;

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

void GlobalChatWindow::fixScrollPosition(float height, bool inversed)
{
	auto old_space = mScrollVerticalSpace;
	auto new_space = old_space + height;

	auto scroll_pos_y = mScrollbox->getVerticalScrollPosition();
	
	if (inversed)
		scroll_pos_y = 1.0f - scroll_pos_y;		
	
	scroll_pos_y /= new_space / old_space;
	
	if (inversed)
		scroll_pos_y = 1.0f - scroll_pos_y;

	mScrollbox->setVerticalScrollPosition(scroll_pos_y);
	mScrollVerticalSpace = new_space;
}

std::shared_ptr<Scene::Node> GlobalChatWindow::createTextMessage(std::shared_ptr<Channel::ChatMessage> msg, int msgid)
{
	auto item = std::make_shared<Scene::Cullable<Scene::Node>>();
	item->setStretch({ 1.0f, 0.0f });
	item->setCullTarget(mScrollbox);
	item->setVisible(false);
	mScrollbox->getContent()->attach(item);

	//auto rect = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>(); // TODO: fix bug with stencil and scrollbar
	//auto rect = std::make_shared<Scene::ClippableScissor<Scene::Rectangle>>(); // TODO: fix bug with this shit
	auto rect = std::make_shared<Scene::Rectangle>();
	rect->setStretch({ 0.8f, 1.0f });
	rect->setMargin({ 8.0f, 6.0f });
	rect->setAlpha(0.25f);
	rect->setAnchor({ 0.0f, 0.5f });
	rect->setPivot({ 0.0f, 0.5f });
	rect->setX(8.0f);
	rect->setRounding(8.0f);
	rect->setAbsoluteRounding(true);
	item->attach(rect);
	
	assert(CLIENT->hasProfile(msg->getUID()));
	auto profile = CLIENT->getProfile(msg->getUID());

	auto label = std::make_shared<Helpers::Label>();
	label->setFontSize(15.0f);
	label->setAnchor(0.5f);
	label->setPivot(0.5f);
	label->setStretch({ 1.0f, 0.0f });
	label->setMargin({ 24.0f, 0.0f });
	label->setMultiline(true);
	label->setText(profile->getNickName() + ": " + msg->getText());
	rect->attach(label);

	item->updateAbsoluteSize();
	rect->updateAbsoluteSize();
	label->updateAbsoluteSize();
	label->updateTextMesh();

	item->setHeight(label->getAbsoluteHeight() + 24.0f);

	auto text_width = label->getFont()->getStringWidth(label->getText(), label->getFontSize());
	auto abs_width = label->getAbsoluteWidth();

	if (text_width < abs_width)
		rect->setHorizontalSize(rect->getHorizontalSize() - (abs_width - text_width));

	return item;
}
