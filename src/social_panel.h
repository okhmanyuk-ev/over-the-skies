#pragma once

#include <optional>
#include <shared/all.h>
#include "helpers.h"
#include "client.h"

namespace hcg001
{
	class TabsManager
	{
	public:
		class Item;

	public:
		void addContent(int type, std::shared_ptr<Item> node);
		void addButton(int type, std::shared_ptr<Item> node);
		void show(int type);

	public:
		const auto& getContents() const { return mContents; }

	private:
		std::map<int, std::shared_ptr<Item>> mContents;
		std::map<int, std::shared_ptr<Item>> mButtons;
		std::optional<int> mCurrentPage;
	};

	class TabsManager::Item
	{
	public:
		virtual void onJoin() = 0;
		virtual void onEnter() = 0;
		virtual void onLeave() = 0;
	};

	class SocialPanel : public Scene::Node
	{
	public:
		enum class PageType
		{
			Highscores,
			TopGuilds,
		};

	private:
		class TabButton;
		class TabContent;
		class HighscoresPage;
        class TopGuildsPage;

	public:
		SocialPanel();

	private:
		TabsManager mTabsManager;
	};

	class SocialPanel::TabButton : public Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::Rectangle>>, 
		public TabsManager::Item, public std::enable_shared_from_this<SocialPanel::TabButton>
	{
	public:
		TabButton();

	public:
		void onJoin() override;
		void onEnter() override;
		void onLeave() override;

	private:
		std::shared_ptr<Scene::Rectangle> mCheckbox;
	};

	class SocialPanel::TabContent : public Scene::Node, public TabsManager::Item, public std::enable_shared_from_this<SocialPanel::TabContent>
	{
	public:
		void onJoin() override;
		void onEnter() override;
		void onLeave() override;

	public:
		virtual Actions::Collection::UAction getScenario() = 0;
	};

	class SocialPanel::HighscoresPage : public SocialPanel::TabContent,
		public Common::Event::Listenable<NetEvents::HighscoresEvent>
	{
	public:
        HighscoresPage();

	public:
		Actions::Collection::UAction getScenario() override;

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

    class SocialPanel::TopGuildsPage : public SocialPanel::TabContent,
        public Common::Event::Listenable<NetEvents::GuildsTopEvent>
    {
    public:
        TopGuildsPage();

	public:
		Actions::Collection::UAction getScenario() override;

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
