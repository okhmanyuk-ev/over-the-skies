#include "plane.h"

using namespace hcg001;

Plane::Plane()
{
	auto middle = std::make_shared<Scene::Sprite>();
	middle->setTexture(TEXTURE("textures/plane_middle.png"));
	middle->setStretch({ 1.0f, 1.0f });
	attach(middle);

	auto left = std::make_shared<Scene::Sprite>();
	left->setTexture(TEXTURE("textures/plane_left.png"));
	left->setPivot({ 0.0f, 0.5f });
	left->setAnchor({ 0.0f, 0.5f });
	attach(left);

	auto right = std::make_shared<Scene::Sprite>();
	right->setTexture(TEXTURE("textures/plane_right.png"));
	right->setPivot({ 1.0f, 0.5f });
	right->setAnchor({ 1.0f, 0.5f });
	attach(right);

	runAction(Shared::ActionHelpers::ExecuteInfinite([this, middle, left, right] {
		auto color = getColor();

		middle->setColor(color);
		left->setColor(color);
		right->setColor(color);

		auto height = getHeight();

		left->setScale(height / left->getHeight());
		right->setScale(height / right->getHeight());

		middle->setHorizontalMargin(left->getWidth() + right->getWidth());
		middle->setHorizontalPosition(left->getWidth());

		if (mRuby) 
		{
			mRuby->setColor(color);
		}
	}));
}