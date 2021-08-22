#include "gameplay.h"
#include "profile.h"
#include "helpers.h"
#include "client.h"
#include "achievements.h"
#include "gameover_menu.h"

using namespace hcg001;

Gameplay::Gameplay()
{
	mReadyLabel = std::make_shared<Scene::Label>();
	mReadyLabel->setFont(FONT("default"));
	mReadyLabel->setFontSize(28.0f);
	mReadyLabel->setAnchor({ 0.5f, 0.25f });
	mReadyLabel->setPivot(0.5f);
	mReadyLabel->setText(LOCALIZE("READY_MENU_TITLE"));
	getContent()->attach(mReadyLabel);

	mGameField = std::make_shared<Scene::Node>();
	mGameField->setAnchor({ 0.0f, 1.0f });
	getContent()->attach(mGameField);

	mRectangleParticlesHolder = std::make_shared<Scene::Node>();
	mRectangleParticlesHolder->setStretch(1.0f);
	mGameField->attach(mRectangleParticlesHolder);

	mPlaneHolder = std::make_shared<Scene::Node>();
	mPlaneHolder->setStretch(1.0f);
	mGameField->attach(mPlaneHolder);

	mPlayerTrailHolder = std::make_shared<Scene::Node>();
	mPlayerTrailHolder->setStretch(1.0f);
	mGameField->attach(mPlayerTrailHolder);

	mPlayer = createPlayer(PROFILE->getCurrentSkin(), mPlayerTrailHolder);
	mPlayer->setAlpha(0.0f);

	mNickname = std::make_shared<Helpers::Label>();
	mNickname->setText(PROFILE->getNickName());
	mNickname->setPivot(0.5f);
	mNickname->setColor(Graphics::Color::ToNormalized(220, 220, 170));
	mNickname->setAlpha(0.0f);
	mNickname->setEnabled(PROFILE->isNicknameChanged());

	runAction(Actions::Collection::Delayed([this] { return !isTransformReady(); },
		Actions::Collection::MakeSequence(
			Actions::Collection::Execute([this] {
				mPlayer->setPosition({ getAbsoluteWidth() / 2.0f, (-getAbsoluteHeight() / 2.0f) - 32.0f });
				mGameField->attach(mPlayer);

				mNickname->setPosition(mPlayer->getPosition() + glm::vec2({ 0.0f, -32.0f }));
				mGameField->attach(mNickname);
			}),
			Actions::Collection::Wait([this] { return !mPlayer->isTransformReady(); }),
			Actions::Collection::Delayed(0.25f, Actions::Collection::MakeSequence(
				Actions::Collection::Execute([this] {
					spawnPlanes();
				}),
				Actions::Collection::Delayed(0.25f,
					Actions::Collection::Show(mPlayer, 0.25f)
				),
				Actions::Collection::Execute([this] {
					mCanStart = true;
				}),
				Actions::Collection::Show(mNickname, 0.5f)
			))
		)
	));
	
	mScoreLabel = std::make_shared<Scene::Label>();
	mScoreLabel->setFont(FONT("default"));
	mScoreLabel->setAnchor({ 1.0f, 0.0f });
	mScoreLabel->setPivot({ 1.0f, 0.5f });
	mScoreLabel->setPosition({ -16.0f, 24.0f });
	mScoreLabel->setText("0");
    getGui()->attach(mScoreLabel);
	
	mRubiesIndicator = std::make_shared<Helpers::RubiesIndicator>();
	mRubiesIndicator->setInstantRefresh(false);
	getGui()->attach(mRubiesIndicator);

	mJumpParticles = std::make_shared<Scene::RectangleEmitter>();
	mJumpParticles->setHolder(mRectangleParticlesHolder);
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

void Gameplay::onTap()
{
	if (!mCanStart)
		return;

	if (!mReady)
	{
		start();
		mReady = true;
	}

	downslide();
}

void Gameplay::update(Clock::Duration delta)
{
	Scene::Tappable<Screen>::update(delta);

	if (!mReady)
		return;

	physics(Clock::ToSeconds(delta));

	auto projected_player_pos = unproject(mPlayer->project(mPlayer->getAbsoluteSize() / 2.0f));

	if (projected_player_pos.y >= getAbsoluteHeight())
	{
		gameover();
		return;
	}

	{
		auto dir = glm::normalize(mVelocity);
		auto angle = glm::atan(dir.y, dir.x);
		angle -= glm::radians(30.0f);
		mPlayer->setSpriteRotation(angle);
	}

	auto dTime = Clock::ToSeconds(delta);
	camera(dTime);
	removeFarPlanes();
	spawnPlanes();

	setScore(int(mMaxY / 100.0f));

	GAME_STATS("difficulty", getDifficulty());
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
	target.x = -mPlayer->getX() + (getAbsoluteWidth() * 0.33f);
	target.y = mMaxY - (getAbsoluteHeight() * 0.66f);

	if (target.y < 0.0f)
		target.y = 0.0f;

	const float Speed = 8.0f;

	pos += (target - pos) * dTime * Speed;

	mGameField->setPosition(pos);
	Helpers::gSky->moveSky(pos);
}

void Gameplay::jump(bool powerjump)
{
	AUDIO->play(SOUND("sounds/click.wav"));
	PLATFORM->haptic();
	mVelocity.y = -10.0f;

	if (powerjump)
		mVelocity.y *= 1.75f;

	if (powerjump)
		ACHIEVEMENTS->hit("JUMP_BOOSTER_PANEL");
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
	spawnJumpParticles();

	plane->runAction(Actions::Collection::MakeSequence(
		Actions::Collection::ChangeScale(plane, { 0.0f, 0.0f }, 0.25f, Easing::BackIn),
		Actions::Collection::Kill(plane)
	));

	if (plane->hasRuby())
	{
		PROFILE->increaseRubies(1);
		ACHIEVEMENTS->hit("RUBIES_COLLECTED");
		mRubiesIndicator->collectRubyAnim(plane->getRuby());
		mRubiesCollected += 1;
	}
}

void Gameplay::spawnPlanes()
{
	float anim_delay = 0.0f;
	const float AnimWait = 0.125f / 1.125f;

	auto getNextPos = [this] {
		float pos_x = mLastPlanePos.x + glm::linearRand(36.0f, 96.0f);
		float pos_y = mLastPlanePos.y - glm::linearRand(32.0f, 128.0f);
		return glm::vec2({ pos_x, pos_y });
	};

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

		spawnPlane(getNextPos(), anim_delay, false, true, false);
		anim_delay += AnimWait;
	}

	if (mLastPlanePos.x - mPlayer->getX() > 286.0f)
		mLastPlanePos.x = mPlayer->getX() + 286.0f;

	while (mLastPlanePos.y >= mPlayer->getY())
	{
		bool has_ruby = Common::Helpers::Chance(0.05f);
		bool powerjump = Common::Helpers::Chance(0.1f);
		bool moving = Common::Helpers::Chance(0.75f * getDifficulty());
		spawnPlane(getNextPos(), anim_delay, has_ruby, powerjump, moving);
		anim_delay += AnimWait;
	}
}

void Gameplay::spawnPlane(const glm::vec2& pos, float anim_delay, bool has_ruby, bool powerjump, bool moving)
{
	mLastPlanePos = pos;

	auto plane = std::make_shared<Plane>();
	if (!powerjump)
	{
		plane->setBatchGroup("plane");
		plane->setTexture(TEXTURE("textures/plane.png"));
		plane->setSize({ 64.0f, 8.0f });
	}
	else
	{
		plane->setBatchGroup("plane_small");
		plane->setTexture(TEXTURE("textures/plane_small.png"));
		plane->setSize({ 48.0f, 8.0f });
		plane->setColor(Graphics::Color::Coral);
		plane->setPowerjump(true);
		
		auto emitter = std::make_shared<Scene::RectangleEmitter>();
		emitter->setHolder(mRectangleParticlesHolder);
		emitter->setBeginSize({ 6.0f, 6.0f });
		emitter->setDelay(1.0f / 30.0f);
		emitter->setStretch({ 0.75f, 0.0f });
		emitter->setPivot(0.5f);
		emitter->setAnchor({ 0.5f, 1.0f });
		emitter->setDirection({ 0.0f, 1.0f });
		emitter->setBeginColor({ Graphics::Color::FloralWhite, 1.0f });
		plane->attach(emitter);
	}

	plane->setPivot(0.5f);
	plane->setPosition(pos);
	plane->setScale(0.0f);
	plane->runAction(Actions::Collection::Delayed(anim_delay,
		Actions::Collection::ChangeScale(plane, { 1.0f, 1.0f }, 0.5f / 1.25f, Easing::BackOut)
	));

	mPlaneHolder->attach(plane);

	if (has_ruby)
	{
		auto ruby = std::make_shared<Scene::Sprite>();
		ruby->setBatchGroup("plane_ruby");
		ruby->setTexture(TEXTURE("textures/ruby.png"));
		ruby->setPivot({ 0.5f, 1.0f });
		ruby->setAnchor({ 0.5f, 0.0f });
		ruby->setPosition({ 0.0f, -4.0f });
		ruby->setSize(18.0f);
		plane->attach(ruby);
		plane->setRuby(ruby);

		auto emitter = std::make_shared<Scene::RectangleEmitter>();
		emitter->setHolder(mRectangleParticlesHolder);
		emitter->setBeginSize({ 4.0f, 4.0f });
		emitter->setDelay(1.0f / 10.0f);
		emitter->setStretch({ 0.75f, 0.0f });
		emitter->setPivot(0.5f);
		emitter->setAnchor({ 0.5f, 0.0f });
		emitter->setDirection({ 0.0f, -1.0f });
		emitter->setDistance(24.0f);
		emitter->setBeginColor({ Graphics::Color::HotPink, 1.0f });
		ruby->attach(emitter);
	}

	if (moving)
	{
		plane->setMoving(true);

		const float Center = plane->getX();
		bool side = Common::Helpers::Chance(0.5f);
		plane->runAction(Actions::Collection::RepeatInfinite([plane, Center, side] {
			const float Duration = 0.25f;
			const float Distance = 32.0f;
			
			float firstSide = Center + (side ? Distance : -Distance);
			float secondSide = Center - (side ? Distance : -Distance);

			return Actions::Collection::MakeSequence(
				Actions::Collection::ChangeHorizontalPosition(plane, firstSide, Duration),
				Actions::Collection::ChangeHorizontalPosition(plane, Center, Duration),
				Actions::Collection::ChangeHorizontalPosition(plane, secondSide, Duration),
				Actions::Collection::ChangeHorizontalPosition(plane, Center, Duration)
			);
		}));
	}
}

void Gameplay::spawnJumpParticles()
{
	mJumpParticles->emit(16);
}

void Gameplay::removeFarPlanes()
{
	auto planes = mPlaneHolder->getNodes();

	for (const auto plane : planes)
	{
		auto projected_plane_pos = unproject(plane->project(plane->getAbsoluteSize() / 2.0f));

		if (projected_plane_pos.y > getAbsoluteHeight())
		{
			mPlaneHolder->detach(plane);
		}
	}
}

void Gameplay::start()
{
	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::Hide(mReadyLabel, 0.5f),
		Actions::Collection::Kill(mReadyLabel)
	));
	mNickname->clearActions();
	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::Hide(mNickname, 0.5f),
		Actions::Collection::Kill(mNickname)
	));
}

void Gameplay::gameover()
{
	if (mGameovered)
		return;

	mGameovered = true;
	ACHIEVEMENTS->hit("GAME_COMPLETED");
	PROFILE->saveAsync();
	CLIENT->sendGuildContribution(mRubiesCollected);
	auto gameover_screen = std::make_shared<GameoverMenu>(getScore());
	SCENE_MANAGER->switchScreen(gameover_screen);
}

void Gameplay::showRiskLabel(const utf8_string& text)
{
	if (mRiskLabel != nullptr)
	{
		mRiskLabel->runAction(Actions::Collection::Kill(mRiskLabel));
	}

	mRiskLabel = std::make_shared<Scene::Label>();
	mRiskLabel->setFont(FONT("default"));
	mRiskLabel->setFontSize(28.0f);
	mRiskLabel->setText(text);
	mRiskLabel->setAnchor({ 0.5f, 0.33f });
	mRiskLabel->setVerticalPivot(0.5f);
	mRiskLabel->setHorizontalPivot(Common::Helpers::Chance(0.5f) ? 1.0f : 0.0f);
	mRiskLabel->setAlpha(0.0f);
	getContent()->attach(mRiskLabel);

	mRiskLabel->runAction(Actions::Collection::MakeSequence(
		Actions::Collection::Show(mRiskLabel, 0.125f),
		Actions::Collection::Wait(1.0f),
		Actions::Collection::Hide(mRiskLabel, 0.5f),
		Actions::Collection::Kill(mRiskLabel)
	));

	mRiskLabel->runAction(
		Actions::Collection::ChangeHorizontalPivot(mRiskLabel, 0.5f, 0.75f, Easing::ElasticOut)
	);
}

void Gameplay::setScore(int count)
{
	if (mScore < count)
		ACHIEVEMENTS->hit("COVER_DISTANCE", count - mScore);

	mScore = count;
	mScoreLabel->setText(std::to_string(mScore));

	if (mScore > PROFILE->getHighScore())
		PROFILE->setHighScore(mScore);
}

float Gameplay::getDifficulty() const
{
	return glm::clamp((float)mScore / 300.0f, 0.0f, 1.0f);
}
