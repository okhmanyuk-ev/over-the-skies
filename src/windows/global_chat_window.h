#pragma once

#include "window.h"

namespace hcg001
{
	class GlobalChatWindow : public StandardWindow,
		public Common::Event::Listenable<Channel::GlobalChatMessageEvent>
	{
	public:
		GlobalChatWindow();

	private:
		void onEvent(const Channel::GlobalChatMessageEvent& e) override;

	private:
		void refreshMessages();
		void addMessage(int msgid, std::shared_ptr<Channel::ChatMessage> message);
		void removeMessage(int msgid);
		void refreshScrollContent();
		void scrollToBack(bool animated = true);
		void addMessages(int topIndex, int bottomIndex);
		void removeMessages(int topIndex, int bottomIndex);
		void fixScrollPosition(float height);

	private:
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		std::map<int, std::shared_ptr<Scene::Node>> mItems;
		int mWatchIndex = 0;
	};
}