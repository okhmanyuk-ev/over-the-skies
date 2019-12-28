#pragma once

#include <Shared/all.h>
#include "defines.h"
#include "main_menu.h"
#include "gameplay_screen.h"
#include "gameover_menu.h"
#include "sky.h"
#include "player.h"
#include "plane.h"
#include "profile.h"
#include "skin.h"

namespace hcg001
{
	class Application : public Shared::RichApplication
	{
	public:
		Application();
		~Application();

	private:
		void makeLoadingScene();
		void loading(const std::string& stage, float process) override;
		void initialize() override;
		void frame() override;
		void collectRubyAnim(std::shared_ptr<Scene::Node> ruby);

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
		Scene::Scene mLoadingScene;
		Scene::Scene mGameScene;

	private:
		bool mNodeEditor = false;

	private:
		Shared::LocalizationSystem mLocalization = Shared::LocalizationSystem("localization");
		Profile mProfile;
		Shared::SceneEditor mSceneEditor = Shared::SceneEditor(mGameScene);
	};
}
