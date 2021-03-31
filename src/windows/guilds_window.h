#pragma once

#include "window.h"

namespace hcg001
{
	class GuildsWindow : public StandardWindow
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
		MyGuildContent();
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