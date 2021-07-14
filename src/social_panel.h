#pragma once

#include <optional>
#include <shared/all.h>
#include "helpers.h"
#include "client.h"

namespace hcg001
{
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
		Helpers::MappedTabsManager<PageType> mTabsManager;
	};

	class SocialPanel::TabButton : public Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::Rectangle>>, 
		public Helpers::TabsManager::Item, public std::enable_shared_from_this<SocialPanel::TabButton>
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

	class SocialPanel::TabContent : public Scene::Node, public Helpers::TabsManager::Item, public std::enable_shared_from_this<SocialPanel::TabContent>
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
		void refresh();

    private:
        std::shared_ptr<Scene::ClippableStencil<Scene::Rectangle>> mBackground;
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		std::shared_ptr<Scene::Node> mGrid;

	private:
		NetEvents::GuildsTopEvent mTopGuilds;
    };
}
