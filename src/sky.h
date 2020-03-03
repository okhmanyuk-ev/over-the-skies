#pragma once

#include <Shared/all.h>

namespace hcg001
{
	class Sky : public Scene::Actionable<Scene::Rectangle>
	{
	public:
		Sky();

	private:
		void spawnAsteroid(float speed, float normalized_spread);
		void placeStarsToHolder(std::shared_ptr<Scene::Node> holder);

	public:
		void changeColor();
		void moveSky(const glm::vec2& offset);

	protected:
		void update() override;

	private:
		std::shared_ptr<Scene::BloomLayer> mBloomLayer;
		std::shared_ptr<Scene::Node> mStarsBottomLeft;
		std::shared_ptr<Scene::Node> mStarsTopLeft;
		std::shared_ptr<Scene::Node> mStarsBottomRight;
		std::shared_ptr<Scene::Node> mStarsTopRight;
		std::shared_ptr<Scene::Node> mAsteroidsHolder;

	private:
		std::shared_ptr<Scene::Color> mTopColor = std::make_shared<Scene::Color>();
		std::shared_ptr<Scene::Color> mBottomColor = std::make_shared<Scene::Color>();
		glm::vec2 mLastPos = { 0.0f, 0.0f };
	};
}