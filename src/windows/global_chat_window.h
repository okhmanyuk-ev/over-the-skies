#pragma once

#include "window.h"

namespace hcg001
{
	class GlobalChatWindow : public StandardWindow
	{
	public:
		GlobalChatWindow();

	private:
		void addItem(const utf8_string& text);
		void refreshScrollContent();

	private:
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		std::map<int, std::shared_ptr<Scene::Node>> mItems;
	};
}