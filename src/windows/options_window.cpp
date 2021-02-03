#include "options_window.h"
#include "helpers.h"

using namespace hcg001;

OptionsWindow::OptionsWindow()
{
	auto rect = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	rect->setRounding(12.0f);
	rect->setAbsoluteRounding(true);
	rect->setSize({ 314.0f, 128.0f });
	rect->setAnchor(0.5f);
	rect->setPivot(0.5f);
	rect->setTouchable(true);
	rect->setColor(Helpers::BaseWindowColor / 12.0f);
	getContent()->attach(rect);

	auto header = std::make_shared<Scene::Node>();
	header->setHorizontalStretch(1.0f);
	header->setHeight(36.0f);
	rect->attach(header);

	auto footer = std::make_shared<Scene::Node>();
	footer->setHeight(44.0f);
	footer->setHorizontalStretch(1.0f);
	footer->setAnchor({ 0.5f, 1.0f });
	footer->setPivot({ 0.5f, 1.0f });
	rect->attach(footer);

	auto content = std::make_shared<Scene::Node>();
	content->setStretch(1.0f);
	content->setY(header->getHeight());
	content->setVerticalMargin(header->getHeight() + footer->getHeight());
	rect->attach(content);

	auto header_bg = std::make_shared<Scene::Rectangle>();
	header_bg->setStretch(1.0f);
	header_bg->setColor(Helpers::BaseWindowColor);
	header_bg->setAlpha(0.25f);
	header->attach(header_bg);

	auto title = std::make_shared<Helpers::Label>();
	title->setFontSize(20.0f);
	title->setText(LOCALIZE("OPTIONS_WINDOW_TITLE"));
	title->setAnchor(0.5f);
	title->setPivot(0.5f);
	header_bg->attach(title);

	auto nickname_label = std::make_shared<Helpers::Label>();
	nickname_label->setFontSize(16.0f);
	nickname_label->setPosition({ 16.0f, 0.0f });
	nickname_label->setAnchor({ 0.0f, 0.5f });
	nickname_label->setPivot({ 0.0f, 0.5f });
	nickname_label->setText(LOCALIZE("OPTIONS_WINDOW_NICKNAME"));
	content->attach(nickname_label);

	auto nickname_field = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	nickname_field->setPosition({ 128.0f, 0.0f });
	nickname_field->setSize({ 174.0f, 24.0f });
	nickname_field->setRounding(0.5f);
	nickname_field->setPivot({ 0.0f, 0.5f });
	nickname_field->setAnchor({ 0.0f, 0.5f });
	nickname_field->setColor(Helpers::BaseWindowColor / 24.0f);
	content->attach(nickname_field);

	mNicknameInputField = std::make_shared<Helpers::TextInputField>();
	mNicknameInputField->setAnchor(0.5f);
	mNicknameInputField->setPivot(0.5f);
	mNicknameInputField->setStretch(1.0f);
	mNicknameInputField->getLabel()->setText(PROFILE->getNickName());
	nickname_field->attach(mNicknameInputField);

	auto ok_button = std::make_shared<Helpers::RectangleButton>();
	ok_button->setColor(Helpers::BaseWindowColor);
	ok_button->getLabel()->setText(LOCALIZE("OPTIONS_WINDOW_OK"));
	ok_button->getLabel()->setFontSize(18.0f);
	ok_button->setClickCallback([] {
		SCENE_MANAGER->popWindow();
	});
	ok_button->setAnchor(0.5f);
	ok_button->setPivot(0.5f);
	ok_button->setSize({ 128.0f, 28.0f });
	footer->attach(ok_button);
}

void OptionsWindow::onCloseBegin()
{
	Window::onCloseBegin();

	PROFILE->setNickName(mNicknameInputField->getLabel()->getText());
	PROFILE->save();
}
