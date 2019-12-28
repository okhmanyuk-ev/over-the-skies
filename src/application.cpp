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

	mBloomLayer = std::make_shared<Scene::BloomLayer>();
	mBloomLayer->setStretch({ 1.0f, 1.0f });
	mBloomLayer->setDownscaleFactor(2.0f);
	mSky->attach(mBloomLayer);

	CONSOLE->registerCVar("r_bloom_enabled", { "bool" }, CVAR_GETTER_BOOL_FUNC(mBloomLayer->isPostprocessEnabled),
		CVAR_SETTER_BOOL_FUNC(mBloomLayer->setPostprocessEnabled));

	CONSOLE->registerCVar("r_bloom_blur_passes", { "int" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getBlurPasses),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setBlurPasses));

	CONSOLE->registerCVar("r_bloom_glow_passes", { "int" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getGlowPasses),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setGlowPasses));

	CONSOLE->registerCVar("r_bloom_downscale_factor", { "float" }, CVAR_GETTER_INT_FUNC(mBloomLayer->getDownscaleFactor),
		CVAR_SETTER_INT_FUNC(mBloomLayer->setDownscaleFactor));

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

	// hud holder

	mHudHolder = std::make_shared<Scene::Node>();
	mHudHolder->setStretch({ 1.0f, 1.0f });
	root->attach(mHudHolder);

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

	auto main_menu = std::make_shared<MainMenu>();
	main_menu->setStartCallback([this, main_menu] {
		auto gameplay_screen = std::make_shared<GameplayScreen>(main_menu->getChoosedSkin());
		gameplay_screen->setGameoverCallback([this, main_menu, gameplay_screen] {
			auto gameover_screen = std::make_shared<GameoverMenu>(gameplay_screen->getScore());
			gameover_screen->setClickCallback([this, main_menu] {
				mSceneManager->switchScreen(main_menu);
			});
			mSceneManager->switchScreen(gameover_screen);
		});
		mSceneManager->switchScreen(gameplay_screen);
	});

	mSceneManager->switchScreen(main_menu);

	changeSkyColor();

	Common::Actions::Run(Shared::ActionHelpers::RepeatInfinite([this] {
		return Shared::ActionHelpers::Delayed(10.0f,
			Shared::ActionHelpers::Execute([this] {
				changeSkyColor();
			})
		);
	}));

	Common::Actions::Run(Shared::ActionHelpers::RepeatInfinite([this] {
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

void Application::frame()
{
	mGameScene.frame();

	if (mNodeEditor)
		mSceneEditor.show();
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

void Application::spawnAsteroid(float speed, float normalized_spread)
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