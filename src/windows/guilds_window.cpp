#include "guilds_window.h"
#include "create_guild_window.h"
#include "response_wait_window.h"
#include "confirm_window.h"

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

	auto chat_tab_button = std::make_shared<TabButton>(LOCALIZE("GUILDS_WINDOW_TAB_CHAT"));
	chat_tab_button->setClickCallback([this] {
		mTabsManager.show(PageType::Chat);
	});

	auto info_tab_button = std::make_shared<TabButton>(LOCALIZE("GUILDS_WINDOW_TAB_INFO"));
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

// info content

GuildsWindow::MyGuildContent::InfoContent::InfoContent()
{
	setStretch(1.0f);

	auto guild = CLIENT->getGuild(PROFILE->getGuildId())->getJson();

	std::string title = guild["title"];
	std::set<int> members = guild["members"];
	int score = guild["score"];

	auto top_section = std::make_shared<Scene::Node>();
	top_section->setStretch({ 1.0f, 0.0f });
	top_section->setHeight(128.0f);
	attach(top_section);

	auto bottom_section = std::make_shared<Scene::Node>();
	bottom_section->setStretch(1.0f);
	bottom_section->setVerticalMargin(top_section->getHeight());
	bottom_section->setAnchor({ 0.5f, 1.0f });
	bottom_section->setPivot({ 0.5f, 1.0f });
	attach(bottom_section);

	auto info_bg = std::make_shared<Scene::Rectangle>();
	info_bg->setStretch(1.0f);
	info_bg->setPivot(0.5f);
	info_bg->setAnchor(0.5f);
	info_bg->setMargin(16.0f);
	info_bg->setAbsoluteRounding(true);
	info_bg->setRounding(8.0f);
	info_bg->setAlpha(0.125f);
	top_section->attach(info_bg);

	auto label = std::make_shared<Helpers::Label>();
	label->setPivot({ 0.0f, 0.5f });
	label->setPosition({ 16.0f, 16.0f });
	label->setText("name: " + title);
	info_bg->attach(label);

	auto label2 = std::make_shared<Helpers::Label>();
	label2->setPivot({ 0.0f, 0.5f });
	label2->setPosition({ 16.0f, 48.0f });
	label2->setText("members: " + std::to_string(members.size()));
	info_bg->attach(label2);

	auto label3 = std::make_shared<Helpers::Label>();
	label3->setPivot({ 0.0f, 0.5f });
	label3->setPosition({ 16.0f, 48.0f + (48.0f - 16.0f) });
	label3->setText("score: " + std::to_string(score));
	info_bg->attach(label3);

	auto leave_button = std::make_shared<Helpers::Button>();
	leave_button->setButtonColor(Helpers::Pallete::ButtonColorRed);
	leave_button->getLabel()->setText(LOCALIZE("GUILDS_WINDOW_LEAVE"));
	leave_button->getLabel()->setFontSize(18.0f);
	leave_button->setAnchor({ 1.0f, 0.0f });
	leave_button->setPivot({ 1.0f, 0.0f });
	leave_button->setSize({ 96.0f, 24.0f });
	leave_button->setPosition({ -8.0f, 32.0f });
	leave_button->setClickCallback([] {
		auto title = LOCALIZE("ARE YOU SURE ?");
		auto yes_text = LOCALIZE("YES");
		auto no_text = LOCALIZE("NO");
		auto confirm_window = std::make_shared<ConfirmWindow>(title, yes_text, no_text);
		confirm_window->setYesCallback([] {
			SCENE_MANAGER->popWindow([] {
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
		});
		confirm_window->setNoCallback([] {
			SCENE_MANAGER->popWindow();
		});
		SCENE_MANAGER->pushWindow(confirm_window);
	});
	info_bg->attach(leave_button);

	std::vector<std::shared_ptr<Scene::Node>> items;

	int num = 0;

	for (auto uid : members)
	{
		num += 1;

		auto item = std::make_shared<Member>(num, uid);
		item->setAnchor(0.5f);
		item->setPivot(0.5f);
		items.push_back(item);
	}

	const glm::vec2 ItemSize = { 314.0f, 48.0f };

	auto grid = Shared::SceneHelpers::MakeVerticalGrid(ItemSize, items);
	grid->setY(Member::VerticalMargin / 2.0f);
	grid->setHeight(grid->getHeight() + Member::VerticalMargin);

	auto scrollbox = std::make_shared<Scene::ClippableScissor<Scene::Scrollbox>>();
	scrollbox->setStretch(1.0f);
	scrollbox->getBounding()->setStretch(1.0f);
	scrollbox->getContent()->setSize(grid->getSize());
	scrollbox->getContent()->attach(grid);
	scrollbox->setSensitivity({ 0.0f, 1.0f });
	bottom_section->attach(scrollbox);

	auto scrollbar = std::make_shared<Shared::SceneHelpers::VerticalScrollbar>();
	scrollbar->setX(-4.0f);
	scrollbar->setScrollbox(scrollbox);
	scrollbox->attach(scrollbar);

	auto top_gradient = std::make_shared<Scene::Rectangle>();
	top_gradient->setStretch({ 1.0f, 0.0f });
	top_gradient->setHeight(Member::VerticalMargin);
	top_gradient->setAnchor({ 0.5f, 0.0f });
	top_gradient->setPivot({ 0.5f, 0.0f });
	top_gradient->setVerticalGradient({ Helpers::Pallete::WindowBody, 1.0f }, { Helpers::Pallete::WindowBody, 0.0f });
	scrollbox->attach(top_gradient);

	auto bottom_gradient = std::make_shared<Scene::Rectangle>();
	bottom_gradient->setStretch({ 1.0f, 0.0f });
	bottom_gradient->setHeight(Member::VerticalMargin);
	bottom_gradient->setAnchor({ 0.5f, 1.0f });
	bottom_gradient->setPivot({ 0.5f, 1.0f });
	bottom_gradient->setVerticalGradient({ Helpers::Pallete::WindowBody, 0.0f }, { Helpers::Pallete::WindowBody, 1.0f });
	scrollbox->attach(bottom_gradient);
}

// member

GuildsWindow::MyGuildContent::InfoContent::Member::Member(int num, int uid)
{
	setStretch({ 1.0f, 0.0f });
	setMargin({ 16.0f, VerticalMargin });
	setHeight(48.0f);
	setRounding(8.0f);
	setAbsoluteRounding(true);
	setColor(Helpers::Pallete::WindowItem);

	auto num_label = std::make_shared<Helpers::Label>();
	num_label->setFontSize(18.0f);
	num_label->setPosition({ 24.0f, 0.0f });
	num_label->setPivot(0.5f);
	num_label->setAnchor({ 0.0f, 0.5f });
	num_label->setText(std::to_string(num));
	attach(num_label);

	CLIENT->requireProfile(uid);

	auto name = std::make_shared<Helpers::ProfileListenable<Helpers::Label>>();
	name->setPosition({ 48.0f, 0.0f });
	name->setAnchor({ 0.0f, 0.5f });
	name->setPivot({ 0.0f, 0.5f });
	name->setFontSize(16.0f);
	name->setColor(Helpers::Pallete::YellowLabel);
	name->setProfileUID(uid);
	name->setProfileCallback([name](auto profile) {
		name->setText(profile->getNickName());
	});
	attach(name);
}

// search content

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

