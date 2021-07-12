#include "social_panel.h"
#include "helpers.h"

using namespace hcg001;

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

	auto centerizeTabScrollbox = [this](PageType page) {
		auto node = mTabButtons.at(page);
		auto scroll_taget_pos = mTabButtonScrollbox->screenToScrollPosition(node->project({ 0.0f, 0.0f }));
		mTabButtonScrollbox->runAction(
			Actions::Collection::ChangeScrollPosition(mTabButtonScrollbox, scroll_taget_pos, 0.5f, Easing::ExponentialOut)
		);
	};
    
	auto score_tab_button = createTabButton(LOCALIZE("SCORE"));
	score_tab_button->setClickCallback([this] {
        showPage(PageType::Highscores);
	});

	auto rubies_tab_button = createTabButton(LOCALIZE("TOP GUILDS"));
	rubies_tab_button->setClickCallback([this] {
		showPage(PageType::TopGuilds);
	});

	const glm::vec2 TabItemSize = { 72.0f, footer->getHeight() };
	
	auto tab_button_grid = Shared::SceneHelpers::MakeHorizontalGrid(TabItemSize, {
		score_tab_button,
		rubies_tab_button
	});

	mTabButtons.insert({ PageType::Highscores, score_tab_button });
	mTabButtons.insert({ PageType::TopGuilds, rubies_tab_button });
	
	tab_button_grid->setAnchor(0.5f);
	tab_button_grid->setPivot(0.5f);

	mTabButtonScrollbox = std::make_shared<Scene::Scrollbox>();
	mTabButtonScrollbox->setTouchable(false);
	mTabButtonScrollbox->setStretch(1.0f);
	//mTabButtonScrollbox->getBounding()->setStretch({ 0.0f, 1.0f });
	mTabButtonScrollbox->getBounding()->setSize(TabItemSize);
	mTabButtonScrollbox->getBounding()->setAnchor(0.5f);
	mTabButtonScrollbox->getBounding()->setPivot(0.5f);
	mTabButtonScrollbox->getContent()->setSize(tab_button_grid->getSize());
	mTabButtonScrollbox->getContent()->attach(tab_button_grid);
	footer->attach(mTabButtonScrollbox);


	auto highscores_page = std::make_shared<HighscoresPage>();
	body->attach(highscores_page);

	auto top_guilds_page = std::make_shared<TopGuildsPage>();
	body->attach(top_guilds_page);
    
    mTabContents.insert({ PageType::Highscores, highscores_page });
    mTabContents.insert({ PageType::TopGuilds, top_guilds_page });
    
    showPage(PageType::Highscores);

	//mPagesManager.addPage(PageType::Scores, page1);
	//mPagesManager.addPage(PageType::Rubies, page2);
	//mPagesManager.showPage(PageType::Scores);
	//mPagesManager.setPageChangedCallback([centerizeTabScrollbox](auto from, auto to) {
	//	centerizeTabScrollbox((PageType)to);
	//});*/

	/*auto right_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::Node>>>();
	right_button->setStretch({ 0.0f, 1.0f }); // use body height
	right_button->setWidth(48.0f);
	right_button->setAnchor(0.5f);
	right_button->setPivot({ 1.0f, 0.5f });
	right_button->setX(178.0f);
	right_button->setClickCallback([this] {
		//mPagesManager.showNextPage();
	});
	body->attach(right_button);

	auto right_arrow_img = std::make_shared<Shared::SceneHelpers::Adaptive<Scene::Sprite>>();
	right_arrow_img->setAdaptSize(24.0f);
	right_arrow_img->setAnchor(0.5f);
	right_arrow_img->setPivot(0.5f);
	right_arrow_img->setTexture(TEXTURE("textures/right_arrow.png"));
	right_arrow_img->setAlpha(0.25f);
	right_button->attach(right_arrow_img);

	auto left_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::Node>>>();
	left_button->setStretch({ 0.0f, 1.0f }); // use body height
	left_button->setWidth(48.0f);
	left_button->setAnchor(0.5f);
	left_button->setPivot({ 0.0f, 0.5f });
	left_button->setX(-178.0f);
	left_button->setClickCallback([this] {
		//mPagesManager.showPrevPage();
	});
	body->attach(left_button);

	auto left_arrow_img = std::make_shared<Shared::SceneHelpers::Adaptive<Scene::Sprite>>();
	left_arrow_img->setAdaptSize(24.0f);
	left_arrow_img->setAnchor(0.5f);
	left_arrow_img->setPivot(0.5f);
	left_arrow_img->setTexture(TEXTURE("textures/right_arrow.png"));
	left_arrow_img->setAlpha(0.25f);
	left_arrow_img->setRadialAnchor(0.5f); // flip
	left_button->attach(left_arrow_img);*/


	//auto page = std::make_shared<Page>();
	//body->attach(page);
}

std::shared_ptr<SocialPanel::TabButton> SocialPanel::createTabButton(const utf8_string& text)
{
	auto result = std::make_shared<TabButton>();
	//result->setSize({ 64.0f, 20.0f });
	result->setStretch(1.0f);
	result->setMargin({ 2.0f, 0.0f });
	result->setAnchor({ 0.5f, 0.5f });
	result->setPivot({ 0.5f, 0.5f });
	//result->setRounding(0.5f);
	result->setAlpha(0.125f);
	//result->setAlpha(0.0f);

	auto label = std::make_shared<Helpers::Label>();
	label->setAnchor(0.5f);
	label->setPivot(0.5f);
	label->setFontSize(13.0f);
	label->setText(text);
	result->attach(label);

	return result;
}

void SocialPanel::showPage(PageType type)
{
    for (auto [_type, page] : mTabContents)
    {
        page->setEnabled(false);
    }
    
    mTabContents.at(type)->setEnabled(true);
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

	runAction(Actions::Collection::RepeatInfinite([this] {
		return Actions::Collection::MakeSequence(
			Actions::Collection::Wait(1.25f),
			Actions::Collection::ChangeVerticalScrollPosition(mScrollbox, 1.0f, 3.0f, Easing::CubicInOut),
			Actions::Collection::Wait(0.75f),
			Actions::Collection::ChangeVerticalScrollPosition(mScrollbox, 0.0f, 3.0f, Easing::CubicInOut)
		);
	}));

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
}

void SocialPanel::TopGuildsPage::onEvent(const NetEvents::GuildsTopEvent& e)
{
    //
}
