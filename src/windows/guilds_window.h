#pragma once

#include "window.h"

namespace hcg001
{
	class GuildsWindow : public Window
	{
	public:
		class SearchContent;
		class MyGuildContent;

	public:
		GuildsWindow();

	private:
		void createMyGuildContent();
		void createGuildSearchContent();
	};

	class GuildsWindow::MyGuildContent : public Scene::Node
	{
	public:
		enum class PageType
		{
			Chat,
			Info,
		};

	private:
		class TabButton;
		class TabContent;

	private:
		class ChatContent;
		class InfoContent;

	public:
		MyGuildContent();

	private:
		Helpers::MappedTabsManager<PageType> mTabsManager;
	};

	class GuildsWindow::MyGuildContent::TabButton : public Helpers::Button, public Helpers::TabsManager::Item
	{
	public:
		TabButton(const utf8_string& text);

	public:
		void onJoin() override;
		void onEnter() override;
		void onLeave() override;
	};

	class GuildsWindow::MyGuildContent::TabContent : public Scene::Node, public Helpers::TabsManager::Item
	{
	public:
		void onJoin() override;
		void onEnter() override;
		void onLeave() override;
	};

	class GuildsWindow::MyGuildContent::ChatContent : public GuildsWindow::MyGuildContent::TabContent
	{
	public:
		ChatContent();
	};

	class GuildsWindow::MyGuildContent::InfoContent : public GuildsWindow::MyGuildContent::TabContent
	{
	private:
		class Member;

	public:
		InfoContent();
	};

	class GuildsWindow::MyGuildContent::InfoContent::Member : public Scene::Rectangle
	{
	public:
		static inline const float VerticalMargin = 8.0f;

	public:
		Member(int num, int uid);
	};

	class GuildsWindow::SearchContent : public Scene::Node
	{
	private:
		class Item;

	public:
		SearchContent();

	private:
		void createGuildItems(const std::vector<int> ids);
	};

	class GuildsWindow::SearchContent::Item : public Helpers::GuildInfoListenable<Scene::Rectangle>
	{
	public:
		Item(int guildId);

		void refresh(Channel::GuildPtr guild_info);

	private:
		std::shared_ptr<Helpers::Label> mTitle;
		std::shared_ptr<Helpers::Label> mMembersLabel;
	};
}