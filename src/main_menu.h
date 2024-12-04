#pragma once

#include "screen.h"
#include "skin.h"
#include <common/timestep_fixer.h>
#include "helpers.h"

namespace hcg001
{
	class MainMenu : public Screen, public std::enable_shared_from_this<MainMenu>
	{
	private:
		const glm::vec2 ItemSize = { 96.0f + 16.0f, 96.0f + 48.0f };

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
		std::vector<std::shared_ptr<Scene::Node>> mItems;
		std::shared_ptr<Scene::Scrollbox> mScrollbox;
		bool mDecideButtons = false;
		bool mButtonsAnimating = false;
		bool mPlayButtonVisible = false;
		std::shared_ptr<Helpers::RubiesIndicator> mRubiesIndicator = nullptr;
	};
}