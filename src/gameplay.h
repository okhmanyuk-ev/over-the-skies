#pragma once

#include "screen.h"
#include "plane.h"
#include "player.h"
#include "skin.h"
#include "helpers.h"
#include <common/timestep_fixer.h>

namespace hcg001
{
	class Gameplay : public Scene::Tappable<Screen>,
		public Common::Event::Listenable<Platform::Input::Keyboard::Event>
	{
	public:
		enum class PlaneBonus
		{
			Ruby,
			Magnet
		};

		enum class JumpType
		{
			Normal,
			PowerJump,
			FallJump
		};

	private:
		const float MaxFallVelocity = 20.0f;

	public:
		Gameplay();

	protected:
		void onTap() override;
		void onEvent(const Platform::Input::Keyboard::Event& e) override;
		void update(Clock::Duration delta) override;
		
	private:
		void physics(float dTime);
		void camera(Clock::Duration dTime);
		void jump(JumpType jump_type);
		void downslide();
		void collide(std::shared_ptr<Plane> plane);
		void removeFarPlanes();
		void spawnPlanes();
		void spawnPlane(const glm::vec2& pos, float anim_delay, std::optional<PlaneBonus> bonus, bool powerjump, bool moving);
		void spawnJumpParticles();
		void start();
		void gameover();
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
		std::shared_ptr<Scene::Label> mRiskLabel = nullptr;
		std::shared_ptr<Scene::Emitter> mJumpParticles;
		std::shared_ptr<Helpers::RubiesIndicator> mRubiesIndicator;
		std::shared_ptr<Scene::Rectangle> mBonusesHolder;

	public:
		auto getScore() const { return mScore; }

	private:
		bool mReady = false;
		glm::vec2 mVelocity = { 0.0f, 0.0f };
		float mMaxY = 0.0f;
		int mScore = 0;
		bool mDownslide = false;
		glm::vec2 mLastPlanePos = { 0.0f, 0.0f };
		bool mCanStart = false;
		bool mGameovered = false;
		int mRubiesCollected = 0;
	};
}
