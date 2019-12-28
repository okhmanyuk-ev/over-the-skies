#pragma once

#include <Shared/all.h>

namespace hcg001
{
	class Sky : public Scene::Actionable<Scene::Node>
	{
	public:
		Sky();

	private:
		void spawnAsteroid(float speed, float normalized_spread);
		void placeStarsToHolder(std::shared_ptr<Scene::Node> holder);

	public:
		void changeColor();

	protected:
		void draw() override;

	private:
		std::shared_ptr<Scene::Node> mStarsHolder1;
		std::shared_ptr<Scene::Node> mStarsHolder2;
		std::shared_ptr<Scene::Node> mAsteroidsHolder;

	private:
		std::shared_ptr<Scene::Color> mTopColor = std::make_shared<Scene::Color>();
		std::shared_ptr<Scene::Color> mBottomColor = std::make_shared<Scene::Color>();
	};
}