#include "input_window.h"
#include "helpers.h"

using namespace hcg001;

InputWindow::InputWindow(const utf8_string& text, ChangeTextCallback changeTextCallback) :
	mChangeTextCallback(changeTextCallback)
{
	setFadeAlpha(0.95f);
	setCloseOnMissclick(false);

	mLabel = std::make_shared<Helpers::Label>();
	mLabel->setAnchor({ 0.5f, 0.25f });
	mLabel->setPivot(0.5f);
	mLabel->setWidth(314.0f);
	mLabel->setMultiline(true);
	mLabel->setMultilineAlign(Graphics::TextMesh::Align::Center);
	mLabel->setFontSize(32.0f);
	mLabel->setText(text);
	getContent()->attach(mLabel);

	mApplyButton = std::make_shared<Helpers::RectangleButton>();
	mApplyButton->getLabel()->setText(LOCALIZE("INPUT_WINDOW_APPLY"));
	mApplyButton->getLabel()->setFontSize(18.0f);
	mApplyButton->setClickCallback([this] {
		mChangeTextCallback(mLabel->getText());
		SCENE_MANAGER->popWindow();
	});
	mApplyButton->setAnchor({ 0.5f, 0.85f });
	mApplyButton->setPivot(0.5f);
	mApplyButton->setSize({ 96.0f, 36.0f });
	mApplyButton->setX(-56.0f);
	mApplyButton->setVisible(false);
	getContent()->attach(mApplyButton);

	mCancelButton = std::make_shared<Helpers::RectangleButton>();
	mCancelButton->getLabel()->setText(LOCALIZE("INPUT_WINDOW_CANCEL"));
	mCancelButton->getLabel()->setFontSize(18.0f);
	mCancelButton->setClickCallback([this] {
		SCENE_MANAGER->popWindow();
	});
	mCancelButton->setAnchor({ 0.5f, 0.85f });
	mCancelButton->setPivot(0.5f);
	mCancelButton->setSize({ 96.0f, 36.0f });
	mCancelButton->setX(56.0f);
	mCancelButton->setVisible(false);
	getContent()->attach(mCancelButton);
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

	if (e.key == Platform::Input::Keyboard::Key::Backspace)
	{
		if (!text.empty())
		{
			text.pop_back();
			mLabel->setText(text);
		}
		return;
	}

	if (e.key == Platform::Input::Keyboard::Key::Shift)
		return;

	if (e.key == Platform::Input::Keyboard::Key::Ctrl)
		return;

	if (PLATFORM->isKeyPressed(Platform::Input::Keyboard::Key::Ctrl))
		return;

	mLabel->setText(text + e.asciiChar);
}

void InputWindow::onOpenBegin()
{
	Window::onOpenBegin();

	assert(StackCount == 0);
	StackCount += 1;
	PLATFORM->showVirtualKeyboard();
	PLATFORM->setVirtualKeyboardText(mLabel->getText().cpp_str());
}

void InputWindow::onOpenEnd()
{
	Window::onOpenEnd();

	mApplyButton->setVisible(true);
	mCancelButton->setVisible(true);
}

void InputWindow::onCloseBegin()
{
	PLATFORM->hideVirtualKeyboard();
	StackCount -= 1;
	assert(StackCount == 0);

	mApplyButton->setVisible(false);
	mCancelButton->setVisible(false);

	Window::onCloseBegin();
}
