#pragma once

#include "screen.h"
#include "plane.h"
#include "player.h"
#include "skin.h"

namespace hcg001
{
	class Gameplay : public Scene::Actionable<Screen>
	{
	private:
		const float MaxFallVelocity = 20.0f;

	public:
		Gameplay(Skin skin);

	protected:
		void touch(Touch type, const glm::vec2& pos) override;
		void update() override;
		
	private:
		void physics(float dTime);
		void camera(float dTime);
		void jump(bool powerjump);
		void downslide();
		void collide(std::shared_ptr<Plane> plane);
		void removeFarPlanes();
		void spawnPlanes();
		void spawnPlane(const glm::vec2& pos, float anim_delay, bool has_ruby, bool powerjump, bool moving);
		void spawnJumpParticles();
		void start();
		void gameover();
		void tap();
		void showRiskLabel(const utf8_string& text);
		void setScore(int count);
		float getDifficulty() const;

	private:
		std::shared_ptr<Scene::Label> mReadyLabel;
		std::shared_ptr<Scene::Node> mGameField;
		std::shared_ptr<Scene::Node> mPlaneHolder;
		std::shared_ptr<Scene::Node> mPlayerTrailHolder;
		std::shared_ptr<Scene::Node> mRectangleParticlesHolder;
		std::shared_ptr<Player> mPlayer;
		std::shared_ptr<Scene::Label> mScoreLabel;
		std::shared_ptr<Scene::Actionable<Scene::Label>> mRiskLabel = nullptr;
		Audio::Sound mClickSound = Audio::Sound(Platform::Asset("sounds/click.wav"));
		std::shared_ptr<Shared::SceneHelpers::RectangleEmitter> mJumpParticles;

	public:
		auto getScore() const { return mScore; }

	private:
		bool mReady = false;
		float mPhysTimeAccumulator = 0.0f;
		glm::vec2 mVelocity = { 0.0f, 0.0f };
		float mMaxY = 0.0f;
		int mScore = 0;
		bool mDownslide = false;
		glm::vec2 mLastPlanePos = { 0.0f, 0.0f };
		bool mCanStart = false;

	public:
		using MoveSkyCallback = std::function<void(const glm::vec2&)>;

	public:
		void setGameoverCallback(std::function<void()> value) { mGameoverCallback = value; }
		void setRubyCallback(std::function<void(std::shared_ptr<Scene::Node>)> value) { mRubyCallback = value; }
		void setMoveSkyCallback(MoveSkyCallback value) { mMoveSkyCallback = value; }

	private:
		std::function<void()> mGameoverCallback = nullptr;
		std::function<void(std::shared_ptr<Scene::Node>)> mRubyCallback = nullptr;
		MoveSkyCallback mMoveSkyCallback = nullptr;
	};
}