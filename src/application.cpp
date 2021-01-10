#include "application.h"
#include "main_menu.h"
#include "gameplay.h"
#include "gameover_menu.h"
#include "sky.h"
#include "player.h"
#include "plane.h"
#include "skin.h"
#include "profile.h"
#include "cheats.h"
#include "daily_reward_window.h"
#include "helpers.h"
#include "hud.h"
#include "client.h"

using namespace hcg001;

Application::Application() : Shared::Application(PROJECT_NAME, { Flag::Audio, Flag::Scene, Flag::Network })
{
	PLATFORM->setTitle(PRODUCT_NAME);
	PLATFORM->resize(360, 640);
	RENDERER->setVsync(true);
#if !defined(PLATFORM_MOBILE)
	PLATFORM->rescale(1.5f);
#endif

	ENGINE->addSystem<Client>(std::make_shared<Client>());
	ENGINE->addSystem<Profile>(std::make_shared<Profile>());
	PROFILE->load();

	PLATFORM->initializeBilling({
		{ "rubies.001", [this] { 
			addRubies(500);
		} }
	});

	PRECACHE_FONT_ALIAS("fonts/sansation.ttf", "default");

	STATS->setAlignment(Shared::StatsSystem::Align::BottomRight);

	Scene::Sprite::DefaultSampler = Renderer::Sampler::Linear;

#if defined(BUILD_DEVELOPER)
	CONSOLE->execute("hud_show_fps 1");
	CONSOLE->execute("hud_show_drawcalls 1");
#else
	CONSOLE_DEVICE->setEnabled(false);
	STATS->setEnabled(false);
#endif

	CACHE->makeAtlases();

	FRAME->addOne([this] {
		initialize();
	});
}

Application::~Application()
{
	ENGINE->removeSystem<Client>();
	PROFILE->save();
}

void Application::initialize()
{
	auto root = getScene()->getRoot();

	auto sky = std::make_shared<Sky>();
	root->attach(sky, Scene::Node::AttachDirection::Front);

	auto main_menu = std::make_shared<MainMenu>();
	main_menu->setStartCallback([sky, main_menu] {
		auto gameplay = std::make_shared<Gameplay>(main_menu->getChoosedSkin());
		gameplay->setGameoverCallback([main_menu, gameplay] {
			auto gameover_screen = std::make_shared<GameoverMenu>(gameplay->getScore());
			gameover_screen->setClickCallback([main_menu] {
				SCENE_MANAGER->switchScreen(main_menu);
			});
			SCENE_MANAGER->switchScreen(gameover_screen);
		});
		gameplay->setMoveSkyCallback([sky](auto offset) {
			sky->moveSky(offset);
		});
		SCENE_MANAGER->switchScreen(gameplay);
	});

	sky->changeColor(Graphics::Color::Hsv::HueBlue, Graphics::Color::Hsv::HueRed);

	SCENE_MANAGER->switchScreen(main_menu, [this] {
		tryShowDailyReward();
	});

	Actions::Run(Actions::Factory::RepeatInfinite([sky] {
		return Actions::Factory::Delayed(10.0f,
			Actions::Factory::Execute([sky] {
				sky->changeColor();
			})
		);
	}));

	// hud

	Helpers::gHud = std::make_shared<Hud>();
	SCENE_MANAGER->getWindowHolder()->attach(Helpers::gHud); // after screens, before windows
}

void Application::frame()
{
	adaptToScreen(getScene()->getRoot());
	ShowCheatsMenu();
}

void Application::addRubies(int count)
{
	PROFILE->increaseRubies(count);
	PROFILE->saveAsync();

	for (int i = 0; i < count; i++)
	{
		if (i > 8)
			break;

		auto ruby = std::make_shared<Scene::Actionable<Scene::Sprite>>();
		ruby->setTexture(TEXTURE("textures/ruby.png"));
		ruby->setPivot(0.5f);
		ruby->setAnchor(0.5f);
		ruby->setSize(24.0f);
		ruby->setPosition(glm::linearRand(glm::vec2(-64.0f), glm::vec2(64.0f)));
		ruby->setAlpha(0.0f);
		ruby->runAction(Actions::Factory::MakeSequence(
			Actions::Factory::Wait(i * (0.125f / 1.25f)),
			Actions::Factory::Show(ruby, 0.25f, Easing::CubicIn),
			Actions::Factory::Execute([this, ruby] {
				FRAME->addOne([this, ruby] {
					Helpers::gHud->collectRubyAnim(ruby);
				});
			})
		));
		Helpers::gHud->attach(ruby);
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
	SCENE_MANAGER->pushWindow(window);
}

void Application::adaptToScreen(std::shared_ptr<Scene::Node> node)
{
	glm::vec2 size = { (float)PLATFORM->getLogicalWidth(), (float)PLATFORM->getLogicalHeight() };
	glm::vec2 dimensions = { 360.0f, 640.0f };

	auto scale = size / dimensions;
	node->setScale(glm::min(scale.x, scale.y));
}
