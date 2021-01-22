#pragma once

#include <shared/all.h>
#include "screen.h"
#include "skin.h"

namespace hcg001
{
	class BuySkinMenu : public Shared::SceneManager::Screen
	{
	public:
		BuySkinMenu(Skin skin);

	protected:	
		void onLeaveBegin() override;
		
		std::unique_ptr<Actions::Action> createEnterAction() override;
		std::unique_ptr<Actions::Action> createLeaveAction() override;

	private:
		std::shared_ptr<Scene::Label> mTitle = nullptr;
		std::shared_ptr<Scene::Sprite> mImage = nullptr;
		std::shared_ptr<Shared::SceneHelpers::FastButton> mBuyButton = nullptr;
		std::shared_ptr<Shared::SceneHelpers::FastButton> mCancelButton = nullptr;

	public:
		void setExitCallback(std::function<void()> value) { mExitCallback = value; }

	private:
		std::function<void()> mExitCallback = nullptr;
	};
}
