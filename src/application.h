#pragma once

#include <Shared/all.h>
#include "defines.h"
#include "main_menu.h"
#include "ready_menu.h"
#include "gameover_menu.h"
#include "sky.h"
#include "player.h"
#include "plane.h"
#include "profile.h"
#include "skin.h"

namespace hcg001
{
	class Application : public Shared::RichApplication,
		public Common::EventSystem::Listenable<Shared::TouchEmulator::Event>
	{
	private:
		const glm::vec2 PlaneSize = { 72.0f, 8.0f };
		const glm::vec2 PlayerSize = { 18.0f, 18.0f };
		const float GameFieldWidth = 360.0f;

	public:
		Application();
		~Application();

	private:
		void makeLoadingScene();
		void loading(const std::string& stage, float process) override;
		void initialize() override;
		void frame() override;
		void physics(float dTime);
		void collide(std::shared_ptr<Plane> plane);
		void ready();
		void start();
		void gameover();
		void jump();
		void slide(float distance);
		void spawnPlane(const glm::vec2& pos, float anim_delay);
		void removeFarPlanes();
		void clearField(std::function<void()> callback);
		void spawnParticle(const glm::vec2& pos, std::shared_ptr<Renderer::Texture> texture, const glm::vec3& color = Graphics::Color::White);
		void placeStarsToHolder(std::shared_ptr<Scene::Node> holder);
		void changeSkyColor();
		void spawnPlanes();
		
	private:
		void event(const Shared::TouchEmulator::Event& e) override;

	private:
		std::shared_ptr<Sky> mSky = std::make_shared<Sky>();
		std::shared_ptr<Player> mPlayer = std::make_shared<Player>();
		std::shared_ptr<Scene::Trail> mTrail = nullptr;
		std::shared_ptr<Scene::Node> mGameField = std::make_shared<Scene::Node>();
		std::shared_ptr<Scene::Node> mHudHolder = std::make_shared<Scene::Node>();
		std::shared_ptr<Scene::Node> mPlaneHolder = std::make_shared<Scene::Node>();
		std::shared_ptr<Scene::Label> mScoreLabel = std::make_shared<Scene::Label>();
		struct 
		{
			std::shared_ptr<Scene::Sprite> sprite = std::make_shared<Scene::Sprite>();
			std::shared_ptr<Scene::Label> label = std::make_shared<Scene::Label>();
		} mRubyScore;
		std::shared_ptr<Scene::Node> mParticlesHolder = std::make_shared<Scene::Node>();
		
		std::shared_ptr<Scene::BloomLayer> mBloomLayer = std::make_shared<Scene::BloomLayer>();
		std::shared_ptr<Scene::Node> mStarsHolder1 = std::make_shared<Scene::Node>();
		std::shared_ptr<Scene::Node> mStarsHolder2 = std::make_shared<Scene::Node>();
		
		std::shared_ptr<Shared::SceneManager> mSceneManager;

		std::shared_ptr<MainMenu> mMainMenu;
		std::shared_ptr<ReadyMenu> mReadyMenu;
		std::shared_ptr<GameoverMenu> mGameoverMenu;

	private:
		Scene::Scene mLoadingScene;
		Scene::Scene mGameScene;

	private:
		bool mNodeEditor = false;
		bool mStarted = false;
		glm::vec2 mVelocity = { 0.0f, 0.0f };
		float mTouchPos = 0.0f;
		bool mTouching = false;
		float mMaxY = 0.0f;
		float mTimeAccumulator = 0.0f;
		int mScore = 0;

	private:
		Shared::LocalizationSystem mLocalization = Shared::LocalizationSystem("localization");
		Profile mProfile;
		Shared::SceneEditor mSceneEditor = Shared::SceneEditor(mGameScene);
	};
}
