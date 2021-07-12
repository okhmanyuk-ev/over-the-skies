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
			TopScores,
			TopRubies,
			MyScores,
			MyRubies
		};

	public:
		class Page;

	public:
		SocialPanel();

	private:
		using TabButton = Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::Rectangle>>;

		std::shared_ptr<TabButton> createTabButton(const utf8_string& text);

	private:
		std::shared_ptr<Scene::Scrollbox> mTabButtonScrollbox;
		//std::map<PageType, std::shared_ptr<Scene::Node>> mTabButtons;


		struct PageData
		{
			std::shared_ptr<Scene::Node> button;
			std::shared_ptr<Scene::Node> content;
		};
		std::map<PageType, PageData> mPages;
	};

	class SocialPanel::Page : public Scene::Node,
		public Common::Event::Listenable<NetEvents::HighscoresEvent>
	{
	public:
		Page();

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


	// class who can manage pages
	// void addPage(std::shared_ptr<Page>)
	// void removePage(..same)
	// void showPage(int) (or choosePage)
	// int getPagesCount()
	


	// page class
	// onEnter
	// onLeave
}