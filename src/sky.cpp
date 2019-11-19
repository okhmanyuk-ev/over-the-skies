#include "sky.h"

#include <random>

using namespace hcg001;

Sky::Sky()
{
	setStretch({ 1.0f, 1.0f });
	mTopColor->setColor(Graphics::Color::Black);
	mBottomColor->setColor(Graphics::Color::Black);

	runAction(Shared::ActionHelpers::ExecuteInfinite([this] {
		auto top = mTopColor->getColor() * 255.0f;
		auto bottom = mBottomColor->getColor() * 255.0f;

		GAME_STATS("sky top color",
			std::to_string(static_cast<int>(top.r)) + " " +
			std::to_string(static_cast<int>(top.g)) + " " +
			std::to_string(static_cast<int>(top.b)));

		GAME_STATS("sky bottom color",
			std::to_string(static_cast<int>(bottom.r)) + " " +
			std::to_string(static_cast<int>(bottom.g)) + " " +
			std::to_string(static_cast<int>(bottom.b)));
	}));
}

void Sky::changeColor(glm::vec3 top, glm::vec3 bottom)
{
	const float ChangeDuration = 2.5f;

	runAction(Shared::ActionHelpers::ChangeColor(mTopColor, top, ChangeDuration, Common::Easing::QuadraticInOut));
	runAction(Shared::ActionHelpers::ChangeColor(mBottomColor, bottom, ChangeDuration, Common::Easing::QuadraticInOut));
}

void Sky::draw()
{
	auto model = glm::scale(getTransform(), { getSize(), 1.0f });

	std::vector<Renderer::Vertex::PositionColor> vertices = {
		{ { 0.0f, 0.0f, 0.0f }, { mTopColor->getColor() } },
		{ { 0.0f, 1.0f, 0.0f }, { mBottomColor->getColor() } },
		{ { 1.0f, 1.0f, 0.0f }, { mBottomColor->getColor() } },
		{ { 1.0f, 0.0f, 0.0f }, { mTopColor->getColor() } }
	};

	static const std::vector<uint32_t> indices = { 0, 1, 2, 0, 2, 3 };

	GRAPHICS->draw(Renderer::Topology::TriangleList, vertices, indices, model);

	Node::draw();
}