#pragma once

#include <shared/all.h>
#include "skin.h"

namespace hcg001
{
	class Player : public Scene::Actionable<Scene::Node>, public Scene::Color
	{
	public:
		Player(Skin skin);

	protected:
		void makeSprite();

	public:
		void setSpriteRotation(float value) { mSprite->setRotation(value); }

	protected:
		void update() override;

	private:
		std::shared_ptr<Scene::Sprite> mSprite = nullptr;
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

	class BasketballPlayer : public Player
	{
	public:
		BasketballPlayer(std::weak_ptr<Scene::Node> trailHolder);
	};

	class FootballPlayer : public Player
	{
	public:
		FootballPlayer(std::weak_ptr<Scene::Node> trailHolder);
	};

	class VynilPlayer : public Player
	{
	public:
		VynilPlayer(std::weak_ptr<Scene::Node> trailHolder);
	};

	inline std::shared_ptr<Player> createPlayer(Skin skin, std::weak_ptr<Scene::Node> trailHolder)
	{
		if (skin == Skin::Ball)
			return std::make_shared<BallPlayer>(trailHolder);
		else if (skin == Skin::Snowflake)
			return std::make_shared<SnowflakePlayer>(trailHolder);
		else if (skin == Skin::Basketball)
			return std::make_shared<BasketballPlayer>(trailHolder);
		else if (skin == Skin::Football)
			return std::make_shared<FootballPlayer>(trailHolder);
		else if (skin == Skin::Vynil)
			return std::make_shared<VynilPlayer>(trailHolder);
		else
			assert(false);
	}
}