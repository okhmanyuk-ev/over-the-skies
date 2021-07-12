#pragma once

#include <optional>
#include <shared/all.h>
#include "helpers.h"
#include "client.h"

namespace hcg001
{
    class PagesManager;

	class SocialPanel : public Scene::Node
	{
	public:
		enum class PageType
		{
			Highscores,
			TopGuilds,
		};

	private:
        class HighscoresPage;
        class TopGuildsPage;

	public:
		SocialPanel();

	private:
		using TabButton = Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::Rectangle>>;

		std::shared_ptr<TabButton> createTabButton(const utf8_string& text);

        void showPage(PageType type);
        
	private:
		std::shared_ptr<Scene::Scrollbox> mTabButtonScrollbox;
		std::map<PageType, std::shared_ptr<Scene::Node>> mTabButtons;
        std::map<PageType, std::shared_ptr<Scene::Node>> mTabContents;
	};

	class SocialPanel::HighscoresPage : public Scene::Node,
		public Common::Event::Listenable<NetEvents::HighscoresEvent>
	{
	public:
        HighscoresPage();

	private:
		void onEvent(const NetEvents::HighscoresEvent& e) override;

	private:
		void refresh();

	private:
		std::shared_ptr<Scene::ClippableStencil<Scene::Rectangle>> mBackground;
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		std::shared_ptr<Scene::Node> mGrid;

	private:
		NetEvents::HighscoresEvent mHighscores;
	};

    class SocialPanel::TopGuildsPage : public Scene::Node,
        public Common::Event::Listenable<NetEvents::GuildsTopEvent>
    {
    public:
        TopGuildsPage();

    private:
        void onEvent(const NetEvents::GuildsTopEvent& e) override;

    private:
        std::shared_ptr<Scene::ClippableStencil<Scene::Rectangle>> mBackground;
    };

	// class who can manage pages
	// void addPage(std::shared_ptr<Page>)
	// void removePage(..same)
	// void showPage(int) (or choosePage)
	// int getPagesCount()
	


	// page class
	// onEnter
	// onLeave
}
