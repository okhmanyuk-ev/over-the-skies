#include "chat_widget.h"
#include "helpers.h"

using namespace hcg001;

ChatWidget::ChatWidget()
{
	mScrollbox = std::make_shared<Scene::Scrollbox>();
	mScrollbox->setStretch(1.0f);
	mScrollbox->getBounding()->setStretch(1.0f);
	mScrollbox->getContent()->setStretch({ 1.0f, 0.0f });
	mScrollbox->setSensitivity({ 0.0f, 1.0f });
	mScrollbox->setScrollOrigin({ 0.0f, 1.0f });
	mScrollbox->setScrollPosition({ 0.0f, 1.0f });
	attach(mScrollbox);

	auto scrollbar = std::make_shared<Shared::SceneHelpers::VerticalScrollbar>();
	scrollbar->setScrollbox(mScrollbox);
	mScrollbox->attach(scrollbar);

	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::WaitOneFrame(),
		Actions::Collection::ExecuteInfinite([this] {
			mScrollVerticalSpace = mScrollbox->getVerticalScrollSpace();
			refreshMessages();
		})
	));
}

void ChatWidget::message(int msgid)
{
	if (mItems.empty())
		return;

	if (mItems.count(msgid) != 0) 
	{
		// some message was updated, remove it now
		// refreshMessages later will add it again
		removeMessage(msgid);
		return;
	}

	if (mScrollbox->isTouching())
		return;

	if (!mItems.rbegin()->second->isVisible()) // return if we not at the end of list
		return;
	
	if (msgid != getLastHistoryIndex()) // return if not last message (last message means we receive new message)
		return;

	// if we are here this means that we receive new message
	// wait until refreshMessages will add this message
	// and scroll down with animation

	runAction(Actions::Collection::Delayed([this, msgid] { return mItems.count(msgid) == 0; },
		Actions::Collection::Execute([this] {
			scrollToBack();
		})
	));
}

void ChatWidget::refreshMessages()
{
	if (!CLIENT->isConnected())
		return;
	
	//STATS_INDICATE_GROUP("chat", "chat items size", mItems.size());
	//STATS_INDICATE_GROUP("chat", "chat history size", history.size());
	//STATS_INDICATE_GROUP("chat", "chat watch position", mWatchIndex);

	if (isHistoryEmpty())
		return;

	if (mScrollbox->isTouching())
		return;

	const int TopVisibleInHistory = getFirstHistoryIndex();
	const int BottomVisibleInHistory = getLastHistoryIndex();

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

		if (!hasIndexInHistory(i))
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
			if (!hasIndexInHistory(i)) {
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
			if (!hasIndexInHistory(i)) {
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

void ChatWidget::addMessage(int msgid)
{
	auto item = createMessageNode(msgid);

	fixScrollPosition(item->getHeight(), mWatchIndex >= msgid);
	
	assert(mItems.count(msgid) == 0);
	mItems.insert({ msgid, item });

	refreshScrollContent(); // TODO: executes to many times (we use ranges)
}

void ChatWidget::removeMessage(int msgid)
{
	assert(mItems.count(msgid) > 0);
	auto item = mItems.at(msgid);

	fixScrollPosition(-item->getHeight(), mWatchIndex >= msgid);
	
	mScrollbox->getContent()->detach(item);
	mItems.erase(msgid);

	refreshScrollContent(); // TODO: executes to many times (we use ranges)
}

void ChatWidget::refreshScrollContent()
{
	float height = 0.0f;

	for (auto [index, item] : mItems)
	{
		item->setY(height);
		height += item->getHeight();
	}

	mScrollbox->getContent()->setHeight(height);
}

void ChatWidget::scrollToBack(bool animated)
{
	if (animated)
		runAction(Actions::Collection::ChangeVerticalScrollPosition(mScrollbox, 1.0f, 0.25f, Easing::CubicOut));
	else
		mScrollbox->setVerticalScrollPosition(1.0f);
}

void ChatWidget::addMessages(int topIndex, int bottomIndex)
{
	assert(CLIENT->isConnected());
	assert(topIndex <= bottomIndex);

	// check range for profiles

	for (int index = topIndex; index <= bottomIndex; index++)
	{
		if (!hasIndexInHistory(index))
			continue;

		if (CLIENT->hasProfile(getProfileIdFromMessage(index)))
			continue;

		return; // discard whole range if no profile in some message
	}

	int count = 0;

	for (int index = topIndex; index <= bottomIndex; index++) 
	{
		if (!hasIndexInHistory(index))
			continue;

		addMessage(index);
		count += 1;
	}
	LOG((std::to_string(count) + " message(s) added").c_str());
}

void ChatWidget::removeMessages(int topIndex, int bottomIndex)
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

void ChatWidget::fixScrollPosition(float height, bool inversed)
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
