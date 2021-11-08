#include "screen.h"

using namespace hcg001;

Screen::Screen()
{
	setEnabled(false);
	setInteractions(false);
	setStretch(1.0f);
	setAnchor(0.5f);
	setPivot(0.5f);
	getRenderLayerColor()->setAlpha(0.0f);

	mContent = std::make_shared<Scene::Node>();
	mContent->setStretch(1.0f);
	mContent->setAnchor(0.5f);
	mContent->setPivot(0.5f);
	attach(mContent);

	mBlur = std::make_shared<Scene::BlurredGlass>();
	mBlur->setStretch(1.0f);
	attach(mBlur);

	mGui = std::make_shared<Shared::SceneHelpers::SafeArea>();
	attach(mGui);
}

void Screen::onEnterBegin()
{
	setEnabled(true);
	setRenderLayerEnabled(true);
	mContent->setScale(0.95f);
	mBlur->setBlurIntensity(1.0f);
}

void Screen::onEnterEnd()
{
	setInteractions(true);
	setRenderLayerEnabled(false);
}

void Screen::onLeaveBegin()
{
	setInteractions(false);
	setRenderLayerEnabled(true);
}

void Screen::onLeaveEnd()
{
	setEnabled(false);
	getRenderLayerColor()->setAlpha(0.0f);
	setRenderLayerEnabled(false);
}

void Screen::onWindowAppearing()
{
	setInteractions(false);
}

void Screen::onWindowDisappearing()
{
	setInteractions(true);
}

std::unique_ptr<Actions::Action> Screen::createEnterAction()
{
	const float Duration = 0.25f;

	return Actions::Collection::MakeParallel(
		Actions::Collection::Show(getRenderLayerColor(), Duration, Easing::Linear),
		Actions::Collection::ChangeBlurIntensity(mBlur, 0.0f, Duration, Easing::CubicIn),
		Actions::Collection::ChangeScale(mContent, { 1.0f, 1.0f }, Duration, Easing::CubicIn)
	);
};

std::unique_ptr<Actions::Action> Screen::createLeaveAction()
{
	const float Duration = 0.25f;

	return Actions::Collection::MakeParallel(
		Actions::Collection::Hide(getRenderLayerColor(), Duration, Easing::Linear),
		Actions::Collection::ChangeBlurIntensity(mBlur, 1.0f, Duration, Easing::CubicOut),
		Actions::Collection::ChangeScale(mContent, { 0.95f, 0.95f }, Duration, Easing::CubicOut)
	);
};