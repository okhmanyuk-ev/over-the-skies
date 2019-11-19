#pragma once

#include <Shared/all.h>

namespace hcg001
{	
	class Plane : public Scene::Actionable<Scene::Sprite>
	{
	public:
		Plane()
		{
			//auto middle = std::make_shared<Scene::Sprite>();
			setTexture(TEXTURE("textures/plane_middle.png"));
			//setStretch({ 1.0f, 1.0f });
			//attach(middle);

	/*		const float CornerWidth = 8.0f;

			auto left = std::make_shared<Scene::Sprite>();
			left->setTexture(GET_CACHED_TEXTURE("textures/plane_left.png"));
			left->setAnchor({ 0.0f, 0.5f });
			left->setPivot({ 1.0f, 0.5f });
			left->setVerticalStretch(1.0f);
			left->setWidth(CornerWidth);
			middle->attach(left);

			auto right = std::make_shared<Scene::Sprite>();
			right->setTexture(GET_CACHED_TEXTURE("textures/plane_right.png"));
			right->setAnchor({ 1.0f, 0.5f });
			right->setPivot({ 0.0f, 0.5f });
			right->setVerticalStretch(1.0f);
			right->setWidth(CornerWidth);
			middle->attach(right);*/
		}

	public:
		std::shared_ptr<Scene::Node> ruby = nullptr;

	private:
	//	std::shared_ptr<Scene::Sprite> mMiddle;
	};
}