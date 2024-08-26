#include "buy_skin_menu.h"
#include "profile.h"
#include "helpers.h"
#include "achievements.h"

using namespace hcg001;

BuySkinMenu::BuySkinMenu(Skin skin)
{
	mTitle = std::make_shared<Scene::Label>();
	mTitle->setFont(FONT("default"));
	mTitle->setFontSize(34.0f);
	mTitle->setAnchor({ -0.5f, 0.25f });
	mTitle->setPivot({ 0.5f, 0.5f });
	mTitle->setText(LOCALIZE("BUY_SKIN_TITLE"));
	getContent()->attach(mTitle);

	mImage = std::make_shared<Scene::Sprite>();
	mImage->setTexture(TEXTURE(SkinPath.at(skin)));
	mImage->setAnchor({ 1.5f, 0.5f });
	mImage->setPivot({ 0.5f, 0.5f });
	mImage->setSize({ 96.0f, 96.0f });
	getContent()->attach(mImage);

	mBuyButton = std::make_shared<Helpers::Button>();
	mBuyButton->setActiveColor({ 1.0f, 1.0f, 1.0f, 0.33f });
	mBuyButton->getLabel()->setFontSize(20.0f);
	mBuyButton->getLabel()->setText(LOCALIZE("BUY_SKIN_BUY"));
	mBuyButton->setClickCallback([this, skin] {
		PROFILE->decreaseRubies(SkinCost.at(skin));
		auto skins = PROFILE->getSkins();
		skins.insert((int)skin);
		PROFILE->setSkins(skins);
		PROFILE->save();
		ACHIEVEMENTS->hit("SKIN_UNLOCKED");
		SCENE_MANAGER->switchScreenBack();
	});
	mBuyButton->setSize({ 128.0f, 48.0f });
	mBuyButton->setAnchor({ 0.5f, 1.25f });
	mBuyButton->setPivot({ 1.0f, 0.5f });
	mBuyButton->setPosition({ -8.0f, 0.0f });
	mBuyButton->refresh();
	getContent()->attach(mBuyButton);

	mCancelButton = std::make_shared<Helpers::Button>();
	mCancelButton->setActiveColor({ 1.0f, 1.0f, 1.0f, 0.33f });
	mCancelButton->getLabel()->setFontSize(20.0f);
	mCancelButton->getLabel()->setText(LOCALIZE("BUY_SKIN_CANCEL"));
	mCancelButton->setClickCallback([this] {
		SCENE_MANAGER->switchScreenBack();
	});
	mCancelButton->setSize({ 128.0f, 48.0f });
	mCancelButton->setAnchor({ 0.5f, 1.25f });
	mCancelButton->setPivot({ 0.0f, 0.5f });
	mCancelButton->setPosition({ 8.0f, 0.0f });
	mCancelButton->refresh();
	getContent()->attach(mCancelButton);

	auto rubies = std::make_shared<Helpers::RubiesIndicator>();
	getGui()->attach(rubies);

	mTitle->setHorizontalAnchor(0.5f);
	mImage->setHorizontalAnchor(0.5f);
	mBuyButton->setVerticalAnchor(0.75f);
	mCancelButton->setVerticalAnchor(0.75f);
}