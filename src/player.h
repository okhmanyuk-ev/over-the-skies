#pragma once

#include <shared/all.h>
#include "skin.h"

namespace hcg001
{
	class Player : public Scene::Actionable<Scene::Node>, public Scene::Color
	{
	public:
		Player(Skin skin);

	public:
		void setSpriteRotation(float value) { mSprite->setRotation(value); }

	protected:
		void update() override;

	private:
		std::shared_ptr<Scene::Sprite> mSprite;
		Skin mSkin;
	};

	class BallPlayer : public Player
	{
	public:
		BallPlayer(std::weak_ptr<Scene::Node> trailHolder);
	};

	class SnowflakePlayer : public Player
	{
	public:
		SnowflakePlayer(std::weak_ptr<Scene::Node> trailHolder);
	};

	class FlamePlayer : public Player
	{
	public:
		FlamePlayer(std::weak_ptr<Scene::Node> trailHolder);
	};

	inline std::shared_ptr<Player> createPlayer(Skin skin, std::weak_ptr<Scene::Node> trailHolder, std::weak_ptr<Scene::Node> spriteParticlesHolder)
	{
		if (skin == Skin::Ball)
			return std::make_shared<BallPlayer>(trailHolder);
		else if (skin == Skin::Snowflake)
			return std::make_shared<SnowflakePlayer>(trailHolder);
		else if (skin == Skin::Flame)
			return std::make_shared<FlamePlayer>(spriteParticlesHolder);
	}
}