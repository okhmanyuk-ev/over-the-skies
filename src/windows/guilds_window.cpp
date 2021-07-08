#include "guilds_window.h"
#include "create_guild_window.h"
#include "response_wait_window.h"

using namespace hcg001;

GuildsWindow::GuildsWindow()
{
	getBackground()->setSize({ 314.0f, 386.0f });
	getTitle()->setText(LOCALIZE("GUILDS_WINDOW_TITLE"));

	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::Wait([this] { return getState() != Window::State::Opened; }),
		Actions::Collection::Wait([] {
			return !CLIENT->isConnected();
		}),
		Actions::Collection::Execute([this] {
			if (!PROFILE->isInGuild())
			{
				createGuildSearchContent();
				return;
			}

			runAction(Actions::Collection::MakeSequence(
				Actions::Collection::Execute([] {
					CLIENT->requireGuildInfo(PROFILE->getGuildId());
				}),
				Actions::Collection::Wait([] {
					return !CLIENT->hasGuild(PROFILE->getGuildId());
				}),
				Actions::Collection::Execute([this] {
					createMyGuildContent();
				})
			));
		})
	));
}

void GuildsWindow::createMyGuildContent()
{
	auto content = std::make_shared<MyGuildContent>();
	getBody()->attach(content);
}

void GuildsWindow::createGuildSearchContent()
{
	auto content = std::make_shared<SearchContent>();
	getBody()->attach(content);
}

GuildsWindow::MyGuildContent::MyGuildContent()
{
	setStretch(1.0f);

	auto content_holder = std::make_shared<Scene::Node>();
	content_holder->setStretch(1.0f);
	content_holder->setAnchor({ 0.5f, 1.0f });
	content_holder->setPivot({ 0.5f, 1.0f });
	content_holder->setVerticalMargin(32.0f);
	attach(content_holder);

	auto info_content = std::make_shared<InfoContent>();
	content_holder->attach(info_content);

	auto chat_content = std::make_shared<ChatContent>();
	content_holder->attach(chat_content);

	auto chat_tab_button = std::make_shared<Helpers::Button>();
	chat_tab_button->getLabel()->setText(LOCALIZE("CHAT"));
	chat_tab_button->getLabel()->setFontSize(18.0f);
	chat_tab_button->setSize({ 128.0f, 28.0f });
	chat_tab_button->setPosition({ 18.0f, 9.0f });
	attach(chat_tab_button);

	auto info_tab_button = std::make_shared<Helpers::Button>();
	info_tab_button->getLabel()->setText(LOCALIZE("INFO"));
	info_tab_button->getLabel()->setFontSize(18.0f);
	info_tab_button->setSize({ 128.0f, 28.0f });
	info_tab_button->setPosition({ 170.0f, 9.0f });
	attach(info_tab_button);

	for (auto button : { chat_tab_button, info_tab_button })
	{
		button->setClickCallback([button, chat_tab_button, info_tab_button, info_content, chat_content] {
			info_content->setEnabled(button == info_tab_button);
			chat_content->setEnabled(button == chat_tab_button);
		});
	}

	chat_tab_button->click();
}

GuildsWindow::MyGuildContent::ChatContent::ChatContent()
{
	setStretch(1.0f);

	auto asdaw = std::make_shared<Helpers::Button>();
	asdaw->getLabel()->setText("THIS IS CHAT");
	asdaw->getLabel()->setFontSize(18.0f);
	asdaw->setSize({ 128.0f, 28.0f });
	asdaw->setAnchor(0.5f);
	asdaw->setPivot(0.5f);
	attach(asdaw);
}

GuildsWindow::MyGuildContent::InfoContent::InfoContent()
{
	setStretch(1.0f);

	auto guild = CLIENT->getGuild(PROFILE->getGuildId())->getJson();

	std::string title = guild["title"];
	std::set<int> members = guild["members"];

	auto label = std::make_shared<Helpers::Label>();
	label->setPivot({ 0.0f, 0.5f });
	label->setPosition({ 16.0f, 16.0f });
	label->setText("name: " + title);
	attach(label);

	auto label2 = std::make_shared<Helpers::Label>();
	label2->setPivot({ 0.0f, 0.5f });
	label2->setPosition({ 16.0f, 48.0f });
	label2->setText("members: " + std::to_string(members.size()));
	attach(label2);

	auto exit_button = std::make_shared<Helpers::Button>();
	exit_button->getLabel()->setText(LOCALIZE("EXIT"));
	exit_button->getLabel()->setFontSize(18.0f);
	exit_button->setAnchor({ 0.5f, 1.0f });
	exit_button->setPivot(0.5f);
	exit_button->setSize({ 128.0f, 28.0f });
	exit_button->setY(-24.0f);
	exit_button->setClickCallback([] {
		auto window = std::make_shared<ResponseWaitWindow>();
		SCENE_MANAGER->pushWindow(window);
		window->runAction(Actions::Collection::MakeSequence(
			Actions::Collection::Wait([window] { return window->getState() != Window::State::Opened; }),
			Actions::Collection::Execute([] {
				CLIENT->exitGuild();
			}),
			Actions::Collection::Breakable(5.0f,
				Actions::Collection::WaitEvent<Channel::ExitedFromGuildEvent>([](const auto& e) {
					LOG("exited from guild");
				})
			),
			Actions::Collection::Wait(0.5f),
			Actions::Collection::Execute([] {
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
	attach(exit_button);
}

GuildsWindow::SearchContent::SearchContent()
{
	setStretch(1.0f);

	auto create_button = std::make_shared<Helpers::Button>();
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
	attach(create_button);

	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::Execute([] {
			CLIENT->requestGuildList();
		}),
		Actions::Collection::Breakable(5.0f,
			Actions::Collection::WaitEvent<Channel::GuildListEvent>([this](const auto& e) {
				LOGF("recevied guild list with {} guilds", e.ids.size());
				createGuildItems(e.ids);
			})
		)
	));
}

void GuildsWindow::SearchContent::createGuildItems(const std::vector<int> ids)
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

	attach(scrollbox);
}

GuildsWindow::SearchContent::Item::Item(int guildId)
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

	mMembersLabel = std::make_shared<Helpers::Label>();
	mMembersLabel->setAnchor({ 0.0f, 0.5f });
	mMembersLabel->setPivot({ 1.0f, 0.5f });
	mMembersLabel->setX(214.0f);
	mMembersLabel->setFontSize(15.0f);
	attach(mMembersLabel);

	auto join_button = std::make_shared<Helpers::Button>();
	join_button->setTouchMask(1 << 1);
	join_button->getLabel()->setText(LOCALIZE("JOIN"));
	join_button->setClickCallback([guildId] {
		auto window = std::make_shared<ResponseWaitWindow>();
		SCENE_MANAGER->pushWindow(window);
		window->runAction(Actions::Collection::MakeSequence(
			Actions::Collection::Wait([window] { return window->getState() != Window::State::Opened; }),
			Actions::Collection::Execute([guildId] {
				CLIENT->joinGuild(guildId);
			}),
			Actions::Collection::Breakable(5.0f,
				Actions::Collection::WaitEvent<Channel::JoinedToGuildEvent>([](const auto& e) {
					LOGF("joined to guild {}", e.id);
				})
			),
			Actions::Collection::Wait(0.5f),
			Actions::Collection::Execute([] {
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

void GuildsWindow::SearchContent::Item::refresh(Channel::GuildPtr guild_info)
{
	auto members_count = guild_info->getJson()["members"].get<std::set<int>>().size();

	mTitle->setText(guild_info->getJson()["title"].get<std::string>());
	mMembersLabel->setText(fmt::format("{}/{}", members_count, 100));
}

