#include "application.h"

using namespace hcg001;

Application::Application() : RichApplication(PROJECT_CODE)
{
	PLATFORM->setTitle(PROJECT_NAME);
	PLATFORM->resize(360, 640);

	ENGINE->setCustomSystem(LOCALIZATION_SYSTEM_KEY, &mLocalization);
	ENGINE->setCustomSystem(PROFILE_SYSTEM_KEY, &mProfile);
	
	PROFILE->load();

	addLoadingTasks({
		{ "fonts", [this] {
			PRECACHE_FONT_ALIAS("fonts/sansation.ttf", "default");
		} },
		{ "textures", [this] {
			PRECACHE_TEXTURE_ALIAS("textures/ruby.png", "ruby");
		} }
	});

	CONSOLE->execute("r_vsync 1");

	std::srand((unsigned int)std::time(nullptr));

	CONSOLE->registerCVar("g_node_editor", { "bool" }, CVAR_GETTER_BOOL(mNodeEditor), CVAR_SETTER_BOOL(mNodeEditor));

	STATS->setAlignment(Shared::StatsSystem::Align::BottomRight);

	makeLoadingScene();
}

Application::~Application()
{
	PROFILE->save();
}

void Application::makeLoadingScene()
{
	auto root = mLoadingScene.getRoot();

	auto logo = std::make_shared<Scene::Sprite>();
	logo->setTexture(TEXTURE("textures/logo.png"));
	logo->setAnchor({ 0.5f, 0.5f });
	logo->setPivot({ 0.5f, 0.5f });
	logo->setSampler(Renderer::Sampler::Linear);
	logo->setSize({ 286.0f, 286.0f });
	root->attach(logo);
}

void Application::loading(const std::string& stage, float process)
{
	mLoadingScene.frame();
#if defined BUILD_DEVELOPER
	RichApplication::loading(stage, process);
#endif
}

void Application::initialize()
{
#if defined(BUILD_DEVELOPER)
	CONSOLE->execute("hud_show_fps 2");
	CONSOLE->execute("hud_show_drawcalls 1");
#elif defined(BUILD_PRODUCTION)
	CONSOLE_DEVICE->setEnabled(false);
	STATS->setEnabled(false);
#endif

	Scene::Debug::Font = FONT("default");

	auto root = mGameScene.getRoot();

	// sky

	mSky = std::make_shared<Sky>();
	root->attach(mSky);

	// stars holder

	CONSOLE->registerCVar("r_bloom_blur_passes", { "int" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getBlurPasses),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setBlurPasses));

	CONSOLE->registerCVar("r_bloom_glow_passes", { "int" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getGlowPasses),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setGlowPasses));

	CONSOLE->registerCVar("r_bloom_downscale_factor", { "float" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getDownscaleFactor),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setDownscaleFactor));

	mBloomLayer = std::make_shared<Scene::BloomLayer>();
	mBloomLayer->setStretch({ 1.0f, 1.0f });
	mBloomLayer->setDownscaleFactor(2.0f);
	mSky->attach(mBloomLayer);

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

	mAsteroidsHolder = std::make_shared<Scene::Node>();
	mAsteroidsHolder->setStretch({ 1.0f, 1.0f });
	mBloomLayer->attach(mAsteroidsHolder);

	// game field

	mGameField = std::make_shared<Scene::Node>();
	mGameField->setVerticalAnchor(1.0f);
	mGameField->setHorizontalSize(GameFieldWidth);
	mGameField->setHorizontalAnchor(0.5f);
	mGameField->setHorizontalPivot(0.5f);
	root->attach(mGameField);

	// hud holder

	mHudHolder = std::make_shared<Scene::Node>();
	mHudHolder->setStretch({ 1.0f, 1.0f });
	root->attach(mHudHolder);

	// plane holder

	mPlaneHolder = std::make_shared<Scene::Node>();
	mPlaneHolder->setStretch({ 1.0f, 1.0f });
	mGameField->attach(mPlaneHolder);

	// trail

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
	mScoreLabel->setAlpha(0.0f);
	mHudHolder->attach(mScoreLabel);

	// ruby score sprite

	mRubyScore.sprite = std::make_shared<Scene::Sprite>();
	mRubyScore.sprite->setTexture(TEXTURE("ruby"));
	mRubyScore.sprite->setPivot({ 0.0f, 0.0f });
	mRubyScore.sprite->setAnchor({ 0.0f, 0.0f });
	mRubyScore.sprite->setPosition({ 16.0f, 16.0f });
	mRubyScore.sprite->setSize({ 24.0f, 24.0f });
	mHudHolder->attach(mRubyScore.sprite);

	// ruby score label
	mRubyScore.label = std::make_shared<Scene::Label>();
	mRubyScore.label->setFont(FONT("default"));
	mRubyScore.label->setText(std::to_string(PROFILE->getRubies()));
	mRubyScore.label->setAnchor({ 1.0f, 0.5f });
	mRubyScore.label->setPivot({ 0.0f, 0.5f });
	mRubyScore.label->setPosition({ 8.0f, 0.0f });
	mRubyScore.sprite->attach(mRubyScore.label);

	mSceneManager = std::make_shared<Shared::SceneManager>();
	root->attach(mSceneManager);

	// main menu

	mMainMenu = std::make_shared<MainMenu>();
	mMainMenu->setStartCallback([this] {
		mSceneManager->switchScreen(nullptr, [this] {
			ready();
			mSceneManager->switchScreen(mReadyMenu);
		});
	});

	mReadyMenu = std::make_shared<ReadyMenu>();
	mReadyMenu->setReadyCallback([this] {
		mSceneManager->switchScreen(nullptr);
		start();
	});

	mGameoverMenu = std::make_shared<GameoverMenu>();
	mGameoverMenu->setClickCallback([this] {
		mSceneManager->switchScreen(mMainMenu);
	});

	mSceneManager->switchScreen(mMainMenu);
	changeSkyColor();

	Common::Actions::Run(Shared::ActionHelpers::RepeatInfinite([this] {
		return Shared::ActionHelpers::Delayed(10.0f,
			Shared::ActionHelpers::Execute([this] {
				changeSkyColor();
			})
		);
	}));

	Common::Actions::Run(Shared::ActionHelpers::RepeatInfinite([this] {
		return Shared::ActionHelpers::Delayed(1.0f, 
			Shared::ActionHelpers::Execute([this] {
				spawnAsteroid();
			})
		);
	}));
}

void Application::frame()
{
	mGameScene.frame();

	if (mNodeEditor)
		mSceneEditor.show();

	if (!mStarted)
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

	if (mPlayer->project({ mPlayer->getSize() / 2.0f }).y >= PLATFORM->getHeight())
	{
		gameover();
		return;
	}

#if defined(BUILD_DEVELOPER)
	if (PLATFORM->isKeyPressed(Platform::Keyboard::Key::Space))
		jump();
#endif

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

		float stars_delta = (delta * 0.0005f);

		mStarsHolder1->setVerticalAnchor(mStarsHolder1->getVerticalAnchor() + stars_delta);
		mStarsHolder2->setVerticalAnchor(mStarsHolder2->getVerticalAnchor() + stars_delta);

		if (mStarsHolder1->getVerticalAnchor() >= 2.0f)
			mStarsHolder1->setVerticalAnchor(mStarsHolder1->getVerticalAnchor() - 2.0f);

		if (mStarsHolder2->getVerticalAnchor() >= 2.0f)
			mStarsHolder2->setVerticalAnchor(mStarsHolder2->getVerticalAnchor() - 2.0f);
	}

	removeFarPlanes();
	spawnPlanes();
}

void Application::physics(float dTime)
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

void Application::collide(std::shared_ptr<Plane> plane)
{
	jump();

	mScore += 1;
	mScoreLabel->setText(std::to_string(mScore));
	Common::Actions::Run(Shared::ActionHelpers::Shake(mScoreLabel, 2.0f, 0.2f));

	if (mScore > PROFILE->getHighScore())
		PROFILE->setHighScore(mScore);

	plane->runAction(Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::ChangeScale(plane, { 0.0f, 0.0f }, 0.25f, Common::Easing::BackIn),
		Shared::ActionHelpers::Kill(plane)
	));

	if (plane->hasRuby())
	{
		PROFILE->increaseRubies(1);

		auto ruby = plane->getRuby();
		auto pos = mHudHolder->unproject(ruby->project({ 0.0f, 0.0f }));
		ruby->setAnchor({ 0.0f, 0.0f });
		ruby->setPivot({ 0.0f, 0.0f });
		ruby->setPosition(pos);
		plane->detach(ruby);
		mHudHolder->attach(ruby);

		auto dest_pos = mHudHolder->unproject(mRubyScore.sprite->project({ 0.0f, 0.0f }));

		const float MoveDuration = 0.75f;

		Common::Actions::Run(Shared::ActionHelpers::MakeSequence(
			Shared::ActionHelpers::MakeParallel(
				Shared::ActionHelpers::ChangePosition(ruby, dest_pos, MoveDuration, Common::Easing::QuarticInOut),
				Shared::ActionHelpers::ChangeSize(ruby, mRubyScore.sprite->getSize(), MoveDuration, Common::Easing::QuarticInOut)
			),
			Shared::ActionHelpers::Kill(ruby),
			Shared::ActionHelpers::Execute([this] {
				mRubyScore.label->setText(std::to_string(PROFILE->getRubies()));
			}),
			Shared::ActionHelpers::Shake(mRubyScore.label, 2.0f, 0.2f)
		));
	}
}

void Application::ready()
{
	mPlayer->setTexture(TEXTURE(SkinPath.at(mMainMenu->getChoosedSkin())));
	mScore = 0;
	Common::Actions::Run(Shared::ActionHelpers::Delayed(0.25f, Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::Show(mPlayer, 0.25f),
		Shared::ActionHelpers::Execute([this] {
			spawnPlanes();
		}))
	));
	mPlayer->setPosition({ GameFieldWidth / 2.0f, (-PLATFORM->getLogicalHeight() / 2.0f) + 36.0f });
}

void Application::start()
{
	mStarted = true;

	auto skin = mMainMenu->getChoosedSkin();

	if (skin == Skin::Ball)
	{
		assert(!mTrail);
		mTrail = std::make_shared<Scene::Trail>(mGameField);
		mTrail->setAnchor({ 0.5f, 0.5f });
		mTrail->setPivot({ 0.5f, 0.5f });
		mTrail->setStretch({ 0.9f, 0.9f });
		mTrail->setLifetime(0.2f);
		mTrail->setNarrowing(true);
		mPlayer->attach(mTrail);
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
		assert(!mTrail);
		mTrail = std::make_shared<Scene::Trail>(mGameField);
		mTrail->setAnchor({ 0.5f, 0.5f });
		mTrail->setPivot({ 0.5f, 0.5f });
		mTrail->setStretch({ 0.9f, 0.9f });
		mTrail->setLifetime(0.75f);
		mTrail->setColor(Graphics::Color::Aquamarine);
		mTrail->setNarrowing(true);
		mPlayer->attach(mTrail);
	}

	Common::Actions::Run(Shared::ActionHelpers::Show(mScoreLabel, 0.25f));
	mScoreLabel->setText("0");
	mRubyScore.label->setText(std::to_string(PROFILE->getRubies()));
}

void Application::gameover()
{
	mPlayer->clearActions();

	if (mTrail)
	{
		mPlayer->detach(mTrail);
		mTrail.reset();
	}

	mGameoverMenu->updateScore(mScore);
	
	clearField([this] {
		Common::Actions::Run(Shared::ActionHelpers::ChangePosition(mGameField, { mGameField->getX(), 0.0f }, 0.75f, Common::Easing::ExponentialOut));
		mSceneManager->switchScreen(mGameoverMenu);
	});
	
	mMaxY = 0.0f;
	mVelocity = { 0.0f, 0.0f };
	mStarted = false;

	TASK->addTask([this] {
		PROFILE->save();
	});
}

void Application::jump()
{
	const float JumpVelocity = -10.0f;
	mVelocity.y = JumpVelocity;
}

void Application::slide(float distance)
{
	if (!mStarted)
		return;

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

void Application::spawnPlane(const glm::vec2& pos, float anim_delay)
{
	auto plane = std::make_shared<Plane>();

	auto width = PlaneSize.x;
	auto diff = PlaneSize.x - PlayerSize.x;

	width -= diff * glm::clamp(static_cast<float>(mScore) / 100.0f, 0.0f, 1.0f);

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

void Application::removeFarPlanes()
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

void Application::clearField(std::function<void()> callback)
{
	const float HideDuration = 0.125f;

	for (auto node : mPlaneHolder->getNodes())
	{
		auto plane = std::static_pointer_cast<Plane>(node);
		plane->runAction(Shared::ActionHelpers::MakeSequence(
			Shared::ActionHelpers::Hide(plane, HideDuration),
			Shared::ActionHelpers::Kill(plane)
		));
	}

	Common::Actions::Run(Shared::ActionHelpers::Delayed(HideDuration, Shared::ActionHelpers::Execute(callback) ));
	Common::Actions::Run(Shared::ActionHelpers::Hide(mPlayer, HideDuration));
	Common::Actions::Run(Shared::ActionHelpers::Hide(mScoreLabel, HideDuration));
}

void Application::spawnParticle(const glm::vec2& pos, std::shared_ptr<Renderer::Texture> texture, const glm::vec3& color)
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

void Application::placeStarsToHolder(std::shared_ptr<Scene::Node> holder)
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

void Application::changeSkyColor()
{
	auto topHsv = glm::vec3(glm::linearRand(0.0f, 360.0f), 0.75f, 0.125f);
	auto bottomHsv = glm::vec3(glm::linearRand(0.0f, 360.0f), 0.25f, 0.5f);

	mSky->changeColor(glm::rgbColor(topHsv), glm::rgbColor(bottomHsv));
}

void Application::spawnPlanes()
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

void Application::event(const Shared::TouchEmulator::Event& e)
{
	if (e.type == Shared::TouchEmulator::Event::Type::Begin)
	{
		mTouching = true;
		mTouchPos = static_cast<float>(e.x);
	}
	else if (e.type == Shared::TouchEmulator::Event::Type::Continue && mTouching)
	{
		auto distance = static_cast<float>(e.x) - mTouchPos;
		slide(distance);
		mTouchPos = static_cast<float>(e.x);
	}
	else if (e.type == Shared::TouchEmulator::Event::Type::End)
	{
		mTouching = false;
	}
}

void Application::spawnAsteroid()
{
	auto start_anchor = glm::linearRand(glm::vec2(1.0f, -0.5f), glm::vec2(1.5f, 0.0f));

	auto asteroid = std::make_shared<Scene::Actionable<Scene::Rectangle>>();
	asteroid->setPivot({ 0.5f, 0.5f });
	asteroid->setAnchor(start_anchor);
	asteroid->setSize(2.0f);

	auto trail = std::make_shared<Scene::Trail>(mAsteroidsHolder);
	trail->setStretch(1.0f);
	trail->setLifetime(0.25f);
	asteroid->attach(trail);

	const float Speed = 512.0f + 256.0f;
	const float Duration = 5.0f;
	const glm::vec2 Direction = { -1.0f, 1.0f };

	asteroid->runAction(Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::ChangePositionByDirection(asteroid, Direction, Speed, Duration),
		Shared::ActionHelpers::Kill(asteroid)
	));

	mAsteroidsHolder->attach(asteroid);
}