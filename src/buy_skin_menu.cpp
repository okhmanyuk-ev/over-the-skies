#include "buy_skin_menu.h"
#include "profile.h"
#include "helpers.h"

using namespace hcg001;

BuySkinMenu::BuySkinMenu(Skin skin)
{
	setStretch(1.0f);

	mTitle = std::make_shared<Scene::Label>();
	mTitle->setFont(FONT("default"));
	mTitle->setFontSize(34.0f);
	mTitle->setAnchor({ -0.5f, 0.25f });
	mTitle->setPivot({ 0.5f, 0.5f });
	mTitle->setText(LOCALIZE("BUY_SKIN_TITLE"));
	attach(mTitle);

	mImage = std::make_shared<Scene::Sprite>();
	mImage->setTexture(TEXTURE(SkinPath.at(skin)));
	mImage->setAnchor({ 1.5f, 0.5f });
	mImage->setPivot({ 0.5f, 0.5f });
	mImage->setSize({ 96.0f, 96.0f });
	attach(mImage);

	mBuyButton = std::make_shared<Helpers::Button>();
	mBuyButton->setActiveColor({ 1.0f, 1.0f, 1.0f, 0.33f });
	mBuyButton->setAdaptiveFontSize(false);
	mBuyButton->getLabel()->setFontSize(20.0f);
	mBuyButton->getLabel()->setText(LOCALIZE("BUY_SKIN_BUY"));
	mBuyButton->setClickCallback([this, skin] {
		PROFILE->decreaseRubies(SkinCost.at(skin));
		auto skins = PROFILE->getSkins();
		skins.insert((int)skin);
		PROFILE->setSkins(skins);
		PROFILE->save();
		mExitCallback();
	});
	mBuyButton->setSize({ 128.0f, 48.0f });
	mBuyButton->setAnchor({ 0.5f, 1.25f });
	mBuyButton->setPivot({ 1.0f, 0.5f });
	mBuyButton->setPosition({ -8.0f, 0.0f });
	mBuyButton->refresh();
	attach(mBuyButton);

	mCancelButton = std::make_shared<Helpers::Button>();
	mCancelButton->setActiveColor({ 1.0f, 1.0f, 1.0f, 0.33f });
	mCancelButton->setAdaptiveFontSize(false);
	mCancelButton->getLabel()->setFontSize(20.0f);
	mCancelButton->getLabel()->setText(LOCALIZE("BUY_SKIN_CANCEL"));
	mCancelButton->setClickCallback([this] {
		mExitCallback();
	});
	mCancelButton->setSize({ 128.0f, 48.0f });
	mCancelButton->setAnchor({ 0.5f, 1.25f });
	mCancelButton->setPivot({ 0.0f, 0.5f });
	mCancelButton->setPosition({ 8.0f, 0.0f });
	mCancelButton->refresh();
	attach(mCancelButton);
}

void BuySkinMenu::onLeaveBegin()
{
	setInteractions(false);
}

std::unique_ptr<Actions::Action> BuySkinMenu::createEnterAction()
{
	const float Duration = 0.25f;

	return Actions::Collection::MakeParallel(
		Actions::Collection::ChangeHorizontalAnchor(mTitle, 0.5f, Duration, Easing::CubicOut),
		Actions::Collection::Delayed(Duration / 2.0f,
			Actions::Collection::ChangeHorizontalAnchor(mImage, 0.5f, Duration, Easing::CubicOut)
		),
		Actions::Collection::Delayed(Duration,
			Actions::Collection::MakeParallel(
				Actions::Collection::ChangeVerticalAnchor(mBuyButton, 0.75f, Duration, Easing::CubicOut),
				Actions::Collection::ChangeVerticalAnchor(mCancelButton, 0.75f, Duration, Easing::CubicOut)
			)
		)
	);
}

std::unique_ptr<Actions::Action> BuySkinMenu::createLeaveAction()
{
	const float Duration = 0.25f;

	return Actions::Collection::MakeParallel(
		Actions::Collection::MakeParallel(
			Actions::Collection::ChangeVerticalAnchor(mBuyButton, 1.25f, Duration, Easing::CubicIn),
			Actions::Collection::ChangeVerticalAnchor(mCancelButton, 1.25f, Duration, Easing::CubicIn)
		),
		Actions::Collection::Delayed(Duration / 2.0f,
			Actions::Collection::ChangeHorizontalAnchor(mImage, 1.5f, Duration, Easing::CubicIn)
		),
		Actions::Collection::Delayed(Duration,
			Actions::Collection::ChangeHorizontalAnchor(mTitle, -0.5f, Duration, Easing::CubicIn)
		)
	);
}