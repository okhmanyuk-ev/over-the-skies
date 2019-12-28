#include "gameplay_screen.h"
#include "profile.h"

using namespace hcg001;

GameplayScreen::GameplayScreen(Skin skin)
{
	setTouchable(true);

	mReadyLabel = std::make_shared<Scene::Label>();
	mReadyLabel->setFont(FONT("default"));
	mReadyLabel->setFontSize(28.0f);
	mReadyLabel->setAnchor({ 0.5f, 0.25f });
	mReadyLabel->setPivot({ 0.5f, 0.5f });
	mReadyLabel->setText(LOCALIZE("READY_MENU_TITLE"));
	attach(mReadyLabel);

	// game field

	mGameField = std::make_shared<Scene::Node>();
	mGameField->setVerticalAnchor(1.0f);
	mGameField->setHorizontalSize(GameFieldWidth);
	mGameField->setHorizontalAnchor(0.5f);
	mGameField->setHorizontalPivot(0.5f);
	attach(mGameField);

	// plane holder

	mPlaneHolder = std::make_shared<Scene::Node>();
	mPlaneHolder->setStretch({ 1.0f, 1.0f });
	mGameField->attach(mPlaneHolder);

	// particles

	mParticlesHolder = std::make_shared<Scene::Node>();
	mParticlesHolder->setVerticalAnchor(1.0f);
	mParticlesHolder->setHorizontalStretch(1.0f);
	mGameField->attach(mParticlesHolder);

	// player

	mPlayer = std::make_shared<Player>();
	mPlayer->setSize(PlayerSize);
	mPlayer->setPivot({ 0.5f, 0.5f });
	mPlayer->setAlpha(0.0f);
	mGameField->attach(mPlayer);

	// score label

	mScoreLabel = std::make_shared<Scene::Label>();
	mScoreLabel->setFont(FONT("default"));
	mScoreLabel->setAnchor({ 1.0f, 0.0f });
	mScoreLabel->setPivot({ 1.0f, 0.0f });
	mScoreLabel->setPosition({ -16.0f, 16.0f });
	mScoreLabel->setText("0");
	attach(mScoreLabel);


	//

	mPlayer->setTexture(TEXTURE(/*SkinPath.at(mMainMenu->getChoosedSkin())*/"textures/skins/ball.png"));
	Common::Actions::Run(Shared::ActionHelpers::Delayed(0.25f, Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::Show(mPlayer, 0.25f),
		Shared::ActionHelpers::Execute([this] {
			spawnPlanes();
		}))
	));
	mPlayer->setPosition({ GameFieldWidth / 2.0f, (-PLATFORM->getLogicalHeight() / 2.0f) + 36.0f });

	setupTrail(skin);
}

void GameplayScreen::touch(Touch type, const glm::vec2& pos)
{
	Scene::Actionable<Screen>::touch(type, pos);

	if (!mReady)
	{
		if (type == Touch::Begin)
		{
			start();
			mReady = true;
		}
		else
		{
			return;
		}
	}

	if (type == Touch::Begin)
	{
		mTouching = true;
		mTouchPos = pos.x;
	}
	else if (type == Touch::Continue && mTouching)
	{
		auto distance = pos.x - mTouchPos;
		slide(distance);
		mTouchPos = pos.x;
	}
	else if (type == Touch::End)
	{
		mTouching = false;
	}
}

void GameplayScreen::update()
{
	Scene::Actionable<Screen>::update();

	if (!mReady)
		return;

	float dTime = Clock::ToSeconds(FRAME->getTimeDelta());

	mTimeAccumulator += dTime;

	const float PhysTimeStep = 1.0f / 120.0f;

	while (mTimeAccumulator >= PhysTimeStep)
	{
		physics(PhysTimeStep);
		mTimeAccumulator -= PhysTimeStep;
	}

	if (mTimeAccumulator > 0.0f)
	{
		physics(mTimeAccumulator);
		mTimeAccumulator = 0.0f;
	}

	if (mPlayer->project(mPlayer->getSize() / 2.0f).y >= PLATFORM->getHeight())
	{
		gameover();
		return;
	}

	// camera

	{
		if (mMaxY < -mPlayer->getY())
			mMaxY = -mPlayer->getY();

		float y = mGameField->getY();
		float target = mMaxY - (PLATFORM->getLogicalHeight() / 2.0f);

		if (target < 0.0f)
			target = 0.0f;

		float delta = (target - y) * 0.09375f * dTime * 100.0f;
		y += delta;

		mGameField->setY(y);
		GAME_STATS("Y", y);
	
	//	mAsteroidsHolder->setY(y / 2.0f);

	/*	float stars_delta = (delta * 0.0005f);

		mStarsHolder1->setVerticalAnchor(mStarsHolder1->getVerticalAnchor() + stars_delta);
		mStarsHolder2->setVerticalAnchor(mStarsHolder2->getVerticalAnchor() + stars_delta);

		if (mStarsHolder1->getVerticalAnchor() >= 2.0f)
			mStarsHolder1->setVerticalAnchor(mStarsHolder1->getVerticalAnchor() - 2.0f);

		if (mStarsHolder2->getVerticalAnchor() >= 2.0f)
			mStarsHolder2->setVerticalAnchor(mStarsHolder2->getVerticalAnchor() - 2.0f);*/
	}

	removeFarPlanes();
	spawnPlanes();
}

void GameplayScreen::physics(float dTime)
{
	const float Gravitation = 0.3f;
	const float MaxFallSpeed = 10.0f;

	mVelocity.y += Gravitation * dTime * 100.0f;

	if (mVelocity.y > MaxFallSpeed)
		mVelocity.y = MaxFallSpeed;

	float prev_y = mPlayer->getY() + (mPlayer->getHeight() * mPlayer->getVerticalPivot());
	float prev_l = mPlayer->getX() - (mPlayer->getWidth() * mPlayer->getHorizontalPivot());
	float prev_r = mPlayer->getX() + (mPlayer->getWidth() * mPlayer->getHorizontalPivot());

	auto pos = mPlayer->getPosition();
	pos += mVelocity * dTime * 100.0f;
	mPlayer->setPosition(pos);

	float next_y = mPlayer->getY() + (mPlayer->getHeight() * mPlayer->getVerticalPivot());
	float next_l = mPlayer->getX() - (mPlayer->getWidth() * mPlayer->getHorizontalPivot());
	float next_r = mPlayer->getX() + (mPlayer->getWidth() * mPlayer->getHorizontalPivot());

	if (mVelocity.y > 0.0f) // if move down 
	{
		glm::vec2 min = { glm::min(prev_l, next_l), prev_y };
		glm::vec2 max = { glm::max(prev_r, next_r), next_y };

		for (const auto plane : mPlaneHolder->getNodes())
		{
			float plane_y = plane->getY() - (plane->getHeight() * plane->getVerticalPivot());
			float plane_l = plane->getX() - (plane->getWidth() * plane->getHorizontalPivot());
			float plane_r = plane->getX() + (plane->getWidth() * plane->getHorizontalPivot());

			if (min.y > plane_y)
				continue;

			if (max.y < plane_y)
				continue;

			if (min.x > plane_r)
				continue;

			if (max.x < plane_l)
				continue;

			mVelocity.y = 0.0f;
			mPlayer->setY(plane_y - (mPlayer->getHeight() * mPlayer->getVerticalPivot()));
			collide(std::static_pointer_cast<Plane>(plane));
			break;
		}
	}
}

void GameplayScreen::jump()
{
	const float JumpVelocity = -10.0f;
	mVelocity.y = JumpVelocity;
}

void GameplayScreen::collide(std::shared_ptr<Plane> plane)
{
	jump();

	mScore += 1;
	mScoreLabel->setText(std::to_string(mScore));
	runAction(Shared::ActionHelpers::Shake(mScoreLabel, 2.0f, 0.2f));

	if (mScore > PROFILE->getHighScore())
		PROFILE->setHighScore(mScore);

	plane->runAction(Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::ChangeScale(plane, { 0.0f, 0.0f }, 0.25f, Common::Easing::BackIn),
		Shared::ActionHelpers::Kill(plane)
	));

	if (plane->hasRuby())
	{
		PROFILE->increaseRubies(1);
		mRubyCallback(plane->getRuby());
	}
}

void GameplayScreen::spawnPlanes()
{
	float anim_delay = 0.0f;
	const float AnimWait = 0.125f / 1.25f;
	const float PlaneStep = 96.0f;

	if (!mPlaneHolder->hasNodes())
	{
		spawnPlane(mPlayer->getPosition() + glm::vec2(0.0f, PlaneStep / 2.0f), anim_delay);
		anim_delay += AnimWait;
	}

	while (mPlaneHolder->getNodes().size() < 20)
	{
		float min = PlaneSize.x;
		float max = GameFieldWidth - PlaneSize.x;
		float d = (mGameField->getWidth() - GameFieldWidth) / 2.0f;
		spawnPlane({ glm::linearRand(min, max) + d, mPlaneHolder->getNodes().back()->getY() - PlaneStep }, anim_delay);
		anim_delay += AnimWait;
	}
}

void GameplayScreen::spawnPlane(const glm::vec2& pos, float anim_delay)
{
	auto plane = std::make_shared<Plane>();

	auto width = PlaneSize.x;
	auto diff = PlaneSize.x - PlayerSize.x;

	width -= diff * glm::clamp(static_cast<float>(/*mScore*/ 0.0f) / 100.0f, 0.0f, 1.0f);

	plane->setSize({ width, PlaneSize.y });
	plane->setPivot({ 0.5f, 0.5f });
	plane->setPosition(pos);
	plane->setScale({ 0.0f, 0.0f });
	plane->runAction(Shared::ActionHelpers::Delayed(anim_delay,
		Shared::ActionHelpers::ChangeScale(plane, { 1.0f, 1.0f }, 0.5f / 1.25f, Common::Easing::BackOut)
	));

	mPlaneHolder->attach(plane);

	if (glm::linearRand(0.0f, 1.0f) < 0.05f) // ruby on plane
	{
		auto ruby = std::make_shared<Scene::Sprite>();
		ruby->setTexture(TEXTURE("ruby"));
		ruby->setPivot({ 0.5f, 1.0f });
		ruby->setAnchor({ 0.5f, 0.0f });
		ruby->setPosition({ 0.0f, -4.0f });
		ruby->setSize({ 18.0f, 18.0f });
		plane->attach(ruby);
		plane->setRuby(ruby);
	}
}

void GameplayScreen::spawnParticle(const glm::vec2& pos, std::shared_ptr<Renderer::Texture> texture, const glm::vec3& color)
{
	auto particle = std::make_shared<Scene::Actionable<Scene::Sprite>>();
	particle->setTexture(texture);
	particle->setPosition(pos);
	particle->setSize({ 12.0f, 12.0f });
	particle->setPivot({ 0.5f, 0.5f });
	particle->setRotation(glm::radians(glm::linearRand(0.0f, 360.0f)));
	particle->setColor(color);

	auto direction = glm::normalize(glm::diskRand(1.0f));
	const float Duration = 1.0f;

	particle->runAction(Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::MakeParallel(
			Shared::ActionHelpers::ChangePositionByDirection(particle, direction, 25.0f, Duration),
			Shared::ActionHelpers::Hide(particle, Duration),
			Shared::ActionHelpers::ChangeScale(particle, { 0.0f, 0.0f }, Duration),
			Shared::ActionHelpers::ChangeColor(particle, Graphics::Color::White, Duration)
		),
		Shared::ActionHelpers::Kill(particle)
	));

	mParticlesHolder->attach(particle);
}

void GameplayScreen::removeFarPlanes()
{
	auto planes = mPlaneHolder->getNodes();

	for (const auto plane : planes)
	{
		auto projected_plane_pos = plane->project(plane->getSize() / 2.0f);

		if (projected_plane_pos.y > PLATFORM->getHeight())
		{
			mPlaneHolder->detach(plane);
		}
	}
}

void GameplayScreen::start()
{
	runAction(Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::Hide(mReadyLabel, 0.5f),
		Shared::ActionHelpers::Kill(mReadyLabel)
	));
}

void GameplayScreen::gameover()
{
	mGameoverCallback();
	PROFILE->saveAsync();
}

void GameplayScreen::slide(float distance)
{
	if (distance == 0.0f)
		return;

	auto x = mPlayer->getX();

	x += (distance / PLATFORM->getScale()) * 1.75f;

	float half_width = mPlayer->getWidth() * 0.5f;
	float min = half_width;
	float max = PLATFORM->getLogicalWidth() - half_width;

	if (x < min)
		x = min;

	if (x > max)
		x = max;

	mPlayer->setX(x);
}

void GameplayScreen::setupTrail(Skin skin)
{
	if (skin == Skin::Ball)
	{
		auto trail = std::make_shared<Scene::Trail>(mGameField);
		trail->setAnchor({ 0.5f, 0.5f });
		trail->setPivot({ 0.5f, 0.5f });
		trail->setStretch({ 0.9f, 0.9f });
		trail->setLifetime(0.2f);
		trail->setNarrowing(true);
		mPlayer->attach(trail);
	}
	else if (skin == Skin::Snowflake)
	{
		mPlayer->runAction(Shared::ActionHelpers::RepeatInfinite([this] {
			return Shared::ActionHelpers::Delayed(0.025f, Shared::ActionHelpers::Execute([this] {
				FRAME->addOne([this] {
					spawnParticle(mPlayer->getPosition(), TEXTURE("textures/skins/snowflake.png"));
				});
			}));
		}));
	}
	else if (skin == Skin::Third)
	{
		mPlayer->runAction(Shared::ActionHelpers::RepeatInfinite([this] {
			return Shared::ActionHelpers::Delayed(0.025f, Shared::ActionHelpers::Execute([this] {
				FRAME->addOne([this] {
					spawnParticle(mPlayer->getPosition(), TEXTURE("textures/skins/ball.png"), Graphics::Color::Gold);
				});
			}));
		}));
	}
	else if (skin == Skin::Fourth)
	{
		auto trail = std::make_shared<Scene::Trail>(mGameField);
		trail->setAnchor({ 0.5f, 0.5f });
		trail->setPivot({ 0.5f, 0.5f });
		trail->setStretch({ 0.9f, 0.9f });
		trail->setLifetime(0.75f);
		trail->setColor(Graphics::Color::Aquamarine);
		trail->setNarrowing(true);
		mPlayer->attach(trail);
	}
}