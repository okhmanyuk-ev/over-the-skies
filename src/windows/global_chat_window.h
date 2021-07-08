#pragma once

#include "window.h"
#include "chat_widget.h"

namespace hcg001
{
	class GlobalChatWidget : public ChatWidget
	{
	public:
		bool isHistoryEmpty() const override;
		int getFirstHistoryIndex() const override;
		int getLastHistoryIndex() const override;
		bool hasIndexInHistory(int index) const override;
		int getProfileIdFromMessage(int index) const override;
		std::shared_ptr<Scene::Node> createMessageNode(int msgid) const override;
	};

	class GlobalChatWindow : public Window,
		public Common::Event::Listenable<Channel::GlobalChatMessageEvent>
	{
	public:
		GlobalChatWindow();

	private:
		void onEvent(const Channel::GlobalChatMessageEvent& e) override;

	private:
		std::shared_ptr<GlobalChatWidget> mChatWidget;
	};
}