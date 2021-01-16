#include "helpers.h"

using namespace hcg001::Helpers;

Label::Label()
{
	setFont(FONT("default"));
	setFontSize(18.0f);
}

TextInputField::TextInputField()
{
	setClickCallback([this] {
		PLATFORM->showVirtualKeyboard();
		PLATFORM->setVirtualKeyboardText(mLabel->getText().cpp_str());
	});

	mLabel = std::make_shared<Label>();
	mLabel->setAnchor(0.5f);
	mLabel->setPivot(0.5f);
	attach(mLabel);
}

void TextInputField::onEvent(const Shared::TouchEmulator::Event& e)
{
	if (PLATFORM->isVirtualKeyboardOpened())
		PLATFORM->hideVirtualKeyboard();
}

void TextInputField::onEvent(const Platform::System::VirtualKeyboardTextChanged& e)
{
	mLabel->setText(e.text);
}