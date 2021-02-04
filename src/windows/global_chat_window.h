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
		void addMessage(int msgid, std::shared_ptr<Channel::ChatMessage> message);
		void refreshScrollContent();
		void scrollToBack(bool animated = true);

	private:
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		std::map<int, std::shared_ptr<Scene::Node>> mItems;
	};
}