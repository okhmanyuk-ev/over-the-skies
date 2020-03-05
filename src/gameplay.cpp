#include "gameplay.h"
#include "profile.h"
#include "helpers.h"

using namespace hcg001;

Gameplay::Gameplay(Skin skin)
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
	mGameField->setAnchor({ 0.0f, 1.0f });
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
	mPlayer->getSprite()->setAlpha(0.0f);
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

	mPlayer->getSprite()->setTexture(TEXTURE(SkinPath.at(skin)));
	Common::Actions::Run(Shared::ActionHelpers::Delayed(0.25f, Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::Show(mPlayer->getSprite(), 0.25f),
		Shared::ActionHelpers::Execute([this] {
			spawnPlanes();
		}))
	));
	mPlayer->setPosition({ PLATFORM->getLogicalWidth() / 2.0f, (-PLATFORM->getLogicalHeight() / 2.0f) - 32.0f });
	
	setupTrail(skin);

	// jump particles

	mJumpParticles = std::make_shared<Shared::SceneHelpers::RectangleEmitter>(mParticlesHolder);
	mJumpParticles->setRunning(false);
	mJumpParticles->setBeginSize({ 8.0f, 8.0f });
	mJumpParticles->setStretch({ 1.0f, 0.0f });
	mJumpParticles->setPivot({ 0.5f, 0.5f });
	mJumpParticles->setAnchor({ 0.5f, 1.0f });
	mJumpParticles->setDistance(48.0f);
	mJumpParticles->setMinDuration(0.25f);
	mJumpParticles->setMaxDuration(0.75f);
	mPlayer->attach(mJumpParticles);
}

void Gameplay::touch(Touch type, const glm::vec2& pos)
{
	Scene::Actionable<Screen>::touch(type, pos);

	if (type != Touch::Begin)
		return;

	tap();
}

void Gameplay::update()
{
	Scene::Actionable<Screen>::update();

	if (!mReady)
		return;

	float dTime = Clock::ToSeconds(FRAME->getTimeDelta());

	mPhysTimeAccumulator += dTime;

	const float PhysTimeStep = 1.0f / 120.0f;

	while (mPhysTimeAccumulator >= PhysTimeStep)
	{
		physics(PhysTimeStep);
		mPhysTimeAccumulator -= PhysTimeStep;
	}

	if (mPhysTimeAccumulator > 0.0f)
	{
		physics(mPhysTimeAccumulator);
		mPhysTimeAccumulator = 0.0f;
	}

	if (mPlayer->project(mPlayer->getSize() / 2.0f).y >= PLATFORM->getHeight())
	{
		gameover();
		return;
	}

	{
		auto dir = glm::normalize(mVelocity);
		auto angle = glm::atan(dir.y, dir.x);
		angle -= glm::radians(30.0f);
		mPlayer->getSprite()->setRotation(angle);
	}

	camera(dTime);
	removeFarPlanes();
	spawnPlanes();
}

void Gameplay::physics(float dTime)
{
	const float Gravitation = 0.075f;

	mVelocity.y += Gravitation * dTime * 100.0f;

	if (mVelocity.y > MaxFallVelocity)
		mVelocity.y = MaxFallVelocity;

	float prev_y = mPlayer->getY() + (mPlayer->getHeight() * mPlayer->getVerticalPivot());
	float prev_l = mPlayer->getX() - (mPlayer->getWidth() * mPlayer->getHorizontalPivot());
	float prev_r = mPlayer->getX() + (mPlayer->getWidth() * mPlayer->getHorizontalPivot());

	auto pos = mPlayer->getPosition();
	pos += mVelocity * dTime * 100.0f;
	mPlayer->setPosition(pos);

	float next_y = mPlayer->getY() + (mPlayer->getHeight() * mPlayer->getVerticalPivot());
	float next_l = mPlayer->getX() - (mPlayer->getWidth() * mPlayer->getHorizontalPivot());
	float next_r = mPlayer->getX() + (mPlayer->getWidth() * mPlayer->getHorizontalPivot());

	if (mVelocity.y > 0.0f && mDownslide) // if move down 
	{
		glm::vec2 min = { glm::min(prev_l, next_l), prev_y };
		glm::vec2 max = { glm::max(prev_r, next_r), next_y };

		for (const auto node : mPlaneHolder->getNodes())
		{
			auto plane = std::static_pointer_cast<Plane>(node);

			if (plane->isCrashed())
				continue;

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
			collide(plane);
			break;
		}
	}
}

void Gameplay::camera(float dTime)
{
	if (mMaxY < -mPlayer->getY())
		mMaxY = -mPlayer->getY();

	auto pos = mGameField->getPosition();
	glm::vec2 target;
	target.x = -mPlayer->getX() + (PLATFORM->getLogicalWidth() * 0.33f);
	target.y = mMaxY - (PLATFORM->getLogicalHeight() * 0.66f);

	if (target.y < 0.0f)
		target.y = 0.0f;

	const float Speed = 8.0f;

	pos += (target - pos) * dTime * Speed;

	mGameField->setPosition(pos);
	mMoveSkyCallback(pos);
}

void Gameplay::jump(bool powerjump)
{
	AUDIO->play(mClickSound);
	mVelocity.y = -10.0f;

	if (powerjump)
	{
		mVelocity.y *= 1.75f;
		mPlayer->getSprite()->setColor(Graphics::Color::Yellow);
		runAction(Shared::ActionHelpers::Delayed(0.75f, 
			Shared::ActionHelpers::ChangeColor(mPlayer->getSprite(), Graphics::Color::White, 0.5f)
		));
	}
	increaseScore((int)glm::abs(mVelocity.y));
}

void Gameplay::downslide()
{
	if (mDownslide)
		return;

	mVelocity.y = MaxFallVelocity;
	mDownslide = true;
}

void Gameplay::collide(std::shared_ptr<Plane> plane)
{
	plane->setCrashed(true);
	mVelocity.x = 3.0f;
	jump(plane->isPowerjump());

	if (plane->isPowerjump() && plane->isMoving())
		showRiskLabel(LOCALIZE("RISK_PERFECT"));
	else if (plane->isPowerjump())
		showRiskLabel(LOCALIZE("RISK_GREAT"));

	mDownslide = false;
	//spawnCrashParticles(mPlayer->getPosition() + glm::vec2(0.0f, mPlayer->getHeight() * mPlayer->getVerticalPivot()));
	spawnJumpParticles();

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

void Gameplay::spawnPlanes()
{
	float anim_delay = 0.0f;
	const float AnimWait = 0.125f / 1.125f;

	if (!mPlaneHolder->hasNodes())
	{
		auto pos = mPlayer->getPosition();
		pos.y += 96.0f;
		spawnPlane(pos, anim_delay, false, false, false);
		anim_delay += AnimWait;

		pos.y -= 56.0f;
		pos.x += 96.0f;
		spawnPlane(pos, anim_delay, false, false, false);
		anim_delay += AnimWait;
	}

	if (mLastPlanePos.x - mPlayer->getX() > 286.0f)
		mLastPlanePos.x = mPlayer->getX() + 286.0f;

	while (mLastPlanePos.y >= mPlayer->getY())
	{
		bool has_ruby = Helpers::Chance(0.05f);
		bool powerjump = Helpers::Chance(0.1f);
		bool moving = Helpers::Chance(0.1f);
		float pos_x = mLastPlanePos.x + glm::linearRand(36.0f, 96.0f);
		float pos_y = mLastPlanePos.y - glm::linearRand(32.0f, 128.0f);
		spawnPlane({ pos_x, pos_y }, anim_delay, has_ruby, powerjump, moving);
		anim_delay += AnimWait;
	}
}

void Gameplay::spawnPlane(const glm::vec2& pos, float anim_delay, bool has_ruby, bool powerjump, bool moving)
{
	mLastPlanePos = pos;

	auto plane = std::make_shared<Plane>();
	plane->setSize({ 64.0f, 8.0f });

	if (powerjump)
	{
		plane->setSize({ 48.0f, 8.0f });
		plane->setColor(Graphics::Color::Yellow);
		plane->setPowerjump(true);
		
		auto emitter = std::make_shared<Shared::SceneHelpers::RectangleEmitter>(mParticlesHolder);
		emitter->setBeginSize({ 6.0f, 6.0f });
		emitter->setMinDelay(0.025f);
		emitter->setMaxDelay(0.1f);
		emitter->setStretch({ 0.75f, 0.0f });
		emitter->setPivot({ 0.5f, 0.5f });
		emitter->setAnchor({ 0.5f, 1.0f });
		emitter->setDirection({ 0.0f, 1.0f });
		emitter->setBeginColor({ Graphics::Color::Yellow, 1.0f });
		plane->attach(emitter);
	}

	plane->setPivot({ 0.5f, 0.5f });
	plane->setPosition(pos);
	plane->setScale({ 0.0f, 0.0f });
	plane->runAction(Shared::ActionHelpers::Delayed(anim_delay,
		Shared::ActionHelpers::ChangeScale(plane, { 1.0f, 1.0f }, 0.5f / 1.25f, Common::Easing::BackOut)
	));

	mPlaneHolder->attach(plane);

	if (has_ruby)
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

	if (moving)
	{
		plane->setMoving(true);

		const float BasePosition = plane->getX();
		plane->runAction(Shared::ActionHelpers::RepeatInfinite([plane, BasePosition] {
			const float Duration = 0.25f;
			const float Distance = 32.0f;
			return Shared::ActionHelpers::MakeSequence(
				Shared::ActionHelpers::ChangeHorizontalPosition(plane, BasePosition + Distance, Duration),
				Shared::ActionHelpers::ChangeHorizontalPosition(plane, BasePosition, Duration),
				Shared::ActionHelpers::ChangeHorizontalPosition(plane, BasePosition - Distance, Duration),
				Shared::ActionHelpers::ChangeHorizontalPosition(plane, BasePosition, Duration)
			);
		}));
	}
}

void Gameplay::spawnJumpParticles()
{
	for (int i = 0; i < 16; i++)
	{
		mJumpParticles->emit();
	}
}

void Gameplay::removeFarPlanes()
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

void Gameplay::start()
{
	runAction(Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::Hide(mReadyLabel, 0.5f),
		Shared::ActionHelpers::Kill(mReadyLabel)
	));
}

void Gameplay::gameover()
{
	mGameoverCallback();
	PROFILE->saveAsync();
}

void Gameplay::setupTrail(Skin skin)
{
	if (skin == Skin::Ball)
	{
		auto trail = std::make_shared<Scene::Trail>(mGameField);
		trail->setAnchor({ 0.5f, 0.5f });
		trail->setPivot({ 0.5f, 0.5f });
		trail->setStretch({ 0.9f, 0.9f });
		trail->setLifetime(0.2f);
		trail->setNarrowing(true);
		mPlayer->runAction(Shared::ActionHelpers::ExecuteInfinite([this, trail] {
			trail->setColor(mPlayer->getSprite()->getColor());
		}));
		mPlayer->attach(trail);
	}
	else if (skin == Skin::Snowflake)
	{
		auto emitter = std::make_shared<Shared::SceneHelpers::SpriteEmitter>(mParticlesHolder);
		emitter->setPivot({ 0.5f, 0.5f });
		emitter->setAnchor({ 0.5f, 0.5f });
		emitter->setTexture(TEXTURE("textures/skins/snowflake.png"));
		emitter->setDelay(0.0f);
		emitter->setBeginSize({ 12.0f, 12.0f });
		emitter->setDistance(16.0f);
		mPlayer->runAction(Shared::ActionHelpers::ExecuteInfinite([this, emitter] {
			emitter->setBeginColor(mPlayer->getSprite()->getColor());
		}));
		mPlayer->attach(emitter);
	}
}

void Gameplay::tap()
{
	if (!mReady)
	{
		start();
		mReady = true;
	}
	
	downslide();
}

void Gameplay::showRiskLabel(const utf8_string& text)
{
	if (mRiskLabel != nullptr)
	{
		mRiskLabel->runAction(Shared::ActionHelpers::Kill(mRiskLabel));
	}

	mRiskLabel = std::make_shared<Scene::Actionable<Scene::Label>>();
	mRiskLabel->setFont(FONT("default"));
	mRiskLabel->setFontSize(28.0f);
	mRiskLabel->setText(text);
	mRiskLabel->setAnchor({ 0.5f, 0.33f });
	mRiskLabel->setVerticalPivot(0.5f);
	mRiskLabel->setHorizontalPivot(Helpers::Chance(0.5f) ? 1.0f : 0.0f);
	mRiskLabel->setAlpha(0.0f);
	attach(mRiskLabel);

	mRiskLabel->runAction(Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::Show(mRiskLabel, 0.125f),
		Shared::ActionHelpers::Wait(1.0f),
		Shared::ActionHelpers::Hide(mRiskLabel, 0.5f),
		Shared::ActionHelpers::Kill(mRiskLabel)
	));

	mRiskLabel->runAction(
		Shared::ActionHelpers::ChangeHorizontalPivot(mRiskLabel, 0.5f, 0.75f, Common::Easing::ElasticOut)
	);
}

void Gameplay::increaseScore(int count)
{
	mScore += count;
	mScoreLabel->setText(std::to_string(mScore));

	if (mScore > PROFILE->getHighScore())
		PROFILE->setHighScore(mScore);
}