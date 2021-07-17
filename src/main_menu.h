#pragma once

#include "screen.h"
#include "skin.h"
#include <common/timestep_fixer.h>
#include "helpers.h"

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

	protected:
		void onEnterBegin() override;

	private:
		void refresh();
		void menuPhysics(float dTime);
		std::vector<std::shared_ptr<Scene::Node>> createScrollItems();

	public:
		auto getRubiesIndicator() const { return mRubiesIndicator; }

	private:
		Skin mChoosedSkin = Skin::Ball;
		std::shared_ptr<Scene::Node> mScrollTarget = nullptr;
		std::vector<std::shared_ptr<Scene::Node>> mItems;
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		bool mDecideButtons = false;
		bool mButtonsAnimating = false;
		bool mPlayButtonVisible = false;
		std::map<Skin, std::shared_ptr<Scene::Node>> mSkinItems;
		std::shared_ptr<Helpers::RubiesIndicator> mRubiesIndicator = nullptr;
	};
}