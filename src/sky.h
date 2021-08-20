#pragma once

#include <shared/all.h>

namespace hcg001
{
	class Sky : public Scene::Rectangle
	{
	public:
		Sky();

	private:
		void spawnAsteroid(float speed, float normalized_spread);
		void placeStarsToHolder(std::shared_ptr<Scene::Node> holder);

	public:
		void changeColor(float top_hue = -1.0f, float bottom_hue = -1.0f);
		void moveSky(const glm::vec2& offset);

	private:
		std::shared_ptr<Scene::BloomLayer> mBloomLayer;
		std::shared_ptr<Scene::Node> mStarsBottomLeft;
		std::shared_ptr<Scene::Node> mStarsTopLeft;
		std::shared_ptr<Scene::Node> mStarsBottomRight;
		std::shared_ptr<Scene::Node> mStarsTopRight;
		std::shared_ptr<Scene::Node> mAsteroidsHolder;

	private:
		glm::vec2 mLastPos = { 0.0f, 0.0f };
	};
}