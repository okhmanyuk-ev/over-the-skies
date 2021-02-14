#pragma once

#include "window.h"

namespace hcg001
{
	class GuildsWindow : public StandardWindow
	{
	public:
		class Item;

	public:
		GuildsWindow();

	private:
		void createGuildItems(const std::vector<int> ids);
	};

	class GuildsWindow::Item : public Helpers::GuildInfoListenable<Scene::Rectangle>
	{
	public:
		Item(int guildId);

		void refresh(Channel::GuildPtr guild_info);

	private:
		std::shared_ptr<Helpers::Label> mTitle;
	};
}