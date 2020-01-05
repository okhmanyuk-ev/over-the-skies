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
	mBloomLayer->setStretch({ 1.0f, 1.0f });
	mBloomLayer->setDownscaleFactor(2.0f);
	attach(mBloomLayer);

	CONSOLE->registerCVar("r_bloom_enabled", { "bool" }, CVAR_GETTER_BOOL_FUNC(mBloomLayer->isPostprocessEnabled),
		CVAR_SETTER_BOOL_FUNC(mBloomLayer->setPostprocessEnabled));

	CONSOLE->registerCVar("r_bloom_blur_passes", { "int" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getBlurPasses),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setBlurPasses));

	CONSOLE->registerCVar("r_bloom_glow_passes", { "int" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getGlowPasses),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setGlowPasses));

	CONSOLE->registerCVar("r_bloom_downscale_factor", { "float" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getDownscaleFactor),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setDownscaleFactor));

	// stars holder

	mStarsHolder1 = std::make_shared<Scene::Node>();
	mStarsHolder1->setStretch({ 1.0f, 1.0f });
	mStarsHolder1->setAnchor({ 0.5f, 1.0f });
	mStarsHolder1->setPivot({ 0.5f, 1.0f });
	mBloomLayer->attach(mStarsHolder1);

	mStarsHolder2 = std::make_shared<Scene::Node>();
	mStarsHolder2->setStretch({ 1.0f, 1.0f });
	mStarsHolder2->setAnchor({ 0.5f, 0.0f });
	mStarsHolder2->setPivot({ 0.5f, 1.0f });
	mBloomLayer->attach(mStarsHolder2);

	placeStarsToHolder(mStarsHolder1);
	placeStarsToHolder(mStarsHolder2);

	// asteroids

	mAsteroidsHolder = std::make_shared<Scene::Node>();
	mAsteroidsHolder->setStretch({ 1.0f, 1.0f });
	mBloomLayer->attach(mAsteroidsHolder);

	runAction(Shared::ActionHelpers::RepeatInfinite([this] {
		auto duration = glm::linearRand(5.0f, 20.0f);
		return Shared::ActionHelpers::Delayed(duration,
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

	setCornerColor(Scene::Rectangle::Corner::TopLeft, mTopColor->getColor());
	setCornerColor(Scene::Rectangle::Corner::TopRight, mTopColor->getColor());
	setCornerColor(Scene::Rectangle::Corner::BottomLeft, mBottomColor->getColor());
	setCornerColor(Scene::Rectangle::Corner::BottomRight, mBottomColor->getColor());
}

void Sky::spawnAsteroid(float speed, float normalized_spread)
{
	auto asteroid = std::make_shared<Scene::Actionable<Scene::Rectangle>>();
	asteroid->setPivot({ 0.5f, 0.5f });
	asteroid->setAnchor({ 0.25f + normalized_spread, 0.0f });
	asteroid->setSize(2.0f);
	asteroid->setY(-mAsteroidsHolder->getY());

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

void Sky::moveSky(float y)
{	
	mAsteroidsHolder->setY(y / 2.0f);

	if (y < mLastY)
		mLastY = y;

	float delta = y - mLastY;
	mLastY = y;

	float stars_delta = (delta * 0.0005f);

	mStarsHolder1->setVerticalAnchor(mStarsHolder1->getVerticalAnchor() + stars_delta);
	mStarsHolder2->setVerticalAnchor(mStarsHolder2->getVerticalAnchor() + stars_delta);

	if (mStarsHolder1->getVerticalAnchor() >= 2.0f)
		mStarsHolder1->setVerticalAnchor(mStarsHolder1->getVerticalAnchor() - 2.0f);

	if (mStarsHolder2->getVerticalAnchor() >= 2.0f)
		mStarsHolder2->setVerticalAnchor(mStarsHolder2->getVerticalAnchor() - 2.0f);
}