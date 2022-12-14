#include "input_window.h"
#include "helpers.h"

using namespace hcg001;

InputWindow::InputWindow(const utf8_string& title, const utf8_string& text, ChangeTextCallback changeTextCallback) :
	mChangeTextCallback(changeTextCallback)
{
	const float DefaultWindowHeight = 102.0f;
	const float MinLabelBgHeight = 32.0f;
	const float DefaultLabelBgHeight = 12.0f;

	getBackground()->setSize({ 314.0f, DefaultWindowHeight });
	getBackground()->setAnchor({ 0.5f, 0.25f });
	getTitle()->setText(title);

	auto button = std::make_shared<Helpers::Button>();
	button->setStretch({ 1.0f, 0.0f });
	button->setMargin({ 16.0f, 0.0f });
	button->setAnchor({ 0.5f, 0.0f });
	button->setPivot({ 0.5f, 0.0f });
	button->setHeight(DefaultLabelBgHeight);
	button->setY(12.0f);
	button->setRounding(8.0f);
	button->setAbsoluteRounding(true);
	button->setColor(Helpers::Pallete::InputField);
	button->setAdaptiveFontSize(false);
	button->setActiveCallback([] {
		PLATFORM->showVirtualKeyboard();
	});
	getBody()->attach(button);

	mLabel = button->getLabel();
	mLabel->setAnchor(0.5f);
	mLabel->setPivot(0.5f);
	mLabel->setStretch({ 1.0f, 0.0f });
	mLabel->setMargin({ 16.0f, 0.0f });
	mLabel->setMultiline(true);
	mLabel->setAlign(Graphics::TextMesh::Align::Center);
	mLabel->setFontSize(24.0f);
	mLabel->setText(text);
	
	runAction(Actions::Collection::ExecuteInfinite([this, button, MinLabelBgHeight, DefaultWindowHeight, DefaultLabelBgHeight](auto delta) {
		auto label_bg_h = DefaultLabelBgHeight;
		label_bg_h += mLabel->getAbsoluteHeight();
		label_bg_h = glm::max(MinLabelBgHeight, label_bg_h);
		button->setHeight(Common::Helpers::SmoothValueAssign(button->getHeight(), label_bg_h, delta));
		
		auto win_h = DefaultWindowHeight;
		win_h += button->getAbsoluteHeight();
		getBackground()->setHeight(win_h);
	}));

	mApplyButton = std::make_shared<Helpers::Button>();
	mApplyButton->getLabel()->setText(LOCALIZE("WINDOW_APPLY"));
	mApplyButton->setClickCallback([this] {
		mChangeTextCallback(mLabel->getText());
		SCENE_MANAGER->popWindow();
	});
	mApplyButton->setAnchor({ 0.5f, 1.0f });
	mApplyButton->setPivot(0.5f);
	mApplyButton->setSize({ 96.0f, 28.0f });
	mApplyButton->setPosition({ -56.0f, -24.0f });
	getBody()->attach(mApplyButton);

	mCancelButton = std::make_shared<Helpers::Button>();
	mCancelButton->getLabel()->setText(LOCALIZE("WINDOW_CANCEL"));
	mCancelButton->setClickCallback([this] {
		SCENE_MANAGER->popWindow();
	});
	mCancelButton->setAnchor({ 0.5f, 1.0f });
	mCancelButton->setPivot(0.5f);
	mCancelButton->setSize({ 96.0f, 28.0f });
	mCancelButton->setPosition({ 56.0f, -24.0f });
	getBody()->attach(mCancelButton);

	auto indicator = std::make_shared<Scene::Rectangle>();
	indicator->setAlpha(0.0f);
	indicator->setWidth(2.0f);
	indicator->setRounding(1.0f);
	indicator->runAction(Actions::Collection::ExecuteInfinite([indicator, this] {
		auto font = mLabel->getFont();
		
		auto font_scale = font->getScaleFactorForSize(mLabel->getFontSize());
		auto height = font->getAscent() * font_scale;
		height -= font->getDescent() * font_scale;

		indicator->setHeight(height);

		if (mLabel->getText().empty())
		{
			indicator->setPosition({ 0.0f, 0.0f });
			indicator->setAnchor(0.5f);
			indicator->setPivot(0.5f);
			return;
		}

		indicator->setAnchor(0.0f);
		indicator->setPivot({ 0.5f, 0.0f });

		auto index = mLabel->getText().length() - 1;
		
		auto [pos, size] = mLabel->getSymbolBounds(index);
		auto line_y = mLabel->getSymbolLineY(index);
		
		indicator->setX(pos.x + size.x);
		indicator->setY(line_y);
	}));
	indicator->runAction(Actions::Collection::RepeatInfinite([indicator] {
		return Actions::Collection::MakeSequence(
			Actions::Collection::ChangeAlpha(indicator, 1.0f, 0.125f),
			Actions::Collection::Wait(0.25f),
			Actions::Collection::ChangeAlpha(indicator, 0.0f, 0.125f),
			Actions::Collection::Wait(0.25f)
		);
	}));
	mLabel->attach(indicator);
}

void InputWindow::onEvent(const Platform::System::VirtualKeyboardTextChanged& e)
{
	mLabel->setText(e.text);
}

void InputWindow::onEvent(const Platform::System::VirtualKeyboardEnterPressed& e)
{
	mChangeTextCallback(mLabel->getText());
	SCENE_MANAGER->popWindow();
}

void InputWindow::onEvent(const Platform::Input::Keyboard::Event& e)
{
	if (e.type != Platform::Input::Keyboard::Event::Type::Pressed)
		return;

	auto& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard)
		return;

	auto text = mLabel->getText();

	if (e.key != Platform::Input::Keyboard::Key::Backspace)
		return;

	if (text.empty())
		return;

	text.pop_back();
	mLabel->setText(text);
}

void InputWindow::onEvent(const Platform::Input::Keyboard::CharEvent& e)
{
	auto& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard)
		return;

	auto text = mLabel->getText();

	mLabel->setText(text + e.codepoint);
}

void InputWindow::onOpenBegin()
{
	Window::onOpenBegin();

	assert(StackCount == 0);
	StackCount += 1;
	PLATFORM->showVirtualKeyboard();
	PLATFORM->setVirtualKeyboardText(mLabel->getText().cpp_str());
}

void InputWindow::onCloseBegin()
{
	PLATFORM->hideVirtualKeyboard();
	StackCount -= 1;
	assert(StackCount == 0);

	Window::onCloseBegin();
}
