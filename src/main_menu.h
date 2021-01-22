#pragma once

#include "screen.h"
#include "skin.h"
#include "buy_skin_menu.h"
#include <common/timestep_fixer.h>

namespace hcg001
{
	class MainMenu : public Screen
	{
	private:
		const float ItemSize = 96.0f;
		const float ItemSpaceBetween = 16.0f;
		const float ScrollPadding = 48.0f;
		const float SlotWidth = ItemSize + ItemSpaceBetween;

	public:
		MainMenu();

	public:
		void setStartCallback(std::function<void()> value) { mStartCallback = value; }

	private:
		void refresh();
		void menuPhysics(float dTime);
		std::vector<std::shared_ptr<Scene::Node>> createScrollItems();

	private:
		Skin mChoosedSkin = Skin::Ball;
		std::shared_ptr<Scene::Node> mScrollTarget = nullptr;
		std::function<void()> mStartCallback = nullptr;
		std::vector<std::shared_ptr<Scene::Node>> mItems;
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		bool mDecideButtons = false;
		bool mButtonsAnimating = false;
		bool mPlayButtonVisible = false;
	};
}