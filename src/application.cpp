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
#include "windows/daily_reward_window.h"
#include "helpers.h"
#include "hud.h"
#include "client.h"
#include "achievements.h"

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
	ENGINE->addSystem<Achievements>(std::make_shared<Achievements>());

	PROFILE->load();

	PLATFORM->initializeBilling({
		{ "rubies.001", [this] { 
			addRubies(500);
		} }
	});

	PRECACHE_FONT_ALIAS("fonts/sansation.ttf", "default");

	STATS->setAlignment(Shared::StatsSystem::Align::BottomRight);

	Scene::Sprite::DefaultSampler = Renderer::Sampler::Linear;
    Scene::Label::DefaultFont = FONT("default");

#if defined(BUILD_DEVELOPER)
	CONSOLE->execute("hud_show_fps 1");
	CONSOLE->execute("hud_show_drawcalls 1");
	CONSOLE->execute("hud_show_batches 1");
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
	PROFILE->save();
	ENGINE->removeSystem<Client>();
	ENGINE->removeSystem<Achievements>();
}

void Application::initialize()
{
	auto root = getScene()->getRoot();

	auto sky = std::make_shared<Sky>();
	root->attach(sky, Scene::Node::AttachDirection::Front);

	auto main_menu = std::make_shared<MainMenu>();
	main_menu->setStartCallback([sky, main_menu] {
		auto gameplay = std::make_shared<Gameplay>();
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

	Actions::Run(Actions::Collection::MakeSequence(
		Actions::Collection::WaitOneFrame(),
		Actions::Collection::Wait(0.25f),
		Actions::Collection::Execute([sky] {
			sky->changeColor(Graphics::Color::Hsv::HueBlue, Graphics::Color::Hsv::HueRed);
		}),
		Actions::Collection::RepeatInfinite([sky] {
			return Actions::Collection::Delayed(10.0f,
				Actions::Collection::Execute([sky] {
					sky->changeColor();
				})
			);
		})	
	));

	Actions::Run(Actions::Collection::MakeSequence(
		Actions::Collection::WaitOneFrame(),
		Actions::Collection::Execute([this, main_menu] {
			SCENE_MANAGER->switchScreen(main_menu, [this] {
				tryShowDailyReward();
			});
		})
	));

	// hud

	Helpers::gHud = std::make_shared<Hud>();
	SCENE_MANAGER->getWindowHolder()->attach(Helpers::gHud); // after screens, before windows
}

void Application::onFrame()
{
	adaptToScreen(getScene()->getRoot());
	ShowCheatsMenu();
	GAME_STATS("connected", CLIENT->isConnected());
	GAME_STATS("event listeners", EVENT->getListenersCount());
}

void Application::addRubies(int count)
{
	ACHIEVEMENTS->hit("RUBIES_COLLECTED", count);
	PROFILE->increaseRubies(count);
	PROFILE->saveAsync();

	for (int i = 0; i < count; i++)
	{
		if (i > 8)
			break;

		auto ruby = std::make_shared<Scene::Sprite>();
		ruby->setTexture(TEXTURE("textures/ruby.png"));
		ruby->setPivot(0.5f);
		ruby->setAnchor(0.5f);
		ruby->setSize(24.0f);
		ruby->setPosition(glm::linearRand(glm::vec2(-64.0f), glm::vec2(64.0f)));
		ruby->setAlpha(0.0f);
		ruby->runAction(Actions::Collection::MakeSequence(
			Actions::Collection::Wait(i * (0.125f / 1.25f)),
			Actions::Collection::Show(ruby, 0.25f, Easing::CubicIn),
			Actions::Collection::Execute([this, ruby] {
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
	node->setStretch(1.0f / node->getScale());
}

void Application::onEvent(const Helpers::PrintEvent& e)
{
	auto root = getScene()->getRoot();

	auto rect = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	rect->setTouchable(true);
	rect->setSize({ 264.0f, 42.0f });
	rect->setAlpha(0.25f);
	rect->setRounding(12.0f);
	rect->setAbsoluteRounding(true);
	rect->setAnchor({ -0.5f, 0.125f });
	rect->setPivot(0.5f);
	rect->setMargin({ 0.0f, -18.0f });
	rect->runAction(Actions::Collection::MakeSequence(
		Actions::Collection::ChangeHorizontalAnchor(rect, 0.5f, 0.5f, Easing::CubicOut),
		Actions::Collection::Wait(3.0f),
		Actions::Collection::ChangeHorizontalAnchor(rect, 1.5f, 0.5f, Easing::CubicIn),
		Actions::Collection::Kill(rect)
	));
	root->attach(rect);

	auto label = std::make_shared<Scene::Label>();
	label->setFont(FONT("default"));
	label->setFontSize(16.0f);
	label->setText(e.text);
	label->setMultiline(true);
	label->setMultilineAlign(Graphics::TextMesh::Align::Center);
	label->setStretch({ 1.0f, 0.0f });
	label->setHorizontalMargin(-rect->getVerticalMargin());
	label->setPivot(0.5f);
	label->setAnchor(0.5f);
	rect->attach(label);

	rect->runAction(Actions::Collection::ExecuteInfinite([rect, label] {
		rect->setHeight(label->getHeight());
	}));
}

void Application::onEvent(const Shared::Profile::ProfileSavedEvent& e)
{
	CLIENT->commit();
}
