#include "gameplay.h"
#include "profile.h"
#include "helpers.h"
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

	runAction(Actions::Collection::Delayed([this] { return !isTransformReady(); },
		Actions::Collection::MakeSequence(
			Actions::Collection::Execute([this] {
				mPlayer->setPosition({ getAbsoluteWidth() / 2.0f, (-getAbsoluteHeight() / 2.0f) - 32.0f });
				mGameField->attach(mPlayer);
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
				})
			))
		)
	));

	mScoreLabel = std::make_shared<Helpers::Label>();
	mScoreLabel->setAnchor({ 1.0f, 0.0f });
	mScoreLabel->setPivot({ 1.0f, 0.5f });
	mScoreLabel->setPosition({ -16.0f, 24.0f });
	mScoreLabel->setText(L"0");
	mScoreLabel->setFontSize(22.0f);
    getGui()->attach(mScoreLabel);

	mRubiesIndicator = std::make_shared<Helpers::RubiesIndicator>();
	mRubiesIndicator->setInstantRefresh(false);
	getGui()->attach(mRubiesIndicator);

	mJumpParticles = std::make_shared<Scene::Emitter>();
	mJumpParticles->setHolder(mRectangleParticlesHolder);
	mJumpParticles->setRunning(false);
	mJumpParticles->setStretch({ 1.0f, 0.0f });
	mJumpParticles->setPivot({ 0.5f, 0.5f });
	mJumpParticles->setAnchor({ 0.5f, 1.0f });
	mJumpParticles->setDistance(48.0f);
	mJumpParticles->setMinDuration(0.25f);
	mJumpParticles->setMaxDuration(0.75f);
	mJumpParticles->setCreateParticleCallback([] {
		auto particle = std::make_shared<Scene::Rectangle>();
		particle->setSize(8.0f);
		return particle;
	});
	mPlayer->attach(mJumpParticles);

	/*mBonusesHolder = std::make_shared<Scene::Rectangle>();
	mBonusesHolder->setRounding(1.0f);
	mBonusesHolder->setSize({ 32.0f, 128.0f });
	mBonusesHolder->setPosition({ 16.0f, 64.0f });
	mBonusesHolder->setColor({ Graphics::Color::Black, 0.25f });
	getGui()->attach(mBonusesHolder);*/
}

void Gameplay::onTap()
{
	if (!mCanStart)
		return;

	PLATFORM->haptic(Platform::System::HapticType::Low);

	if (!mReady)
	{
		start();
		mReady = true;
	}

	downslide();
}

void Gameplay::onEvent(const Platform::Input::Keyboard::Event& e)
{
	if (e.type != Platform::Input::Keyboard::Event::Type::Pressed)
		return;

	tap();
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

	camera(delta);
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

	if (mVelocity.y > 0.0f) // if move down
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

void Gameplay::camera(Clock::Duration dTime)
{
	if (mMaxY < -mPlayer->getY())
		mMaxY = -mPlayer->getY();

	glm::vec2 target;
	target.x = -mPlayer->getX() + (getAbsoluteWidth() * 0.33f);
	target.y = mMaxY - (getAbsoluteHeight() * 0.66f);

	if (target.y < 0.0f)
		target.y = 0.0f;

	auto pos = Common::Helpers::SmoothValue(mGameField->getPosition(), target, dTime, 0.08f);

	mGameField->setPosition(pos);
	Helpers::gSky->moveSky(pos);
}

void Gameplay::jump(JumpType jump_type)
{
	//AUDIO->play(SOUND("sounds/click.wav"));
	PLATFORM->haptic(Platform::System::HapticType::High);
	mVelocity.y = -10.0f;

	if (jump_type == JumpType::PowerJump)
	{
		mVelocity.y *= 1.75f;
		ACHIEVEMENTS->hit("JUMP_BOOSTER_PANEL");
	}
	else if (jump_type == JumpType::FallJump)
	{
		mVelocity.y /= 3.0f;
	}
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

	auto jump_type = mDownslide ? (plane->isPowerjump() ? JumpType::PowerJump :
		JumpType::Normal) : JumpType::FallJump;

	jump(jump_type);

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

	if (plane->hasRubies())
	{
		ACHIEVEMENTS->hit("RUBIES_COLLECTED");
		float delay = 0.0f;
		for (auto ruby : plane->getRubies())
		{
			auto _ruby = std::static_pointer_cast<Scene::Adaptive<Scene::Sprite>>(ruby);
			_ruby->bakeAdaption();
			_ruby->setAdaptingEnabled(false);
			PROFILE->increaseRubies(1);
			mRubiesIndicator->collectRubyAnim(_ruby, delay);
			mRubiesCollected += 1;
			delay += 0.125f;
		}
	}
	if (plane->getMagnet() != nullptr)
	{
		//
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
		spawnPlane(pos, anim_delay, std::nullopt, false, false);
		anim_delay += AnimWait;

		pos.y -= 56.0f;
		pos.x += 96.0f;
		spawnPlane(pos, anim_delay, /*PlaneBonus::Magnet*/std::nullopt, false, false);
		anim_delay += AnimWait;

		spawnPlane(getNextPos(), anim_delay, PlaneBonus::Ruby, true, false);
		anim_delay += AnimWait;
	}

	if (mLastPlanePos.x - mPlayer->getX() > 286.0f)
		mLastPlanePos.x = mPlayer->getX() + 286.0f;

	while (mLastPlanePos.y >= mPlayer->getY())
	{
		bool has_ruby = Common::Helpers::Chance(0.05f);
		bool powerjump = Common::Helpers::Chance(0.1f);
		bool moving = Common::Helpers::Chance(0.75f * getDifficulty());

		std::optional<PlaneBonus> bonus;

		if (has_ruby)
			bonus = PlaneBonus::Ruby;

		spawnPlane(getNextPos(), anim_delay, bonus, powerjump, moving);
		anim_delay += AnimWait;
	}
}

void Gameplay::spawnPlane(const glm::vec2& pos, float anim_delay, std::optional<PlaneBonus> bonus, bool powerjump, bool moving)
{
	mLastPlanePos = pos;

	auto plane = std::make_shared<Plane>();
	plane->setBatchGroup("plane");
	if (!powerjump)
	{
		plane->setWidth(64.0f);
	}
	else
	{
		plane->setWidth(48.0f);
		plane->setColor(Graphics::Color::Coral);
		plane->setPowerjump(true);
		
		auto emitter = std::make_shared<Scene::Emitter>();
		emitter->setHolder(mRectangleParticlesHolder);
		emitter->setDelay(1.0f / 30.0f);
		emitter->setStretch({ 0.75f, 0.0f });
		emitter->setPivot(0.5f);
		emitter->setAnchor({ 0.5f, 1.0f });
		emitter->setDirection({ 0.0f, 1.0f });
		emitter->setCreateParticleCallback([] {
			auto particle = std::make_shared<Scene::Rectangle>();
			particle->setSize(6.0f);
			particle->setColor(Graphics::Color::FloralWhite);
			return particle;
		});
		plane->attach(emitter);
	}

	plane->setPivot(0.5f);
	plane->setPosition(pos);
	plane->setScale(0.0f);
	plane->runAction(Actions::Collection::Delayed(anim_delay,
		Actions::Collection::ChangeScale(plane, { 1.0f, 1.0f }, 0.5f / 1.25f, Easing::BackOut)
	));

	mPlaneHolder->attach(plane);

	if (bonus == PlaneBonus::Ruby)
	{
		auto tripple_ruby = Common::Helpers::Chance(0.33f);

		auto addEmitterForRuby = [this](auto ruby) {
			auto emitter = std::make_shared<Scene::Emitter>();
			emitter->setHolder(mRectangleParticlesHolder);
			emitter->setSpawnrate(10.0f);
			emitter->setStretch({ 0.75f, 0.0f });
			emitter->setAnchor({ 0.5f, 0.0f });
			emitter->setPivot(0.5f);
			emitter->setDirection({ 0.0f, -1.0f });
			emitter->setDistance(24.0f);
			emitter->setCreateParticleCallback([] {
				auto particle = std::make_shared<Scene::Rectangle>();
				particle->setSize(4.0f);
				particle->setBatchGroup("ruby_particle");
				particle->setColor(Graphics::Color::HotPink);
				return particle;
			});
			ruby->attach(emitter);
		};

		if (tripple_ruby)
		{
			auto left_ruby = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
			left_ruby->setAdaptSize(14.0f);
			left_ruby->setBatchGroup("plane_ruby");
			left_ruby->setTexture(TEXTURE("textures/ruby.png"));
			left_ruby->setPivot({ 1.25f, 1.0f });
			left_ruby->setAnchor({ 0.5f, 0.0f });
			left_ruby->setPosition({ 0.0f, -6.0f });
			plane->addRuby(left_ruby);
			plane->attach(left_ruby);
			addEmitterForRuby(left_ruby);

			auto right_ruby = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
			right_ruby->setAdaptSize(14.0f);
			right_ruby->setBatchGroup("plane_ruby");
			right_ruby->setTexture(TEXTURE("textures/ruby.png"));
			right_ruby->setPivot({ -0.25f, 1.0f });
			right_ruby->setAnchor({ 0.5f, 0.0f });
			right_ruby->setPosition({ 0.0f, -6.0f });
			plane->addRuby(right_ruby);
			plane->attach(right_ruby);
			addEmitterForRuby(right_ruby);
		}

		auto ruby = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
		ruby->setAdaptSize(18.0f);
		ruby->setBatchGroup("plane_ruby");
		ruby->setTexture(TEXTURE("textures/ruby.png"));
		ruby->setPivot({ 0.5f, 1.0f });
		ruby->setAnchor({ 0.5f, 0.0f });
		ruby->setPosition({ 0.0f, -4.0f });
		plane->attach(ruby);
		plane->addRuby(ruby);
		addEmitterForRuby(ruby);
	}

	if (bonus == PlaneBonus::Magnet)
	{
		auto magnet = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
		magnet->setAdaptSize(20.0f);
		magnet->setBatchGroup("plane_magnet");
		magnet->setTexture(TEXTURE("textures/magnet.png"));
		magnet->setPivot({ 0.5f, 1.0f });
		magnet->setAnchor({ 0.5f, 0.0f });
		magnet->setPosition({ 0.0f, -4.0f });
		plane->setMagnet(magnet);
		plane->attach(magnet);

		auto emitter = std::make_shared<Scene::Emitter>();
		emitter->setHolder(mRectangleParticlesHolder);
		emitter->setSpawnrate(30.0f);
	//	emitter->setStretch({ 0.75f, 0.0f });
		emitter->setAnchor(0.5f);
		emitter->setPivot(0.5f);
		emitter->setDistance(32.0f);
		emitter->setMinDuration(1.0f);
		emitter->setMaxDuration(3.0f);
		emitter->setCreateParticleCallback([] {
			static auto texture = GRAPHICS->makeGenericTexture({ 32, 32 }, [] {
				GRAPHICS->drawCircle({ Graphics::Color::White, 1.0f }, { Graphics::Color::Yellow, 0.0f });
			});
			auto particle = std::make_shared<Scene::Sprite>();
			particle->setBatchGroup("magnet_particle");
			particle->setTexture(texture);
			particle->setBlendMode(skygfx::BlendStates::Additive);
			return particle;
		});
		magnet->attach(emitter);
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
}

void Gameplay::gameover()
{
	if (mGameovered)
		return;

	mGameovered = true;
	ACHIEVEMENTS->hit("GAME_COMPLETED");
	PROFILE->saveAsync();
	auto gameover_screen = std::make_shared<GameoverMenu>(getScore());
	SCENE_MANAGER->switchScreen(gameover_screen);
}

void Gameplay::showRiskLabel(const std::wstring& text)
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
	mScoreLabel->setText(std::to_wstring(mScore));

	if (mScore > PROFILE->getHighScore())
		PROFILE->setHighScore(mScore);
}

float Gameplay::getDifficulty() const
{
	return glm::clamp((float)mScore / 300.0f, 0.0f, 1.0f);
}
