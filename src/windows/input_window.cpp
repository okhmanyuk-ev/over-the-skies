#include "input_window.h"
#include "helpers.h"

using namespace hcg001;

InputWindow::InputWindow(const utf8_string& text, ChangeTextCallback changeTextCallback) :
	mChangeTextCallback(changeTextCallback)
{
	const float DefaultWindowHeight = 102.0f;
	const float MinLabelBgHeight = 32.0f;
	const float DefaultLabelBgHeight = 12.0f;

	getBackground()->setSize({ 314.0f, DefaultWindowHeight });
	getBackground()->setAnchor({ 0.5f, 0.25f });
	getTitle()->setText(LOCALIZE("INPUT_WINDOW_TITLE"));

	setCloseOnMissclick(false);

	auto label_bg = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::Rectangle>>>();
	label_bg->setStretch({ 1.0f, 0.0f });
	label_bg->setMargin({ 16.0f, 0.0f });
	label_bg->setAnchor({ 0.5f, 0.0f });
	label_bg->setPivot({ 0.5f, 0.0f });
	label_bg->setHeight(DefaultLabelBgHeight);
	label_bg->setY(12.0f);
	label_bg->setRounding(8.0f);
	label_bg->setAbsoluteRounding(true);
	label_bg->setColor(Graphics::Color::Black);
	label_bg->setAlpha(0.5f);
	label_bg->setClickCallback([] {
		PLATFORM->showVirtualKeyboard();
	});
	getBody()->attach(label_bg);

	mLabel = std::make_shared<Helpers::Label>();
	mLabel->setAnchor(0.5f);
	mLabel->setPivot(0.5f);
	mLabel->setStretch({ 1.0f, 0.0f });
	mLabel->setMargin({ 16.0f, 0.0f });
	mLabel->setMultiline(true);
	mLabel->setMultilineAlign(Graphics::TextMesh::Align::Center);
	mLabel->setFontSize(24.0f);
	mLabel->setText(text);
	label_bg->attach(mLabel);

	runAction(Actions::Collection::ExecuteInfinite([this, label_bg, MinLabelBgHeight, DefaultWindowHeight, DefaultLabelBgHeight] {
		auto label_bg_h = DefaultLabelBgHeight;
		label_bg_h += mLabel->getAbsoluteHeight();
		label_bg_h = glm::max(MinLabelBgHeight, label_bg_h);
		label_bg->setHeight(Helpers::SmoothValueSetup(label_bg->getHeight(), label_bg_h));
		
		auto win_h = DefaultWindowHeight;
		win_h += label_bg->getAbsoluteHeight();
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

	auto rect = std::make_shared<Scene::Rectangle>();
	rect->setAlpha(0.0f);
	rect->setWidth(2.0f);
	rect->setRounding(1.0f);
	rect->runAction(Actions::Collection::ExecuteInfinite([rect, this] {
		auto font = mLabel->getFont();
		
		auto font_scale = font->getScaleFactorForSize(mLabel->getFontSize());
		auto height = font->getAscent() * font_scale;
		height -= font->getDescent() * font_scale;

		rect->setHeight(height);

		if (mLabel->getText().empty())
		{
			rect->setPosition({ 0.0f, 0.0f });
			rect->setAnchor(0.5f);
			rect->setPivot(0.5f);
			return;
		}

		rect->setAnchor(0.0f);
		rect->setPivot({ 0.5f, 0.0f });

		auto index = mLabel->getText().length() - 1;
		
		auto [pos, size] = mLabel->getSymbolBounds(index);
		auto line_y = mLabel->getSymbolLineY(index);
		
		rect->setX(pos.x + size.x);
		rect->setY(line_y);
	}));
	rect->runAction(Actions::Collection::RepeatInfinite([rect] {
		return Actions::Collection::MakeSequence(
			Actions::Collection::ChangeAlpha(rect, 1.0f, 0.125f),
			Actions::Collection::Wait(0.25f),
			Actions::Collection::ChangeAlpha(rect, 0.0f, 0.125f),
			Actions::Collection::Wait(0.25f)
		);
	}));
	mLabel->attach(rect);
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
