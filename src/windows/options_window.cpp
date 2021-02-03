#include "options_window.h"

using namespace hcg001;

OptionsWindow::OptionsWindow()
{
	getBackground()->setSize({ 314.0f, 128.0f });
	getTitle()->setText(LOCALIZE("OPTIONS_WINDOW_TITLE"));

	auto nickname_label = std::make_shared<Helpers::Label>();
	nickname_label->setFontSize(16.0f);
	nickname_label->setPosition({ 16.0f, 24.0f });
	nickname_label->setPivot({ 0.0f, 0.5f });
	nickname_label->setText(LOCALIZE("OPTIONS_WINDOW_NICKNAME"));
	getBody()->attach(nickname_label);

	auto nickname_field = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	nickname_field->setPosition({ 128.0f, 24.0f });
	nickname_field->setSize({ 174.0f, 24.0f });
	nickname_field->setRounding(0.5f);
	nickname_field->setPivot({ 0.0f, 0.5f });
	nickname_field->setColor(Helpers::BaseWindowColor / 24.0f);
	getBody()->attach(nickname_field);

	mNicknameInputField = std::make_shared<Helpers::TextInputField>();
	mNicknameInputField->setAnchor(0.5f);
	mNicknameInputField->setPivot(0.5f);
	mNicknameInputField->setStretch(1.0f);
	mNicknameInputField->getLabel()->setText(PROFILE->getNickName());
	nickname_field->attach(mNicknameInputField);

	auto ok_button = std::make_shared<Helpers::RectangleButton>();
	ok_button->setColor(Helpers::BaseWindowColor);
	ok_button->getLabel()->setText(LOCALIZE("WINDOW_OK"));
	ok_button->getLabel()->setFontSize(18.0f);
	ok_button->setClickCallback([] {
		SCENE_MANAGER->popWindow();
	});
	ok_button->setAnchor({ 0.5f, 1.0f });
	ok_button->setPivot({ 0.5f, 0.5f });
	ok_button->setY(-24.0f);
	ok_button->setSize({ 128.0f, 28.0f });
	getBody()->attach(ok_button);
}

void OptionsWindow::onCloseBegin()
{
	Window::onCloseBegin();

	PROFILE->setNickName(mNicknameInputField->getLabel()->getText());
	PROFILE->save();
}
