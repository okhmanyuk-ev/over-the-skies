#pragma once

#include <Shared/all.h>
#include "screen.h"
#include "skin.h"
#include "profile.h"

namespace hcg001
{
	class BuySkinMenu : public Scene::Actionable<Shared::SceneManager::Screen>
	{
	public:
		BuySkinMenu(Skin skin);

	protected:	
		void onLeaveBegin() override;
		
		std::unique_ptr<Common::Actions::Action> createEnterAction() override;
		std::unique_ptr<Common::Actions::Action> createLeaveAction() override;

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
