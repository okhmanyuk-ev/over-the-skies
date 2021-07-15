#include "screen.h"

using namespace hcg001;

Screen::Screen()
{
	setEnabled(false);
	setInteractions(false);
	setStretch(1.0f);
	setAnchor(0.5f);
	setPivot(0.5f);
	setAlpha(0.0f);
}

void Screen::onEnterBegin()
{
	setEnabled(true);
	setScale(0.95f);
}

void Screen::onEnterEnd()
{
	setInteractions(true);
}

void Screen::onLeaveBegin()
{
	setInteractions(false);
}

void Screen::onLeaveEnd()
{
	setEnabled(false);
	setAlpha(0.0f);
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
	return Actions::Collection::MakeParallel(
		Actions::Collection::Show(shared_from_this(), 0.25f),
		Actions::Collection::ChangeScale(shared_from_this(), { 1.0f, 1.0f }, 0.25f, Easing::QuadraticOut)
	);
};

std::unique_ptr<Actions::Action> Screen::createLeaveAction()
{
	return Actions::Collection::MakeParallel(
		Actions::Collection::Hide(shared_from_this(), 0.25f),
		Actions::Collection::ChangeScale(shared_from_this(), { 0.95f, 0.95f }, 0.25f, Easing::QuadraticOut)
	);
};