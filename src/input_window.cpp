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

	runAction(Actions::Factory::Delayed(2.0f, Actions::Factory::Execute([this] {
		auto ok_button = std::make_shared<Shared::SceneHelpers::FastButton>();
		ok_button->setRounding(6.0f);
		ok_button->setAbsoluteRounding(true);
		ok_button->getLabel()->setText(LOCALIZE("INPUT_WINDOW_APPLY"));
		ok_button->getLabel()->setFontSize(18.0f);
		ok_button->setClickCallback([this] {
			mChangeTextCallback(mLabel->getText());
			SCENE_MANAGER->popWindow();
		});
		ok_button->setAnchor({ 0.5f, 0.85f });
		ok_button->setPivot(0.5f);
		ok_button->setSize({ 96.0f, 36.0f });
		ok_button->setX(-56.0f);
		getContent()->attach(ok_button);

		auto cancel_button = std::make_shared<Shared::SceneHelpers::FastButton>();
		cancel_button->setRounding(6.0f);
		cancel_button->setAbsoluteRounding(true);
		cancel_button->getLabel()->setText(LOCALIZE("INPUT_WINDOW_CANCEL"));
		cancel_button->getLabel()->setFontSize(18.0f);
		cancel_button->setClickCallback([this] {
			SCENE_MANAGER->popWindow();
		});
		cancel_button->setAnchor({ 0.5f, 0.85f });
		cancel_button->setPivot(0.5f);
		cancel_button->setSize({ 96.0f, 36.0f });
		cancel_button->setX(56.0f);
		getContent()->attach(cancel_button);
	})));
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

void InputWindow::onCloseBegin()
{
	PLATFORM->hideVirtualKeyboard();
	StackCount -= 1;
	assert(StackCount == 0);

	Window::onCloseBegin();
}
