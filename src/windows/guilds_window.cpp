#include "guilds_window.h"
#include "create_guild_window.h"

using namespace hcg001;

GuildsWindow::GuildsWindow()
{
	getBackground()->setSize({ 314.0f, 386.0f });
	getTitle()->setText(LOCALIZE("GUILDS_WINDOW_TITLE"));

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

	getBody()->attach(grid);
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
	join_button->setColor(Helpers::ButtonColor);
	join_button->getLabel()->setText(LOCALIZE("JOIN"));
	join_button->getLabel()->setFontSize(15.0f);
	join_button->setClickCallback([] {
		LOG("join button");
	});
	join_button->setAnchor({ 1.0f, 0.5f });
	join_button->setPivot({ 1.0f, 0.5f });
	join_button->setSize({ 64.0f, 28.0f });
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

