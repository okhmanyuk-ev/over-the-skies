#include "application.h"

using namespace hcg001;

Application::Application() : RichApplication(PROJECT_CODE)
{
	PLATFORM->setTitle(PROJECT_NAME);
	PLATFORM->resize(360, 640);

	ENGINE->addSystem<Profile>(std::make_shared<Profile>());

	LOCALIZATION->loadDicrionaries("localization");
	LOCALIZATION->setLanguage(Shared::LocalizationSystem::Language::English);

	PROFILE->load();

	PLATFORM->initializeBilling({
		{ "rubies.001", [this] { 
			addRubies(500);
		} }
	});

	addLoadingTasks({
		{ "fonts", [this] {
			PRECACHE_FONT_ALIAS("fonts/sansation.ttf", "default");
		} },
		{ "textures", [this] {
			PRECACHE_TEXTURE_ALIAS("textures/ruby.png", "ruby");
		} }
	});

	setPayloadWaiting(0.0f);

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

	mSceneManager = std::make_shared<Shared::SceneManager>();
	root->attach(mSceneManager);

	auto main_menu = std::make_shared<MainMenu>();
	main_menu->setStartCallback([this, main_menu] {
		auto gameplay = std::make_shared<Gameplay>(main_menu->getChoosedSkin());
		gameplay->setGameoverCallback([this, main_menu, gameplay] {
			auto gameover_screen = std::make_shared<GameoverMenu>(gameplay->getScore());
			gameover_screen->setClickCallback([this, main_menu] {
				mSceneManager->switchScreen(main_menu);
			});
			mSceneManager->switchScreen(gameover_screen);
		});
		gameplay->setRubyCallback([this](auto ruby) {
			collectRubyAnim(ruby);
		});
		gameplay->setMoveSkyCallback([this](auto offset) {
			mSky->moveSky(offset);
		});
		mSceneManager->switchScreen(gameplay);
	});

	mSky->changeColor(Graphics::Color::Hsv::HueBlue, Graphics::Color::Hsv::HueRed); 

	mSceneManager->switchScreen(main_menu, [this] {
		tryShowDailyReward();
	});

	Common::Actions::Run(Shared::ActionHelpers::RepeatInfinite([this] {
		return Shared::ActionHelpers::Delayed(10.0f,
			Shared::ActionHelpers::Execute([this] {
				mSky->changeColor();
			})
		);
	}));

	// hud holder

	mHudHolder = std::make_shared<Scene::Node>();
	mHudHolder->setStretch(1.0f);
	mSceneManager->getWindowHolder()->attach(mHudHolder); // after screens, before windows

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
}

void Application::frame()
{
	mGameScene.frame();

	if (mNodeEditor)
		mSceneEditor.show();

	ShowCheatsMenu(mSceneManager);
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
		Shared::ActionHelpers::Execute([this] {
			mRubyScore.label->setText(std::to_string(PROFILE->getRubies()));
		}),
		Shared::ActionHelpers::Shake(mRubyScore.label, 2.0f, 0.2f)
	));
}

void Application::addRubies(int count)
{
	PROFILE->increaseRubies(count);
	PROFILE->saveAsync();

	if (!isInitialized())
		return;

	for (int i = 0; i < count; i++)
	{
		if (i > 8)
			break;

		auto ruby = std::make_shared<Scene::Actionable<Scene::Sprite>>();
		ruby->setTexture(TEXTURE("ruby"));
		ruby->setPivot(0.5f);
		ruby->setAnchor(0.5f);
		ruby->setSize(24.0f);
		ruby->setPosition(glm::linearRand(glm::vec2(-64.0f), glm::vec2(64.0f)));
		ruby->setAlpha(0.0f);
		ruby->runAction(Shared::ActionHelpers::MakeSequence(
			Shared::ActionHelpers::Wait(i * (0.125f / 1.25f)),
			Shared::ActionHelpers::Show(ruby, 0.25f, Common::Easing::CubicIn),
			Shared::ActionHelpers::Execute([this, ruby] {
				FRAME->addOne([this, ruby] {
					collectRubyAnim(ruby);
				});
			})
		));
		mHudHolder->attach(ruby);
	}
}

void Application::tryShowDailyReward()
{
	const long long OneDay = 60 * 60 * 24;

	auto now = Clock::SystemNowSeconds();

	auto current_day = PROFILE->getDailyRewardDay();
	auto delta = now - PROFILE->getDailyRewardTime();

	if (delta < OneDay)
		return;

	if (delta < OneDay * 2)
		current_day += 1;
	else
		current_day = 1;

	current_day = glm::min(current_day, 7);

	auto window = std::make_shared<DailyRewardWindow>(current_day);
	window->setClaimCallback([this, current_day, now] {
		auto rubies_count = DailyRewardWindow::DailyRewardMap.at(current_day);
		
		PROFILE->setDailyRewardTime(now);
		PROFILE->setDailyRewardDay(current_day);
		PROFILE->saveAsync();

		addRubies(rubies_count);
	});
	mSceneManager->pushWindow(window);
}

void Application::event(const Profile::RubiesChangedEvent& e)
{
	if (!isInitialized())
		return;

	//mRubyScore.label->setText(std::to_string(PROFILE->getRubies()));
}