#include "helpers.h"
#include "windows/input_window.h"

using namespace hcg001::Helpers;

Label::Label()
{
	setFont(FONT("default"));
	setFontSize(18.0f);
}

Button::Button()
{
	setRounding(0.5f);
	setColor(ButtonColor);

	mLabel = std::make_shared<Label>();
	mLabel->setAnchor(0.5f);
	mLabel->setPivot(0.5f);
	mLabel->runAction(Actions::Collection::ExecuteInfinite([this] {
		if (!mAdaptiveFontSize)
			return;

		mLabel->setFontSize(getAbsoluteHeight() * (18.0f / 28.0f));
	}));
	attach(mLabel);
}

RectangleButton::RectangleButton()
{
	setHighlightEnabled(false);
	setRounding(0.5f);
}

TextInputField::TextInputField()
{
	setRounding(0.5f);
	setColor(Helpers::HeadWindowColor / 24.0f);

	setClickCallback([this] {
		auto text = mLabel->getText();
		auto callback = [this](auto text) {
			mLabel->setText(text);
		};
		auto input_window = std::make_shared<InputWindow>(text, callback);
		SCENE_MANAGER->pushWindow(input_window);
	});

	mLabel = std::make_shared<Label>();
	mLabel->setAnchor(0.5f);
	mLabel->setPivot(0.5f);
	attach(mLabel);
}

float hcg001::Helpers::SmoothValueSetup(float src, float dst)
{
	auto delta = dst - src;
	auto dTime = Clock::ToSeconds(FRAME->getTimeDelta());
	const float speed = 10.0f;
	return src + (delta * dTime * speed);
}

WaitingIndicator::WaitingIndicator()
{
	setSize(24.0f);

	auto circle = std::make_shared<Scene::Circle>();
	circle->setStretch(1.0f);
	circle->setAnchor(0.5f);
	circle->setPivot(0.5f);
	circle->setFill(0.2f);
	circle->setPie(0.66f);
	attach(circle);

	runAction(Actions::Collection::RepeatInfinite([circle] {
		return Actions::Collection::ChangeRadialAnchor(circle, 0.0f, 1.0f, 1.0f);
		
		/*const auto PieMin = 0.125f;
		const auto PieMax = 1.0f - 0.125f;

		return Actions::Collection::MakeSequence(
			Actions::Collection::ChangeCirclePie(circle, PieMin, PieMax, 1.0f, Easing::CubicInOut),
			Actions::Collection::ChangeCirclePie(circle, PieMax, PieMin, 1.0f, Easing::CubicInOut)
		);*/
	}));
}