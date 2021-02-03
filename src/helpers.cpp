#include "helpers.h"
#include "windows/input_window.h"

using namespace hcg001::Helpers;

Label::Label()
{
	setFont(FONT("default"));
	setFontSize(18.0f);
}

RectangleButton::RectangleButton()
{
	setHighlightEnabled(false);
	setRounding(0.5f);
}

TextInputField::TextInputField()
{
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