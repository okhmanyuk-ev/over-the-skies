#include "application.h"
#include "main_menu.h"
#include "gameplay.h"
#include "gameover_menu.h"
#include "sky.h"
#include "player.h"
#include "plane.h"
#include "skin.h"
#include "profile.h"
#include "windows/daily_reward_window.h"
#include "helpers.h"
#include "achievements.h"
#include "yandex.h"
#include <sky/sky.h>

using namespace hcg001;

Application::Application()
{
	PLATFORM->setTitle(PRODUCT_NAME);
#if defined(PLATFORM_MAC)
	PLATFORM->resize(720, 1280);
#elif defined(PLATFORM_WINDOWS)
	PLATFORM->resize(540, 960);
#else
	PLATFORM->resize(360, 640);
#endif

#if defined(PLATFORM_MAC)
	std::static_pointer_cast<Shared::ConsoleDevice>(CONSOLE_DEVICE)->setHiddenButtonEnabled(false);
#endif

	// limit maximum time delta to avoid animation breaks
	sky::Scheduler::Instance->setTimeDeltaLimit(sky::FromSeconds(1.0f / 30.0f));

	sky::Locator<Profile>::Init();
	sky::Locator<Achievements>::Init();

	PROFILE->load();

	PLATFORM->initializeBilling({
		{ "rubies.001", [this] {
			addRubies(500);
		} }
	});

	sky::PrecacheFont("fonts/sansation.ttf", "default");
	sky::GetService<Shared::StatsSystem>()->setAlignment(Shared::StatsSystem::Align::BottomRight);

	sky::GetService<Scene::Scene>()->setScreenAdaption(glm::vec2{ 360.0f, 640.0f });
	Scene::Sprite::DefaultSampler = skygfx::Sampler::Linear;
	Scene::Sprite::DefaultTexture = sky::GetTexture("textures/default.png");
    Scene::Label::DefaultFont = sky::GetFont("default");
	Scene::Scrollbox::DefaultInertiaFriction = 0.05f;

	sky::GetService<sky::Cache>()->makeAtlases();

	sky::RunAction([this] {
		initialize();
	});
}

Application::~Application()
{
	PROFILE->save();
	sky::Locator<Profile>::Reset();
	sky::Locator<Achievements>::Reset();
}

void Application::initialize()
{
	Yandex::InitSdk();
	auto lang = sky::Localization::Language::English;
	const auto& args = sky::GetService<sky::Application>()->getStartupKeyValues();

	if (args.contains("lang"))
	{
		auto value = args.at("lang");
		if (value == "ru")
			lang = sky::Localization::Language::Russian;
	}

	sky::GetService<sky::Localization>()->setLanguage(lang);

	auto root = sky::GetService<Scene::Scene>()->getRoot();

	Helpers::gSky = std::make_shared<Sky>();
	root->attach(Helpers::gSky, Scene::Node::AttachDirection::Front);

	sky::RunAction(sky::Actions::Sequence(
		sky::Actions::WaitGlobalFrame(),
		sky::Actions::Wait(0.25f),
		[] {
			//sky->changeColor(Graphics::Color::Hsv::HueBlue, Graphics::Color::Hsv::HueRed);
			Helpers::gSky->changeColor(205.0f, 15.0f);
		},
		sky::Actions::RepeatInfinite([] {
			return sky::Actions::Delayed(10.0f, [] {
				Helpers::gSky->changeColor();
			});
		})
	));

	Helpers::gMainMenu = std::make_shared<MainMenu>();

	sky::RunAction(sky::Actions::Sequence(
		sky::Actions::WaitGlobalFrame(),
		[this] {
			SCENE_MANAGER->switchScreen(Helpers::gMainMenu, [this] {
				tryShowDailyReward();
				sky::RunAction(sky::Actions::Delayed(3.0f, [] {
					Helpers::gSky->spawnSomeAsteroids();
				}));
			});
		}
	));

	auto tada_particles_holder = std::make_shared<Scene::Node>();
	SCENE_MANAGER->attach(tada_particles_holder);
	Helpers::AchievementNotify::ParticlesHolder = tada_particles_holder;
}

void Application::onFrame()
{
	showCheats();
	sky::Indicator("event listeners", sky::GetService<sky::Dispatcher>()->getListenersCount());
}

void Application::addRubies(int count)
{
	PROFILE->increaseRubies(count);
	PROFILE->save();

	for (int i = 0; i < count; i++)
	{
		if (i > 8)
			break;

		auto ruby = std::make_shared<Scene::Sprite>();
		ruby->setTexture(sky::GetTexture("textures/ruby.png"));
		ruby->setPivot(0.5f);
		ruby->setAnchor(0.5f);
		ruby->setSize(24.0f);
		ruby->setPosition(glm::linearRand(glm::vec2(-64.0f), glm::vec2(64.0f)));
		ruby->setAlpha(0.0f);
		ruby->runAction(sky::Actions::Sequence(
			sky::Actions::Wait(i * (0.125f / 1.25f)),
			sky::Actions::Show(ruby, 0.25f, Easing::CubicIn),
			[this, ruby] {
				sky::RunAction([this, ruby] {
					Helpers::gMainMenu->getRubiesIndicator()->collectRubyAnim(ruby);
				});
			}
		));
		Helpers::gMainMenu->attach(ruby);
	}
}

void Application::tryShowDailyReward()
{
	const long long OneDay = 60 * 60 * 24;

	auto now = sky::SystemNowSeconds();

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
		PROFILE->save();

		addRubies(rubies_count);
	});
	SCENE_MANAGER->pushWindow(window);
}

void Application::onEvent(const Achievements::AchievementEarnedEvent& e)
{
	auto node = std::make_shared<Helpers::AchievementNotify>(e.item);
	node->setAnchor({ 0.5f, 0.0f });
	node->setPivot({ 0.5f, 1.0f });
	node->runAction(sky::Actions::Sequence(
		sky::Actions::Concurrent(
			sky::Actions::ChangeVerticalPivot(node, 0.5f, 0.25f, Easing::CubicOut),
			sky::Actions::ChangeVerticalAnchor(node, 0.125f, 0.25f, Easing::CubicOut)
		),
		sky::Actions::Wait(0.25f),
		[node] {
			node->showTada();
		},
		sky::Actions::Wait(2.0f),
		sky::Actions::Concurrent(
			sky::Actions::ChangeVerticalPivot(node, 1.0f, 0.25f, Easing::CubicIn),
			sky::Actions::ChangeVerticalAnchor(node, 0.0f, 0.25f, Easing::CubicIn)
		),
		sky::Actions::Kill(node)
	));
	sky::GetService<Scene::Scene>()->getRoot()->attach(node);
}

void Application::showCheats()
{
#if !defined(BUILD_DEVELOPER)
	return;
#endif

	static bool HideThisMenu = false;

	if (HideThisMenu)
		return;

	ImGui::Begin("dev", nullptr, ImGui::User::ImGuiWindowFlags_ControlPanel);
	ImGui::SetWindowPos(ImGui::User::BottomLeftCorner());

	static bool Enabled = false;

	if (Enabled)
	{
		if (ImGui::Button("HIDE THIS MENU"))
			HideThisMenu = true;

		ImGui::Separator();

		if (ImGui::Button("CLEAR PROFILE"))
		{
			PROFILE->clear();
		}

		if (ImGui::Button("RUBIES +10"))
		{
			PROFILE->setRubies(PROFILE->getRubies() + 10);
		}

		if (ImGui::Button("DAILY REWARD WINDOW"))
		{
			auto window = std::make_shared<DailyRewardWindow>(2);
			SCENE_MANAGER->pushWindow(window);
		}

		if (ImGui::Button("COMPLETE ALL ACHIEVEMENTS"))
		{
			for (auto item : ACHIEVEMENTS->getItems())
			{
				auto& progress = ACHIEVEMENTS->getProgress(item.name);
				progress = item.required;
			}
			PROFILE->save();
		}

		if (ImGui::Button("FAKE ACHIEVEMENT EARNED EVENT"))
		{
			//auto item = *ACHIEVEMENTS->getItems().begin();
			auto item = ACHIEVEMENTS->getItemByName("COVER_DISTANCE_100000").value();
			sky::Emit(Achievements::AchievementEarnedEvent{ item });
		}

		if (ImGui::Button("SPAWN BLURRED GLASS"))
		{
			sky::GetService<sky::CommandProcessor>()->execute("spawn_blur_glass");
		}

		if (ImGui::Button("SPAWN GRAY GLASS"))
		{
			sky::GetService<sky::CommandProcessor>()->execute("spawn_gray_glass");
		}

		if (ImGui::Button("SPAWN SHOCKWAVE"))
		{
			sky::GetService<sky::CommandProcessor>()->execute("spawn_shockwave");
		}

		if (ImGui::Button("SPAWN SHOCKWAVE (LONG)"))
		{
			sky::GetService<sky::CommandProcessor>()->execute("spawn_shockwave 5.0");
		}

		if (ImGui::Button("SPAWN SHOCKWAVE (VERY LONG)"))
		{
			sky::GetService<sky::CommandProcessor>()->execute("spawn_shockwave 10.0");
		}

		if (ImGui::Button("TOGGLE BLOOM"))
		{
			sky::GetService<sky::CommandProcessor>()->execute("if r_bloom_enabled 1 \"r_bloom_enabled 0\" \"r_bloom_enabled 1\"");
		}

		if (ImGui::Button("SPAWN ASTEROIDS"))
		{
			Helpers::gSky->spawnSomeAsteroids();
		}
	}

	ImGui::Checkbox("DEV", &Enabled);
	ImGui::End();
}
