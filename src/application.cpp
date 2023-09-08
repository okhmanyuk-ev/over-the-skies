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

using namespace hcg001;

Application::Application() : Shared::Application(PROJECT_NAME, { Flag::Audio, Flag::Scene, Flag::Network })
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
	FRAME->setTimeDeltaLimit(Clock::FromSeconds(1.0f / 30.0f));

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

	Scene::Sprite::DefaultSampler = skygfx::Sampler::Linear;
	Scene::Sprite::DefaultTexture = TEXTURE("textures/default.png");
    Scene::Label::DefaultFont = FONT("default");
	Scene::Scrollbox::DefaultInertiaFriction = 0.05f;

	CACHE->makeAtlases();

	FRAME->addOne([this] {
		initialize();
	});
}

Application::~Application()
{
	PROFILE->save();
	ENGINE->removeSystem<Achievements>();
}

void Application::initialize()
{
	auto root = getScene()->getRoot();

	Helpers::gSky = std::make_shared<Sky>();
	root->attach(Helpers::gSky, Scene::Node::AttachDirection::Front);

	Actions::Run(Actions::Collection::MakeSequence(
		Actions::Collection::WaitOneFrame(),
		Actions::Collection::Wait(0.25f),
		Actions::Collection::Execute([] {
			//sky->changeColor(Graphics::Color::Hsv::HueBlue, Graphics::Color::Hsv::HueRed);
			Helpers::gSky->changeColor(205.0f, 15.0f);
		}),
		Actions::Collection::RepeatInfinite([] {
			return Actions::Collection::Delayed(10.0f,
				Actions::Collection::Execute([] {
					Helpers::gSky->changeColor();
				})
			);
		})	
	));

	Helpers::gMainMenu = std::make_shared<MainMenu>();

	Actions::Run(Actions::Collection::MakeSequence(
		Actions::Collection::WaitOneFrame(),
		Actions::Collection::Execute([this] {
			SCENE_MANAGER->switchScreen(Helpers::gMainMenu, [this] {
				tryShowDailyReward();
			});
		})
	));

	auto tada_particles_holder = std::make_shared<Scene::Node>();
	SCENE_MANAGER->attach(tada_particles_holder);
	Helpers::AchievementNotify::ParticlesHolder = tada_particles_holder;
}

void Application::onFrame()
{
	adaptToScreen(getScene()->getRoot());
	showCheats();
	GAME_STATS("event listeners", EVENT->getListenersCount());
}

void Application::addRubies(int count)
{
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
					Helpers::gMainMenu->getRubiesIndicator()->collectRubyAnim(ruby);
				});
			})
		));
		Helpers::gMainMenu->attach(ruby);
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

void Application::onEvent(const Achievements::AchievementEarnedEvent& e)
{
	auto node = std::make_shared<Helpers::AchievementNotify>(e.item);
	node->setAnchor({ 0.5f, 0.0f });
	node->setPivot({ 0.5f, 1.0f });
	node->runAction(Actions::Collection::MakeSequence(
		Actions::Collection::MakeParallel(
			Actions::Collection::ChangeVerticalPivot(node, 0.5f, 0.25f, Easing::CubicOut),
			Actions::Collection::ChangeVerticalAnchor(node, 0.125f, 0.25f, Easing::CubicOut)
		),
		Actions::Collection::Wait(0.25f),
		Actions::Collection::Execute([node] {
			node->showTada();
		}),
		Actions::Collection::Wait(2.0f),
		Actions::Collection::MakeParallel(
			Actions::Collection::ChangeVerticalPivot(node, 1.0f, 0.25f, Easing::CubicIn),
			Actions::Collection::ChangeVerticalAnchor(node, 0.0f, 0.25f, Easing::CubicIn)
		),
		Actions::Collection::Kill(node)
	));
	getScene()->getRoot()->attach(node);
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
			PROFILE->saveAsync();
		}

		if (ImGui::Button("FAKE ACHIEVEMENT EARNED EVENT"))
		{
			auto item = *ACHIEVEMENTS->getItems().begin();
			EVENT->emit(Achievements::AchievementEarnedEvent{ item });
		}

		if (ImGui::Button("SPAWN BLURRED GLASS"))
		{
			CONSOLE->execute("spawn_blur_glass");
		}

		if (ImGui::Button("SPAWN GRAY GLASS"))
		{
			CONSOLE->execute("spawn_gray_glass");
		}

		if (ImGui::Button("SPAWN SHOCKWAVE"))
		{
			CONSOLE->execute("spawn_shockwave");
		}

		if (ImGui::Button("SPAWN SHOCKWAVE (LONG)"))
		{
			CONSOLE->execute("spawn_shockwave 5.0");
		}

		if (ImGui::Button("SPAWN SHOCKWAVE (VERY LONG)"))
		{
			CONSOLE->execute("spawn_shockwave 10.0");
		}

		if (ImGui::Button("TOGGLE BLOOM"))
		{
			CONSOLE->execute("if r_bloom_enabled 1 \"r_bloom_enabled 0\" \"r_bloom_enabled 1\"");
		}
	}

	ImGui::Checkbox("DEV", &Enabled);
	ImGui::End();
}
