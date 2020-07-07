#pragma once

#include <shared/all.h>
#include "defines.h"
#include "main_menu.h"
#include "gameplay.h"
#include "gameover_menu.h"
#include "sky.h"
#include "player.h"
#include "plane.h"
#include "skin.h"
#include "profile.h"
#include "cheats.h"
#include "daily_reward_window.h"
#include "helpers.h"
#include "hud.h"

namespace hcg001
{
	class Application : public Shared::RichApplication,
		public Common::FrameSystem::Frameable
	{
	public:
		Application();
		~Application();

	private:
		void initialize();
		void frame() override;
		void addRubies(int count);
		void tryShowDailyReward();
		void adaptToScreen(std::shared_ptr<Scene::Node> node);

	private:
		std::shared_ptr<Sky> mSky;
		
	private:
		Scene::Scene mGameScene;

	private:
		Shared::SceneEditor mSceneEditor = Shared::SceneEditor(mGameScene);
	};
}
