#include "guilds_window.h"
#include "create_guild_window.h"
#include "response_wait_window.h"

using namespace hcg001;

GuildsWindow::GuildsWindow()
{
	getBackground()->setSize({ 314.0f, 386.0f });
	getTitle()->setText(LOCALIZE("GUILDS_WINDOW_TITLE"));

	runAction(Actions::Factory::MakeSequence(
		Actions::Factory::Wait([this] { return getState() != Window::State::Opened; }),
		Actions::Factory::Wait([] {
			return !CLIENT->isConnected();
		}),
		Actions::Factory::Execute([this] {
			if (PROFILE->isInGuild())
			{
				runAction(Actions::Factory::MakeSequence(
					Actions::Factory::Execute([] {
						CLIENT->requireGuildInfo(PROFILE->getGuildId());
					}),
					Actions::Factory::Wait([] {
						return !CLIENT->hasGuild(PROFILE->getGuildId());
					}),
					Actions::Factory::Execute([this] {
						createMyGuildContent();
					})
				));
			}
			else
			{
				createGuildSearchContent();
			}
		})
	));
}


void GuildsWindow::createMyGuildContent()
{
	auto guild = CLIENT->getGuild(PROFILE->getGuildId())->getJson();

	std::string title = guild["title"];
	std::set<int> members = guild["members"];

	auto label = std::make_shared<Helpers::Label>();
	label->setPivot({ 0.0f, 0.5f });
	label->setPosition({ 16.0f, 16.0f });
	label->setText("name: " + title);
	getBody()->attach(label);

	auto label2 = std::make_shared<Helpers::Label>();
	label2->setPivot({ 0.0f, 0.5f });
	label2->setPosition({ 16.0f, 48.0f });
	label2->setText("members: " + std::to_string(members.size()));
	getBody()->attach(label2);

	auto exit_button = std::make_shared<Helpers::Button>();
	exit_button->setColor(Helpers::ButtonColor);
	exit_button->getLabel()->setText(LOCALIZE("EXIT"));
	exit_button->getLabel()->setFontSize(18.0f);
	exit_button->setClickCallback([] {
		auto window = std::make_shared<ResponseWaitWindow>();
		SCENE_MANAGER->pushWindow(window);
		window->runAction(Actions::Factory::MakeSequence(
			Actions::Factory::Wait([window] { return window->getState() != Window::State::Opened; }),
			Actions::Factory::Execute([] {
				CLIENT->exitGuild();
			}),
			Actions::Factory::Breakable(5.0f, 
				Actions::Factory::WaitEvent<Channel::ExitedFromGuildEvent>([](const auto& e) {
					LOG("exited from guild");
				})
			),
			Actions::Factory::Wait(0.5f),
			Actions::Factory::Execute([] {
				SCENE_MANAGER->popWindow([] {
					if (PROFILE->isInGuild())
						return;
					
					SCENE_MANAGER->popWindow([] {
						auto window = std::make_shared<GuildsWindow>();
						SCENE_MANAGER->pushWindow(window);
					});
				});
			})
		));
	});
	exit_button->setAnchor({ 0.5f, 1.0f });
	exit_button->setPivot(0.5f);
	exit_button->setSize({ 128.0f, 28.0f });
	exit_button->setY(-24.0f);
	getBody()->attach(exit_button);
}

void GuildsWindow::createGuildSearchContent()
{
	auto create_button = std::make_shared<Helpers::Button>();
	create_button->setColor(Helpers::ButtonColor);
	create_button->getLabel()->setText(LOCALIZE("CREATE"));
	create_button->getLabel()->setFontSize(18.0f);
	create_button->setClickCallback([] {
		auto window = std::make_shared<CreateGuildWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
	create_button->setAnchor({ 0.5f, 1.0f });
	create_button->setPivot(0.5f);
	create_button->setSize({ 128.0f, 28.0f });
	create_button->setY(-24.0f);
	getBody()->attach(create_button);

	runAction(Actions::Factory::MakeSequence(
		Actions::Factory::Wait([this] { return getState() != Window::State::Opened; }),
		Actions::Factory::Execute([] {
			CLIENT->requestGuildList();
		}),
		Actions::Factory::Breakable(5.0f, 
			Actions::Factory::WaitEvent<Channel::GuildListEvent>([this](const auto& e) {
				LOGF("recevied guild list with {} guilds", e.ids.size());
				createGuildItems(e.ids);
			})
		)
	));
}

void GuildsWindow::createGuildItems(const std::vector<int> ids)
{
	std::vector<std::shared_ptr<Scene::Node>> items;

	for (auto id : ids)
	{
		auto item = std::make_shared<Item>(id);
		item->setAnchor(0.5f);
		item->setPivot(0.5f);
		items.push_back(item);
	}

	const glm::vec2 ItemSize = { 314.0f, 48.0f };

	auto grid = Shared::SceneHelpers::MakeVerticalGrid(ItemSize, items);

	auto scrollbox = std::make_shared<Scene::ClippableScissor<Scene::Scrollbox>>();
	scrollbox->setStretch(1.0f);
	scrollbox->getBounding()->setStretch(1.0f);
	scrollbox->getContent()->setSize(grid->getSize());
	scrollbox->getContent()->attach(grid);
	scrollbox->setVerticalMargin(48.0f);
	scrollbox->setSensitivity({ 0.0f, 1.0f });

	getBody()->attach(scrollbox);
}

GuildsWindow::Item::Item(int guildId)
{
	setStretch({ 1.0f, 0.0f });
	setMargin(8.0f);
	setHeight(48.0f);
	setRounding(8.0f);
	setAbsoluteRounding(true);
	setAlpha(0.125f);

	mTitle = std::make_shared<Helpers::Label>();
	mTitle->setAnchor({ 0.0f, 0.5f });
	mTitle->setPivot({ 0.0f, 0.5f });
	mTitle->setX(8.0f);
	mTitle->setFontSize(15.0f);
	attach(mTitle);

	auto join_button = std::make_shared<Helpers::Button>();
	join_button->setTouchMask(1 << 1);
	join_button->setColor(Helpers::ButtonColor);
	join_button->getLabel()->setText(LOCALIZE("JOIN"));
	join_button->setClickCallback([guildId] {
		auto window = std::make_shared<ResponseWaitWindow>();
		SCENE_MANAGER->pushWindow(window);
		window->runAction(Actions::Factory::MakeSequence(
			Actions::Factory::Wait([window] { return window->getState() != Window::State::Opened; }),
			Actions::Factory::Execute([guildId] {
				CLIENT->joinGuild(guildId);
			}),
			Actions::Factory::Breakable(5.0f, 
				Actions::Factory::WaitEvent<Channel::JoinedToGuildEvent>([](const auto& e) {
					LOGF("joined to guild {}", e.id);
				})
			),
			Actions::Factory::Wait(0.5f),
			Actions::Factory::Execute([] {
				SCENE_MANAGER->popWindow([] {
					if (!PROFILE->isInGuild())
						return;
					
					SCENE_MANAGER->popWindow([] {
						auto window = std::make_shared<GuildsWindow>();
						SCENE_MANAGER->pushWindow(window);
					});
				});
			})
		));
	});
	join_button->setAnchor({ 1.0f, 0.5f });
	join_button->setPivot({ 1.0f, 0.5f });
	join_button->setSize({ 64.0f, 24.0f });
	join_button->setX(-8.0f);
	attach(join_button);

	setGuildCallback([this](auto guild_info) {
		refresh(guild_info);
	});

	setGuildID(guildId);

	CLIENT->requireGuildInfo(guildId);
}

void GuildsWindow::Item::refresh(Channel::GuildPtr guild_info)
{
	mTitle->setText(guild_info->getJson()["title"].get<std::string>());
}

