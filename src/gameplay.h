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
		const glm::vec2 PlayerSize = { 18.0f, 18.0f };
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
		void spawnParticle(const glm::vec2& pos, std::shared_ptr<Renderer::Texture> texture, const glm::vec3& color = Graphics::Color::White);
		void spawnCrashParticles(const glm::vec2& pos);
		void start();
		void gameover();
		void setupTrail(Skin skin);
		void tap();
		void showRiskLabel(const utf8_string& text);

	private:
		std::shared_ptr<Scene::Label> mReadyLabel;
		std::shared_ptr<Scene::Node> mGameField;
		std::shared_ptr<Scene::Node> mPlaneHolder;
		std::shared_ptr<Scene::Node> mParticlesHolder;
		std::shared_ptr<Player> mPlayer;
		std::shared_ptr<Scene::Label> mScoreLabel;
		std::shared_ptr<Scene::Actionable<Scene::Label>> mRiskLabel = nullptr;
		Audio::Sound mClickSound = Audio::Sound(Platform::Asset("sounds/click.wav"));

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