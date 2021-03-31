#pragma once

#include <shared/all.h>
#include "client.h"

namespace hcg001
{
	class ChatWidget : public Scene::ClippableScissor<Scene::Node>
	{
	public:
		ChatWidget();

	public:
		void message(int msgid);
		
	public:
		virtual bool isHistoryEmpty() const = 0;
		virtual int getFirstHistoryIndex() const = 0;
		virtual int getLastHistoryIndex() const = 0;
		virtual bool hasIndexInHistory(int index) const = 0;
		virtual int getProfileIdFromMessage(int index) const = 0;
		virtual std::shared_ptr<Scene::Node> createMessageNode(int msgid) const = 0;

	private:
		void refreshMessages();
		void addMessage(int msgid);
		void removeMessage(int msgid);
		void refreshScrollContent();
		void scrollToBack(bool animated = true);
		void addMessages(int topIndex, int bottomIndex);
		void removeMessages(int topIndex, int bottomIndex);
		void fixScrollPosition(float height, bool inversed);

	public:
		auto getScrollbox() const { return mScrollbox; }

	private:
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		std::map<int, std::shared_ptr<Scene::Node>> mItems;
		int mWatchIndex = 0;
		float mScrollVerticalSpace = 0.0f;
	};
}
