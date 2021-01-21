#include "social_panel.h"
#include "helpers.h"

using namespace hcg001;

SocialPanel::SocialPanel()
{
	setSize({ 256.0f, 96.0f });
	
	mRect = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	mRect->setStretch(1.0f);
	mRect->setAlpha(0.125f);
	mRect->setAbsoluteRounding(true);
	mRect->setRounding(8.0f);
	attach(mRect);

	mScrollbox = std::make_shared<Scene::Scrollbox>();
	mScrollbox->setStretch(1.0f);
	mScrollbox->setSensitivity({ 0.0f, 1.0f });
	mScrollbox->getBounding()->setStretch(1.0f);
	mScrollbox->getBounding()->setAnchor(0.5f);
	mScrollbox->getBounding()->setPivot(0.5f);
	mRect->attach(mScrollbox);

	auto table_number_header = std::make_shared<Helpers::Label>();
	table_number_header->setText(LOCALIZE("#"));
	table_number_header->setFontSize(12.0f);
	table_number_header->setAnchor({ 0.0f, 0.0f });
	table_number_header->setPivot({ 0.5f, 1.0f });
	table_number_header->setPosition({ 16.0f, -4.0f });
	table_number_header->setAlpha(0.5f);
	attach(table_number_header);

	auto table_name_header = std::make_shared<Helpers::Label>();
	table_name_header->setText(LOCALIZE("NAME"));
	table_name_header->setFontSize(12.0f);
	table_name_header->setAnchor({ 0.0f, 0.0f });
	table_name_header->setPivot({ 0.5f, 1.0f });
	table_name_header->setPosition({ 32.0f + (96.0f / 2.0f), -4.0f });
	table_name_header->setAlpha(0.5f);
	attach(table_name_header);

	auto table_score_header = std::make_shared<Helpers::Label>();
	table_score_header->setText(LOCALIZE("SCORE"));
	table_score_header->setFontSize(12.0f);
	table_score_header->setAnchor({ 0.0f, 0.0f });
	table_score_header->setPivot({ 0.5f, 1.0f });
	table_score_header->setPosition({ 32.0f + 96.0f + (64.0f / 2.0f), -4.0f });
	table_score_header->setAlpha(0.5f);
	attach(table_score_header);

	auto table_skin_header = std::make_shared<Helpers::Label>();
	table_skin_header->setText(LOCALIZE("SKIN"));
	table_skin_header->setFontSize(12.0f);
	table_skin_header->setAnchor({ 0.0f, 0.0f });
	table_skin_header->setPivot({ 0.5f, 1.0f });
	table_skin_header->setPosition({ 32.0f + 96.0f + 64.0f + (64.0f / 2.0f), -4.0f });
	table_skin_header->setAlpha(0.5f);
	attach(table_skin_header);

	auto refresh_button = std::make_shared<Scene::Clickable<Helpers::Label>>();
	refresh_button->setClickCallback([this] {
		CLIENT->clearProfiles();
		refresh();
	});
	refresh_button->setText("refresh"); // TODO: localize
	refresh_button->setFontSize(12.0f);
	refresh_button->setAnchor({ 1.0f, 1.0f });
	refresh_button->setPivot({ 1.0f, 0.0f });
	refresh_button->setPosition({ 0.0f, 4.0f });
	refresh_button->setAlpha(0.25f);
	attach(refresh_button);

	/*Actions::Run(Actions::Factory::ExecuteInfinite([this] {
		auto overscroll = mScrollbox->mOverscroll;
		GAME_STATS("overscroll", std::to_string(overscroll.x) + " " + std::to_string(overscroll.y));
		auto scroll_pos = mScrollbox->getScrollPosition();
		GAME_STATS("scroll_pos", std::to_string(scroll_pos.x) + " " + std::to_string(scroll_pos.y));
	}));*/

	runAction(Actions::Factory::RepeatInfinite([this] {
		return Actions::Factory::MakeSequence(
			Actions::Factory::Wait(1.0f),
			Actions::Factory::ChangeVerticalScrollPosition(mScrollbox, 1.0f, 3.0f, Easing::CubicInOut),
			Actions::Factory::Wait(1.0f),
			Actions::Factory::ChangeVerticalScrollPosition(mScrollbox, 0.0f, 3.0f, Easing::CubicInOut)
		);
	}));
}

void SocialPanel::refresh()
{
	if (mGrid)
	{
		mScrollbox->getContent()->detach(mGrid);
	}

	std::vector<std::shared_ptr<Scene::Node>> v_items;

	const glm::vec2 VerticalGridItemSize = { 256.0f, 16.0f };

	bool grayed_line = true;

	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < mHighscores.uids.size(); i++)
		{
			auto uid = mHighscores.uids.at(i);

			CLIENT->requireProfile(uid);

			auto wait_profile_callback = [uid] {
				return CLIENT->getProfiles().count(uid) == 0;
			};

			const float LabelFontSize = 13.0f;

			auto count_label = std::make_shared<Helpers::Label>();
			count_label->setFontSize(LabelFontSize);
			count_label->setText(std::to_string(i + 1) + ".");
			count_label->setAnchor(0.5f);
			count_label->setPivot(0.5f);

			auto nickname_scissor = std::make_shared<Scene::ClippableScissor<Scene::Node>>();
			nickname_scissor->setStretch(1.0f);

			auto nickname_label = std::make_shared<Scene::Actionable<Helpers::Label>>();
			nickname_label->setAnchor({ 0.0f, 0.5f });
			nickname_label->setPivot({ 0.0f, 0.5f });
			nickname_label->setFontSize(LabelFontSize);
			nickname_label->runAction(Actions::Factory::Delayed(wait_profile_callback, Actions::Factory::Execute([nickname_label, uid] {
				nickname_label->setText(CLIENT->getProfiles().at(uid)->getNickName());
			})));
			nickname_scissor->attach(nickname_label);

			auto score_label = std::make_shared<Scene::Actionable<Helpers::Label>>();
			score_label->setAnchor(0.5f);
			score_label->setPivot(0.5f);
			score_label->setFontSize(LabelFontSize);
			score_label->runAction(Actions::Factory::Delayed(wait_profile_callback, Actions::Factory::Execute([score_label, uid] {
				score_label->setText(std::to_string(CLIENT->getProfiles().at(uid)->getHighScore()));
			})));

			auto skin_img = std::make_shared<Scene::Actionable<Shared::SceneHelpers::Adaptive<Scene::Sprite>>>();
			skin_img->setAdaptSize({ 8.0f, 8.0f });
			skin_img->setAnchor(0.5f);
			skin_img->setPivot(0.5f);
			skin_img->runAction(Actions::Factory::Delayed(wait_profile_callback, Actions::Factory::Execute([skin_img, uid] {
				skin_img->setTexture(TEXTURE(SkinPath.at(CLIENT->getProfiles().at(uid)->getCurrentSkin())));
			})));

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

			v_items.push_back(h_grid);
		}
	}

	mGrid = Shared::SceneHelpers::MakeVerticalGrid(VerticalGridItemSize, v_items);
	//mGrid->setAnchor({ 0.5f, 0.0f });
	//mGrid->setPivot({ 0.5f, 0.0f });
	//mGrid->setY(8.0f);

	mScrollbox->getContent()->setSize(mGrid->getSize());
	mScrollbox->getContent()->attach(mGrid);
}

void SocialPanel::onEvent(const Helpers::HighscoresEvent& e)
{
	mHighscores = e;
	refresh();
}
