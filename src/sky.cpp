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

	mBloomLayer = std::make_shared<Scene::BloomLayer>();
	mBloomLayer->setStretch(1.0f);
	mBloomLayer->setDownscaleFactor(2.0f);
	mBloomLayer->setGlowIntensity(2.0f);
	mBloomLayer->setBrightThreshold(0.0f);
	attach(mBloomLayer);

	CONSOLE->registerCVar("r_bloom_enabled", { "bool" }, CVAR_GETTER_BOOL_FUNC(mBloomLayer->isPostprocessEnabled),
		CVAR_SETTER_BOOL_FUNC(mBloomLayer->setPostprocessEnabled));

	CONSOLE->registerCVar("r_bloom_blur_passes", { "int" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getBlurPasses),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setBlurPasses));

	CONSOLE->registerCVar("r_bloom_glow_intensity", { "float" }, CVAR_GETTER_FLOAT_FUNC(mBloomLayer->getGlowIntensity),
		CVAR_SETTER_FLOAT_FUNC(mBloomLayer->setGlowIntensity));

	CONSOLE->registerCVar("r_bloom_downscale_factor", { "float" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getDownscaleFactor),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setDownscaleFactor));

	// stars holder

	mStarsBottomLeft = std::make_shared<Scene::Node>();
	mStarsBottomLeft->setStretch({ 1.0f, 1.0f });
	mStarsBottomLeft->setAnchor({ 0.0f, 1.0f });
	mStarsBottomLeft->setPivot({ 0.0f, 1.0f });
	mBloomLayer->attach(mStarsBottomLeft);

	mStarsTopLeft = std::make_shared<Scene::Node>();
	mStarsTopLeft->setStretch({ 1.0f, 1.0f });
	mStarsTopLeft->setAnchor({ 0.0f, 0.0f });
	mStarsTopLeft->setPivot({ 0.0f, 1.0f });
	mBloomLayer->attach(mStarsTopLeft);

	mStarsBottomRight = std::make_shared<Scene::Node>();
	mStarsBottomRight->setStretch({ 1.0f, 1.0f });
	mStarsBottomRight->setAnchor({ 1.0f, 1.0f });
	mStarsBottomRight->setPivot({ 0.0f, 1.0f });
	mBloomLayer->attach(mStarsBottomRight);

	mStarsTopRight = std::make_shared<Scene::Node>();
	mStarsTopRight->setStretch({ 1.0f, 1.0f });
	mStarsTopRight->setAnchor({ 1.0f, 0.0f });
	mStarsTopRight->setPivot({ 0.0f, 1.0f });
	mBloomLayer->attach(mStarsTopRight);

	placeStarsToHolder(mStarsBottomLeft);
	placeStarsToHolder(mStarsTopLeft);
	placeStarsToHolder(mStarsBottomRight);
	placeStarsToHolder(mStarsTopRight);

	// asteroids

	mAsteroidsHolder = std::make_shared<Scene::Node>();
	mAsteroidsHolder->setStretch({ 1.0f, 1.0f });
	mBloomLayer->attach(mAsteroidsHolder);

	runAction(Shared::ActionHelpers::RepeatInfinite([this] {
		auto delay = glm::linearRand(5.0f, 20.0f);
		return Shared::ActionHelpers::Delayed(delay,
			Shared::ActionHelpers::Insert([this] {
				auto seq = Shared::ActionHelpers::MakeSequence();
				auto global_spread = glm::linearRand(0.0f, 1.0f);
				auto speed = glm::linearRand(256.0f + 128.0f, 512.0f + 256.0f);
				auto count = glm::linearRand(1, 3);
				for (int i = 0; i < count; i++)
				{
					auto local_spread = glm::linearRand(-0.125f, 0.125f);
					seq->add(Shared::ActionHelpers::Delayed(glm::linearRand(0.0f, 0.25f), 
						Shared::ActionHelpers::Execute([this, speed, global_spread, local_spread] {
							spawnAsteroid(speed, global_spread + local_spread);
						})
					));
				}
				return std::move(seq);
			})
		);
	}));
}

void Sky::changeColor()
{
	auto top_hsv = glm::vec3(glm::linearRand(0.0f, 360.0f), 0.75f, 0.125f);
	auto bottom_hsv = glm::vec3(glm::linearRand(0.0f, 360.0f), 0.25f, 0.5f);
	auto top = glm::rgbColor(top_hsv);
	auto bottom = glm::rgbColor(bottom_hsv);

	const float ChangeDuration = 2.5f;

	runAction(Shared::ActionHelpers::ChangeColor(mTopColor, top, ChangeDuration, Common::Easing::QuadraticInOut));
	runAction(Shared::ActionHelpers::ChangeColor(mBottomColor, bottom, ChangeDuration, Common::Easing::QuadraticInOut));
}

void Sky::update()
{
	Scene::Actionable<Scene::Rectangle>::update();
	setVerticalGradient(mTopColor->getColor(), mBottomColor->getColor());
}

void Sky::spawnAsteroid(float speed, float normalized_spread)
{
	auto asteroid = std::make_shared<Scene::Actionable<Scene::Rectangle>>();
	asteroid->setPivot({ 0.5f, 0.5f });
	asteroid->setAnchor({ 0.25f + normalized_spread, 0.0f });
	asteroid->setSize(2.0f);
	asteroid->setPosition(-mAsteroidsHolder->getPosition());

	auto trail = std::make_shared<Scene::Trail>(mAsteroidsHolder);
	trail->setStretch(1.0f);
	trail->setLifetime(0.25f);
	asteroid->attach(trail);

	const float Duration = 5.0f;
	const glm::vec2 Direction = { -0.75f, 1.0f };

	asteroid->runAction(Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::ChangePositionByDirection(asteroid, Direction, speed, Duration),
		Shared::ActionHelpers::Kill(asteroid)
	));

	mAsteroidsHolder->attach(asteroid);
}

void Sky::placeStarsToHolder(std::shared_ptr<Scene::Node> holder)
{
	Common::Actions::Run(Shared::ActionHelpers::RepeatInfinite([holder] {
		return Shared::ActionHelpers::MakeSequence(
			Shared::ActionHelpers::Execute([holder] {
				auto size = glm::linearRand(4.0f, 6.0f);
				
				auto star = std::make_shared<Scene::Actionable<Scene::Sprite>>();
				star->setTexture(TEXTURE("textures/background_star.png"));
				star->setAnchor(glm::linearRand(glm::vec2(0.0f), glm::vec2(1.0f)));
				star->setPivot({ 0.5f, 0.5f });
				star->setSize({ size, size });
				star->setRotation(glm::radians(45.0f));
				star->setScale({ 0.0f, 0.0f });
				star->setAlpha(0.0f);
				holder->attach(star);

				const float ShowDuration = 2.0f;
				const float HoldDuration = 0.5f;
				const float StarAlpha = 0.75f;

				star->runAction(Shared::ActionHelpers::MakeSequence(
					Shared::ActionHelpers::MakeParallel(
						Shared::ActionHelpers::ChangeScale(star, { 1.0f, 1.0f }, ShowDuration, Common::Easing::QuadraticOut),
						Shared::ActionHelpers::ChangeAlpha(star, StarAlpha, ShowDuration, Common::Easing::QuadraticOut)
					),
					Shared::ActionHelpers::Delayed(HoldDuration,
						Shared::ActionHelpers::MakeParallel(
							Shared::ActionHelpers::ChangeScale(star, { 0.0f, 0.0f }, ShowDuration, Common::Easing::QuadraticIn),
							Shared::ActionHelpers::Hide(star, ShowDuration, Common::Easing::QuadraticIn)
						)
					),
					Shared::ActionHelpers::Kill(star)
				));
			}),
			Shared::ActionHelpers::Wait(0.25f)
		);
	}));
}

void Sky::moveSky(const glm::vec2& offset)
{	
	mAsteroidsHolder->setPosition(offset / 2.0f);

	if (offset.y < mLastPos.y)
		mLastPos.y = offset.y;

	if (offset.x > mLastPos.x)
		mLastPos.x = offset.x;

	float delta_x = offset.x - mLastPos.x;
	float delta_y = offset.y - mLastPos.y;

	mLastPos = offset;

	float stars_delta_x = (delta_x * 0.0005f);
	float stars_delta_y = (delta_y * 0.0005f);

	glm::vec2 stars_delta = { stars_delta_x, stars_delta_y };

	auto moveStars = [](std::shared_ptr<Scene::Node> stars, const glm::vec2& stars_delta) {
		stars->setAnchor(stars->getAnchor() + stars_delta);

		if (stars->getVerticalAnchor() >= 2.0f)
			stars->setVerticalAnchor(stars->getVerticalAnchor() - 2.0f);

		if (stars->getHorizontalAnchor() <= -1.0f)
			stars->setHorizontalAnchor(stars->getHorizontalAnchor() + 2.0f);
	};

	moveStars(mStarsBottomLeft, stars_delta);
	moveStars(mStarsTopLeft, stars_delta);
	moveStars(mStarsBottomRight, stars_delta);
	moveStars(mStarsTopRight, stars_delta);
}