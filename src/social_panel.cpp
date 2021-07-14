#include "social_panel.h"
#include "helpers.h"

using namespace hcg001;

// social panel

SocialPanel::SocialPanel()
{
	setStretch({ 1.0f, 0.0f });
	setSize({ 0.0f, 136.0f });
    setScale(1.125f);

	auto header = std::make_shared<Scene::Node>();
	header->setStretch({ 1.0f, 0.0f });
	header->setAnchor({ 0.5f, 0.0f });
	header->setPivot({ 0.5f, 0.0f });
	header->setSize({ 0.0f, 16.0f });
	attach(header);

	auto footer = std::make_shared<Scene::Node>();
	footer->setStretch({ 1.0f, 0.0f });
	footer->setAnchor({ 0.5f, 1.0f });
	footer->setPivot({ 0.5f, 1.0f });
	footer->setSize({ 0.0f, 24.0f });
	attach(footer);

	auto body = std::make_shared<Scene::Node>();
	body->setStretch(1.0f);
	body->setAnchor({ 0.5f, 0.0f });
	body->setPivot({ 0.5f, 0.0f });
	body->setPosition({ 0.0f, header->getHeight() });
	body->setMargin({ 0.0f, header->getHeight() + footer->getHeight() });
	attach(body);

	auto highscores_button = std::make_shared<TabButton>();
	highscores_button->setClickCallback([this] {
		mTabsManager.show(PageType::Highscores);
	});

	auto top_guilds_button = std::make_shared<TabButton>();
	top_guilds_button->setClickCallback([this] {
		mTabsManager.show(PageType::TopGuilds);
	});

	const glm::vec2 TabItemSize = { 24.0f, footer->getHeight() };
	
	auto tab_button_grid = Shared::SceneHelpers::MakeHorizontalGrid(TabItemSize, {
		highscores_button,
		top_guilds_button
	});
	tab_button_grid->setAnchor(0.5f);
	tab_button_grid->setPivot(0.5f);
	footer->attach(tab_button_grid);

	mTabsManager.addButton(PageType::Highscores, highscores_button);
	mTabsManager.addButton(PageType::TopGuilds, top_guilds_button);
	
	auto highscores_page = std::make_shared<HighscoresPage>();
	body->attach(highscores_page);

	auto top_guilds_page = std::make_shared<TopGuildsPage>();
	body->attach(top_guilds_page);

	mTabsManager.addContent(PageType::Highscores, highscores_page);
	mTabsManager.addContent(PageType::TopGuilds, top_guilds_page);
	
	runAction(Actions::Collection::RepeatInfinite([this] {
		auto seq = Actions::Collection::MakeSequence();

		for (auto [type, _content] : mTabsManager.getContents())
		{
			auto content = std::static_pointer_cast<TabContent>(_content);
			seq->add(Actions::Collection::Execute([this, type] { 
				mTabsManager.show((PageType)type);
			}));
			seq->add(Actions::Collection::Wait(1.0f));
			seq->add(content->getScenario());
			seq->add(Actions::Collection::Wait(1.0f));
		}

		return seq;
	}));
	
	/*mTabsManager.show((int)PageType::Highscores);

	for (auto [type, _content] : mTabsManager.getContents())
	{
		auto content = std::static_pointer_cast<TabContent>(_content);
		runAction(Actions::Collection::RepeatInfinite([content] {
			return content->getScenario();
		}));
	}*/
}

// tab button

SocialPanel::TabButton::TabButton()
{
	setSize(12.0f);
	setPivot(0.5f);
	setAnchor(0.5f);
	setAlpha(0.25f);
	setRounding(0.5f);
	setClickEnabled(false);

	mCheckbox = std::make_shared<Scene::Rectangle>();
	mCheckbox->setAnchor(0.5f);
	mCheckbox->setPivot(0.5f);
	mCheckbox->setStretch(0.75f);
	mCheckbox->setAlpha(0.75f);
	mCheckbox->setRounding(0.5f);
	attach(mCheckbox);
}

void SocialPanel::TabButton::onJoin()
{
	mCheckbox->setVisible(false);
	mCheckbox->setScale(0.0f);
}

void SocialPanel::TabButton::onEnter()
{
	//setClickEnabled(false);
	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::Execute([this] {
			mCheckbox->setVisible(true);
		}),
		Actions::Collection::ChangeScale(mCheckbox, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 0.5f, Easing::ExponentialOut),
		Actions::Collection::Execute([this] {
		//	setClickEnabled(true);
		})
	));
}

void SocialPanel::TabButton::onLeave()
{
	//setClickEnabled(false);
	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::ChangeScale(mCheckbox, { 1.0f, 1.0f }, { 0.0f, 0.0f }, 0.5f, Easing::ExponentialOut),
		Actions::Collection::Execute([this] {
			mCheckbox->setVisible(false);
		//	setClickEnabled(true);
		})
	));
}

// tab content

void SocialPanel::TabContent::onJoin()
{
	setHorizontalAnchor(1.5f);
}

void SocialPanel::TabContent::onEnter()
{
	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::Execute([this] {
			setVisible(true);
		}),
		Actions::Collection::ChangeHorizontalAnchor(shared_from_this(), 1.5f, 0.5f, 0.5f, Easing::ExponentialOut)
	));
}

void SocialPanel::TabContent::onLeave()
{
	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::ChangeHorizontalAnchor(shared_from_this(), 0.5f, -0.5f, 0.5f, Easing::ExponentialOut),
		Actions::Collection::Execute([this] {
			setVisible(false);
		})
	));
}

// highscores page

SocialPanel::HighscoresPage::HighscoresPage()
{
	setStretch({ 0.0f, 1.0f });
	setSize({ 256.0f, 0.0f });
	setPivot(0.5f);
	setAnchor(0.5f);
	
	mBackground = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	mBackground->setStretch(1.0f);
	mBackground->setAlpha(0.125f);
	mBackground->setAbsoluteRounding(true);
	mBackground->setRounding(8.0f);
	attach(mBackground);

	mScrollbox = std::make_shared<Scene::Scrollbox>();
	mScrollbox->setStretch(1.0f);
	mScrollbox->setTouchable(false);
	//mScrollbox->setSensitivity({ 0.0f, 1.0f });
	mScrollbox->getBounding()->setStretch(1.0f);
	mScrollbox->getBounding()->setAnchor(0.5f);
	mScrollbox->getBounding()->setPivot(0.5f);
	mBackground->attach(mScrollbox);

	auto table_headers_holder = std::make_shared<Scene::Node>();
	table_headers_holder->setStretch({ 1.0f, 0.0f });
	table_headers_holder->setSize({ 0.0f, 16.0f });
	table_headers_holder->setAnchor({ 0.5f, 0.0f });
	table_headers_holder->setPivot({ 0.5f, 1.0f });
	attach(table_headers_holder);

	auto table_number_header = std::make_shared<Helpers::Label>();
	table_number_header->setText(LOCALIZE("SOCIAL_PANEL_NUM"));
	table_number_header->setFontSize(12.0f);
	table_number_header->setAnchor({ 0.0f, 0.5f });
	table_number_header->setPivot({ 0.5f, 0.5f });
	table_number_header->setPosition({ 16.0f, 0.0f });
	table_number_header->setAlpha(0.5f);
	table_headers_holder->attach(table_number_header);

	auto table_name_header = std::make_shared<Helpers::Label>();
	table_name_header->setText(LOCALIZE("SOCIAL_PANEL_NAME"));
	table_name_header->setFontSize(12.0f);
	table_name_header->setAnchor({ 0.0f, 0.5f });
	table_name_header->setPivot({ 0.5f, 0.5f });
	table_name_header->setPosition({ 32.0f + (96.0f / 2.0f), 0.0f });
	table_name_header->setAlpha(0.5f);
	table_headers_holder->attach(table_name_header);

	auto table_score_header = std::make_shared<Helpers::Label>();
	table_score_header->setText(LOCALIZE("SOCIAL_PANEL_SCORE"));
	table_score_header->setFontSize(12.0f);
	table_score_header->setAnchor({ 0.0f, 0.5f });
	table_score_header->setPivot({ 0.5f, 0.5f });
	table_score_header->setPosition({ 32.0f + 96.0f + (64.0f / 2.0f), 0.0f });
	table_score_header->setAlpha(0.5f);
	table_headers_holder->attach(table_score_header);

	auto table_skin_header = std::make_shared<Helpers::Label>();
	table_skin_header->setText(LOCALIZE("SOCIAL_PANEL_SKIN"));
	table_skin_header->setFontSize(12.0f);
	table_skin_header->setAnchor({ 0.0f, 0.5f });
	table_skin_header->setPivot({ 0.5f, 0.5f });
	table_skin_header->setPosition({ 32.0f + 96.0f + 64.0f + (64.0f / 2.0f), 0.0f });
	table_skin_header->setAlpha(0.5f);
	table_headers_holder->attach(table_skin_header);
}

Actions::Collection::UAction SocialPanel::HighscoresPage::getScenario()
{
	return Actions::Collection::MakeSequence(
		Actions::Collection::Wait(1.25f),
		Actions::Collection::ChangeVerticalScrollPosition(mScrollbox, 1.0f, 5.0f, Easing::CubicInOut),
		Actions::Collection::Wait(0.75f),
		Actions::Collection::ChangeVerticalScrollPosition(mScrollbox, 0.0f, 5.0f, Easing::CubicInOut)
	);
}

void SocialPanel::HighscoresPage::onEvent(const NetEvents::HighscoresEvent& e)
{
	mHighscores = e;

	
	if (!mHighscores.uids.empty())
	{
		while (mHighscores.uids.size() < 10)
			mHighscores.uids.push_back(mHighscores.uids.at(mHighscores.uids.size() - 1));
	}

	refresh();
}

void SocialPanel::HighscoresPage::refresh()
{
	if (mGrid)
	{
		mScrollbox->getContent()->detach(mGrid);
	}

	std::vector<std::shared_ptr<Scene::Node>> v_items;

	const glm::vec2 VerticalGridItemSize = { 256.0f, 16.0f };

	bool grayed_line = true;

	for (int i = 0; i < mHighscores.uids.size(); i++)
	{
		auto uid = mHighscores.uids.at(i);

		CLIENT->requireProfile(uid);

		const float LabelFontSize = 13.0f;

		auto count_label = std::make_shared<Helpers::Label>();
		count_label->setFontSize(LabelFontSize);
		count_label->setText(std::to_string(i + 1) + ".");
		count_label->setAnchor(0.5f);
		count_label->setPivot(0.5f);

		auto nickname_scissor = std::make_shared<Scene::ClippableScissor<Scene::Node>>();
		nickname_scissor->setStretch(1.0f);

		auto nickname_label = std::make_shared<Helpers::ProfileListenable<Helpers::Label>>();
		nickname_label->setAnchor({ 0.0f, 0.5f });
		nickname_label->setPivot({ 0.0f, 0.5f });
		nickname_label->setFontSize(LabelFontSize);
		nickname_label->setProfileUID(uid);
		nickname_label->setProfileCallback([nickname_label](auto profile) {
			nickname_label->setText(profile->getNickName());
		});
		nickname_scissor->attach(nickname_label);

		auto score_label = std::make_shared<Helpers::ProfileListenable<Helpers::Label>>();
		score_label->setAnchor(0.5f);
		score_label->setPivot(0.5f);
		score_label->setFontSize(LabelFontSize);
		score_label->setProfileUID(uid);
		score_label->setProfileCallback([score_label](auto profile) {
			score_label->setText(std::to_string(profile->getHighScore()));
		});

		auto skin_img = std::make_shared<Helpers::ProfileListenable<Scene::Adaptive<Scene::Sprite>>>();
		skin_img->setAdaptSize({ 8.0f, 8.0f });
		skin_img->setAnchor(0.5f);
		skin_img->setPivot(0.5f);
		skin_img->setProfileUID(uid);
		skin_img->setProfileCallback([skin_img](auto profile) {
			skin_img->setTexture(TEXTURE(SkinPath.at(profile->getCurrentSkin())));
		});

		auto h_grid = Shared::SceneHelpers::MakeHorizontalGrid(VerticalGridItemSize.y, {
			{ 32.0f, count_label },
			{ 96.0f, nickname_scissor },
			{ 64.0f, score_label },
			{ 64.0f, skin_img },
		});

		if (grayed_line)
		{
			auto gray_rect = std::make_shared<Scene::Rectangle>();
			gray_rect->setColor(Graphics::Color::Black);
			gray_rect->setAlpha(0.125f);
			gray_rect->setSize(VerticalGridItemSize);
			gray_rect->attach(h_grid);
			h_grid = gray_rect;
		}

		grayed_line = !grayed_line;

		auto cullable_h_grid = std::make_shared<Scene::Cullable<Scene::Node>>();
		cullable_h_grid->setSize(VerticalGridItemSize);
		cullable_h_grid->attach(h_grid);

		v_items.push_back(cullable_h_grid);
	}

	mGrid = Shared::SceneHelpers::MakeVerticalGrid(VerticalGridItemSize, v_items);
	mScrollbox->getContent()->setSize(mGrid->getSize());
	mScrollbox->getContent()->attach(mGrid);
}

// top guilds page

SocialPanel::TopGuildsPage::TopGuildsPage()
{
    setStretch({ 0.0f, 1.0f });
    setSize({ 256.0f, 0.0f });
    setPivot(0.5f);
    setAnchor(0.5f);
    
    mBackground = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
    mBackground->setStretch(1.0f);
    mBackground->setAlpha(0.125f);
    mBackground->setAbsoluteRounding(true);
    mBackground->setRounding(8.0f);
    attach(mBackground);

	mScrollbox = std::make_shared<Scene::Scrollbox>();
	mScrollbox->setStretch(1.0f);
	mScrollbox->setTouchable(false);
	//mScrollbox->setSensitivity({ 0.0f, 1.0f });
	mScrollbox->getBounding()->setStretch(1.0f);
	mScrollbox->getBounding()->setAnchor(0.5f);
	mScrollbox->getBounding()->setPivot(0.5f);
	mBackground->attach(mScrollbox);

	auto table_headers_holder = std::make_shared<Scene::Node>();
	table_headers_holder->setStretch({ 1.0f, 0.0f });
	table_headers_holder->setSize({ 0.0f, 16.0f });
	table_headers_holder->setAnchor({ 0.5f, 0.0f });
	table_headers_holder->setPivot({ 0.5f, 1.0f });
	attach(table_headers_holder);

	auto table_number_header = std::make_shared<Helpers::Label>();
	table_number_header->setText(LOCALIZE("SOCIAL_PANEL_NUM"));
	table_number_header->setFontSize(12.0f);
	table_number_header->setAnchor({ 0.0f, 0.5f });
	table_number_header->setPivot({ 0.5f, 0.5f });
	table_number_header->setPosition({ 16.0f, 0.0f });
	table_number_header->setAlpha(0.5f);
	table_headers_holder->attach(table_number_header);

	auto table_name_header = std::make_shared<Helpers::Label>();
	table_name_header->setText(LOCALIZE("SOCIAL_PANEL_GUILD_NAME"));
	table_name_header->setFontSize(12.0f);
	table_name_header->setAnchor({ 0.0f, 0.5f });
	table_name_header->setPivot({ 0.5f, 0.5f });
	table_name_header->setPosition({ 32.0f + (96.0f / 2.0f), 0.0f });
	table_name_header->setAlpha(0.5f);
	table_headers_holder->attach(table_name_header);

	auto table_score_header = std::make_shared<Helpers::Label>();
	table_score_header->setText(LOCALIZE("SOCIAL_PANEL_SCORE"));
	table_score_header->setFontSize(12.0f);
	table_score_header->setAnchor({ 0.0f, 0.5f });
	table_score_header->setPivot({ 0.5f, 0.5f });
	table_score_header->setPosition({ 32.0f + 96.0f + (128.0f / 2.0f), 0.0f });
	table_score_header->setAlpha(0.5f);
	table_headers_holder->attach(table_score_header);
}

Actions::Collection::UAction SocialPanel::TopGuildsPage::getScenario()
{
	return Actions::Collection::MakeSequence(
		Actions::Collection::Wait(1.25f),
		Actions::Collection::ChangeVerticalScrollPosition(mScrollbox, 1.0f, 5.0f, Easing::CubicInOut),
		Actions::Collection::Wait(0.75f),
		Actions::Collection::ChangeVerticalScrollPosition(mScrollbox, 0.0f, 5.0f, Easing::CubicInOut)
	);
}

void SocialPanel::TopGuildsPage::onEvent(const NetEvents::GuildsTopEvent& e)
{
	mTopGuilds = e;

	if (!mTopGuilds.ids.empty())
	{
		while (mTopGuilds.ids.size() < 10)
			mTopGuilds.ids.push_back(mTopGuilds.ids.at(mTopGuilds.ids.size() - 1));
	}

	refresh();
}

void SocialPanel::TopGuildsPage::refresh()
{
	if (mGrid)
	{
		mScrollbox->getContent()->detach(mGrid);
	}

	std::vector<std::shared_ptr<Scene::Node>> v_items;

	const glm::vec2 VerticalGridItemSize = { 256.0f, 16.0f };

	bool grayed_line = true;

	for (int i = 0; i < mTopGuilds.ids.size(); i++)
	{
		auto id = mTopGuilds.ids.at(i);

		CLIENT->requestGuildInfo(id);

		const float LabelFontSize = 13.0f;

		auto count_label = std::make_shared<Helpers::Label>();
		count_label->setFontSize(LabelFontSize);
		count_label->setText(std::to_string(i + 1) + ".");
		count_label->setAnchor(0.5f);
		count_label->setPivot(0.5f);

		auto nickname_scissor = std::make_shared<Scene::ClippableScissor<Scene::Node>>();
		nickname_scissor->setStretch(1.0f);

		auto nickname_label = std::make_shared<Helpers::GuildInfoListenable<Helpers::Label>>();
		nickname_label->setAnchor({ 0.0f, 0.5f });
		nickname_label->setPivot({ 0.0f, 0.5f });
		nickname_label->setFontSize(LabelFontSize);
		nickname_label->setGuildID(id);
		nickname_label->setGuildCallback([nickname_label](auto guild) {
			std::string title = guild->getJson().value("title", "");
			nickname_label->setText(title);
		});
		nickname_scissor->attach(nickname_label);

		auto score_label = std::make_shared<Helpers::GuildInfoListenable<Helpers::Label>>();
		score_label->setAnchor(0.5f);
		score_label->setPivot(0.5f);
		score_label->setFontSize(LabelFontSize);
		score_label->setGuildID(id);
		score_label->setGuildCallback([score_label](auto guild) {
			int score = guild->getJson().value("score", 0);
			score_label->setText(std::to_string(score));
		});

		auto h_grid = Shared::SceneHelpers::MakeHorizontalGrid(VerticalGridItemSize.y, {
			{ 32.0f, count_label },
			{ 96.0f, nickname_scissor },
			{ 128.0f, score_label }
		});

		if (grayed_line)
		{
			auto gray_rect = std::make_shared<Scene::Rectangle>();
			gray_rect->setColor(Graphics::Color::Black);
			gray_rect->setAlpha(0.125f);
			gray_rect->setSize(VerticalGridItemSize);
			gray_rect->attach(h_grid);
			h_grid = gray_rect;
		}

		grayed_line = !grayed_line;

		auto cullable_h_grid = std::make_shared<Scene::Cullable<Scene::Node>>();
		cullable_h_grid->setSize(VerticalGridItemSize);
		cullable_h_grid->attach(h_grid);

		v_items.push_back(cullable_h_grid);
	}

	mGrid = Shared::SceneHelpers::MakeVerticalGrid(VerticalGridItemSize, v_items);
	mScrollbox->getContent()->setSize(mGrid->getSize());
	mScrollbox->getContent()->attach(mGrid);
}