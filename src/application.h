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

namespace hcg001
{
	class Application : public Shared::RichApplication,
		public Common::EventSystem::Listenable<Profile::RubiesChangedEvent>
	{
	public:
		Application();
		~Application();

	private:
		void loading(const std::string& stage, float progress) override;
		void initialize() override;
		void frame() override;
		void collectRubyAnim(std::shared_ptr<Scene::Node> ruby);
		void addRubies(int count);
		void tryShowDailyReward();

	private:
		void event(const Profile::RubiesChangedEvent& e) override;

	private:
		std::shared_ptr<Sky> mSky;
		std::shared_ptr<Scene::Node> mHudHolder;
		struct 
		{
			std::shared_ptr<Scene::Sprite> sprite;
			std::shared_ptr<Scene::Label> label;
		} mRubyScore;
		
		std::shared_ptr<Shared::SceneManager> mSceneManager;

	private:
	//	Shared::SceneHelpers::SplashScene mSplashScene = Shared::SceneHelpers::SplashScene(TEXTURE("textures/logo.png"));
		Scene::Scene mGameScene;

	private:
		bool mNodeEditor = false;

	private:
		Shared::SceneEditor mSceneEditor = Shared::SceneEditor(mGameScene);
	};
}
