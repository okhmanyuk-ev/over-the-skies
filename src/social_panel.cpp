#include "social_panel.h"
#include "helpers.h"

using namespace hcg001;

SocialPanel::SocialPanel()
{
	setSize({ 256.0f, 96.0f });
	setAlpha(0.125f);
	setAbsoluteRounding(true);
	setRounding(8.0f);
}

void SocialPanel::onEvent(const Helpers::HighscoresEvent& e)
{
	if (mGrid)
	{
		detach(mGrid);
	}

	std::vector<std::shared_ptr<Scene::Node>> v_items;

	const float VerticalGridItemHeight = 16.0f;

	for (int i = 0; i < e.uids.size(); i++)
	{
		auto uid = e.uids.at(i);

		if (Client::Profiles.count(uid) == 0)
			CLIENT->requestProfile(uid);

		auto wait_profile_callback = [uid] {
			return Client::Profiles.count(uid) == 0;
		};

		const float LabelFontSize = 14.0f;

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
			auto nickname_bytes = Client::Profiles.at(uid).at("nickname").get<std::vector<int>>();
			auto nickname = utf8_string(nickname_bytes.begin(), nickname_bytes.end());
			nickname_label->setText(nickname);
		})));
		nickname_scissor->attach(nickname_label);

		auto score_label = std::make_shared<Scene::Actionable<Helpers::Label>>();
		score_label->setAnchor(0.5f);
		score_label->setPivot(0.5f);
		score_label->setFontSize(LabelFontSize);
		score_label->runAction(Actions::Factory::Delayed(wait_profile_callback, Actions::Factory::Execute([score_label, uid] {
			auto score = Client::Profiles.at(uid).at("highscore").get<int>();
			score_label->setText(std::to_string(score));
		})));

		auto skin_img = std::make_shared<Scene::Actionable<Shared::SceneHelpers::Adaptive<Scene::Sprite>>>();
		skin_img->setAdaptSize({ 8.0f, 8.0f });
		skin_img->setAnchor(0.5f);
		skin_img->setPivot(0.5f);
		skin_img->runAction(Actions::Factory::Delayed(wait_profile_callback, Actions::Factory::Execute([skin_img, uid] {
			//auto score = Client::Profiles.at(uid).at("highscore").get<int>();
			//score_label->setText(std::to_string(score));
			skin_img->setTexture(TEXTURE(SkinPath.at(Skin::Football)));
		})));

		auto h_grid = Shared::SceneHelpers::MakeHorizontalGrid(VerticalGridItemHeight, {
			{ 32.0f, count_label },
			{ 96.0f, nickname_scissor },
			{ 64.0f, score_label },
			{ 16.0f, skin_img },
		});

		v_items.push_back(h_grid);
	}

	mGrid = Shared::SceneHelpers::MakeVerticalGrid({ 224.0f, VerticalGridItemHeight }, v_items);
	mGrid->setAnchor({ 0.5f, 0.0f });
	mGrid->setPivot({ 0.5f, 0.0f });
	mGrid->setY(8.0f);

	attach(mGrid);
}
