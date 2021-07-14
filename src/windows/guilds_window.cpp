#include "guilds_window.h"
#include "create_guild_window.h"
#include "response_wait_window.h"

using namespace hcg001;

GuildsWindow::GuildsWindow()
{
	getBackground()->setSize({ 314.0f, 512.0f });
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

	auto tab_buttons_holder = std::make_shared<Scene::Node>();
	tab_buttons_holder->setStretch({ 1.0f, 0.0f });
	tab_buttons_holder->setAnchor({ 0.5f, 0.0f });
	tab_buttons_holder->setPivot({ 0.5f, 0.0f });
	tab_buttons_holder->setHeight(28.0f);
	attach(tab_buttons_holder);

	auto content_holder = std::make_shared<Scene::Node>();
	content_holder->setStretch(1.0f);
	content_holder->setAnchor({ 0.5f, 1.0f });
	content_holder->setPivot({ 0.5f, 1.0f });
	content_holder->setVerticalMargin(tab_buttons_holder->getHeight());
	attach(content_holder);

	auto chat_tab_button = std::make_shared<TabButton>(LOCALIZE("CHAT"));
	chat_tab_button->setClickCallback([this] {
		mTabsManager.show(PageType::Chat);
	});

	auto info_tab_button = std::make_shared<TabButton>(LOCALIZE("INFO"));
	info_tab_button->setClickCallback([this] {
		mTabsManager.show(PageType::Info);
	});

	auto tab_button_grid = Shared::SceneHelpers::MakeGrid({ 
		{ chat_tab_button, info_tab_button },
	});

	tab_button_grid->setStretch(1.0f);
	tab_button_grid->setAnchor(0.5f);
	tab_button_grid->setPivot(0.5f);
	tab_buttons_holder->attach(tab_button_grid);

	mTabsManager.addButton(PageType::Chat, chat_tab_button);
	mTabsManager.addButton(PageType::Info, info_tab_button);

	auto chat_content = std::make_shared<ChatContent>();
	content_holder->attach(chat_content);

	auto info_content = std::make_shared<InfoContent>();
	content_holder->attach(info_content);

	mTabsManager.addContent(PageType::Chat, chat_content);
	mTabsManager.addContent(PageType::Info, info_content);

	mTabsManager.show(PageType::Chat);
}

// tab button

GuildsWindow::MyGuildContent::TabButton::TabButton(const utf8_string& text)
{
	getLabel()->setText(text);
	getLabel()->setFontSize(18.0f);
	setStretch(1.0f);
	setAnchor(0.5f);
	setPivot(0.5f);
	setRounding(0.0f);
	setMargin({ 2.0f, 0.0f });
}

void GuildsWindow::MyGuildContent::TabButton::onJoin()
{
	//setEnabled(false);
}

void GuildsWindow::MyGuildContent::TabButton::onEnter()
{
	//setEnabled(true);
}

void GuildsWindow::MyGuildContent::TabButton::onLeave()
{
	//setEnabled(false);
}

// tab content

void GuildsWindow::MyGuildContent::TabContent::onJoin()
{
	setEnabled(false);
}

void GuildsWindow::MyGuildContent::TabContent::onEnter()
{
	setEnabled(true);
}

void GuildsWindow::MyGuildContent::TabContent::onLeave()
{
	setEnabled(false);
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
	int score = guild["score"];

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

	auto label3 = std::make_shared<Helpers::Label>();
	label3->setPivot({ 0.0f, 0.5f });
	label3->setPosition({ 16.0f, 48.0f + (48.0f - 16.0f) });
	label3->setText("score: " + std::to_string(score));
	attach(label3);

	auto leave_button = std::make_shared<Helpers::Button>();
	leave_button->setButtonColor(Helpers::Pallete::ButtonColorRed);
	leave_button->getLabel()->setText(LOCALIZE("GUILDS_WINDOW_LEAVE"));
	leave_button->getLabel()->setFontSize(18.0f);
	leave_button->setAnchor({ 0.5f, 1.0f });
	leave_button->setPivot(0.5f);
	leave_button->setSize({ 128.0f, 28.0f });
	leave_button->setY(-24.0f);
	leave_button->setClickCallback([] {
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
	attach(leave_button);
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
	setColor(Helpers::Pallete::WindowItem);

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

