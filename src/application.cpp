#include "application.h"

using namespace hcg001;

Application::Application() : RichApplication(PROJECT_CODE)
{
	PLATFORM->setTitle(PROJECT_NAME);
	PLATFORM->resize(360, 640);

	ENGINE->setCustomSystem(&mGameSystems);

	GAME_SYSTEMS->setProfile(&mProfile);

	LOCALIZATION->loadDicrionaries("localization");
	LOCALIZATION->setLanguage(Shared::LocalizationSystem::Language::English);

	PROFILE->load();

	PLATFORM->initializeBilling({ "rubies.001" });

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
}

Application::~Application()
{
	PROFILE->save();
}

void Application::loading(const std::string& stage, float progress)
{
	//mSplashScene.updateProgress(progress);
	//mSplashScene.frame();
#if defined BUILD_DEVELOPER
	RichApplication::loading(stage, progress);
#endif
}

void Application::initialize()
{
#if defined(BUILD_DEVELOPER)
	CONSOLE->execute("hud_show_fps 1");
	CONSOLE->execute("hud_show_drawcalls 1");
#else
	CONSOLE_DEVICE->setEnabled(false);
	STATS->setEnabled(false);
#endif

	Scene::Debug::Font = FONT("default");

	auto root = mGameScene.getRoot();

	// sky

	mSky = std::make_shared<Sky>();
	root->attach(mSky);

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
		gameplay_screen->setRubyCallback([this](auto ruby) {
			collectRubyAnim(ruby);
		});
		gameplay_screen->setMoveSkyCallback([this](auto y) {
			mSky->moveSky(y);
		});
		mSceneManager->switchScreen(gameplay_screen);
	});

	mSceneManager->switchScreen(main_menu);

	mSky->changeColor();

	Common::Actions::Run(Shared::ActionHelpers::RepeatInfinite([this] {
		return Shared::ActionHelpers::Delayed(10.0f,
			Shared::ActionHelpers::Execute([this] {
				mSky->changeColor();
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

void Application::collectRubyAnim(std::shared_ptr<Scene::Node> ruby)
{
	auto pos = mHudHolder->unproject(ruby->project({ 0.0f, 0.0f }));
	ruby->setAnchor({ 0.0f, 0.0f });
	ruby->setPivot({ 0.0f, 0.0f });
	ruby->setPosition(pos);
	ruby->getParent()->detach(ruby);
	mHudHolder->attach(ruby);

	auto dest_pos = mHudHolder->unproject(mRubyScore.sprite->project({ 0.0f, 0.0f }));

	const float MoveDuration = 0.75f;

	Common::Actions::Run(Shared::ActionHelpers::MakeSequence(
		Shared::ActionHelpers::MakeParallel(
			Shared::ActionHelpers::ChangePosition(ruby, dest_pos, MoveDuration, Common::Easing::QuarticInOut),
			Shared::ActionHelpers::ChangeSize(ruby, mRubyScore.sprite->getSize(), MoveDuration, Common::Easing::QuarticInOut)
		),
		Shared::ActionHelpers::Kill(ruby),
		Shared::ActionHelpers::Shake(mRubyScore.label, 2.0f, 0.2f)
	));
}

void Application::event(const Profile::RubiesChangedEvent& e)
{
	mRubyScore.label->setText(std::to_string(PROFILE->getRubies()));
}